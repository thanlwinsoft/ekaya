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
					 public ITfCompositionSink,
					 public ITfMouseSink,
					 public ITfTextLayoutSink
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

	// ITfTextLayoutSink
	STDMETHODIMP OnLayoutChange(ITfContext *pContext, TfLayoutCode lcode, ITfContextView *pContextView);

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
	STDMETHODIMP OnMouseEvent(ULONG uEdge,
                         ULONG uQuadrant,
                         DWORD dwBtnStatus,
                         BOOL *pfEaten);
	virtual HRESULT STDMETHODCALLTYPE OnStartComposition( 
            /* [in] */ __RPC__in_opt ITfCompositionView *pComposition,
            /* [out] */ __RPC__out BOOL *pfOk);
        
    virtual HRESULT STDMETHODCALLTYPE OnUpdateComposition( 
            /* [in] */ __RPC__in_opt ITfCompositionView *pComposition,
            /* [in] */ __RPC__in_opt ITfRange *pRangeNew);
        
    virtual HRESULT STDMETHODCALLTYPE OnEndComposition( 
            /* [in] */ __RPC__in_opt ITfCompositionView *pComposition);
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

	void setComposition(ITfComposition * composition);
	ITfComposition * getComposition();
	void setCompositionRange(ITfRangeBackup * range) 
	{ if (mpCompositionRange) mpCompositionRange->Release(); mpCompositionRange = range; };
	ITfRangeBackup * getCompositionRange(void) { return mpCompositionRange; }
	ITfThreadMgr * getThreadManager() { return mpThreadMgr; };
	
private:
	HRESULT setTextEditSink(ITfDocumentMgr *pDocMgrFocus);
	bool ignoreKey(WPARAM code);
	void initKeyboards();
	bool mOpen;
	bool mDisabled;
	bool mRawCodes;
	long mRefCount;
	int mActiveKeyboard;
	std::bitset<4> mKeyState;
	TfClientId mClientId;
	DWORD mThreadEventCookie;
	DWORD mEditEventCookie;
	DWORD mMouseCookie;
	DWORD mTextLayoutCookie;
	DWORD mContextOwnerCookie;
	ULONG_PTR mGdiToken;
	ITfThreadMgr * mpThreadMgr;
	ITfContext * mpTextEditSinkContext;
	EkayaLangBarButton * mpLangBarButton;
	std::vector <EkayaKeyboardFactory*> mKeyboardFactories;
	std::vector <EkayaKeyboard*> mKeyboards;
	std::wstring mContext;
	ITfComposition * mpComposition;
	ITfRangeBackup * mpCompositionRange;
};

#endif
