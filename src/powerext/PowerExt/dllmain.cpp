// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "PowerExt_i.h"
#include "dllmain.h"

CPowerExtModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
 
	// Init the common controls.
	INITCOMMONCONTROLSEX iccex = { sizeof(INITCOMMONCONTROLSEX), ICC_DATE_CLASSES };
	InitCommonControlsEx(&iccex);

	return _AtlModule.DllMain(dwReason, lpReserved); 
}
