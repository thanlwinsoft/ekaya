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
#include "Ekaya.h"

namespace EKAYA_NS {

// Use different IDs for 32 bit, 64 bit and 32 bit debug
#ifdef _M_AMD64
// {F9DCA3B7-0812-48a4-B002-C7DCE0D3F069}
const GUID GUID_PROFILE = 
{ 0xf9dca3b7, 0x812, 0x48a4, { 0xb0, 0x2, 0xc7, 0xdc, 0xe0, 0xd3, 0xf0, 0x69 } };

// {3EB1B464-27B2-49fa-9698-F8F26FE0E101}
const GUID GUID_LANGBAR_BUTTON = 
{ 0x3eb1b464, 0x27b2, 0x49fa, { 0x96, 0x98, 0xf8, 0xf2, 0x6f, 0xe0, 0xe1, 0x1 } };

// {119D8CAC-D4D6-4e58-B6B6-A0DB26A01625}
const CLSID CLSID_EKAYA_SERVICE =
{ 0x119d8cac, 0xd4d6, 0x4e58, { 0xb6, 0xb6, 0xa0, 0xdb, 0x26, 0xa0, 0x16, 0x25 } };

// {2871C7B9-E43B-446c-8EB9-0BB4F4619C4E}
const GUID GUID_CATEGORY = 
{ 0x2871c7b9, 0xe43b, 0x446c, { 0x8e, 0xb9, 0xb, 0xb4, 0xf4, 0x61, 0x9c, 0x4e } };

const char * TEXTSERVICE_DESC_A = "Ekaya Input Method (64)";
const wchar_t * TEXTSERVICE_DESC = L"Ekaya Input Method (64)";
#else

#ifdef _DEBUG
// {9B92BBBF-980D-47a3-B182-15050F8538DE}
const GUID GUID_PROFILE = 
{ 0x9b92bbbf, 0x980d, 0x47a3, { 0xb1, 0x82, 0x15, 0x5, 0xf, 0x85, 0x38, 0xde } };

// {DF8DACE9-9815-409e-BD9A-F1485F8282B5}
const GUID GUID_LANGBAR_BUTTON = 
{ 0xdf8dace9, 0x9815, 0x409e, { 0xbd, 0x9a, 0xf1, 0x48, 0x5f, 0x82, 0x82, 0xb5 } };

// {0ED1B095-A469-4a68-B7F9-84E2611C813A}
const CLSID CLSID_EKAYA_SERVICE = 
{ 0xed1b095, 0xa469, 0x4a68, { 0xb7, 0xf9, 0x84, 0xe2, 0x61, 0x1c, 0x81, 0x3a } };

// {D42EC696-09BA-439e-9782-C7E398041642}
const GUID GUID_CATEGORY =
{ 0xd42ec696, 0x9ba, 0x439e, { 0x97, 0x82, 0xc7, 0xe3, 0x98, 0x4, 0x16, 0x42 } };

const char * TEXTSERVICE_DESC_A = "Ekaya Input Method (debug)";
const wchar_t * TEXTSERVICE_DESC = L"Ekaya Input Method (debug)";
#else

// {B7B86798-9EBE-41bd-B159-214A4CEA8805}
const GUID GUID_PROFILE = 
{ 0xb7b86798, 0x9ebe, 0x41bd, { 0xb1, 0x59, 0x21, 0x4a, 0x4c, 0xea, 0x88, 0x5 } };

// {6B9CF85A-3F9B-4317-AD69-50DB0D40B7DD}
const GUID GUID_LANGBAR_BUTTON = 
{ 0x6b9cf85a, 0x3f9b, 0x4317, { 0xad, 0x69, 0x50, 0xdb, 0xd, 0x40, 0xb7, 0xdd } };

// {ACC98569-4415-4303-8FDC-3DB142656E77}
const CLSID CLSID_EKAYA_SERVICE = 
{ 0xacc98569, 0x4415, 0x4303, { 0x8f, 0xdc, 0x3d, 0xb1, 0x42, 0x65, 0x6e, 0x77 } };

// {1702CADD-F93B-48ea-BD11-D48AFEDDBED0}
const GUID GUID_CATEGORY =
{ 0x1702cadd, 0xf93b, 0x48ea, { 0xbd, 0x11, 0xd4, 0x8a, 0xfe, 0xdd, 0xbe, 0xd0 } };

const char * TEXTSERVICE_DESC_A = "Ekaya Input Method";
const wchar_t * TEXTSERVICE_DESC = L"Ekaya Input Method";
#endif
#endif

const ULONG TEXTSERVICE_ICON_INDEX = 0;

const LANGID TEXTSERVICE_LANGID = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
const char * TEXTSERVICE_MODEL = "Apartment";

HINSTANCE g_hInst = NULL;
CRITICAL_SECTION g_cs;
LONG g_cRefDll = 0;

}
