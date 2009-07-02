#include "Ekaya.h"

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

#ifdef _DEBUG
// {0ED1B095-A469-4a68-B7F9-84E2611C813A}
static const CLSID CLSID_EKAYA_SERVICE = 
{ 0xed1b095, 0xa469, 0x4a68, { 0xb7, 0xf9, 0x84, 0xe2, 0x61, 0x1c, 0x81, 0x3a } };
const char * TEXTSERVICE_DESC_A = "Ekaya Input Method (debug)";
const wchar_t * TEXTSERVICE_DESC = L"Ekaya Input Method (debug)";
#else
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
