
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

extern const GUID GUID_COMPARTMENT_KEYBOARD_DISABLED;
extern const GUID GUID_COMPARTMENT_EMPTYCONTEXT;
extern const GUID GUID_TFCAT_TIP_KEYBOARD;
extern const CLSID CLSID_EKAYA_SERVICE;
extern const GUID GUID_PROFILE;
extern const GUID GUID_LANGBAR_BUTTON;
extern const GUID GUID_LANGBAR_ICON;
