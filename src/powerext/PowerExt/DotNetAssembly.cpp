#include "stdafx.h"
#include "AssemblyInfo.h"
#include "DotNetAssembly.h"
#include "StrongName.h"
#include "../PowerExt/HexEncoder.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <atlbase.h>
#include <MetaHost.h>
#include <cor.h>


DotNetAssembly::DotNetAssembly()
{
	_hr = CoInitialize(nullptr);
}

DotNetAssembly::~DotNetAssembly()
{
	CoUninitialize();
}

bool DotNetAssembly::CreateMetaHost()
{
	bool result = _metaHost != nullptr;
	if (!result)
	{
		_hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, reinterpret_cast<LPVOID*>(&_metaHost));
		result = SUCCEEDED(_hr) && _metaHost != nullptr;
	}

	return result;
}

bool DotNetAssembly::EnumerateRuntimes()
{
	bool result = !_runtimeVersions.empty();
	if (_metaHost != nullptr)
	{
		IEnumUnknown* runtimesEnum;
		_hr = _metaHost->EnumerateInstalledRuntimes(&runtimesEnum);
		if (SUCCEEDED(_hr))
		{
			_runtimeVersions.clear();

			ICLRRuntimeInfo* runtimeInfo = nullptr;
			ULONG fetched = 0;
			HRESULT hr;
			while ((hr = runtimesEnum->Next(1, reinterpret_cast<IUnknown**>(&runtimeInfo), &fetched)) == S_OK && fetched > 0)
			{
				wchar_t version[20];
				DWORD versionStringSize = sizeof(version) / sizeof(wchar_t);
				hr = runtimeInfo->GetVersionString(version, &versionStringSize);
				if (SUCCEEDED(hr))
				{
					_runtimeVersions.push_back(version);
				}
			}

			if (!_runtimeVersions.empty())
			{
				std::sort(_runtimeVersions.begin(), _runtimeVersions.end());
				result = true;
			}
		}
	}

	return result;
}

bool DotNetAssembly::CreateRuntimeInfo()
{
	bool result = _runtimeInfo != nullptr;
	if (_metaHost != nullptr && _runtimeInfo == nullptr)
	{
		_hr = _metaHost->GetRuntime(_runtimeVersions.back().c_str(), IID_ICLRRuntimeInfo, reinterpret_cast<LPVOID*>(&_runtimeInfo));
		result = SUCCEEDED(_hr);
	}

	return result;
}

bool DotNetAssembly::CreateMetaDataDispenser(bool bSkipHostRuntime)
{
	bool result = _metaDataDispenser != nullptr;
	if (bSkipHostRuntime)
	{
		// Legacy .NET 2.0 way of getting the IMetaDataDispenser reference
		_hr = CoCreateInstance(CLSID_CorMetaDataDispenser, nullptr, CLSCTX_INPROC_SERVER,
			IID_IMetaDataDispenser, reinterpret_cast<LPVOID*>(&_metaDataDispenser));
		if (SUCCEEDED(_hr))
		{
			result = SUCCEEDED(_hr) && _metaDataDispenser != nullptr;
		}
	}
	else
	{
		if (_runtimeInfo != nullptr && _metaDataDispenser == nullptr)
		{
			_hr = _runtimeInfo->GetInterface(CLSID_CorMetaDataDispenser, IID_IMetaDataDispenserEx, reinterpret_cast<LPVOID*>(&_metaDataDispenser));
			result = SUCCEEDED(_hr);
		}
	}

	return result;
}

bool DotNetAssembly::CreateAssemblyImport(std::wstring assemblyPath)
{
	bool result = _assemblyImport != nullptr;
	if (_metaDataDispenser != nullptr && _assemblyImport == nullptr)
	{
		_assemblyPath = assemblyPath;
		_hr = _metaDataDispenser->OpenScope(assemblyPath.c_str(), ofReadOnly, IID_IMetaDataAssemblyImport, reinterpret_cast<LPUNKNOWN*>(&_assemblyImport));
		result = SUCCEEDED(_hr);
	}

	return result;
}

StrongName DotNetAssembly::CreateAssemblyStrongName()
{
	if (_runtimeInfo != nullptr && _strongName == nullptr)
	{
		_hr = _runtimeInfo->GetInterface(CLSID_CLRStrongName, IID_ICLRStrongName, reinterpret_cast<LPVOID*>(&_strongName));
		if (SUCCEEDED(_hr))
		{
			if (_strongName != nullptr)
			{
				PBYTE ppbStrongNameToken = nullptr;
				ULONG pcbStrongNameToken = 0;
				PBYTE ppbPublicKeyBlob = nullptr;
				ULONG pcbPublicKeyBlob = 0;

				_hr = _strongName->StrongNameTokenFromAssemblyEx(_assemblyPath.c_str(),
					&ppbStrongNameToken,
					&pcbStrongNameToken,
					&ppbPublicKeyBlob,
					&pcbPublicKeyBlob);
				if (SUCCEEDED(_hr)) // 0x8013141b is returned if assembly doesn't have a strong name
				{
					std::wstring publicKey = HexEncoder::Encode(ppbPublicKeyBlob, pcbPublicKeyBlob);
					std::wstring publicKeyToken = HexEncoder::Encode(ppbStrongNameToken, pcbStrongNameToken);
					return StrongName(publicKey, publicKeyToken);
				}
			}
		}
	}
	else
	{
		if (_runtimeInfo == nullptr && _strongName == nullptr)
		{
			// Legacy .NET 2.0 way of getting StrongName out
			return StrongName(_assemblyPath);
		}
	}

	return StrongName(L"", L"");
}

AssemblyInfo DotNetAssembly::ReadAssemblyProperties()
{
	mdAssembly mda;
	_hr = _assemblyImport->GetAssemblyFromScope(&mda);
	if (SUCCEEDED(_hr))
	{
		TCHAR				szName[MAX_PATH];
		ULONG				cchName = MAX_PATH;
		ULONG				pchName;
		ASSEMBLYMETADATA	pMetaData;
		DWORD				pdwAssemblyFlags;

		memset(szName, '\0', MAX_PATH);
		memset(&pMetaData, '\0', sizeof(ASSEMBLYMETADATA));

		const void* public_key;
		unsigned long publicKeySize;
		unsigned long hashAlgo;

		_hr = _assemblyImport->GetAssemblyProps(
			mda,
			nullptr, nullptr,    // Public Key.
			nullptr,          // Hash Algorithm.
			nullptr, 0, nullptr, // Name.
			&pMetaData,
			nullptr); // Flags.
		if (SUCCEEDED(_hr))
		{
			std::wstring name = L"";
			std::wstring version = L"";
			std::wstring culture = L"";
			std::wstring publicKey = L"";
			std::wstring publicKeyToken = L"";
			std::wstring processorArchitecture = L"";

			// Allocate space for the arrays in the ASSEMBLYMETADATA structure.
			if (pMetaData.cbLocale)
				pMetaData.szLocale = new WCHAR[pMetaData.cbLocale];
			if (pMetaData.ulProcessor)
				pMetaData.rProcessor = new DWORD[pMetaData.ulProcessor];
			if (pMetaData.ulOS)
				pMetaData.rOS = new OSINFO[pMetaData.ulOS];

			_hr = _assemblyImport->GetAssemblyProps(mda, &public_key, &publicKeySize, &hashAlgo, static_cast<LPWSTR>(szName), cchName, &pchName, &pMetaData, &pdwAssemblyFlags);
			if (SUCCEEDED(_hr))
			{
				StrongName sn = CreateAssemblyStrongName();
				name = std::wstring(szName);
				version = GetVersionFromAssemblyMetaData(pMetaData);
				culture = GetCultureFromAssemblyMetaData(pMetaData);
				publicKey = sn.GetPublicKey();
				publicKeyToken = sn.GetPublicKeyToken();
				processorArchitecture = GetProcessorArchitectureFromFlags(pdwAssemblyFlags);
			}

			if (pMetaData.szLocale)
				delete[] pMetaData.szLocale;
			if (pMetaData.rProcessor)
				delete[] pMetaData.rProcessor;
			if (pMetaData.rOS)
				delete[] pMetaData.rOS;

			AssemblyInfo assemblyInfo(name, version, culture, publicKey, publicKeyToken, processorArchitecture);
			return assemblyInfo;
		}
	}

	return AssemblyInfo(); // Empty
}

/// <summary>
/// Gets Assembly Version of the Assembly. Assembly version is usually made up of 
/// four parts - major, minor, build and revision.
/// </summary>
/// <param name="assemblyMetaData">
/// The ASSEMBLYMETADATA returned by the IMetaDataAssemblyImport->GetAssemblyProps() method. 
/// </param>
/// <returns>Returns the Assembly version as string.</returns>
std::wstring DotNetAssembly::GetVersionFromAssemblyMetaData(ASSEMBLYMETADATA& assemblyMetaData)
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
std::wstring DotNetAssembly::GetCultureFromAssemblyMetaData(ASSEMBLYMETADATA& assemblyMetaData)
{
	std::wstring culture(_T("neutral"));
	if (assemblyMetaData.szLocale != nullptr)
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
std::wstring DotNetAssembly::GetProcessorArchitectureFromFlags(DWORD pdwAssemblyFlags)
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

HRESULT DotNetAssembly::GetLastHResult() const
{
	return _hr;
}
