// PowerContextMenu.cpp : Implementation of CPowerContextMenu

#include "stdafx.h"
#include "PowerContextMenu.h"

#include <atlconv.h>

// CPowerContextMenu

STDMETHODIMP CPowerContextMenu::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID)
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

	if (0 == uNumFiles)
	{
		GlobalUnlock (stg.hGlobal);
		ReleaseStgMedium (&stg);
		return E_INVALIDARG;
	}

	// Get the name of the first file and store it in our
	// member variable m_szFile.

	if (0 == DragQueryFile (hDrop, 0, m_szFile, MAX_PATH))
		hr = E_INVALIDARG;

	GlobalUnlock (stg.hGlobal);
	ReleaseStgMedium (&stg);

	return hr;
}

HRESULT CPowerContextMenu::QueryContextMenu (
	HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd,
	UINT uidLastCmd, UINT uFlags)
{
	// If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
	if (uFlags & CMF_DEFAULTONLY)
		return MAKE_HRESULT (SEVERITY_SUCCESS, FACILITY_NULL, 0);

	InsertMenu (hmenu, uMenuIndex, MF_BYPOSITION,
		uidFirstCmd, _T("SimpleShlExt Test Item"));

	return MAKE_HRESULT (SEVERITY_SUCCESS, FACILITY_NULL, 1);
}

HRESULT CPowerContextMenu::GetCommandString (
	UINT_PTR idCmd, UINT uFlags, UINT* pwReserved,
	LPSTR pszName, UINT cchMax)
{
	USES_CONVERSION;

	// Check idCmd, it must be 0 since we have only one menu item.

	if (0 != idCmd)
		return E_INVALIDARG;

	// If Explorer is asking for a help string, copy our string into the

	// supplied buffer.

	if (uFlags & GCS_HELPTEXT)
	{
		LPCTSTR szText = _T("PowerContextMenu Help String");

		if (uFlags & GCS_UNICODE)
		{
			// We need to cast pszName to a Unicode string, and then use the

			// Unicode string copy API.

			lstrcpynW ((LPWSTR) pszName, T2CW(szText), cchMax);
		}
		else
		{
			// Use the ANSI string copy API to return the help string.

			lstrcpynA (pszName, T2CA(szText), cchMax);
		}

		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT CPowerContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
	// If lpVerb really points to a string, ignore this function call and bail out.
	if (0 != HIWORD(pCmdInfo->lpVerb))
		return E_INVALIDARG;

	// Get the command index - the only valid one is 0.
	switch (LOWORD(pCmdInfo->lpVerb))
	{
	case 0:
		{
			TCHAR szMsg[MAX_PATH + 32];
			wsprintf (szMsg, _T("The selected file was:\n\n%s"), m_szFile);
			MessageBox (pCmdInfo->hwnd, szMsg, _T("PowerContextMenu"), MB_ICONINFORMATION);
			return S_OK;
		}
		break;

	default:
		return E_INVALIDARG;
		break;
	}
}
