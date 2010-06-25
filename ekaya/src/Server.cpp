// Server registration code based on code samples

#include <assert.h>

#include "Ekaya.h"
#include "EkayaInputProcessor.h"

// from Register.cpp
BOOL RegisterProfiles();
void UnregisterProfiles();
BOOL RegisterCategories();
void UnregisterCategories();
BOOL RegisterServer();
void UnregisterServer();

void FreeGlobalObjects(void);

class EkayaClassFactory;
static EkayaClassFactory *g_ObjectInfo[1] = { NULL };

//+---------------------------------------------------------------------------
//  DllAddRef
//----------------------------------------------------------------------------

void DllAddRef(void)
{
    InterlockedIncrement(&g_cRefDll);
}

//+---------------------------------------------------------------------------
//  DllRelease
//----------------------------------------------------------------------------

void DllRelease(void)
{
    if (InterlockedDecrement(&g_cRefDll) < 0) // g_cRefDll == -1 with zero refs
    {
        EnterCriticalSection(&g_cs);

        // need to check ref again after grabbing mutex
        if (g_ObjectInfo[0] != NULL)
        {
            FreeGlobalObjects();
        }
        assert(g_cRefDll == -1);

        LeaveCriticalSection(&g_cs);
    }
}

//+---------------------------------------------------------------------------
//  EkayaClassFactory declaration with IClassFactory Interface
//----------------------------------------------------------------------------

class EkayaClassFactory : public IClassFactory
{
public:
    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    // IClassFactory methods
    STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObj);
    STDMETHODIMP LockServer(BOOL fLock);

    // Constructor
    EkayaClassFactory(REFCLSID rclsid, HRESULT (*pfnCreateInstance)(IUnknown *pUnkOuter, REFIID riid, void **ppvObj))
        : mRclsId(rclsid)
    {
        mpfnCreateInstance = pfnCreateInstance;
    }

public:
    REFCLSID mRclsId;
    HRESULT (*mpfnCreateInstance)(IUnknown *pUnkOuter, REFIID riid, void **ppvObj);
private:
	EkayaClassFactory & operator=( const EkayaClassFactory & ) {}
};

//+---------------------------------------------------------------------------
//
//  EkayaClassFactory::QueryInterface
//
//----------------------------------------------------------------------------

STDAPI EkayaClassFactory::QueryInterface(REFIID riid, void **ppvObj)
{
    if (IsEqualIID(riid, IID_IClassFactory) || IsEqualIID(riid, IID_IUnknown))
    {
        *ppvObj = this;
        DllAddRef();
        return NOERROR;
    }
    *ppvObj = NULL;
    return E_NOINTERFACE;
}

//+---------------------------------------------------------------------------
//  EkayaClassFactory::AddRef
//----------------------------------------------------------------------------

STDAPI_(ULONG) EkayaClassFactory::AddRef()
{
    DllAddRef();
    return g_cRefDll+1; // -1 w/ no refs
}

//+---------------------------------------------------------------------------
//  EkayaClassFactory::Release
//----------------------------------------------------------------------------

STDAPI_(ULONG) EkayaClassFactory::Release()
{
    DllRelease();
    return g_cRefDll+1; // -1 w/ no refs
}

//+---------------------------------------------------------------------------
//  EkayaClassFactory::CreateInstance
//----------------------------------------------------------------------------

STDAPI EkayaClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObj)
{
    return mpfnCreateInstance(pUnkOuter, riid, ppvObj);
}

//+---------------------------------------------------------------------------
//  EkayaClassFactory::LockServer
//----------------------------------------------------------------------------

STDAPI EkayaClassFactory::LockServer(BOOL fLock)
{
    if (fLock)
    {
        DllAddRef();
    }
    else
    {
        DllRelease();
    }

    return S_OK;
}

//+---------------------------------------------------------------------------
//  BuildGlobalObjects
//----------------------------------------------------------------------------

void BuildGlobalObjects(void)
{
    // Build CClassFactory Objects
    g_ObjectInfo[0] = new EkayaClassFactory(CLSID_EKAYA_SERVICE, EkayaInputProcessor::CreateInstance);

    // You can add more object info here.
    // Don't forget to increase number of item for g_ObjectInfo[],
}

//+---------------------------------------------------------------------------
//  FreeGlobalObjects
//----------------------------------------------------------------------------

void FreeGlobalObjects(void)
{
    // Free CClassFactory Objects
    for (int i = 0; i < ARRAYSIZE(g_ObjectInfo); i++)
    {
        if (NULL != g_ObjectInfo[i])
        {
            delete g_ObjectInfo[i];
            g_ObjectInfo[i] = NULL;
        }
    }
}

//+---------------------------------------------------------------------------
//  DllGetClassObject
//----------------------------------------------------------------------------
__declspec(dllexport)
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppvObj)
{
    if (g_ObjectInfo[0] == NULL)
    {
        EnterCriticalSection(&g_cs);

            // need to check ref again after grabbing mutex
            if (g_ObjectInfo[0] == NULL)
            {
                BuildGlobalObjects();
            }

        LeaveCriticalSection(&g_cs);
    }

    if (IsEqualIID(riid, IID_IClassFactory) ||
        IsEqualIID(riid, IID_IUnknown))
    {
        for (int i = 0; i < ARRAYSIZE(g_ObjectInfo); i++)
        {
            if (NULL != g_ObjectInfo[i] &&
                IsEqualGUID(rclsid, g_ObjectInfo[i]->mRclsId))
            {
                *ppvObj = (void *)g_ObjectInfo[i];
                DllAddRef();    // class factory holds DLL ref count
                return NOERROR;
            }
        }
    }

    *ppvObj = NULL;

    return CLASS_E_CLASSNOTAVAILABLE;
}

//+---------------------------------------------------------------------------
//  DllCanUnloadNow
//----------------------------------------------------------------------------
__declspec(dllexport)
STDAPI DllCanUnloadNow(void)
{
    if (g_cRefDll >= 0) // -1 with no refs
        return S_FALSE;

    return S_OK;
}

//+---------------------------------------------------------------------------
//  DllUnregisterServer
//----------------------------------------------------------------------------
__declspec(dllexport)
STDAPI DllUnregisterServer(void)
{
    UnregisterProfiles();
    UnregisterCategories();
    UnregisterServer();

    return S_OK;
}

//+---------------------------------------------------------------------------
//  DllRegisterServer
//----------------------------------------------------------------------------
__declspec(dllexport)
STDAPI DllRegisterServer(void)
{
    // register this service's profile with the tsf
    if (!RegisterServer() ||
        !RegisterProfiles() ||
        !RegisterCategories())
    {
        DllUnregisterServer(); // cleanup any loose ends
        return E_FAIL;
    }

    return S_OK;
}

