#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

using namespace std;

int main() 
{
    HRESULT hres;

    // Component Object Model (COM) library initialization
    // this is a technology that is used for interaction between program components in Windows.
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres)) 
    {
        cerr << "Failed to initialize COM library. Error code: " << hres << endl;
        return 1;
    }

    // Windows Management Instrumentation (WMI) initialization
    // is the mechanism for managing information about the Windows system
    hres = CoInitializeSecurity(
        nullptr,
        -1,                          
        nullptr,
        nullptr,
        RPC_C_AUTHN_LEVEL_PKT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr,
        EOAC_NONE,
        nullptr
    );
    if (FAILED(hres)) 
    {
        cerr << "Failed to initialize security. Error code: " << hres << endl;
        CoUninitialize();
        return 1;
    }

    IWbemLocator* pLoc = nullptr;
    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        (LPVOID*)&pLoc
    );
    if (FAILED(hres)) 
    {
        cerr << "Failed to create IWbemLocator object. Error code: " << hres << endl;
        CoUninitialize();
        return 1;
    }

    // conntection to WMI creation
    IWbemServices* pSvc = nullptr;
    // connect to the local root\cimv2 namespace
    hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, 0, 0, 0, 0, &pSvc);
    if (FAILED(hres)) 
    {
        cerr << "Could not connect to WMI namespace. Error code: " << hres << endl;
        pLoc->Release();
        CoUninitialize();
        return 1;
    }

    // set security levels on the proxy
    hres = CoSetProxyBlanket(
        pSvc,                        // the proxy to set
        RPC_C_AUTHN_WINNT,           // authentication service
        RPC_C_AUTHZ_NONE,            // authorization service
        nullptr,                     // Server principal name
        RPC_C_AUTHN_LEVEL_CALL,      // authentication level
        RPC_C_IMP_LEVEL_IMPERSONATE, // impersonation level
        nullptr,                     // client identity
        EOAC_NONE                    // proxy capabilities
    );
    if (FAILED(hres)) 
    {
        cerr << "Could not set proxy blanket. Error code: " << hres << endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;
    }

    IEnumWbemClassObject* pEnumerator = nullptr;
    // execute the query
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_ComputerSystem"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr,
        &pEnumerator
    );
    if (FAILED(hres)) 
    {
        cerr << "Query for operating system data failed. Error code: " << hres << endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;
    }

    // get the data from the query
    IWbemClassObject* pclsObj;
    ULONG uReturn = 0;

    while (pEnumerator) 
    {
        hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (0 == uReturn) break;

        VARIANT vtProp;

        hres = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
        wprintf(L"Computer name: %s\n", vtProp.bstrVal);
        VariantClear(&vtProp);

        hres = pclsObj->Get(L"UserName", 0, &vtProp, 0, 0);
        wprintf(L"Username: %s\n", vtProp.bstrVal);
        VariantClear(&vtProp);

        hres = pclsObj->Get(L"Manufacturer", 0, &vtProp, 0, 0);
        wprintf(L"Manufacturer: %s\n", vtProp.bstrVal);
        VariantClear(&vtProp);

        hres = pclsObj->Get(L"Model", 0, &vtProp, 0, 0);
        wprintf(L"Model: %s\n", vtProp.bstrVal);
        VariantClear(&vtProp);

        hres = pclsObj->Get(L"NumberOfProcessors", 0, &vtProp, 0, 0);
        wprintf(L"Number of processors: %d\n", vtProp.uintVal);
        VariantClear(&vtProp);

        hres = pclsObj->Get(L"TotalPhysicalMemory", 0, &vtProp, 0, 0);
        wprintf(L"Total physical memory: %s\n", vtProp.bstrVal);
        VariantClear(&vtProp);

        pclsObj->Release();
    }

    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();

    return 0;
}
