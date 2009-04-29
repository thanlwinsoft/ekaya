/*
 * Copyright (C) 2009 ThanLwinSoft.org
 *
 * The Ekaya library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The KMFL library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with the Ekaya library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 *
 */
#include <windows.h>
#include <ole2.h>
#include <olectl.h>
#include <assert.h>

#include "Ekaya.h"
#include "EkayaLangBarButton.h"
#include "EkayaKeyboardFactory.h"
#include "EkayaKeyboard.h"
#include "EkayaEditSession.h"
#include "EkayaInputProcessor.h"
#include "KmflKeyboardFactory.h"
#include "MessageLogger.h"

static const TF_PRESERVEDKEY KEY_ONOFF = { 0x20, TF_MOD_CONTROL };

static const WCHAR ONOFF_DESC[]    = L"OnOff";

// {7963550C-192E-41e0-A7B0-898881899F1F}
static const GUID GUID_PRESERVEDKEY_ONOFF = 
{ 0x7963550c, 0x192e, 0x41e0, { 0xa7, 0xb0, 0x89, 0x88, 0x81, 0x89, 0x9f, 0x1f } };


EkayaInputProcessor::EkayaInputProcessor()
: mOpen(false), mDisabled(true), mRefCount(1), mActiveKeyboard(-1),
  mClientId(TF_CLIENTID_NULL),
  mThreadEventCookie(TF_INVALID_COOKIE), mEditEventCookie(TF_INVALID_COOKIE),
  mMouseCookie(TF_INVALID_COOKIE),
  mpThreadMgr(NULL), mpLangBarButton(NULL), 
  mpComposition(NULL), mpCompositionRange(NULL)
{
	DllAddRef();
	mKeyboardFactories.push_back(new KmflKeyboardFactory());
	initKeyboards();
}

EkayaInputProcessor::~EkayaInputProcessor()
{
	std::vector<EkayaKeyboardFactory*>::iterator i = mKeyboardFactories.begin();
	while (i != mKeyboardFactories.end())
	{
		delete (*i);
		++i;
	}
	DllRelease();
}

// IUnknown
STDAPI EkayaInputProcessor::QueryInterface(REFIID riid, void **ppvObj)
{
	if (ppvObj == NULL)
        return E_INVALIDARG;

    *ppvObj = NULL;

    if (IsEqualIID(riid, IID_IUnknown) ||
        IsEqualIID(riid, IID_ITfTextInputProcessor))
    {
        *ppvObj = (ITfTextInputProcessor *)this;
    }
    else if (IsEqualIID(riid, IID_ITfThreadMgrEventSink))
    {
        *ppvObj = (ITfThreadMgrEventSink *)this;
    }
    else if (IsEqualIID(riid, IID_ITfTextEditSink))
    {
        *ppvObj = (ITfTextEditSink *)this;
    }
    else if (IsEqualIID(riid, IID_ITfKeyEventSink))
    {
        *ppvObj = (ITfKeyEventSink *)this;
    }
	else if (IsEqualIID(riid, IID_ITfCompositionSink))
    {
        *ppvObj = (ITfCompositionSink *)this;
    }
	else if (IsEqualIID(riid, IID_ITfMouseSink))
    {
        *ppvObj = (ITfMouseSink *)this;
    }

    if (*ppvObj)
    {
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

STDAPI_(ULONG) EkayaInputProcessor::AddRef(void)
{
	++mRefCount;
	return mRefCount;
}

STDAPI_(ULONG) EkayaInputProcessor::Release(void)
{
	unsigned long count = --mRefCount;
	if (count == 0)
		delete this;
	assert(count >= 0);
	return count;
}

// ITfTextInputProcessor
STDAPI EkayaInputProcessor::Activate(ITfThreadMgr *pThreadMgr, TfClientId tfClientId)
{
	mpThreadMgr = pThreadMgr;
    mpThreadMgr->AddRef();
    mClientId = tfClientId;

	HRESULT status = S_OK;

	ITfSource *pSource;
    if (mpThreadMgr->QueryInterface(IID_ITfSource, (void **)&pSource) == S_OK)
	{
        if (pSource->AdviseSink(IID_ITfThreadMgrEventSink, (ITfThreadMgrEventSink *)this, &mThreadEventCookie) != S_OK)
		{
			//Don't try to Unadvise cookie later
			mThreadEventCookie = TF_INVALID_COOKIE;
			status = E_FAIL;
		}
		
	}
	else status = E_FAIL;
    pSource->Release();

	// Document and Keystroke manager
	ITfDocumentMgr *pDocMgrFocus;
    if (status == S_OK &&
		(mpThreadMgr->GetFocus(&pDocMgrFocus) == S_OK) &&
        (pDocMgrFocus != NULL))
    {
        setTextEditSink(pDocMgrFocus);
        pDocMgrFocus->Release();
    }

	ITfKeystrokeMgr *pKeystrokeMgr;
	if (status == S_OK &&
		mpThreadMgr->QueryInterface(IID_ITfKeystrokeMgr, (void **)&pKeystrokeMgr) == S_OK)
	{
		status = pKeystrokeMgr->AdviseKeyEventSink(mClientId, (ITfKeyEventSink *)this, TRUE);
		status = pKeystrokeMgr->PreserveKey(mClientId, 
                                GUID_PRESERVEDKEY_ONOFF,
                                &KEY_ONOFF,
                                ONOFF_DESC,
                                (ULONG)wcslen(ONOFF_DESC));

		pKeystrokeMgr->Release();
	}
	else status = E_FAIL;

	

	// language bar
	ITfLangBarItemMgr *pLangBarItemMgr;
    if (status == S_OK &&
		mpThreadMgr->QueryInterface(IID_ITfLangBarItemMgr, (void **)&pLangBarItemMgr) == S_OK)
	{
	    if ((mpLangBarButton = new EkayaLangBarButton(this)) != NULL)
		{
		    if (pLangBarItemMgr->AddItem(mpLangBarButton) != S_OK)
			{
				mpLangBarButton->Release();
				mpLangBarButton = NULL;
				status = E_FAIL;
			}
			else mpLangBarButton->AddRef();
		}
	    pLangBarItemMgr->Release();
	}
	else status = E_FAIL;

	if (status != S_OK)
	{
		Deactivate(); // cleanup any half-finished init
	}
    return status;
}

STDAPI EkayaInputProcessor::Deactivate()
{

	ITfSource *pSource;
	if (mThreadEventCookie != TF_INVALID_COOKIE)
	{
		if (mpThreadMgr->QueryInterface(IID_ITfSource, (void **)&pSource) == S_OK)
		{
			pSource->UnadviseSink(mThreadEventCookie);
			pSource->Release();
		}
	    mThreadEventCookie = TF_INVALID_COOKIE;
	}

	if (mMouseCookie != TF_INVALID_COOKIE)
	{
		ITfMouseTracker * pMouseTracker;
		if (mpThreadMgr->QueryInterface(IID_ITfMouseTracker, (void **)&pMouseTracker) == S_OK)
		{
			pMouseTracker->UnadviseMouseSink(mMouseCookie);
			pMouseTracker->Release();
		}
		mMouseCookie = TF_INVALID_COOKIE;
	}

	setTextEditSink(NULL);

	ITfKeystrokeMgr *pKeystrokeMgr;
    if (mpThreadMgr->QueryInterface(IID_ITfKeystrokeMgr, (void **)&pKeystrokeMgr) == S_OK)
	{
		pKeystrokeMgr->UnadviseKeyEventSink(mClientId);
		pKeystrokeMgr->UnpreserveKey(GUID_PRESERVEDKEY_ONOFF, &KEY_ONOFF);
		pKeystrokeMgr->Release();
	}

	// language bar
	ITfLangBarItemMgr *pLangBarItemMgr;
	if (mpLangBarButton != NULL &&
		mpThreadMgr->QueryInterface(IID_ITfLangBarItemMgr, (void **)&pLangBarItemMgr) == S_OK)
    {
        pLangBarItemMgr->RemoveItem(mpLangBarButton);
	    mpLangBarButton->Release();
		mpLangBarButton = NULL;
        pLangBarItemMgr->Release();
    }

    mClientId = TF_CLIENTID_NULL;

    return S_OK;
}

HRESULT EkayaInputProcessor::setTextEditSink(ITfDocumentMgr *pDocMgrFocus)
{
	ITfSource *pSource;
    HRESULT fRet;

    // clear out any previous sink first
    if (mEditEventCookie != TF_INVALID_COOKIE)
    {
        if (mpTextEditSinkContext &&
			mpTextEditSinkContext->QueryInterface(IID_ITfSource, (void **)&pSource) == S_OK)
        {
            pSource->UnadviseSink(mEditEventCookie);
            pSource->Release();
        }

        mpTextEditSinkContext->Release();
        mpTextEditSinkContext = NULL;
        mEditEventCookie = TF_INVALID_COOKIE;
    }

    if (pDocMgrFocus == NULL)
        return S_OK; // caller just wanted to clear the previous sink

    // setup a new sink advised to the topmost context of the document
    if (pDocMgrFocus->GetTop(&mpTextEditSinkContext) != S_OK)
        return E_FAIL;

    if (mpTextEditSinkContext == NULL)
        return S_OK; // empty document, no sink possible

    fRet = E_FAIL;

    if (mpTextEditSinkContext)
	{
		if (mpTextEditSinkContext->QueryInterface(IID_ITfSource, (void **)&pSource) == S_OK)
		{
			if (pSource->AdviseSink(IID_ITfTextEditSink, (ITfTextEditSink *)this, &mEditEventCookie) == S_OK)
			{
				fRet = S_OK;
			}
			else
			{
				mEditEventCookie = TF_INVALID_COOKIE;
			}
			if (pSource->AdviseSink(IID_ITfTextLayoutSink, (ITfTextLayoutSink *)this, &mTextLayoutCookie) == S_OK)
			{
				fRet = S_OK;
			}
			else
			{
				mTextLayoutCookie = TF_INVALID_COOKIE;
			}
			pSource->Release();
		}
	}

    if (fRet != S_OK)
    {
        mpTextEditSinkContext->Release();
        mpTextEditSinkContext = NULL;
    }
	return fRet;
}

// ITfThreadMgrEventSink
STDAPI EkayaInputProcessor::OnInitDocumentMgr(ITfDocumentMgr *pDocMgr)
{
	MessageLogger::logMessage("InitDocumentMgr %lx\n", (long)pDocMgr);
	return S_OK;
}

STDAPI EkayaInputProcessor::OnUninitDocumentMgr(ITfDocumentMgr *pDocMgr)
{
	MessageLogger::logMessage("UninitDocumentMgr %lx\n", (long)pDocMgr);
	return S_OK;
}

STDAPI EkayaInputProcessor::OnSetFocus(ITfDocumentMgr *pDocMgrFocus, ITfDocumentMgr *pDocMgrPrevFocus)
{
	MessageLogger::logMessage("SetFocus %lx\n", (long)pDocMgrFocus);
	return setTextEditSink(pDocMgrFocus);
}

STDAPI EkayaInputProcessor::OnPushContext(ITfContext *pContext)		
{
	MessageLogger::logMessage("PushContext %lx\n", (long)pContext);

	return S_OK;
}

STDAPI EkayaInputProcessor::OnPopContext(ITfContext *pContext)
{
	MessageLogger::logMessage("PopContext %lx\n", (long)pContext);
	return S_OK;
}

// ITfTextEditSink
STDAPI EkayaInputProcessor::OnEndEdit(ITfContext *pContext, TfEditCookie ecReadOnly, ITfEditRecord *pEditRecord)
{
	BOOL fSelectionChanged;
    IEnumTfRanges *pEnumTextChanges;
    ITfRange *pRange;
	MessageLogger::logMessage("EndEdit ");
    //
    // did the selection change?
    // The selection change includes the movement of caret as well. 
    // The caret position is represent as the empty selection range when
    // there is no selection.
    //
    if (pEditRecord->GetSelectionStatus(&fSelectionChanged) == S_OK &&
        fSelectionChanged)
    {
		MessageLogger::logMessage("Selection changed\n");
    }

    // text modification?
    if (pEditRecord->GetTextAndPropertyUpdates(TF_GTP_INCL_TEXT, NULL, 0, &pEnumTextChanges) == S_OK)
    {
        if (pEnumTextChanges->Next(1, &pRange, NULL) == S_OK)
        {
            //
            // pRange is the updated range.
            //

            pRange->Release();
        }

        pEnumTextChanges->Release();
    }

	return S_OK;
}

    // ITfKeyEventSink
STDAPI EkayaInputProcessor::OnSetFocus(BOOL fForeground)
{
	MessageLogger::logMessage("SetFocus %d\n", fForeground);
	return S_OK;
}

/**
* Test whether the specified key is a special character that should be ignored
*/
bool EkayaInputProcessor::ignoreKey(WPARAM wParam)
{
	if (wParam < 0x30)
	{
		if (wParam != VK_SHIFT && wParam != VK_CONTROL && wParam != VK_SPACE)
			return true;
		return false;
	}
	else if (wParam < 0x5B)
	{
		return false;
	}
	else if (wParam >= 0xBA && wParam < 0xE0)
	{
		return false;
	}
	return true;
}

STDAPI EkayaInputProcessor::OnTestKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	MessageLogger::logMessage("OnTestKeyDown %x %x\n", (int)wParam, (int)lParam);
	HRESULT hr = S_OK;
	if (isKeyboardOpen() && !isKeyboardDisabled())
	{
		*pfEaten = FALSE;
	}
	else
	{
		if (ignoreKey(wParam)) 
		{
			*pfEaten = FALSE;
			// exit the context otherwise we get strange effects
			EkayaEndContextSession * pEditSession = NULL;
			if ((pEditSession = new EkayaEndContextSession(this, pContext, wParam)) == NULL)
				return E_FAIL;

			// A lock is required
			// nb: this method is one of the few places where it is legal to use
			// the TF_ES_SYNC flag
			hr = pContext->RequestEditSession(mClientId, pEditSession, TF_ES_SYNC | TF_ES_READWRITE, &hr);

			pEditSession->Release();
		}
		// ignore ctrl keys
		else if (mKeyState.at(KEY_CTRL) && (wParam != VK_CONTROL))
		{
			*pfEaten = FALSE;
		}
		else
		{
			*pfEaten = TRUE;
		}
	}
	return hr;
}

STDMETHODIMP EkayaInputProcessor::OnKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	if (isKeyboardOpen() && !isKeyboardDisabled())
	{
		*pfEaten = FALSE;
	}
	else
	{
		*pfEaten = TRUE;
	}

	EkayaEditSession *pEditSession;
    HRESULT hr = E_FAIL;
	MessageLogger::logMessage("OnKeyDown %x %x\n", (int)wParam, (int)lParam);
	// check for control keys
	if (wParam < 0x30)
	{
		switch (wParam)
		{
		case VK_SHIFT:
			mKeyState.set(KEY_SHIFT);
			return S_OK;
		case VK_CONTROL:
			mKeyState.set(KEY_CTRL);
			return S_OK;
		case VK_SPACE:
			break;
		default:
			// ignore
			*pfEaten = FALSE;
			return S_OK;
		}
		
	}

	if (!mKeyState.at(KEY_SHIFT))
	{
		if (wParam > 0x40 && wParam < 0x5B)
		{
			// allow for lower case
			wParam += 0x20;
		}
		else
		{
			switch (wParam)
			{
			case VK_OEM_1:// 0xBA ';:' for US
				wParam = 0x3B;
				break;
			case VK_OEM_PLUS: // 0xBB   // '+' any country
				wParam = 0x3D;
				break;
			case VK_OEM_COMMA: // 0xBC   // ',' any country
				wParam = 0x2C;
				break;
			case VK_OEM_MINUS: // 0xBD   // '-' any country
				wParam = 0x2D;
				break;
			case VK_OEM_PERIOD: // 0xBE   // '.' any country
				wParam = 0x2E;
				break;
			case VK_OEM_2: // 0xBF   // '/?' for US
				wParam = 0x2F;
				break;
			case VK_OEM_3: // 0xC0   // '`~' for US
				wParam = 0x60;
				break;
			case VK_OEM_4: // 0xDB  //  '[{' for US
				wParam = 0x5B;
				break;
			case VK_OEM_5: // 0xDC  //  '\|' for US
				wParam = 0x5C;
				break;
			case VK_OEM_6: // 0xDD  //  ']}' for US
				wParam = 0x5D;
				break;
			case VK_OEM_7: // 0xDE  //  ''"' for US
				wParam = 0x27;
				break;
			}
		}
	}
	else
	{
		// shifted
		switch (wParam)
		{
		case 0x30:
			wParam = 0x29;
			break;
		case 0x31:
			wParam = 0x21;
			break;
		case 0x32:
			wParam = 0x40;
			break;
		case 0x33:
			wParam = 0x23;
			break;
		case 0x34:
			wParam = 0x24;
			break;
		case 0x35:
			wParam = 0x25;
			break;
		case 0x36:
			wParam = 0x5E;
			break;
		case 0x37:
			wParam = 0x26;
			break;
		case 0x38:
			wParam = 0x2A;
			break;
		case 0x39:
			wParam = 0x28;
			break;
		case VK_OEM_1:// 0xBA ';:' for US
			wParam = 0x3A;
			break;
		case VK_OEM_PLUS: // 0xBB   // '+' any country
			wParam = 0x2B;
			break;
		case VK_OEM_COMMA: // 0xBC   // ',<' any country
			wParam = 0x3C;
			break;
		case VK_OEM_MINUS: // 0xBD   // '-' any country
			wParam = 0x5F;
			break;
		case VK_OEM_PERIOD: // 0xBE   // '.>' any country
			wParam = 0x3E;
			break;
		case VK_OEM_2: // 0xBF   // '/?' for US
			wParam = 0x3F;
			break;
		case VK_OEM_3: // 0xC0   // '`~' for US
			wParam = 0x7E;
			break;
		case VK_OEM_4: // 0xDB  //  '[{' for US
			wParam = 0x7B;
			break;
		case VK_OEM_5: // 0xDC  //  '\|' for US
			wParam = 0x7C;
			break;
		case VK_OEM_6: // 0xDD  //  ']}' for US
			wParam = 0x7D;
			break;
		case VK_OEM_7: // 0xDE  //  ''"' for US
			wParam = 0x22;
			break;
		}
	}
    // we'll insert a char ourselves in place of this keystroke
    if ((pEditSession = new EkayaEditSession(this, pContext, wParam)) == NULL)
		return E_FAIL;

    // A lock is required
    // nb: this method is one of the few places where it is legal to use
    // the TF_ES_SYNC flag
    hr = pContext->RequestEditSession(mClientId, pEditSession, TF_ES_SYNC | TF_ES_READWRITE, &hr);

    pEditSession->Release();
	return hr;
}

STDMETHODIMP EkayaInputProcessor::OnTestKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	MessageLogger::logMessage("OnTestKeyUp %x %x\n", (int)wParam, (int)lParam);
	if (isKeyboardOpen() && !isKeyboardDisabled())
	{
		*pfEaten = FALSE;
	}
	else
	{
		if (ignoreKey(wParam)) 
		{
			*pfEaten = FALSE;
		}
		// ignore ctrl keys
		else if (mKeyState.at(KEY_CTRL) && (wParam != VK_CONTROL))
		{
			*pfEaten = FALSE;
		}
		else
		{
			*pfEaten = TRUE;
		}
	}
	return S_OK;
}

STDMETHODIMP EkayaInputProcessor::OnKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	MessageLogger::logMessage("OnKeyUp %x %x\n", (int)wParam, (int)lParam);
	// check for control keys
	if (wParam < 0x30)
	{
		switch (wParam)
		{
		case VK_SHIFT:
			mKeyState.flip(KEY_SHIFT);
			break;
		case VK_CONTROL:
			mKeyState.flip(KEY_CTRL);
			break;
		case VK_SPACE:
			break;
		default:
			*pfEaten = TRUE;
			return S_OK;
		}
		
	}
	if (isKeyboardOpen() && !isKeyboardDisabled())
	{
		*pfEaten = FALSE;
	}
	else
	{
		*pfEaten = TRUE;
	}
	return S_OK;
}

STDMETHODIMP EkayaInputProcessor::OnPreservedKey(ITfContext *pContext, REFGUID rguid, BOOL *pfEaten)
{
	MessageLogger::logMessage("OnPreservedKey\n");
	if (IsEqualGUID(rguid, GUID_PRESERVEDKEY_ONOFF))
    {
        BOOL fOpen = isKeyboardOpen();
        setKeyboardOpen(fOpen ? FALSE : TRUE);
        *pfEaten = TRUE;
    }
    else
    {
        *pfEaten = FALSE;
    }
	return S_OK;
}

//+---------------------------------------------------------------------------
//
// OnCompositionTerminated
//
// Callback for ITfCompositionSink.  The system calls this method whenever
// someone other than this service ends a composition.
//----------------------------------------------------------------------------

STDAPI EkayaInputProcessor::OnCompositionTerminated(TfEditCookie ecWrite, ITfComposition *pComposition)
{

    // releae our cached composition
    if (mpComposition != NULL)
    {
		if (mpCompositionRange)
		{
			mpCompositionRange->Release();
			mpCompositionRange = NULL;
		}
        mpComposition->Release();
        mpComposition = NULL;
    }

    return S_OK;
}

STDAPI EkayaInputProcessor::OnMouseEvent(ULONG uEdge,
                         ULONG uQuadrant,
                         DWORD dwBtnStatus,
                         BOOL *pfEaten)
{
	return S_OK;
}

// CClassFactory factory callback
HRESULT EkayaInputProcessor::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObj)
{
	EkayaInputProcessor *pCase;
    HRESULT hr;

    if (ppvObj == NULL)
        return E_INVALIDARG;

    *ppvObj = NULL;

    if (NULL != pUnkOuter)
        return CLASS_E_NOAGGREGATION;

    if ((pCase = new EkayaInputProcessor) == NULL)
        return E_OUTOFMEMORY;

    hr = pCase->QueryInterface(riid, ppvObj);

	if (hr == S_OK)
	{
		pCase->Release(); // caller still holds ref if hr == S_OK
	}
    return hr;
}

STDAPI EkayaInputProcessor::OnLayoutChange(ITfContext *pContext, TfLayoutCode lcode, ITfContextView *pContextView)
{
	switch (lcode)
	{
	case TF_LC_CHANGE:
		break;
	case TF_LC_DESTROY:
		break;
	}
	MessageLogger::logMessage("OnLayoutChange\n");
	return S_OK;
}

// utility function for compartment
bool EkayaInputProcessor::isKeyboardDisabled()
{
	ITfCompartmentMgr *pCompMgr = NULL;
    ITfDocumentMgr *pDocMgrFocus = NULL;
    ITfContext *pContext = NULL;
    mDisabled = false;

    if ((mpThreadMgr->GetFocus(&pDocMgrFocus) != S_OK) ||
        (pDocMgrFocus == NULL))
    {
        // if there is no focus document manager object, the keyboard 
        // is disabled.
        mDisabled = true;
    }

    if (!mDisabled && ((pDocMgrFocus->GetTop(&pContext) != S_OK) ||
        (pContext == NULL)))
    {
        // if there is no context object, the keyboard is disabled.
        mDisabled = true;
    }

    if (!mDisabled && pContext->QueryInterface(IID_ITfCompartmentMgr, (void **)&pCompMgr) == S_OK)
    {
        ITfCompartment *pCompartmentDisabled;
        ITfCompartment *pCompartmentEmptyContext;

        // Check GUID_COMPARTMENT_KEYBOARD_DISABLED.
        if (pCompMgr->GetCompartment(GUID_COMPARTMENT_KEYBOARD_DISABLED, &pCompartmentDisabled) == S_OK)
        {
            VARIANT var;
            if (S_OK == pCompartmentDisabled->GetValue(&var))
            {
                if (var.vt == VT_I4) // Even VT_EMPTY, GetValue() can succeed
                {
                    mDisabled = var.lVal? true : false;
                }
            }
            pCompartmentDisabled->Release();
        }

        // Check GUID_COMPARTMENT_EMPTYCONTEXT.
        if (pCompMgr->GetCompartment(GUID_COMPARTMENT_EMPTYCONTEXT, &pCompartmentEmptyContext) == S_OK)
        {
            VARIANT var;
            if (S_OK == pCompartmentEmptyContext->GetValue(&var))
            {
                if (var.vt == VT_I4) // Even VT_EMPTY, GetValue() can succeed
                {
					mDisabled = var.lVal? true : false;
                }
            }
            pCompartmentEmptyContext->Release();
        }

        pCompMgr->Release();
    }

    if (pContext)
        pContext->Release();

    if (pDocMgrFocus)
        pDocMgrFocus->Release();

	return mDisabled;
}

bool EkayaInputProcessor::isKeyboardOpen()
{
	return mOpen;
}

HRESULT EkayaInputProcessor::setKeyboardOpen(bool fOpen)
{
	mOpen = fOpen;
	return S_OK;
}

const std::wstring EkayaInputProcessor::getMessage(const wchar_t * defaultMessage)
{
	// TODO implement localization
	return std::wstring(defaultMessage);
}

void EkayaInputProcessor::setComposition(ITfComposition * composition)
{
	if (mpComposition != composition)
	{
		if (mpComposition)
			mpComposition->Release();
		if (mpCompositionRange)
		{
			mpCompositionRange->Release();
			mpCompositionRange = NULL;
		}
		mpComposition = composition;
		if (mpComposition && mpComposition->GetRange(&mpCompositionRange) == S_OK)
		{
			
			// Mouse
			ITfMouseTracker * pMouseTracker;
			if (mpThreadMgr->QueryInterface(IID_ITfMouseTracker, (void **)&pMouseTracker) == S_OK)
			{
				ITfRange * range = NULL;
				if (pMouseTracker->AdviseMouseSink(mpCompositionRange, this, &mMouseCookie) != S_OK)
				{
					MessageLogger::logMessage("AdviseMouseSink failed\n");
				}
				pMouseTracker->Release();
			}
		}
	}
}

ITfComposition * EkayaInputProcessor::getComposition()
{
	if (mpCompositionRange) mpCompositionRange->Release();
	return mpComposition;
};

void EkayaInputProcessor::initKeyboards()
{
	// TODO initialize factories more generically

	for (size_t i = 0; i < mKeyboardFactories.size(); i++)
	{
		std::vector <EkayaKeyboard * > keyboards = mKeyboardFactories[i]->loadKeyboards();
		for (size_t j = 0; j < keyboards.size(); j++)
		{
			if (keyboards[j])
				mKeyboards.push_back(keyboards[j]);
		}
	}
	// TODO remember last active keyboard
	if (mKeyboards.size())
	{
		mActiveKeyboard = 0;
	}
	MessageLogger::logMessage("initKeyboards %d", (int)mKeyboards.size());
}

void EkayaInputProcessor::setActiveKeyboard(int keyboardIndex)
{
	MessageLogger::logMessage("setActiveKeyboard %d", keyboardIndex);
	mActiveKeyboard = keyboardIndex;
}
