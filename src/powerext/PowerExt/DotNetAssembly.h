#pragma once

#include <vector>
#include <windows.h>
#include <atlbase.h>
#include <MetaHost.h>
#include <cor.h>

class AssemblyInfo;
class StrongName;

class DotNetAssembly
{
public:
	DotNetAssembly();
	~DotNetAssembly();
	bool CreateMetaHost();
	bool EnumerateRuntimes();
	bool CreateRuntimeInfo();
	bool CreateMetaDataDispenser(bool bSkipHostRuntime = false);
	bool CreateAssemblyImport(std::wstring assemblyPath);
	StrongName CreateAssemblyStrongName();
	AssemblyInfo ReadAssemblyProperties();
	static std::wstring GetVersionFromAssemblyMetaData(ASSEMBLYMETADATA& assemblyMetaData);
	static std::wstring GetCultureFromAssemblyMetaData(ASSEMBLYMETADATA& assemblyMetaData);
	static std::wstring GetProcessorArchitectureFromFlags(DWORD pdwAssemblyFlags);
	HRESULT GetLastHResult() const;

private:
	HRESULT _hr;
	std::vector<std::wstring> _runtimeVersions;
	std::wstring _assemblyPath;

	CComPtr<ICLRMetaHost> _metaHost;
	CComPtr<ICLRRuntimeInfo> _runtimeInfo;
	CComPtr<IMetaDataDispenserEx> _metaDataDispenser;
	CComPtr<IMetaDataAssemblyImport> _assemblyImport;
	CComPtr<ICLRStrongName> _strongName;
};

