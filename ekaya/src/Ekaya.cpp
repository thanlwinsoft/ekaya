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


#ifdef _DEBUG
// {9B92BBBF-980D-47a3-B182-15050F8538DE}
static const GUID GUID_PROFILE = 
{ 0x9b92bbbf, 0x980d, 0x47a3, { 0xb1, 0x82, 0x15, 0x5, 0xf, 0x85, 0x38, 0xde } };

// {DF8DACE9-9815-409e-BD9A-F1485F8282B5}
static const GUID GUID_LANGBAR_BUTTON = 
{ 0xdf8dace9, 0x9815, 0x409e, { 0xbd, 0x9a, 0xf1, 0x48, 0x5f, 0x82, 0x82, 0xb5 } };

// {0ED1B095-A469-4a68-B7F9-84E2611C813A}
static const CLSID CLSID_EKAYA_SERVICE = 
{ 0xed1b095, 0xa469, 0x4a68, { 0xb7, 0xf9, 0x84, 0xe2, 0x61, 0x1c, 0x81, 0x3a } };

const char * TEXTSERVICE_DESC_A = "Ekaya Input Method (debug)";
const wchar_t * TEXTSERVICE_DESC = L"Ekaya Input Method (debug)";
#else

// {B7B86798-9EBE-41bd-B159-214A4CEA8805}
static const GUID GUID_PROFILE = 
{ 0xb7b86798, 0x9ebe, 0x41bd, { 0xb1, 0x59, 0x21, 0x4a, 0x4c, 0xea, 0x88, 0x5 } };

// {6B9CF85A-3F9B-4317-AD69-50DB0D40B7DD}
static const GUID GUID_LANGBAR_BUTTON = 
{ 0x6b9cf85a, 0x3f9b, 0x4317, { 0xad, 0x69, 0x50, 0xdb, 0xd, 0x40, 0xb7, 0xdd } };

// {ACC98569-4415-4303-8FDC-3DB142656E77}
static const CLSID CLSID_EKAYA_SERVICE = 
{ 0xacc98569, 0x4415, 0x4303, { 0x8f, 0xdc, 0x3d, 0xb1, 0x42, 0x65, 0x6e, 0x77 } };

const char * TEXTSERVICE_DESC_A = "Ekaya Input Method";
const wchar_t * TEXTSERVICE_DESC = L"Ekaya Input Method";
#endif


const ULONG TEXTSERVICE_ICON_INDEX = 0;

//#define TEXTSERVICE_LANGID    MAKELANGID(LANG_MYANMAR, SUBLANG_DEFAULT)
static const LANGID TEXTSERVICE_LANGID = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
const char * TEXTSERVICE_MODEL = "Apartment";

HINSTANCE g_hInst = NULL;
CRITICAL_SECTION g_cs;
LONG g_cRefDll = 0;
