// PowerPropertySheet.cpp : Implementation of CPowerPropertySheet

#include "stdafx.h"
#include "PowerPropertySheet.h"
#include "dllmain.h"

#include <atlconv.h>
#include <iostream>
#include <string>
#include  <commctrl.h>

// CPowerPropertySheet

STDMETHODIMP CPowerPropertySheet::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID)
{
	FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stg = { TYMED_HGLOBAL };
	HDROP     hDrop;

	// Look for CF_HDROP data in the data object. If there
	// is no such data, return an error back to Explorer.

	if (FAILED(pDataObj->GetData (&fmt, &stg)))
		return E_INVALIDARG;

	// Get a pointer to the actual data.
	hDrop = (HDROP) GlobalLock (stg.hGlobal);

	// Make sure it worked.
	if (NULL == hDrop)
		return E_INVALIDARG;

	UINT uNumFiles = DragQueryFile (hDrop, 0xFFFFFFFF, NULL, 0);
	HRESULT hr = S_OK;

	if (0 == uNumFiles || uNumFiles > 1) // We don't want to deal with multiple file selection
	{
		GlobalUnlock (stg.hGlobal);
		ReleaseStgMedium (&stg);
		wcscpy_s(m_szFile, MAX_PATH, _T(""));
		return E_INVALIDARG;
	}

	// Zero fill the file name buffer
	memset(m_szFile, '\0', MAX_PATH);

	// Get the name of the first file and store it in our
	// member variable m_szFile.
	if (0 == DragQueryFile (hDrop, 0, m_szFile, MAX_PATH))
	{
		hr = E_INVALIDARG;
	}

	GlobalUnlock (stg.hGlobal);
	ReleaseStgMedium (&stg);

	return hr;
}

STDMETHODIMP CPowerPropertySheet::AddPages(LPFNADDPROPSHEETPAGE lpfnAddPageProc, LPARAM lParam)
{
	if (_wcsnicmp(m_szFile, _T(""), sizeof(m_szFile) == 0))
		return S_OK; // Either no files are selected or multiple files were selected

	AssemblyName* pAssemblyName = NULL;
	try
	{
		pAssemblyName = new AssemblyName(m_szFile); // This is freed by PropPageCallbackProc() method
	}
	catch (std::exception&)
	{
		if (pAssemblyName != NULL)
			delete pAssemblyName;
		return S_OK;
	}

	PROPSHEETPAGE  psp;
	HPROPSHEETPAGE hPage;

	psp.dwSize      = sizeof(PROPSHEETPAGE);
	psp.dwFlags     = PSP_USEREFPARENT | PSP_USETITLE | PSP_USEICONID | PSP_USECALLBACK;
	psp.hInstance   = _AtlBaseModule.m_hInst; //_Module.GetResourceInstance();
	psp.pszTemplate = MAKEINTRESOURCE(IDD_PROPPAGE_LARGE);
	psp.pszIcon     = 0;
	psp.pszTitle    = _T(".NET");
	psp.pfnDlgProc  = (DLGPROC)PropPageDlgProc;
	psp.lParam      = (LPARAM)pAssemblyName;
	psp.pfnCallback = PropPageCallbackProc;
	psp.pcRefParent = (UINT*) &_AtlModule.m_nLockCnt;

	hPage = CreatePropertySheetPage(&psp);

	if(NULL != hPage)
	{
		// Call the shell's callback function, so it adds the page to
		// the property sheet.

		if(!lpfnAddPageProc(hPage, lParam))
		{
			DestroyPropertySheetPage(hPage);
		}
	}

	return S_OK;
}

STDMETHODIMP CPowerPropertySheet::ReplacePage(UINT, LPFNADDPROPSHEETPAGE, LPARAM)
{ 
	// We don't use the ReplacePage() method in this shell extension
	return E_NOTIMPL; 
}

BOOL CALLBACK CPowerPropertySheet::PropPageDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		bRet = OnInitDialog(hwnd, lParam);
		break;
	case WM_NOTIFY:
		{
			NMHDR* phdr = (NMHDR*) lParam;
			switch(phdr->code)
			{
			case PSN_APPLY:
				// This property dialog is for information only, so ignore the Apply event
				break;
			}
		}
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_BUTTON_COPY_SELECTED)
		{
			CopyToClipboard(hwnd);
		}
		break;
	}


	return bRet;
}

void CPowerPropertySheet::CopyToClipboard(HWND hDialog)
{
	if (hDialog != NULL)
	{
		HWND hListView = GetDlgItem(hDialog, IDC_LIST1);
		if (hListView != NULL)
		{
			int item = -1;
			//int subitem = 0;
			std::wstring clipboardData;
			TCHAR szText[4096] = {0};
			int cchTextMax = sizeof(szText);

			item = ListView_GetNextItem(hListView, item, LVNI_SELECTED);
			while (item >= 0)
			{
				if (clipboardData.length() > 0)
					clipboardData += _TEXT("\r\n");

				memset(szText, '\0', cchTextMax);
				ListView_GetItemText(hListView, item, 0, (LPTSTR)szText, cchTextMax);
				clipboardData += szText;
				clipboardData += _TEXT(" : ");
				memset(szText, '\0', cchTextMax);
				ListView_GetItemText(hListView, item, 1, (LPTSTR)szText, cchTextMax);
				clipboardData += szText;
				item = ListView_GetNextItem(hListView, item, LVNI_SELECTED);
			}

			//MessageBox(0, clipboardData.c_str(), _TEXT("TEST"), 0);

			if (OpenClipboard(hDialog))
			{
				if (EmptyClipboard()) // Clear out whatever is in the clipboard, so we can put our data in there
				{
					size_t size = (clipboardData.length() + 1) * sizeof(TCHAR);
					// Notice that we have a GlobalAlloc() but no GlobalFree() call. It's not a memory leak.
					// Windows will free the memory when EmptyClipboard() is called next time.
					HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, size); 
					if (hClipboardData != NULL) 
					{ 
						TCHAR* pchData = NULL;
						pchData = (TCHAR*)GlobalLock(hClipboardData);
						memset(pchData, '\0', size);
						memcpy(pchData, clipboardData.c_str(), size);
						GlobalUnlock(hClipboardData);
						SetClipboardData(CF_UNICODETEXT, hClipboardData);
					}
				}
				CloseClipboard();
			}
		}
	}
}

UINT CALLBACK CPowerPropertySheet::PropPageCallbackProc(HWND hwnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
{
	if(PSPCB_RELEASE == uMsg && ppsp->lParam != NULL)
	{
		AssemblyName* pAssemblyName = (AssemblyName*)ppsp->lParam;
		if (pAssemblyName != NULL)
		{
			delete pAssemblyName;
		}
	}

	return 1;
}

AssemblyName* CPowerPropertySheet::GetAssemblyName(LPPROPSHEETPAGE ppsp)
{
	AssemblyName* pAssemblyName = NULL;
	if (ppsp != NULL)
	{
		pAssemblyName = (AssemblyName*) ppsp->lParam;
	}

	return pAssemblyName;
}

BOOL CPowerPropertySheet::OnInitDialog(HWND hwnd, LPARAM lParam)
{        
	AssemblyName* pAssemblyName = GetAssemblyName((PROPSHEETPAGE*) lParam);
	if (pAssemblyName != NULL)
	{
		// Populate the list view control with assembly details
		HWND hListView = GetDlgItem(hwnd, IDC_LIST1);
		if (hListView != NULL)
		{
			// Under some conditions the file properties dialog can be both taller and wider, so
			// we have to have some logic to move/expand our controls.
			ResizeControls(hwnd, hListView); 

			ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT); 
			
			// Set the columns
			CreateListViewColumn(hListView, 0, 100, _T("Property"));
			CreateListViewColumn(hListView, 1, 1600, _T("Value"));

			int item = 0;
			InsertItem(hListView, item++, _TEXT("Path"), (TCHAR*)pAssemblyName->GetPath().c_str());
			InsertItem(hListView, item++, _TEXT("Assembly Name"), (TCHAR*)pAssemblyName->GetName().c_str());
			InsertItem(hListView, item++, _TEXT("Assembly Version"), (TCHAR*)pAssemblyName->GetVersion().c_str());
			InsertItem(hListView, item++, _TEXT("Public Key Token"), (TCHAR*)pAssemblyName->GetPublicKeyToken().c_str());
			InsertItem(hListView, item++, _TEXT("Public Key"), (TCHAR*)pAssemblyName->GetPublicKey().c_str());
			InsertItem(hListView, item++, _TEXT("Strong Name"), (TCHAR*)pAssemblyName->GetFullName().c_str());
		}
	}

	return FALSE;
}

void CPowerPropertySheet::InsertItem(HWND hListView, int item, TCHAR* itemText, TCHAR* subItemText)
{
	LV_ITEM lv;
	lv.iItem = item;
	lv.mask = LVIF_TEXT;
	lv.cchTextMax = 4096;
	lv.iSubItem = 0;
	lv.pszText = itemText;
	ListView_InsertItem(hListView, &lv);
	ListView_SetItemText(hListView, item, 1, subItemText);
}

void CPowerPropertySheet::CreateListViewColumn(HWND hListView, int index, int width, TCHAR* columnName)
{
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.fmt = LVCFMT_LEFT;
	lvc.iSubItem = index;
	lvc.cx = width;
	lvc.pszText = columnName;
	ListView_InsertColumn(hListView, index, &lvc);
}

void CPowerPropertySheet::ResizeControls(HWND hPage, HWND hListView)
{
	// Resize the list view control to fit the property sheet
	HWND hsheet = GetParent(hPage);
	HWND hbutton = GetDlgItem(hPage, IDC_BUTTON_COPY); 

	if (hsheet == NULL || hbutton == NULL)
		return;
	
	RECT listRC;
	RECT pageRC;
	RECT sheetRC;
	RECT copyRC;
			
	GetClientRect(hsheet, &sheetRC); 
	GetClientRect(hPage, &pageRC); 
			
	GetWindowRect(hListView, &listRC);
	GetWindowRect(hbutton, &copyRC);
			
	MapWindowPoints(HWND_DESKTOP, hsheet, (LPPOINT)&listRC, 2);
	MapWindowPoints(HWND_DESKTOP, hsheet, (LPPOINT)&copyRC, 2);
			
	// Move the button close to the right side
	MoveWindow(hbutton, 
		sheetRC.right - 110, 
		copyRC.top, 
		copyRC.right - copyRC.left, 
		copyRC.bottom - copyRC.top, 
		TRUE);
			
	// Expand the list view to fill the page area
	MoveWindow(hListView, 
		listRC.left, 
		listRC.top, 
		(sheetRC.right - sheetRC.left) - 43, 
		(pageRC.bottom - pageRC.top) - (43 - (pageRC.top - listRC.top)), 
		TRUE);
}
