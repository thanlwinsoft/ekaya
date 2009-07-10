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
// {7FE898B0-D166-4ac7-9FE1-24A115C604C6}
static const GUID GUID_COMPARTMENT_KEYBOARD_DISABLED = 
{ 0x7fe898b0, 0xd166, 0x4ac7, { 0x9f, 0xe1, 0x24, 0xa1, 0x15, 0xc6, 0x4, 0xc6 } };

// {21BAD71D-747A-4a9b-82FB-BF2073B381E3}
static const GUID GUID_COMPARTMENT_EMPTYCONTEXT = 
{ 0x21bad71d, 0x747a, 0x4a9b, { 0x82, 0xfb, 0xbf, 0x20, 0x73, 0xb3, 0x81, 0xe3 } };

// {E56010BA-B197-4012-86A7-71AC6F5049A0}
static const GUID GUID_TFCAT_TIP_KEYBOARD = 
{ 0xe56010ba, 0xb197, 0x4012, { 0x86, 0xa7, 0x71, 0xac, 0x6f, 0x50, 0x49, 0xa0 } };

// {6350D903-0F20-4630-A35A-E8844772B7B8}
static const GUID GUID_LANGBAR_ICON = 
{ 0x6350d903, 0xf20, 0x4630, { 0xa3, 0x5a, 0xe8, 0x84, 0x47, 0x72, 0xb7, 0xb8 } };

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
// {AA1115B9-8EFD-4b37-8B33-2CB4F4647F7F}
static const GUID GUID_COMPARTMENT_KEYBOARD_DISABLED = 
{ 0xaa1115b9, 0x8efd, 0x4b37, { 0x8b, 0x33, 0x2c, 0xb4, 0xf4, 0x64, 0x7f, 0x7f } };

// {3AD3EBB6-3D3D-4c7e-A941-BF607A46E0CD}
static const GUID GUID_COMPARTMENT_EMPTYCONTEXT = 
{ 0x3ad3ebb6, 0x3d3d, 0x4c7e, { 0xa9, 0x41, 0xbf, 0x60, 0x7a, 0x46, 0xe0, 0xcd } };

// {3C39B762-8B98-44eb-A672-A2533E808D65}
static const GUID GUID_TFCAT_TIP_KEYBOARD = 
{ 0x3c39b762, 0x8b98, 0x44eb, { 0xa6, 0x72, 0xa2, 0x53, 0x3e, 0x80, 0x8d, 0x65 } };

// {C07C7614-9ACF-48f4-9E0A-51A5BC1C2B79}
static const GUID GUID_LANGBAR_ICON = 
{ 0xc07c7614, 0x9acf, 0x48f4, { 0x9e, 0xa, 0x51, 0xa5, 0xbc, 0x1c, 0x2b, 0x79 } };

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
