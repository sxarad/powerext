// DnaInfo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\PowerExt\AssemblyName.h"

#include <stdio.h>

#include <iostream>
#include <string>

using namespace std;


int _tmain(int argc, TCHAR* argv[])
{
	wcout << _T("DnaInfo.exe (.NET Assembly Information)") << endl << endl;

	if (argc < 2)
	{
		wcout << _T("Usage:") << endl;
		wcout << endl;
		wcout << _T("DnaInfo.exe <assembly>") << endl;
		wcout << endl;
		wcout << _T(" <assembly> = Path to a .NET dll or exe") << endl;
		return 1;
	}

	wstring assemblyFile(argv[1]);
	wcout << assemblyFile << endl << endl;

	try
	{
		AssemblyName assemblyName(assemblyFile);
		std::wstring fullName = assemblyName.GetFullName();
		std::wcout << endl << endl << _T("FullName:") << endl;
		std::wcout << fullName << endl << endl;
	}
	catch (std::exception& e)
	{
		wcout << endl << _T("ERROR") << endl;
		wcout << e.what() << endl << endl ;
	}

	return 0;
}
