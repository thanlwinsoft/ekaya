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
#include <windows.h>
#include <string.h>

void DllAddRef();
void DllRelease();

extern HINSTANCE g_hInst;
extern CRITICAL_SECTION g_cs;
extern LONG g_cRefDll;

extern const LANGID TEXTSERVICE_LANGID;
extern const wchar_t* TEXTSERVICE_DESC;
extern const char *TEXTSERVICE_DESC_A;
extern const char * TEXTSERVICE_MODEL;
extern const ULONG TEXTSERVICE_ICON_INDEX;

extern const CLSID CLSID_EKAYA_SERVICE;
extern const GUID GUID_PROFILE;
extern const GUID GUID_LANGBAR_BUTTON;
