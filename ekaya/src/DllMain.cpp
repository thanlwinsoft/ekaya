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
#include <windows.h>
#include "MessageLogger.h"
#include "Ekaya.h"


BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*pvReserved*/)
{
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
            EKAYA_NS::g_hInst = hInstance;
            if (!InitializeCriticalSectionAndSpinCount(&EKAYA_NS::g_cs, 0))
                return FALSE;
            EKAYA_NS::MessageLogger::logMessage("Ekaya DLL Attach\n");
            break;

        case DLL_PROCESS_DETACH:
            EKAYA_NS::MessageLogger::logMessage("Ekaya DLL Detach\n");
            DeleteCriticalSection(&EKAYA_NS::g_cs);
            break;
    }

    return TRUE;
}
