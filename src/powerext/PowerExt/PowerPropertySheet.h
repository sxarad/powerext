// PowerPropertySheet.h : Declaration of the CPowerPropertySheet

#pragma once
#include "resource.h"       // main symbols

#include "PowerExt_i.h"
#include "AssemblyName.h"

#include <shlobj.h>
#include <comdef.h>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CPowerPropertySheet

class ATL_NO_VTABLE CPowerPropertySheet :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPowerPropertySheet, &CLSID_PowerPropertySheet>,
	public IShellExtInit,
	public IShellPropSheetExt
{
public:
	CPowerPropertySheet()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_POWERPROPERTYSHEET)

	DECLARE_NOT_AGGREGATABLE(CPowerPropertySheet)

	BEGIN_COM_MAP(CPowerPropertySheet)
		COM_INTERFACE_ENTRY(IShellExtInit)
		COM_INTERFACE_ENTRY(IShellPropSheetExt)
	END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

protected:
	TCHAR m_szFile[MAX_PATH];

private:
	static BOOL CALLBACK PropPageDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static UINT CALLBACK PropPageCallbackProc(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp);
	static BOOL OnInitDialog(HWND hwnd, LPARAM lParam);
	static BOOL OnApply(HWND hwnd, PSHNOTIFY* phdr);
	static void CreateListViewColumn(HWND hListView, int index, int width, TCHAR* columnName);
	static void ResizeControls(HWND hPage, HWND hListView);
	static void InsertItem(HWND hListView, int item, TCHAR* itemText, TCHAR* subItemText);
	static AssemblyName* GetAssemblyName(LPPROPSHEETPAGE ppsp);
	static void CopyToClipboard(HWND hListView);

public:
	// IShellExtInit
	STDMETHODIMP Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

public:
	// IShellPropSheetExt
	STDMETHODIMP AddPages(LPFNADDPROPSHEETPAGE, LPARAM);
	STDMETHODIMP ReplacePage(UINT, LPFNADDPROPSHEETPAGE, LPARAM);

public:
};


OBJECT_ENTRY_AUTO(__uuidof(PowerPropertySheet), CPowerPropertySheet)
