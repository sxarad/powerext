// PowerExt.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "PowerExt_i.h"
#include "dllmain.h"
#include "RegistrationManager.h"

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
	return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
	// If we're on NT, add ourselves to the list of approved shell extensions.

	if (0 == (GetVersion() & 0x80000000UL))
	{
		CRegKey reg;
		LONG    lRet;

		lRet = reg.Open (HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"), KEY_SET_VALUE);
		if (ERROR_SUCCESS != lRet)
			return E_ACCESSDENIED;

		lRet = reg.SetStringValue(_T("PowerShellExt extension"), 
			_T("{B2DDD3BC-398F-417E-8324-3E308FE42CC2}"));

		if (ERROR_SUCCESS != lRet)
			return E_ACCESSDENIED;

		RegistrationManager m;
		m.RegisterPropertySheetHandler(_T(".dll"), _T("PowerPropertySheet"), _T("{06DCCEBA-56BD-4E42-9F61-A35675FFF5AE}"), false);
		m.RegisterPropertySheetHandler(_T(".exe"), _T("PowerPropertySheet"), _T("{06DCCEBA-56BD-4E42-9F61-A35675FFF5AE}"), false);
	}

	// registers object, typelib and all interfaces in typelib
	HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	// If we're on NT, remove ourselves from the list of approved shell extensions.

	if (0 == (GetVersion() & 0x80000000UL))
	{
		CRegKey reg;
		LONG    lRet;

		lRet = reg.Open (HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"), KEY_SET_VALUE);
		if (ERROR_SUCCESS == lRet)
		{
			lRet = reg.DeleteValue (_T("{B2DDD3BC-398F-417E-8324-3E308FE42CC2}"));
		}

		RegistrationManager m;
		m.RegisterPropertySheetHandler(_T(".dll"), _T("PowerPropertySheet"), _T("{06DCCEBA-56BD-4E42-9F61-A35675FFF5AE}"), true);
		m.RegisterPropertySheetHandler(_T(".exe"), _T("PowerPropertySheet"), _T("{06DCCEBA-56BD-4E42-9F61-A35675FFF5AE}"), true);
	}	

	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

// DllInstall - Adds/Removes entries to the system registry per user
//              per machine.	
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
	HRESULT hr = E_FAIL;
	static const wchar_t szUserSwitch[] = _T("user");

	if (pszCmdLine != NULL)
	{
		if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
		{
			AtlSetPerUserRegistration(true);
		}
	}

	if (bInstall)
	{	
		hr = DllRegisterServer();
		if (FAILED(hr))
		{	
			DllUnregisterServer();
		}
	}
	else
	{
		hr = DllUnregisterServer();
	}

	return hr;
}
