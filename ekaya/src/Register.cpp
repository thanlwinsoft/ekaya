/*
* Server registration code based on code samples
*/

#include <windows.h>
#include <ole2.h>
#include "msctf.h"
#include "Ekaya.h"
#include "Register.h"

namespace EKAYA_NS {

#define CLSID_STRLEN 38  // strlen("{xxxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxx}")

static const char c_szInfoKeyPrefix[] = "CLSID\\";
static const char c_szInProcSvr32[] = "InProcServer32";
static const char c_szModelName[] = "ThreadingModel";

//+---------------------------------------------------------------------------
//  RegisterProfiles
//----------------------------------------------------------------------------

BOOL RegisterProfiles()
{
    ITfInputProcessorProfiles *pInputProcessProfiles;
    WCHAR achIconFile[MAX_PATH];
    char achFileNameA[MAX_PATH];
    DWORD cchA;
    int cchIconFile;
    HRESULT hr;

    hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles, NULL, CLSCTX_INPROC_SERVER,
                          IID_ITfInputProcessorProfiles, (void**)&pInputProcessProfiles);

    if (hr != S_OK)
        return E_FAIL;

    hr = pInputProcessProfiles->Register(CLSID_EKAYA_SERVICE);

    if (hr != S_OK)
        goto Exit;

    cchA = GetModuleFileNameA(g_hInst, achFileNameA, ARRAYSIZE(achFileNameA));

    cchIconFile = MultiByteToWideChar(CP_ACP, 0, achFileNameA, cchA, achIconFile, ARRAYSIZE(achIconFile)-1);
    achIconFile[cchIconFile] = '\0';
    // always add Ekaya to en_US
    hr = pInputProcessProfiles->AddLanguageProfile(CLSID_EKAYA_SERVICE,
                                  TEXTSERVICE_LANGID, 
                                  GUID_PROFILE, 
                                  TEXTSERVICE_DESC,
                                  (ULONG)wcslen(TEXTSERVICE_DESC),
                                  achIconFile,
                                  cchIconFile,
                                  TEXTSERVICE_ICON_INDEX);
    AddLanguageProfiles(pInputProcessProfiles);
Exit:
    pInputProcessProfiles->Release();
    return (hr == S_OK);
}


void AddLanguageProfiles(ITfInputProcessorProfiles *pInputProcessProfiles)
{
    HRESULT hr = S_OK;
    if (pInputProcessProfiles == NULL)
    {
        hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles, NULL, CLSCTX_INPROC_SERVER,
                          IID_ITfInputProcessorProfiles, (void**)&pInputProcessProfiles);
    }
    else
    {
        pInputProcessProfiles->AddRef();
    }
    if (hr != S_OK)
        return;
    WCHAR achIconFile[MAX_PATH];
    char achFileNameA[MAX_PATH];
    int cchIconFile;
    DWORD cchA = GetModuleFileNameA(g_hInst, achFileNameA, ARRAYSIZE(achFileNameA));
    cchIconFile = MultiByteToWideChar(CP_ACP, 0, achFileNameA, cchA, achIconFile, ARRAYSIZE(achIconFile)-1);
    achIconFile[cchIconFile] = '\0';

    // now add Ekaya to each language which already has an installed layout
    LANGID * pLangIds = NULL;
    ULONG numLanguages = 0;
    hr = pInputProcessProfiles->GetLanguageList(&pLangIds, &numLanguages);
    // This requires admin permission, which on Vista, Win7 it probably won't have
    // when run as an input method for a normal app
    if (SUCCEEDED(hr))
    {
        for (size_t i = 0; i < numLanguages; i++)
        {
            hr = pInputProcessProfiles->AddLanguageProfile(CLSID_EKAYA_SERVICE,
                                      pLangIds[i], 
                                      GUID_PROFILE, 
                                      TEXTSERVICE_DESC, 
                                      (ULONG)wcslen(TEXTSERVICE_DESC),
                                      achIconFile,
                                      cchIconFile,
                                      TEXTSERVICE_ICON_INDEX);
        }
        CoTaskMemFree(pLangIds);
    }
    pInputProcessProfiles->Release();
}
//+---------------------------------------------------------------------------
//  UnregisterProfiles
//----------------------------------------------------------------------------

void UnregisterProfiles()
{
    ITfInputProcessorProfiles *pInputProcessProfiles;
    HRESULT hr;

    hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles, NULL, CLSCTX_INPROC_SERVER,
                          IID_ITfInputProcessorProfiles, (void**)&pInputProcessProfiles);

    if (hr != S_OK)
        return;

    pInputProcessProfiles->Unregister(CLSID_EKAYA_SERVICE);
    pInputProcessProfiles->Release();
}

//+---------------------------------------------------------------------------
//  RegisterCategories
//----------------------------------------------------------------------------

BOOL RegisterCategories()
{
    ITfCategoryMgr *pCategoryMgr;
    HRESULT hr;

    hr = CoCreateInstance(CLSID_TF_CategoryMgr, NULL, CLSCTX_INPROC_SERVER, 
                          IID_ITfCategoryMgr, (void**)&pCategoryMgr);

    if (hr != S_OK)
        return FALSE;

    hr = pCategoryMgr->RegisterCategory(CLSID_EKAYA_SERVICE,
                                        GUID_TFCAT_TIP_KEYBOARD,
                                        CLSID_EKAYA_SERVICE);

    pCategoryMgr->Release();
    return (hr == S_OK);
}

//+---------------------------------------------------------------------------
//  UnregisterCategories
//----------------------------------------------------------------------------

void UnregisterCategories()
{
    ITfCategoryMgr *pCategoryMgr;
    HRESULT hr;

    hr = CoCreateInstance(CLSID_TF_CategoryMgr, NULL, CLSCTX_INPROC_SERVER, 
                          IID_ITfCategoryMgr, (void**)&pCategoryMgr);

    if (hr != S_OK)
        return;

    pCategoryMgr->UnregisterCategory(CLSID_EKAYA_SERVICE,
                                     GUID_TFCAT_TIP_KEYBOARD, 
                                     CLSID_EKAYA_SERVICE);

    pCategoryMgr->Release();
    return;
}

//+---------------------------------------------------------------------------
//
// CLSIDToStringA
//
//----------------------------------------------------------------------------

BOOL CLSIDToStringA(REFGUID refGUID, char *pchA)
{
    static const BYTE GuidMap[] = {3, 2, 1, 0, '-', 5, 4, '-', 7, 6, '-',
                                   8, 9, '-', 10, 11, 12, 13, 14, 15};

    static const char szDigits[] = "0123456789ABCDEF";

    int i;
    char *p = pchA;

    const BYTE * pBytes = (const BYTE *) &refGUID;

    *p++ = '{';
    for (i = 0; i < sizeof(GuidMap); i++)
    {
        if (GuidMap[i] == '-')
        {
            *p++ = '-';
        }
        else
        {
            *p++ = szDigits[ (pBytes[GuidMap[i]] & 0xF0) >> 4 ];
            *p++ = szDigits[ (pBytes[GuidMap[i]] & 0x0F) ];
        }
    }

    *p++ = '}';
    *p   = '\0';

    return TRUE;
}

//+---------------------------------------------------------------------------
//
// RecurseDeleteKey
//
// RecurseDeleteKey is necessary because on NT RegDeleteKey doesn't work if the
// specified key has subkeys
//----------------------------------------------------------------------------
LONG RecurseDeleteKey(HKEY hParentKey, char * lpszKey)
{
    HKEY hKey;
    LONG lRes;
    FILETIME time;
    char szBuffer[256];
    DWORD dwSize = ARRAYSIZE(szBuffer);

    if (RegOpenKeyA(hParentKey, lpszKey, &hKey) != ERROR_SUCCESS)
        return ERROR_SUCCESS; // let's assume we couldn't open it because it's not there

    lRes = ERROR_SUCCESS;
    while (RegEnumKeyExA(hKey, 0, szBuffer, &dwSize, NULL, NULL, NULL, &time)==ERROR_SUCCESS)
    {
        szBuffer[ARRAYSIZE(szBuffer)-1] = '\0';
        lRes = RecurseDeleteKey(hKey, szBuffer);
        if (lRes != ERROR_SUCCESS)
            break;
        dwSize = ARRAYSIZE(szBuffer);
    }
    RegCloseKey(hKey);

    return lRes == ERROR_SUCCESS ? RegDeleteKeyA(hParentKey, lpszKey) : lRes;
}

//+---------------------------------------------------------------------------
//  RegisterServer
//----------------------------------------------------------------------------

BOOL RegisterServer()
{
    DWORD dw;
    HKEY hKey;
    HKEY hSubKey;
    BOOL fRet;
    char achIMEKey[ARRAYSIZE(c_szInfoKeyPrefix) + CLSID_STRLEN];
    char achFileName[MAX_PATH];

    if (!CLSIDToStringA(CLSID_EKAYA_SERVICE, achIMEKey + ARRAYSIZE(c_szInfoKeyPrefix) - 1))
        return FALSE;
    memcpy(achIMEKey, c_szInfoKeyPrefix, sizeof(c_szInfoKeyPrefix)-sizeof(char));

    // Create the keys under HKCR
    fRet = (RegCreateKeyExA(HKEY_CLASSES_ROOT, achIMEKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dw)
            == ERROR_SUCCESS);
    if (fRet)
    {
        fRet &= RegSetValueExA(hKey, NULL, 0, REG_SZ, (BYTE *)TEXTSERVICE_DESC_A, static_cast<DWORD>((strlen(TEXTSERVICE_DESC_A)+1)*sizeof(char)))
            == ERROR_SUCCESS;

        if (fRet &= RegCreateKeyExA(hKey, c_szInProcSvr32, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSubKey, &dw)
            == ERROR_SUCCESS)
        {
            dw = GetModuleFileNameA(g_hInst, achFileName, ARRAYSIZE(achFileName));

            fRet &= RegSetValueExA(hSubKey, NULL, 0, REG_SZ, (BYTE *)achFileName, static_cast<DWORD>((strlen(achFileName)+1)*sizeof(TCHAR))) == ERROR_SUCCESS;
            fRet &= RegSetValueExA(hSubKey, c_szModelName, 0, REG_SZ, (BYTE *)TEXTSERVICE_MODEL, static_cast<DWORD>((strlen(TEXTSERVICE_MODEL)+1)*sizeof(char))) == ERROR_SUCCESS;
            RegCloseKey(hSubKey);
        }
        RegCloseKey(hKey);
    }

    return fRet;
}

//+---------------------------------------------------------------------------
//  UnregisterServer
//----------------------------------------------------------------------------

void UnregisterServer()
{
    char achIMEKey[ARRAYSIZE(c_szInfoKeyPrefix) + CLSID_STRLEN];

    if (!CLSIDToStringA(CLSID_EKAYA_SERVICE, achIMEKey + ARRAYSIZE(c_szInfoKeyPrefix) - 1))
        return;
    memcpy(achIMEKey, c_szInfoKeyPrefix, sizeof(c_szInfoKeyPrefix)-sizeof(char));

    RecurseDeleteKey(HKEY_CLASSES_ROOT, achIMEKey);
}

}
