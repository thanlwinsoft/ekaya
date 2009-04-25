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
#include <assert.h>
#include <limits>
#include <ole2.h>
#include <olectl.h>

#include "UtfConversion.h"

#include "Ekaya.h"
#include "EkayaKeyboard.h"
#include "EkayaLangBarButton.h"
#include "MessageLogger.h"

const DWORD TEXTSERVICE_LANGBARITEMSINK_COOKIE = 0xae4e6c21;

EkayaLangBarButton::EkayaLangBarButton(EkayaInputProcessor *pTextService)
: mpLangBarItemSink(NULL), mRefCount(0)
{
    DllAddRef();

    //
    // initialize TF_LANGBARITEMINFO structure.
    //
    mtfLangBarItemInfo.clsidService = CLSID_EKAYA_SERVICE;    // This LangBarItem belongs to this TextService.
    mtfLangBarItemInfo.guidItem = GUID_LANGBAR_BUTTON;   // GUID of this LangBarItem.
    mtfLangBarItemInfo.dwStyle = TF_LBI_STYLE_BTN_MENU;      // This LangBar is a button type with a menu.
    mtfLangBarItemInfo.ulSort = 0;                           // The position of this LangBar Item is not specified.
	std::wstring langBarDesc = pTextService->getMessage(L"EkayaDesc");
	size_t descLen = sizeof(mtfLangBarItemInfo.szDescription)/sizeof(mtfLangBarItemInfo.szDescription[0]);
	size_t len = min(langBarDesc.length(), descLen);
	wcsncpy_s(mtfLangBarItemInfo.szDescription, langBarDesc.c_str(), len);

    mpTextService = pTextService;
	if (mpTextService)
	    mpTextService->AddRef();

}

//+---------------------------------------------------------------------------
//
// dtor
//
//----------------------------------------------------------------------------

EkayaLangBarButton::~EkayaLangBarButton()
{
    DllRelease();
	if (mpTextService)
	    mpTextService->Release();
}

//+---------------------------------------------------------------------------
//
// QueryInterface
//
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::QueryInterface(REFIID riid, void **ppvObj)
{
    if (ppvObj == NULL)
        return E_INVALIDARG;

    *ppvObj = NULL;

    if (IsEqualIID(riid, IID_IUnknown) ||
        IsEqualIID(riid, IID_ITfLangBarItem) ||
        IsEqualIID(riid, IID_ITfLangBarItemButton))
    {
        *ppvObj = (ITfLangBarItemButton *)this;
    }
    else if (IsEqualIID(riid, IID_ITfSource))
    {
        *ppvObj = (ITfSource *)this;
    }

    if (*ppvObj)
    {
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}


//+---------------------------------------------------------------------------
//
// AddRef
//
//----------------------------------------------------------------------------

STDAPI_(ULONG) EkayaLangBarButton::AddRef()
{
    return ++mRefCount;
}

//+---------------------------------------------------------------------------
//
// Release
//
//----------------------------------------------------------------------------

STDAPI_(ULONG) EkayaLangBarButton::Release()
{
    long cr = --mRefCount;

    assert(mRefCount >= 0);

    if (mRefCount == 0)
    {
        delete this;
    }

    return (ULONG)cr;
}

//+---------------------------------------------------------------------------
//
// GetInfo
//
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::GetInfo(TF_LANGBARITEMINFO *pInfo)
{
    *pInfo = mtfLangBarItemInfo;
    return S_OK;
}

//+---------------------------------------------------------------------------
//
// GetStatus
//
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::GetStatus(DWORD *pdwStatus)
{
    *pdwStatus = 0;
    return S_OK;
}

//+---------------------------------------------------------------------------
//
// Show
//
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::Show(BOOL fShow)
{
    return E_NOTIMPL;
}

//+---------------------------------------------------------------------------
//
// GetTooltipString
//
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::GetTooltipString(BSTR *pbstrToolTip)
{
	std::wstring langbarDesc = mpTextService->getMessage(L"EkayaDesc");
	*pbstrToolTip = SysAllocString(langbarDesc.c_str());

    return (*pbstrToolTip == NULL) ? E_OUTOFMEMORY : S_OK;
}

//+---------------------------------------------------------------------------
//
// OnClick
//
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::OnClick(TfLBIClick click, POINT pt, const RECT *prcArea)
{
    return S_OK;
}

//+---------------------------------------------------------------------------
//
// InitMenu
//
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::InitMenu(ITfMenu *pMenu)
{

    // 
    // Add the keyboard open close item.
    // 
    DWORD dwFlags = 0;
    if (mpTextService->isKeyboardDisabled())
        dwFlags |= TF_LBMENUF_GRAYED;
    else if (mpTextService->isKeyboardOpen())
        dwFlags |= TF_LBMENUF_CHECKED;

	std::wstring desc = mpTextService->getMessage(L"Disable");

    pMenu->AddMenuItem(0,
                       dwFlags, 
                       NULL, 
                       NULL, 
                       desc.c_str(), 
                       (ULONG)desc.length(), 
                       NULL);

	for (size_t i = 0; i < mpTextService->getKeyboards().size(); i++)
	{
		dwFlags = 0;
		if (i == mpTextService->getActiveKeyboard()) dwFlags |= TF_LBMENUF_CHECKED;
		std::basic_string<Utf32> keyboardDesc = mpTextService->getKeyboards()[i]->getDescription();

		std::wstring keyboardDesc16 = UtfConversion::convertUtf32ToUtf16(keyboardDesc);
		dwFlags = 0;
		if (mpTextService->getActiveKeyboard() == i)
		{
			dwFlags |= TF_LBMENUF_CHECKED;
		}
		pMenu->AddMenuItem((UINT)(i+1),
                       dwFlags, 
                       NULL, 
                       NULL, 
					   keyboardDesc16.c_str(), 
					   (ULONG)keyboardDesc16.length(),
                       NULL);
	}

    return S_OK;
}

//+---------------------------------------------------------------------------
//
// OnMenuSelect
//
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::OnMenuSelect(UINT wID)
{
    bool fOpen;
	MessageLogger::logMessage("OnMenuSelect %d", wID);
	fOpen = mpTextService->isKeyboardOpen();
    //
    // This is callback when the menu item is selected.
    //
    switch (wID)
    {
        case 0:
            mpTextService->setKeyboardOpen(fOpen ? false : true);
            break;
		default:
			if (!fOpen) mpTextService->setKeyboardOpen(true);
			if (wID > 0 && wID <= mpTextService->getKeyboards().size())
			{
				mpTextService->setActiveKeyboard(wID - 1);
			}
    }

    return S_OK;
}

//+---------------------------------------------------------------------------
//
// GetIcon
//
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::GetIcon(HICON *phIcon)
{
    *phIcon = (HICON)LoadImage(g_hInst, TEXT("IDI_TEXTSERVICE"), IMAGE_ICON, 16, 16, 0);
 
    return (*phIcon != NULL) ? S_OK : E_FAIL;
}

//+---------------------------------------------------------------------------
//
// GetText
//
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::GetText(BSTR *pbstrText)
{
	std::wstring desc = mpTextService->getMessage(L"EkayaDesc");
	*pbstrText = SysAllocString(desc.c_str());

    return (*pbstrText == NULL) ? E_OUTOFMEMORY : S_OK;
}

//+---------------------------------------------------------------------------
//
// AdviseSink
//
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::AdviseSink(REFIID riid, IUnknown *punk, DWORD *pdwCookie)
{
    //
    // Allow only ITfLangBarItemSink interface.
    //
    if (!IsEqualIID(IID_ITfLangBarItemSink, riid))
        return CONNECT_E_CANNOTCONNECT;

    //
    // Support only one sink.
    //
    if (mpLangBarItemSink != NULL)
        return CONNECT_E_ADVISELIMIT;

    //
    // Query the ITfLangBarItemSink interface and store it into mpLangBarItemSink.
    //
    if (punk->QueryInterface(IID_ITfLangBarItemSink, (void **)&mpLangBarItemSink) != S_OK)
    {
        mpLangBarItemSink = NULL;
        return E_NOINTERFACE;
    }

    //
    // return our cookie.
    //
    *pdwCookie = TEXTSERVICE_LANGBARITEMSINK_COOKIE;
    return S_OK;
}

//+---------------------------------------------------------------------------
//
// UnadviseSink
//
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::UnadviseSink(DWORD dwCookie)
{
    // 
    // Check the given cookie.
    // 
    if (dwCookie != TEXTSERVICE_LANGBARITEMSINK_COOKIE)
        return CONNECT_E_NOCONNECTION;

    //
    // If there is nno connected sink, just fail.
    //
    if (mpLangBarItemSink == NULL)
        return CONNECT_E_NOCONNECTION;

    mpLangBarItemSink->Release();
    mpLangBarItemSink = NULL;

    return S_OK;
}
