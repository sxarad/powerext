#include "StdAfx.h"

#include "AssemblyName.h"
#include "PEImage.h"
#include "DotNetAssembly.h"
#include "AssemblyInfo.h"
#include "HResultDecoder.h"

#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <atlbase.h>
#include <comdef.h>

/// <summary>
/// Initializes a new instance of the AssemblyName  class with the specified assembly file path.
/// </summary>
/// <param name="path">The path of the Assembly.</param>
AssemblyName::AssemblyName(const std::wstring& path)
{
	//MessageBox(nullptr, path.c_str(), L"PowerExt DEBUG", MB_OK);

	HANDLE hFile = CreateFile(path.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE) // Checks if the file exists
	{
		throw std::runtime_error("Error: File not found.");
	}
	
	CloseHandle(hFile);

	//MessageBox(nullptr, L"Before IsManagedImage", L"PowerExt DEBUG", MB_OK);

	if (!PEImage::IsManagedImage(path)) // Makes sure the binary is a valid .NET assembly
	{
		throw std::runtime_error("Error: Not a managed assembly.");
	}

	_path = path;

	//MessageBox(nullptr, L"Before CreateMetaHost", L"PowerExt DEBUG", MB_OK);

	bool bSkipRuntimeHost = false;
	DotNetAssembly dna;
	if (!dna.CreateMetaHost())
	{
		if (dna.GetLastHResult() == E_NOTIMPL)
		{
			bSkipRuntimeHost = true;
			//MessageBox(nullptr, L"Before CreateMetaDataDispenser(false)", L"PowerExt DEBUG", MB_OK);
			dna.CreateMetaDataDispenser(bSkipRuntimeHost);
		}
		else
		{
			throw HResultDecoder(dna.GetLastHResult());
		}
	}

	if (!bSkipRuntimeHost)
	{
		//MessageBox(nullptr, L"Before EnumerateRuntimes", L"PowerExt DEBUG", MB_OK); 

		if (!dna.EnumerateRuntimes())
			throw HResultDecoder(dna.GetLastHResult());

		//MessageBox(nullptr, L"Before CreateRuntimeInfo", L"PowerExt DEBUG", MB_OK);

		if (!dna.CreateRuntimeInfo())
			throw HResultDecoder(dna.GetLastHResult());

		//MessageBox(nullptr, L"Before CreateMetaDataDispenser", L"PowerExt DEBUG", MB_OK);

		if (!dna.CreateMetaDataDispenser())
			throw HResultDecoder(dna.GetLastHResult());
	}

	//MessageBox(nullptr, L"Before CreateAssemblyImport", L"PowerExt DEBUG", MB_OK);

	if (!dna.CreateAssemblyImport(path))
		throw HResultDecoder(dna.GetLastHResult());

	//MessageBox(nullptr, L"Before ReadAssemblyProperties", L"PowerExt DEBUG", MB_OK);

	AssemblyInfo assemblyInfo = dna.ReadAssemblyProperties();
	if (!assemblyInfo.IsOk())
		throw HResultDecoder(dna.GetLastHResult());

	_name = assemblyInfo.Name();
	_culture = assemblyInfo.Culture();
	_processorArchitecture = assemblyInfo.ProcessorArchitecture();
	_publicKey = assemblyInfo.PublicKey();
	_publicKeyToken = assemblyInfo.PublicKeyToken();
	_version = assemblyInfo.Version();
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
