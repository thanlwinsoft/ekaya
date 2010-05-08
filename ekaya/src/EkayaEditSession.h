/*
 * Copyright (C) 2009 ThanLwinSoft.org
 *
 * The Ekaya library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The Ekaya library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with the Ekaya library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 *
 */
#ifndef EkayaEditSession_h
#define EkayaEditSession_h

#include <assert.h>
#include <windows.h>
#include "EkayaInputProcessor.h"

class EkayaBaseEditSession : public ITfEditSession
{
public:
	EkayaBaseEditSession(EkayaInputProcessor *pTextService, ITfContext *pContext)
	{
		mcRef = 1;
        mpContext = pContext;
        mpContext->AddRef();

        mpTextService = pTextService;
        mpTextService->AddRef();
	}
	virtual ~EkayaBaseEditSession()
    {
        mpContext->Release();
        mpTextService->Release();
    }

	// IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj)
    {
        if (ppvObj == NULL)
            return E_INVALIDARG;

        *ppvObj = NULL;

        if (IsEqualIID(riid, IID_IUnknown) ||
            IsEqualIID(riid, IID_ITfEditSession))
        {
            *ppvObj = (ITfLangBarItemButton *)this;
        }

        if (*ppvObj)
        {
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    // ITfEditSession
    STDMETHODIMP DoEditSession(TfEditCookie ec) = 0;

	STDMETHODIMP_(ULONG) AddRef(void)
    {
        return ++mcRef;
    }
    STDMETHODIMP_(ULONG) Release(void)
    {
        long cr = --mcRef;
        assert(mcRef >= 0);
        if (mcRef == 0)
        {
            delete this;
        }
        return cr;
    }
protected:
	ITfContext * mpContext;
	EkayaInputProcessor * mpTextService;
private:
	long mcRef;
};

class EkayaEditSession : public EkayaBaseEditSession
{
public:
    EkayaEditSession(EkayaInputProcessor *pTextService, ITfContext *pContext, WPARAM wParam)
		: EkayaBaseEditSession(pTextService, pContext)
    {
		mwParam = wParam;
    }
	EkayaEditSession(EkayaInputProcessor *pTextService, ITfContext *pContext, std::wstring data)
		: EkayaBaseEditSession(pTextService, pContext)
	{
		mwParam = 0;
		mData = data;
    }
	STDMETHODIMP DoEditSession(TfEditCookie ec);
private:
	std::wstring mData;
    WPARAM mwParam;
};

class EkayaEndContextSession : public EkayaBaseEditSession
{
public:
    EkayaEndContextSession(EkayaInputProcessor *pTextService, ITfContext *pContext, WPARAM wParam)
		: EkayaBaseEditSession(pTextService, pContext)
    {
		mwParam = wParam;
    }
	virtual ~EkayaEndContextSession()
    {
    }
	STDMETHODIMP DoEditSession(TfEditCookie ec);
private:
    WPARAM mwParam;
};

class EkayaSetContextSession : public EkayaBaseEditSession
{
public:
    EkayaSetContextSession(EkayaInputProcessor *pTextService, ITfContext *pContext)
		: EkayaBaseEditSession(pTextService, pContext)
    {
    }
	virtual ~EkayaSetContextSession()
    {
    }
	STDMETHODIMP DoEditSession(TfEditCookie ec);
private:
    WPARAM mwParam;
};

#endif
