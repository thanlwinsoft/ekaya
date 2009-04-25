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
 * License along with the KMFL library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 *
 */
#ifndef EkayaInputProcessor_h
#define EkayaInputProcessor_h

#include <msctf.h>

#include <string>
#include <vector>
#include <bitset>


class EkayaLangBarButton;
class EkayaKeyboard;
class EkayaKeyboardFactory;

class EkayaInputProcessor : public ITfTextInputProcessor,
                     public ITfThreadMgrEventSink,
                     public ITfTextEditSink,
                     public ITfKeyEventSink,
					 public ITfCompositionSink
{
public:
	enum
	{
		KEY_SHIFT,
		KEY_CTRL
	};
    EkayaInputProcessor();
    ~EkayaInputProcessor();

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    // ITfTextInputProcessor
    STDMETHODIMP Activate(ITfThreadMgr *pThreadMgr, TfClientId tfClientId);
    STDMETHODIMP Deactivate();

    // ITfThreadMgrEventSink
    STDMETHODIMP OnInitDocumentMgr(ITfDocumentMgr *pDocMgr);
    STDMETHODIMP OnUninitDocumentMgr(ITfDocumentMgr *pDocMgr);
    STDMETHODIMP OnSetFocus(ITfDocumentMgr *pDocMgrFocus, ITfDocumentMgr *pDocMgrPrevFocus);
    STDMETHODIMP OnPushContext(ITfContext *pContext);
    STDMETHODIMP OnPopContext(ITfContext *pContext);

    // ITfTextEditSink
    STDMETHODIMP OnEndEdit(ITfContext *pContext, TfEditCookie ecReadOnly, ITfEditRecord *pEditRecord);

    // ITfKeyEventSink
    STDMETHODIMP OnSetFocus(BOOL fForeground);
    STDMETHODIMP OnTestKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
    STDMETHODIMP OnKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
    STDMETHODIMP OnTestKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
    STDMETHODIMP OnKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
    STDMETHODIMP OnPreservedKey(ITfContext *pContext, REFGUID rguid, BOOL *pfEaten);
	STDMETHODIMP OnCompositionTerminated(TfEditCookie,ITfComposition *);

    // factory callback
    static HRESULT CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObj);

    ITfThreadMgr *getThreadMgr() { return mpThreadMgr; }

    // utility functions
    bool isKeyboardDisabled();
    bool isKeyboardOpen();
    HRESULT setKeyboardOpen(bool fOpen);
	void setActiveKeyboard(int keyboardIndex);
	int getActiveKeyboard() { return mActiveKeyboard; }
	const std::wstring getMessage(const wchar_t * defaultMessage);
	const std::vector <EkayaKeyboard*> & getKeyboards() { return mKeyboards; }

	void logMessage(const char * msg);
	void logMessage(const char * msg, int param);
	void logMessage(const char * msg, int paramA, int paramB);
	void logMessage(const char * msg, int paramA, int paramB, int paramC);
	void setComposition(ITfComposition * composition) { mpComposition = composition; }
private:
	HRESULT setTextEditSink(ITfDocumentMgr *pDocMgrFocus);
	bool ignoreKey(WPARAM code);
	void initKeyboards();
	bool mOpen;
	bool mDisabled;
	long mRefCount;
	int mActiveKeyboard;
	std::bitset<4> mKeyState;
	TfClientId mClientId;
	DWORD mThreadEventCookie;
	DWORD mEditEventCookie;
	ITfThreadMgr * mpThreadMgr;
	ITfContext * mpTextEditSinkContext;
	EkayaLangBarButton * mpLangBarButton;
	std::vector <EkayaKeyboardFactory*> mKeyboardFactories;
	std::vector <EkayaKeyboard*> mKeyboards;
	std::wstring mContext;
	ITfComposition * mpComposition;
};

#endif
