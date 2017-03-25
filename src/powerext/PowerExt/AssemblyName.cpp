#include "StdAfx.h"
#include "AssemblyName.h"
#include "PEImage.h"
#include "StrongName.h"

#include <iostream>
#include <sstream>

/// <summary>
/// Initializes a new instance of the AssemblyName  class with the specified assembly file path.
/// </summary>
/// <param name="path">The path of the Assembly.</param>
AssemblyName::AssemblyName(const std::wstring& path)
{
	_path = _T("");
	_name = _T("");
	_version = _T("");
	_culture = _T("");
	_publicKeyToken = _T("");
	_publicKey = _T("");
	_processorArchitecture = _T("");

	HANDLE hFile = CreateFile(path.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0); 
	if (hFile == INVALID_HANDLE_VALUE) // Checks if the file exists
	{
		throw std::runtime_error("Error: File not found.");
	}
	else
	{
		CloseHandle(hFile);
	}

	if (!PEImage::IsManagedImage(path)) // Makes sure the binary is a valid .NET assembly
	{
		throw std::runtime_error("Error: Not a managed assembly.");
	}

	_path = path;

	CoInitialize(NULL);

	// Use the IMetaDataDispenser COM interfaces published by the .NET Framework to get Assemly information
	IMetaDataDispenser* pIMetaDataDispenser = NULL;
	HRESULT hr = CoCreateInstance(CLSID_CorMetaDataDispenser, 0, CLSCTX_INPROC_SERVER,
		IID_IMetaDataDispenser, (LPVOID*)&pIMetaDataDispenser);
	if (SUCCEEDED(hr))
	{
		IMetaDataAssemblyImport* pIMetaDataAssemblyImport = NULL;
		hr = pIMetaDataDispenser->OpenScope(_path.c_str(), ofRead, IID_IMetaDataAssemblyImport,
			(LPUNKNOWN*)&pIMetaDataAssemblyImport);
		if (SUCCEEDED(hr))
		{
			mdAssembly mda;
			hr = pIMetaDataAssemblyImport->GetAssemblyFromScope(&mda);
			if (SUCCEEDED(hr))
			{
				TCHAR				szName[MAX_PATH];
				ULONG				cchName = MAX_PATH;
				ULONG				pchName = 0;
				ASSEMBLYMETADATA	pMetaData;
				DWORD				pdwAssemblyFlags = 0;

				memset(szName, '\0', MAX_PATH);
				memset(&pMetaData, '\0', sizeof(ASSEMBLYMETADATA));

				hr = pIMetaDataAssemblyImport->GetAssemblyProps(
					mda, 0, 0, 0, (LPWSTR)szName, cchName, 
					&pchName, &pMetaData, &pdwAssemblyFlags);

				if (SUCCEEDED(hr))
				{
					StrongName strongName(_path);
					_name = std::wstring(szName);
					_version = GetVersionFromAssemblyMetaData(pMetaData);
					_culture = GetCultureFromAssemblyMetaData(pMetaData);
					_publicKey = strongName.GetPublicKey();
					_publicKeyToken = strongName.GetPublicKeyToken();
					_processorArchitecture = GetProcessorArchitectureFromFlags(pdwAssemblyFlags);
				}
				else
				{
					std::wcout << _T("Error: IMetaDataAssemblyImport->GetAssemblyProps() failed.") << std::endl;
				}
			}
			else
			{
				std::wcout << _T("Error: IMetaDataAssemblyImport->GetAssemblyFromScope() failed.") << std::endl;
			}
			pIMetaDataAssemblyImport->Release();
		}
		else
		{
			std::wcout << _T("Error: IMetaDataDispenser->OpenScope() failed.") << std::endl;
		}
		pIMetaDataDispenser->Release();
	}
	else
	{
		std::wcout << _T("Error: CoCreateInstance() failed.") << std::endl;
	}

	CoUninitialize();
}

/// <summary>
/// AssemblyName destructor.
/// </summary>
AssemblyName::~AssemblyName()
{
}

/// <summary>
/// Gets the full name (strong name) of the Assembly.
/// <c>
/// Example of Assembly Full Name:
/// mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089
/// </c>
/// </summary>
/// <returns>Returns the full name (strong name) of the Assembly</returns>.
std::wstring AssemblyName::GetFullName()
{
	std::wstringstream fullNameBuilder;
	fullNameBuilder << _name;

	if (_version.length() > 0) // Without the assembly version, all the other information would be pointless
	{
		fullNameBuilder << _T(", Version=") << _version;
	
		fullNameBuilder << _T(", Culture=");
		if (_culture.length() > 0)
		{
			fullNameBuilder << _culture;
		}
		else
		{
			fullNameBuilder << _T("neutral");
		}

		fullNameBuilder << _T(", PublicKeyToken=");
		if (_publicKeyToken.length() > 0)
		{
			fullNameBuilder << _publicKeyToken;
		}
		else
		{
			fullNameBuilder << _T("null");
		}

		if (_processorArchitecture.length() > 0)
		{
			fullNameBuilder << _T(", ProcessorArchitecture=") << _processorArchitecture;
		}
	}

	return fullNameBuilder.str();
}

/// <summary>
/// Gets Assembly Version of the Assembly. Assembly version is usually made up of 
/// four parts - major, minor, build and revision.
/// </summary>
/// <param name="assemblyMetaData">
/// The ASSEMBLYMETADATA returned by the IMetaDataAssemblyImport->GetAssemblyProps() method. 
/// </param>
/// <returns>Returns the Assembly version as string.</returns>
std::wstring AssemblyName::GetVersionFromAssemblyMetaData(ASSEMBLYMETADATA& assemblyMetaData)
{
	std::wstringstream versionBuilder;
	versionBuilder.setf(std::ios_base::dec, std::ios::basefield);

	versionBuilder << assemblyMetaData.usMajorVersion << ".";
	versionBuilder << assemblyMetaData.usMinorVersion << ".";
	versionBuilder << assemblyMetaData.usBuildNumber << ".";
	versionBuilder << assemblyMetaData.usRevisionNumber;

	return versionBuilder.str(); 
}

/// <summary>
/// Gets Assembly Culture of the Assembly. If the Culture value is empty then 'neutral' assumed.
/// </summary>
/// <param name="assemblyMetaData">
/// The ASSEMBLYMETADATA returned by the IMetaDataAssemblyImport->GetAssemblyProps() method. 
/// </param>
/// <returns>Returns the Assembly Culture as string.</returns>
std::wstring AssemblyName::GetCultureFromAssemblyMetaData(ASSEMBLYMETADATA& assemblyMetaData)
{
	std::wstring culture(_T("neutral"));
	if (assemblyMetaData.szLocale != NULL)
	{
		culture = assemblyMetaData.szLocale;
	}

	return culture;
}

/// <summary>
/// Gets Processor Architecture of the Assembly.
/// </summary>
/// <param name="assemblyMetaData">
/// The pdwAssemblyFlags value returned by the IMetaDataAssemblyImport->GetAssemblyProps() method. 
/// </param>
/// <returns>Returns the Assembly Processor Architecture as string.</returns>
std::wstring AssemblyName::GetProcessorArchitectureFromFlags(DWORD pdwAssemblyFlags)
{
	std::wstring processorArchitecture;
	if (pdwAssemblyFlags & afPA_Mask)
	{
		if (pdwAssemblyFlags & afPA_MSIL)
			processorArchitecture = _T("MSIL");
		else if (pdwAssemblyFlags & afPA_x86)
			processorArchitecture = _T("x86");
		else if (pdwAssemblyFlags & afPA_IA64)
			processorArchitecture = _T("IA64");
		else if (pdwAssemblyFlags & afPA_AMD64)
			processorArchitecture = _T("AMD64");
	}

	return processorArchitecture;
}
