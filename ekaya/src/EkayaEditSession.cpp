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
	//ITfComposition *pComposition = mpTextService->getComposition();
	ITfInsertAtSelection *pInsertAtSelection = NULL;

#if 0
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
		

		ITfContextComposition *pContextComposition = NULL;
		

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
#endif

	std::pair<size_t,size_t> keyResult;
	std::wstring convertedContext;
	size_t oldContextPos = 0;
	if (mwParam == 0)
	{
		keyResult.first = mpTextService->getTextContext().length();
		keyResult.second = keyResult.first + mData.length();
		oldContextPos = keyResult.first;
		convertedContext = mpTextService->getTextContext() + mData;
	}
	else
	{
		std::basic_string<Utf32>context = UtfConversion::convertUtf16ToUtf32(mpTextService->getTextContext());
		oldContextPos = context.length();
		keyResult = keyboard->processKey(static_cast<long>(mwParam), context, static_cast<int>(context.length()));
		MessageLogger::logMessage("processKey %d %d orig %d\n", (int)keyResult.first, (int)keyResult.second, (int)oldContextPos);

		// convert context back to UTF16
		convertedContext = UtfConversion::convertUtf32ToUtf16(context);
	}
	// first, test where a keystroke would go in the document if an insert is done
    if (mpContext->GetSelection(ec, TF_DEFAULT_SELECTION, 1, &tfSelection, &cFetched) != S_OK || cFetched != 1)
	{
		return S_FALSE;
	}
	
	long delCount = static_cast<long>(oldContextPos - keyResult.first);
	// try to shift start
	if (delCount > 0)
	{
		long actual = 0;
		ITfRange * shiftedRange = NULL;
		if (tfSelection.range->Clone(&shiftedRange) == S_OK)
		{
			MessageLogger::logMessage("Cloned range\n");
			ITfRange * docStart = NULL;
			mpContext->GetStart(ec, &docStart);
			if (docStart)
			{
				TF_HALTCOND halt;			
				halt.pHaltRange = docStart;
				halt.aHaltPos = TF_ANCHOR_START;
				halt.dwFlags = 0;
				if (shiftedRange->ShiftStart(ec, -delCount, &actual, &halt) == S_OK)
				{
					MessageLogger::logMessage("Shifted range by %d / %d\n", actual, -delCount);
					tfSelection.range->Release();
					tfSelection.range = shiftedRange;
					if (mpContext->SetSelection(ec, 1, &tfSelection) == S_OK)
					{
						delCount += actual;
						MessageLogger::logMessage("Set selection\n");
					}
				}
				docStart->Release();
				docStart = NULL;
			}
		}
	}
	
	// shift on selection doesn't work
	if (delCount > 0)
	{
			int delCount = static_cast<int>(oldContextPos - keyResult.first);
			
			const int MAX_DEL = 2;

			INPUT delInput[MAX_DEL];
			delInput[0].type = INPUT_KEYBOARD;
			delInput[0].ki.wVk = VK_BACK;
			delInput[0].ki.wScan = 0;
			delInput[0].ki.dwExtraInfo = 0;
			delInput[0].ki.dwFlags = 0;
			delInput[0].ki.time = 0;
			// backspace key's can't be eaten by us or they won't work
			// we need an event after the backspace has finished, so send a second, dummy key
			// which we will eat and continue processing from there
			delInput[1] = delInput[0];
			delInput[1].ki.wVk = EkayaInputProcessor::DUMMY_KEY;

			mpTextService->setTextContext(convertedContext.substr(0, keyResult.first));
			mpTextService->setPendingData(delCount, convertedContext.substr(keyResult.first, keyResult.second - keyResult.first));
			// SentInput really works, send deletes one at a time with dummy key so we get feedback
			SendInput(2, delInput, sizeof(INPUT));
			MessageLogger::logMessage("pending %d delete\n", delCount);
			tfSelection.range->Release();
			return S_OK;

	}

	// ITfRangeACP just returns 0 :-(

	// A special interface is required to insert text at the selection
	if (mpContext->QueryInterface(IID_ITfInsertAtSelection, (void **)&pInsertAtSelection) != S_OK)
	{
		tfSelection.range->Release();
		return S_FALSE;
	}
	// insert the text
	ITfRange *pRangeInsert = NULL;
	if (pInsertAtSelection->InsertTextAtSelection(ec, TF_IAS_QUERYONLY, NULL, 0, &pRangeInsert) != S_OK)
	{
		MessageLogger::logMessage("No insert range\n");
		tfSelection.range->Release();
		pInsertAtSelection->Release();
		return S_FALSE;
	}
	

	// setting backspace characters doesn't work

    // insert the text
    // use SetText here instead of InsertTextAtSelection because a composition has already been started
    // Don't allow to the app to adjust the insertion point inside the composition
	if (fCovered && pRangeInsert->SetText(ec, 0, convertedContext.c_str() + keyResult.first,
		static_cast<LONG>(keyResult.second - keyResult.first)) == S_OK)
	{
		// update the selection, we'll make it an insertion point just past
		// the inserted text.
		tfSelection.range->ShiftEndToRange(ec, pCompositionRange, TF_ANCHOR_END);
		tfSelection.range->Collapse(ec, TF_ANCHOR_END);
		mpContext->SetSelection(ec, 1, &tfSelection);
		if (convertedContext.length() < EkayaInputProcessor::MAX_CONTEXT)
			mpTextService->setTextContext(convertedContext);
		else
		{
			mpTextService->setTextContext(convertedContext.substr(convertedContext.length()
				- EkayaInputProcessor::MAX_CONTEXT, EkayaInputProcessor::MAX_CONTEXT));
		}
		for (size_t i = 0; i < convertedContext.length(); i++)
			MessageLogger::logMessage(" %x", (int)convertedContext[i]);
		MessageLogger::logMessage("\n");
	}
	else
	{
		MessageLogger::logMessage("Insert failed\n");
	}
	// hack to reset after space
	//if (convertedContext.length() && convertedContext[convertedContext.length()-1] == 0x20 && pComposition)
	//{
	//	pComposition->EndComposition(ec);
	//	mpTextService->setComposition(NULL);// includes Release
	//	pComposition = NULL;
	//}
	if (convertedContext.length() && convertedContext[convertedContext.length()-1] == 0x20)
	{
		mpTextService->setTextContext(std::wstring(L""));
	}
	//ITfRangeBackup * pRangeClone = NULL;
	//if (mpContext->CreateRangeBackup(ec, pCompositionRange, &pRangeClone) == S_OK)
	//{
	//	mpTextService->setCompositionRange(pRangeClone);
	//}
	
	//pCompositionRange->Release();
	tfSelection.range->Release();
	pRangeInsert->Release();
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

STDAPI EkayaSetContextSession::DoEditSession(TfEditCookie ec)
{
	TF_SELECTION tfSelection;
	ULONG cFetched;
	if (mpContext->GetSelection(ec, TF_DEFAULT_SELECTION, 1, &tfSelection, &cFetched) != S_OK || cFetched != 1)
	{
		return S_FALSE;
	}
	wchar_t buffer[128];
	tfSelection.range->GetText(ec, 0, buffer, 128, &cFetched);
	if (cFetched > 0)
	{
		for (size_t i = 0; i < cFetched; i++)
			MessageLogger::logMessage("%x ", buffer[i]);
		MessageLogger::logMessage("\nSelection has %d characters\n", (int)cFetched);
	}
	tfSelection.range->Release();
	return S_OK;
}

/*
ITfMouseTracker * pMouseTracker = NULL;
	DWORD mouseCookie;
	if (mpContext->QueryInterface(IID_ITfMouseTracker, (LPVOID*)&pMouseTracker) == S_OK)
	{
		if (mpTextService->getMouseCookie() != TF_INVALID_COOKIE)
		{
			pMouseTracker->UnadviseMouseSink(mpTextService->getMouseCookie());
		}
		if (pMouseTracker->AdviseMouseSink(pCompositionRange, mpTextService, &mouseCookie) == S_OK)
		{
			mpTextService->setMouseCookie(mouseCookie);
		}
		else
		{
			mpTextService->setMouseCookie(TF_INVALID_COOKIE);
		}
	}

	ITfRange * docStart;
		ITfRange * docEnd;
		baseContext->GetStart(ec, &docStart);
		baseContext->GetEnd(ec, &docEnd);
		LONG startDelta, endDelta;
		pCompositionRange->CompareStart(ec, docStart, TF_ANCHOR_START, &startDelta);
		pCompositionRange->CompareEnd(ec, docStart, TF_ANCHOR_END, &endDelta);
		MessageLogger::logMessage("Compare start %d end %d\n", (int)startDelta, (int)endDelta);


*/
