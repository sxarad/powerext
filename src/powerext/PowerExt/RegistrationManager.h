#pragma once

#include <iostream>
#include <string>

class RegistrationManager
{
public:
	RegistrationManager();
	~RegistrationManager();
public:
	bool RegisterPropertySheetHandler(std::wstring extension, std::wstring name, std::wstring guid, bool remove);
};
