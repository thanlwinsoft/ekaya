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

#include "EkayaEditSession.h"
#include "EkayaKeyboard.h"
#include "EkayaInputProcessor.h"
#include "UtfConversion.h"

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

	//ITfRange *pRangeComposition;
    //TF_SELECTION tfSelection;
    //ULONG cFetched;
    WCHAR ch = (WCHAR)(mwParam);
    bool fCovered = true;

	// TODO decide if we really need a composition, try without for now
    // Start the new compositon if there is no composition.
    //if (!_IsComposing())
    //    _StartComposition(pContext);

    //ch = (WCHAR)(mwParam - 'A' + 0x1000);
	ITfInsertAtSelection *pInsertAtSelection = NULL;
	// A special interface is required to insert text at the selection
    if (mpContext->QueryInterface(IID_ITfInsertAtSelection, (void **)&pInsertAtSelection) != S_OK)
    {
        return S_FALSE;
    }

	// insert the text
	ITfRange *pRangeInsert = NULL;
	ITfContextComposition *pContextComposition = NULL;
    if (pInsertAtSelection->InsertTextAtSelection(ec, TF_IAS_QUERYONLY, NULL, 0, &pRangeInsert) != S_OK)
    {
		pInsertAtSelection->Release();
        return S_FALSE;
    }

	// get an interface on the context to deal with compositions
    if (mpContext->QueryInterface(IID_ITfContextComposition, (void **)&pContextComposition) != S_OK)
    {
		pInsertAtSelection->Release();
        return S_FALSE;
    }
	
	LONG shift = 0;
	ITfComposition *pComposition = NULL;
	if ((pContextComposition->StartComposition(ec, pRangeInsert, mpTextService, &pComposition) == S_OK) && (pComposition != NULL))
    {
		/*if (pRangeInsert->ShiftStart(ec, -1, &shift, NULL) != S_OK)
		{
			pInsertAtSelection->Release();
			pRangeInsert->Release();
			pContextComposition->Release();
			return S_FALSE;
		}*/
	}
	else
	{
		pInsertAtSelection->Release();
		pContextComposition->Release();
		return S_FALSE;
	}
    // first, test where a keystroke would go in the document if an insert is done
    //if (mpContext->GetSelection(ec, TF_DEFAULT_SELECTION, 1, &tfSelection, &cFetched) != S_OK || cFetched != 1)
    //    return S_FALSE;

	ULONG contextLength = 0;
	const size_t MAX_CONTEXT_LEN = 16;
	WCHAR wBuffer[MAX_CONTEXT_LEN * 2];

	if (pRangeInsert->GetText(ec, 0, wBuffer, MAX_CONTEXT_LEN * 2, &contextLength) != S_OK)
	{
		pInsertAtSelection->Release();
			pRangeInsert->Release();
		return S_FALSE;
	}
	/*ITfRange * editRange = NULL;
	if (tfSelection.range->Clone(&editRange) != S_OK)
	{
		tfSelection.range->Release();
		return S_FALSE;
	}
	if (tfSelection.range->GetText(ec, 0, wBuffer, MAX_CONTEXT_LEN * 2, &contextLength) != S_OK)
		return S_FALSE;

	if (contextLength == 0)
	{
		TF_HALTCOND halt;
		halt.pHaltRange = NULL;
		halt.aHaltPos = TF_ANCHOR_START;
		halt.dwFlags = 0;
		LONG shift = 0;
		if (editRange->ShiftStart(ec, -1, &shift, NULL) != S_OK)
		{
			tfSelection.range->Release();
			editRange->Release();
			return S_FALSE;
		}
		BOOL noRegion = false;
		if (shift == 0)
		{
			if (editRange->ShiftStartRegion(ec, TF_SD_BACKWARD, &noRegion) != S_OK)
			{
				tfSelection.range->Release();
				editRange->Release();
			}
		}
		if (editRange->GetText(ec, 0, wBuffer, MAX_CONTEXT_LEN * 2, &contextLength) != S_OK)
			return S_FALSE;
	}*/

	std::basic_string<Utf32>context = UtfConversion::convertUtf16ToUtf32(std::wstring(wBuffer, contextLength));
	size_t oldContextPos = context.length();
	int keyResult = keyboard->processKey(static_cast<long>(mwParam), context, static_cast<int>(context.length()));

	
    // is the insertion point covered by a composition?
    //if (_pComposition->GetRange(&pRangeComposition) == S_OK)
    //{
    //    fCovered = IsRangeCovered(ec, tfSelection.range, pRangeComposition);
    //    pRangeComposition->Release();
    //}

	if (keyResult == oldContextPos)
	{
		// no change
		return S_FALSE;
	}
	// convert context back to UTF16
	std::wstring convertedContext = UtfConversion::convertUtf32ToUtf16(context);

	//tfSelection.range->ShiftStart
    // insert the text
    // use SetText here instead of InsertTextAtSelection because a composition has already been started
    // Don't allow to the app to adjust the insertion point inside the composition
	if (fCovered && pRangeInsert->SetText(ec, 0, convertedContext.c_str(),
		static_cast<LONG>(convertedContext.length())) == S_OK)
	{
		// update the selection, we'll make it an insertion point just past
		// the inserted text.
		//tfSelection.range->Collapse(ec, TF_ANCHOR_END);
		//mpContext->SetSelection(ec, 1, &tfSelection);
	}
	pInsertAtSelection->Release();
    pRangeInsert->Release();
	pContextComposition->Release();
	// end the composition now or later?
	pComposition->EndComposition(ec);
	pComposition->Release();

    return S_OK;
}
