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

// Use different IDs for 32 bit, 64 bit and 32 bit debug
#ifdef _M_AMD64
const GUID GUID_PROFILE = { /* 0aebc972-c366-4c72-a53b-ddb953cdb1a0 */
    0x0aebc972,
    0xc366,
    0x4c72,
    {0xa5, 0x3b, 0xdd, 0xb9, 0x53, 0xcd, 0xb1, 0xa0}
  };

const GUID GUID_LANGBAR_BUTTON = { /* 9900a593-1e5c-4873-a493-fe14e8f486fe */
    0x9900a593,
    0x1e5c,
    0x4873,
    {0xa4, 0x93, 0xfe, 0x14, 0xe8, 0xf4, 0x86, 0xfe}
  };

const CLSID CLSID_EKAYA_SERVICE = { /* d8b8ef91-c206-419f-87ce-be28e0e00ae0 */
    0xd8b8ef91,
    0xc206,
    0x419f,
    {0x87, 0xce, 0xbe, 0x28, 0xe0, 0xe0, 0x0a, 0xe0}
  };

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

const char * TEXTSERVICE_DESC_A = "Ekaya Input Method";
const wchar_t * TEXTSERVICE_DESC = L"Ekaya Input Method";
#endif
#endif

const ULONG TEXTSERVICE_ICON_INDEX = 0;

//#define TEXTSERVICE_LANGID    MAKELANGID(LANG_MYANMAR, SUBLANG_DEFAULT)
const LANGID TEXTSERVICE_LANGID = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
const char * TEXTSERVICE_MODEL = "Apartment";

HINSTANCE g_hInst = NULL;
CRITICAL_SECTION g_cs;
LONG g_cRefDll = 0;
