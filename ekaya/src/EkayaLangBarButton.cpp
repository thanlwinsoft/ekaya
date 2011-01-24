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
#include <process.h>

#include "UtfConversion.h"

#include "Ekaya.h"
#include "EkayaKeyboard.h"
#include "EkayaLangBarButton.h"
#include "MessageLogger.h"
#include "resource.h"

using namespace EKAYA_NS;

const DWORD TEXTSERVICE_LANGBARITEMSINK_COOKIE = 0xae4e6c21;

EkayaLangBarButton::EkayaLangBarButton(EkayaInputProcessor *pTextService)
: mpLangBarItemSink(NULL), mRefCount(0)
{
    DllAddRef();
    // initialize TF_LANGBARITEMINFO structure.
	// This LangBarItem belongs to this TextService.
    mtfLangBarItemInfo.clsidService = CLSID_EKAYA_SERVICE;
	// GUID of this LangBarItem.
    mtfLangBarItemInfo.guidItem = GUID_LANGBAR_BUTTON;
	// This LangBar is a button type with a menu.
    mtfLangBarItemInfo.dwStyle = TF_LBI_STYLE_BTN_MENU;
	// The position of this LangBar Item is not specified.
    mtfLangBarItemInfo.ulSort = 0;                           
	std::wstring langBarDesc = pTextService->getMessage(IDS_KEYBOARDS, L"Keyboards");
	size_t descLen = sizeof(mtfLangBarItemInfo.szDescription)/sizeof(mtfLangBarItemInfo.szDescription[0]);
	size_t len = min(langBarDesc.length(), descLen);
	wcsncpy_s(mtfLangBarItemInfo.szDescription, langBarDesc.c_str(), len);

    mpTextService = pTextService;
	if (mpTextService)
	{
	    mpTextService->AddRef();
	}
	// load icons
	for (size_t i = 0; i < mpTextService->getKeyboards().size(); i++)
	{
	    std::basic_string<Utf32> iconFileName = mpTextService->getKeyboards()[i]->getIconFileName();
	    if (iconFileName.length() > 0)
	    {
	        std::wstring wName = UtfConversion::convertUtf32ToUtf16(iconFileName);
			FILE * test = NULL;
			fopen_s(&test, UtfConversion::convertUtf32ToUtf8(iconFileName).c_str(), "r");
			bool exists = false;
			if (test)
			{
				fclose(test);
				MessageLogger::logMessage("Icon exists\n");
				exists = true;
			}
			Gdiplus::Bitmap * bm = Gdiplus::Bitmap::FromFile(wName.c_str(), TRUE);
	        if (bm)
	        {
	            mIcons.push_back(bm);
	        }
			else
			{
				mIcons.push_back(NULL);
			}
	    }
		else mIcons.push_back(NULL);
		std::basic_string<Utf32> helpFileName = mpTextService->getKeyboards()[i]->getHelpFileName();
	    if (helpFileName.length() > 0)
	    {
			std::wstring wName = UtfConversion::convertUtf32ToUtf16(helpFileName);
			mHelpHtml.push_back(wName);
		}
		else mHelpHtml.push_back(L"");
	}
}

//+---------------------------------------------------------------------------
// dtor
//----------------------------------------------------------------------------

EkayaLangBarButton::~EkayaLangBarButton()
{
    DllRelease();
	if (mpTextService)
	{
	    mpTextService->Release();
		mpTextService = NULL;
	}
	if (mIcons.size())
	{
		std::vector<Gdiplus::Bitmap *>::iterator i = mIcons.begin();
		while (i != mIcons.end())
		{
			delete *i;
			++i;
		}
	}
}

//+---------------------------------------------------------------------------
// QueryInterface
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
// AddRef
//----------------------------------------------------------------------------

STDAPI_(ULONG) EkayaLangBarButton::AddRef()
{
    return ++mRefCount;
}

//+---------------------------------------------------------------------------
// Release
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
// GetInfo
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::GetInfo(TF_LANGBARITEMINFO *pInfo)
{
    *pInfo = mtfLangBarItemInfo;
    return S_OK;
}

//+---------------------------------------------------------------------------
// GetStatus
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::GetStatus(DWORD *pdwStatus)
{
    *pdwStatus = 0;
    return S_OK;
}

//+---------------------------------------------------------------------------
// Show
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::Show(BOOL fShow)
{
	MessageLogger::logMessage("EkayaLangBarButton::Show %d\n", (int)fShow);
    return E_NOTIMPL;
}

//+---------------------------------------------------------------------------
// GetTooltipString
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::GetTooltipString(BSTR *pbstrToolTip)
{
	std::wstring langbarDesc = mpTextService->getMessage(IDS_KEYBOARDS_DESC, L"EkayaDesc");
	*pbstrToolTip = SysAllocString(langbarDesc.c_str());

    return (*pbstrToolTip == NULL) ? E_OUTOFMEMORY : S_OK;
}

//+---------------------------------------------------------------------------
// OnClick
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::OnClick(TfLBIClick /*click*/, POINT /*pt*/, const RECT * /*prcArea*/)
{
    return S_OK;
}

//+---------------------------------------------------------------------------
// InitMenu
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::InitMenu(ITfMenu *pMenu)
{
	if (pMenu == NULL)
	{
		return E_FAIL;
	}
    DWORD dwFlags = 0;

	std::wstring desc = mpTextService->getMessage(IDS_HELP, L"Help");
	dwFlags |= TF_LBMENUF_SUBMENU;
	ITfMenu * pHelpMenu = NULL;
    pMenu->AddMenuItem(0,
                       dwFlags, 
                       NULL, 
                       NULL, 
                       desc.c_str(), 
                       (ULONG)desc.length(), 
                       &pHelpMenu);

	HICON helpIcon = (HICON)LoadImageW(g_hInst, MAKEINTRESOURCEW(IDI_EKAYA_HELP), IMAGE_ICON, 16, 16, 0);
	HBITMAP hHelpBitmap = NULL;
	Gdiplus::Bitmap * pHelpBitmap = Gdiplus::Bitmap::FromHICON(helpIcon);
	if (pHelpBitmap) pHelpBitmap->GetHBITMAP(Gdiplus::Color::Transparent, &hHelpBitmap);
	if (pHelpMenu)
	{
		std::wstring desc = mpTextService->getMessage(IDS_EKAYA_HELP, L"EkayaHelp");
		pHelpMenu->AddMenuItem(HELP_BASE_ID,
                       0, 
                       hHelpBitmap, 
                       NULL, 
                       desc.c_str(), 
                       (ULONG)desc.length(), 
                       NULL);
	}
	delete pHelpBitmap;

    //bool loadIcons = (mIcons.size())? false : true;
	for (size_t i = 0; i < mpTextService->getKeyboards().size(); i++)
	{
		dwFlags = 0;
		if (static_cast<int>(i) == mpTextService->getActiveKeyboard())
            dwFlags |= TF_LBMENUF_CHECKED;
		std::basic_string<Utf32> keyboardDesc = mpTextService->getKeyboards()[i]->getDescription();

		std::wstring keyboardDesc16 = UtfConversion::convertUtf32ToUtf16(keyboardDesc);

		// try to load an icon
        HBITMAP hIcon = NULL;
		Gdiplus::Bitmap * bm = mIcons[i];
		if (!bm || bm->GetHBITMAP(Gdiplus::Color::Transparent, &hIcon) != S_OK)
        {
            hIcon = NULL;
		}
		// add the menu item
		pMenu->AddMenuItem((UINT)(i+1),
                       dwFlags, 
                       hIcon, 
                       NULL, 
					   keyboardDesc16.c_str(), 
					   (ULONG)keyboardDesc16.length(),
                       NULL);
		if (pHelpMenu)
		{
			dwFlags = 0;
			if (mHelpHtml[i].length() == 0)
			{
				dwFlags |= TF_LBMENUF_GRAYED;
			}
			pHelpMenu->AddMenuItem((UINT)(HELP_BASE_ID + i+1),
                       dwFlags, 
                       hIcon, 
                       NULL, 
					   keyboardDesc16.c_str(), 
					   (ULONG)keyboardDesc16.length(),
                       NULL);
		}
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
	MessageLogger::logMessage("OnMenuSelect %d", (int)wID);
	fOpen = mpTextService->isKeyboardOpen();
	size_t requiredSize;
    getenv_s( &requiredSize, NULL, 0, "APPDATA");
	char * appDir = new char[requiredSize];
	std::wstring docFile = L"";
	std::string doc;
    // This is callback when the menu item is selected.
    //
    switch (wID)
    {
        case 0:
            break;
		case HELP_BASE_ID:
			if (appDir)
			{
				getenv_s( &requiredSize, appDir, requiredSize, "ProgramFiles" );
				doc = std::string("\"") + std::string(appDir) + EkayaInputProcessor::EKAYA_DIR + "\\doc\\ekaya.html\"";
				docFile = UtfConversion::convertUtf8ToUtf16(doc);
			}
			break;
		default:
			if (!fOpen) mpTextService->setKeyboardOpen(true);
			if (wID > 0 && wID <= mpTextService->getKeyboards().size())
			{
				mpTextService->setActiveKeyboard(wID - 1);
			}
			else if (wID > HELP_BASE_ID && wID <= HELP_BASE_ID + mpTextService->getKeyboards().size())
			{
				assert(mHelpHtml.size() == mpTextService->getKeyboards().size());
				docFile = mHelpHtml[wID - HELP_BASE_ID - 1];
			}
    }
	
	ITfContextView * pContextView = NULL;
	if (docFile.length() > 0 && mpTextService->getTextEditSinkContext() &&
		mpTextService->getTextEditSinkContext()->GetActiveView(&pContextView) == S_OK)
	{
		HWND hWnd = NULL;
		pContextView->GetWnd(&hWnd);
		HINSTANCE hi = ShellExecuteW(hWnd, L"open", docFile.c_str(), NULL, NULL, SW_SHOWNORMAL);
		if (hi != (HINSTANCE)32)
		{
			MessageLogger::logMessage(L"ShellExecute failed %ld %ls\n", hi, docFile.c_str());
		}
	}

	delete [] appDir;
    return S_OK;
}

//+---------------------------------------------------------------------------
//
// GetIcon
//
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::GetIcon(HICON *phIcon)
{
    *phIcon = NULL;
    if (mpTextService && mpTextService->isKeyboardOpen() &&
        mpTextService->getActiveKeyboard() > -1 && mIcons.size() > 0)
    {
        if ((mpTextService->getActiveKeyboard() >= (signed)mIcons.size()) ||
            (mIcons[mpTextService->getActiveKeyboard()]->GetHICON(phIcon) != S_OK))
            *phIcon = NULL;
        if (*phIcon == NULL)
        {
            *phIcon = (HICON)LoadImageW(g_hInst, MAKEINTRESOURCEW(IDI_EKAYA_KEYBOARD), IMAGE_ICON, 16, 16, 0);
        }
    }
    // use default icon
    if (*phIcon == NULL)
    {
        *phIcon = (HICON)LoadImageW(g_hInst, MAKEINTRESOURCEW(IDI_EKAYA_NO_KEYBOARD), IMAGE_ICON, 16, 16, 0);
    }

    return (*phIcon != NULL) ? S_OK : E_FAIL;
}

//+---------------------------------------------------------------------------
//
// GetText
//
//----------------------------------------------------------------------------

STDAPI EkayaLangBarButton::GetText(BSTR *pbstrText)
{
	std::wstring desc = mpTextService->getMessage(IDS_KEYBOARDS, L"Keyboards");
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
