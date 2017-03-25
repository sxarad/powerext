#include "StdAfx.h"
#include "RegistrationManager.h"

RegistrationManager::RegistrationManager()
{
}

RegistrationManager::~RegistrationManager()
{
}

bool RegistrationManager::RegisterPropertySheetHandler(std::wstring extension, 
	std::wstring name, std::wstring guid, bool remove)
{
	bool returnValue = false;
	CRegKey reg;
	LONG result = reg.Open(HKEY_CLASSES_ROOT, extension.c_str());
	if (ERROR_SUCCESS == result)
	{
		TCHAR szValue[MAX_PATH];
		DWORD nChars = MAX_PATH;
		memset(szValue, '\0', MAX_PATH);
		result = reg.QueryStringValue(NULL, (LPTSTR)szValue, &nChars);
		if (ERROR_SUCCESS == result)
		{
			std::wstring propsheetkey(szValue);
			propsheetkey += _T("\\ShellEx\\PropertySheetHandlers\\");
			std::wstring filekey(propsheetkey);
			filekey += name;
			if (!remove)
			{
				CRegKey reg1;
				result = reg1.Create(HKEY_CLASSES_ROOT, filekey.c_str());
				if (ERROR_SUCCESS == result)
				{
					result = reg1.SetStringValue(NULL, guid.c_str()); 
					if (ERROR_SUCCESS == result)
					{
						returnValue = true;
					}
				}
			}
			else
			{
				CRegKey reg2;
				result = reg2.Open(HKEY_CLASSES_ROOT, propsheetkey.c_str(), KEY_READ | KEY_WRITE);
				if (result == ERROR_SUCCESS)
				{
					result = reg2.DeleteSubKey(name.c_str());
					if (result == ERROR_SUCCESS)
					{
						returnValue = true;
					}
				}
			}
		}
	}

	return false;
}
