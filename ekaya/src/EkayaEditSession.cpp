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

#include "EkayaEditSession.h"
#include "EkayaKeyboard.h"
#include "EkayaInputProcessor.h"
#include "UtfConversion.h"
#include "MessageLogger.h"

#include <kmfl/kmflutfconv.h>

STDAPI EkayaEditSession::DoEditSession(TfEditCookie ec)
{
	EkayaKeyboard * keyboard = NULL;
	if (mpTextService->getActiveKeyboard() > -1)
	{
		keyboard = mpTextService->getKeyboards()[mpTextService->getActiveKeyboard()];
	}
	if (!keyboard)
	{
		return S_FALSE;
	}

	// Even with compositions we don't seem to be able to be able to shift the end of the selection reliably
	ITfRange *pCompositionRange = NULL;
    TF_SELECTION tfSelection;
    ULONG cFetched = 0;
    WCHAR ch = (WCHAR)(mwParam);
    bool fCovered = true;
	ITfComposition *pComposition = mpTextService->getComposition();
	ITfInsertAtSelection *pInsertAtSelection = NULL;

	// first, test where a keystroke would go in the document if an insert is done
    if (mpContext->GetSelection(ec, TF_DEFAULT_SELECTION, 1, &tfSelection, &cFetched) != S_OK || cFetched != 1)
	{
		return S_FALSE;
	}
	if (pComposition)
	{
		if (pComposition->GetRange(&pCompositionRange) != S_OK)
		{
			tfSelection.range->Release();
			return S_FALSE;
		}
		LONG resultStart = 0, resultEnd = 0;
		ITfRange * prevRange = NULL;
		if (mpTextService->getCompositionRange())
			mpTextService->getCompositionRange()->Restore(ec, prevRange);
		//if (pCompositionRange->CompareStart(ec, tfSelection.range, TF_ANCHOR_START, &resultStart) == S_OK &&
		//	pCompositionRange->CompareEnd(ec, tfSelection.range, TF_ANCHOR_END, &resultEnd) == S_OK)
		if (prevRange == NULL || (pCompositionRange->CompareStart(ec, prevRange, TF_ANCHOR_START, &resultStart) == S_OK &&
			pCompositionRange->CompareEnd(ec, prevRange, TF_ANCHOR_END, &resultEnd) == S_OK))
		{
			MessageLogger::logMessage("Compare range start %d end %d\n", resultStart, resultEnd);
			if (resultStart > 0 || resultEnd < 0)
			{
				// selection has moved out of composition range, so end composition
				pComposition->EndComposition(ec);
				mpTextService->setComposition(NULL);// releases composition
				pComposition = NULL;
			}
			pCompositionRange->Release();
			pCompositionRange = NULL;
		}
		else
		{
			tfSelection.range->Release();
			return S_FALSE;
		}
	}

	// TODO decide if we really need a composition
    // Start the new compositon if there is no composition.
	if (pComposition == NULL)
	{
		//ch = (WCHAR)(mwParam - 'A' + 0x1000);
		// A special interface is required to insert text at the selection
		if (mpContext->QueryInterface(IID_ITfInsertAtSelection, (void **)&pInsertAtSelection) != S_OK)
		{
			tfSelection.range->Release();
			return S_FALSE;
		}

		// insert the text
		ITfRange *pRangeInsert = NULL;
		ITfContextComposition *pContextComposition = NULL;
		if (pInsertAtSelection->InsertTextAtSelection(ec, TF_IAS_QUERYONLY, NULL, 0, &pRangeInsert) != S_OK)
		{
			tfSelection.range->Release();
			pInsertAtSelection->Release();
			return S_FALSE;
		}

		// get an interface on the context to deal with compositions
		if (mpContext->QueryInterface(IID_ITfContextComposition, (void **)&pContextComposition) != S_OK)
		{
			tfSelection.range->Release();
			pInsertAtSelection->Release();
			return S_FALSE;
		}

		LONG shift = 0;
		if ((pContextComposition->StartComposition(ec, pRangeInsert, mpTextService, &pComposition) == S_OK) && (pComposition != NULL))
		{
			/*if (pRangeInsert->ShiftStart(ec, -1, &shift, NULL) != S_OK)
			{
				pInsertAtSelection->Release();
				pRangeInsert->Release();
				pContextComposition->Release();
				return S_FALSE;
			}*/
			mpTextService->setComposition(pComposition);
			
			pRangeInsert->Release();
		}
		else
		{
			tfSelection.range->Release();
			pInsertAtSelection->Release();
			pContextComposition->Release();
			return S_FALSE;
		}
		// release the interfaces
		pInsertAtSelection->Release();
		pContextComposition->Release();
		pInsertAtSelection = NULL;
		pContextComposition = NULL;
	}
	// get the composition range to use as context
	if (pComposition->GetRange(&pCompositionRange) != S_OK)
	{
		return S_FALSE;
	}

    
	ULONG contextLength = 0;
	const size_t MAX_CONTEXT_LEN = 16;
	WCHAR wBuffer[MAX_CONTEXT_LEN * 2];

	if (pCompositionRange->GetText(ec, 0, wBuffer, MAX_CONTEXT_LEN * 2, &contextLength) != S_OK)
	{
		pCompositionRange->Release();
		tfSelection.range->Release();
		return S_FALSE;
	}
	MessageLogger::logMessage("CompositionRange length %d\n", (int)contextLength);

	std::basic_string<Utf32>context = UtfConversion::convertUtf16ToUtf32(std::wstring(wBuffer, contextLength));
	size_t oldContextPos = context.length();
	int keyResult = keyboard->processKey(static_cast<long>(mwParam), context, static_cast<int>(context.length()));
	MessageLogger::logMessage("processKey %d\n", keyResult);

	// convert context back to UTF16
	std::wstring convertedContext = UtfConversion::convertUtf32ToUtf16(context);

	// temp hack
	if (oldContextPos >= keyResult)
	{
		ITfDocumentMgr * docMgr;
		mpContext->GetDocumentMgr(&docMgr);
		ITfContext *baseContext;
		docMgr->GetBase(&baseContext);
		ITfContextView * contextView;
		mpContext->GetActiveView(&contextView);
		HWND hWindow;
		contextView->GetWnd(&hWindow);

		MSG msg;
		msg.hwnd = hWindow;
		msg.lParam = 0;
		msg.wParam = VK_DELETE;
		msg.time = 0;
		msg.pt.x = 0;
		msg.pt.y = 0;
		msg.message = WM_KEYDOWN;
		//DispatchMessageW(&msg);

		INPUT delInput;
		delInput.type = INPUT_KEYBOARD;
		delInput.ki.wVk = VK_BACK;
		delInput.ki.wScan = 0;
		delInput.ki.dwExtraInfo = 0;
		delInput.ki.dwFlags = 0;
		delInput.ki.time = 0;
		// SentInput really works
		// SendInput(1, &delInput, sizeof(INPUT));

		ITfRange * docStart;
		ITfRange * docEnd;
		baseContext->GetStart(ec, &docStart);
		baseContext->GetEnd(ec, &docEnd);
		LONG startDelta, endDelta;
		pCompositionRange->CompareStart(ec, docStart, TF_ANCHOR_START, &startDelta);
		pCompositionRange->CompareEnd(ec, docStart, TF_ANCHOR_END, &endDelta);
		MessageLogger::logMessage("Compare start %d end %d\n", (int)startDelta, (int)endDelta);

		
	}
	//tfSelection.range->ShiftStart
    // insert the text
    // use SetText here instead of InsertTextAtSelection because a composition has already been started
    // Don't allow to the app to adjust the insertion point inside the composition
	if (fCovered && pCompositionRange->SetText(ec, 0, convertedContext.c_str(),
		static_cast<LONG>(convertedContext.length())) == S_OK)
	{
		// update the selection, we'll make it an insertion point just past
		// the inserted text.
		tfSelection.range->ShiftEndToRange(ec, pCompositionRange, TF_ANCHOR_END);
		tfSelection.range->Collapse(ec, TF_ANCHOR_END);
		mpContext->SetSelection(ec, 1, &tfSelection);
	}
	// hack to reset after space
	if (convertedContext.length() && convertedContext[convertedContext.length()-1] == 0x20 && pComposition)
	{
		pComposition->EndComposition(ec);
		mpTextService->setComposition(NULL);// includes Release
		pComposition = NULL;
	}
	ITfRangeBackup * pRangeClone = NULL;
	if (mpContext->CreateRangeBackup(ec, pCompositionRange, &pRangeClone) == S_OK)
	{
		mpTextService->setCompositionRange(pRangeClone);
	}
	// end the composition now or later?
	//pComposition->EndComposition(ec);
	//pComposition->Release();
	pCompositionRange->Release();
	tfSelection.range->Release();
    return S_OK;
}

STDAPI EkayaEndContextSession::DoEditSession(TfEditCookie ec)
{
	HRESULT hr = S_OK;
	if (mpTextService->getComposition())
	{
		hr = mpTextService->getComposition()->EndComposition(ec);
		mpTextService->setComposition(NULL);// releases composition
	}
	return hr;
}
