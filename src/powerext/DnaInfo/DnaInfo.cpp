// DnaInfo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../PowerExt/AssemblyName.h"
#include "../PowerExt/HResultDecoder.h"

#include <iostream>
#include <string>

int _tmain(int argc, TCHAR* argv[])
{
	std::wcout << _T("DnaInfo.exe (.NET Assembly Information)") << std::endl << std::endl;

	if (argc < 2)
	{
		std::wcout << _T("Usage:") << std::endl;
		std::wcout << std::endl;
		std::wcout << _T("DnaInfo.exe <assembly>") << std::endl;
		std::wcout << std::endl;
		std::wcout << _T(" <assembly> = Path to a .NET dll or exe") << std::endl;
		return 1;
	}

	std::wstring assemblyFile(argv[1]);
	std::wcout << assemblyFile << std::endl << std::endl;

	try
	{
		AssemblyName assemblyName(assemblyFile);
		std::wstring fullName = assemblyName.GetFullName();
		std::wcout << std::endl << std::endl << _T("FullName:") << std::endl;
		std::wcout << fullName << std::endl << std::endl;
	}
	catch (std::exception& e)
	{
		std::wcout << L"Error: " << e.what() << std::endl << std::endl ;
	}
	catch (HResultDecoder e)
	{
		std::wcout << L"Error: " << e.GetErrorCode()
			<< L" (" << e.GetHexErrorCode() << L") -- " 
			<< e.GetErrorMessage() << std::endl << std::endl;
	}

	return 0;
}
