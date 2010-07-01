/*
 * Copyright (C) 2010 ThanLwinSoft.org
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
#include <windows.h>
#include <cstdio>
#include <msctf.h>

#include "Ekaya.h"

using namespace EKAYA_NS;

typedef HRESULT (WINAPI * PDllGetClassObject)(REFCLSID rclsid, REFIID riid, void **ppvObj);

int main(int argc, char ** argv)
{
    HMODULE hLibrary = NULL;
    if (argc < 2)
	    hLibrary = LoadLibraryA("ekaya.dll");
    else
        hLibrary = LoadLibraryA(argv[1]);

    if (!hLibrary)
    {
        fprintf(stderr, "Failed to load ekaya.dll\n");
        exit(-1);
    }
    HRESULT result = CoInitialize(NULL);
    if (result != NOERROR)
        fprintf(stderr, "Failed to CoInitialize\n");
    FARPROC proc = GetProcAddress(hLibrary, "DllGetClassObject");
    PDllGetClassObject getClassProc = (PDllGetClassObject)(proc);
    if (getClassProc)
    {
        void * pRawFactory = NULL;
        //IID ekayaTestIid = { 0xf99addbc, 0x3d5c, 0x4f4e, {0xb7, 0xac, 0xc9, 0x01, 0x3b, 0x59, 0xed, 0x61}};
        result = getClassProc( CLSID_EKAYA_SERVICE, IID_IClassFactory, &pRawFactory);
        if (result == NOERROR && pRawFactory)
        {
            IClassFactory * pFactory = (IClassFactory*) pRawFactory;
            fprintf(stderr, "Have Ekaya factory\n");
            ITfTextInputProcessor * pTextInputProcessor = NULL;
            if (pFactory->CreateInstance(NULL, IID_ITfTextInputProcessor,
                reinterpret_cast<void**>(&pTextInputProcessor)) == NOERROR &&
                pTextInputProcessor != NULL)
            {
                ITfThreadMgr * pThreadMgr = NULL;
                result = CoCreateInstance(  CLSID_TF_ThreadMgr, NULL,
                    CLSCTX_INPROC_SERVER, IID_ITfThreadMgr, (void**)&pThreadMgr);
                if (result == NOERROR && pThreadMgr)
                {
                    pTextInputProcessor->Activate(pThreadMgr, 0);
                    fprintf(stderr, "Ekaya Text Input Processor Activated\n");
                    pTextInputProcessor->Deactivate();
                }
                else
                {
                    fprintf(stderr, "Failed to create ThreadMgr\n");
                }
            }
            pFactory->Release();
        }
    }
    else 
    {
        fprintf(stderr, "Failed to find DllGetClassObject\n");
        CoUninitialize();
        exit(-1);
    }
    CoUninitialize();
	return 0;
}
