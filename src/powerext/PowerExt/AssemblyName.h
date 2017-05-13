#pragma once

#include <string>

#include <cor.h>
#include <metahost.h>
#pragma comment(lib, "mscoree.lib")

/// <summary>
/// AssemblyName class tries to find out Assembly information.
/// </summary>
class AssemblyName
{
protected:
	// Internal members
	std::wstring _path;
	std::wstring _name;
	std::wstring _version;
	std::wstring _culture;
	std::wstring _publicKeyToken;
	std::wstring _publicKey;
	std::wstring _processorArchitecture;
private:
	// Internal methods
	std::wstring GetVersionFromAssemblyMetaData(ASSEMBLYMETADATA& assemblyMetaData);
	std::wstring GetCultureFromAssemblyMetaData(ASSEMBLYMETADATA& assemblyMetaData);
	std::wstring GetProcessorArchitectureFromFlags(DWORD pdwAssemblyFlags);
public:
	// ctor and dtor
	AssemblyName(const std::wstring& path);
	~AssemblyName();
public:
	// Getters
	const std::wstring& GetPath() { return _path; }
	const std::wstring& GetName() { return _name; }
	const std::wstring& GetVersion() { return _version; }
	const std::wstring& GetCulture() { return _culture; }
	const std::wstring& GetPublicKeyToken() { return _publicKeyToken; }
	const std::wstring& GetPublicKey() { return _publicKey; }
	const std::wstring& GetProcessorArchitecture() { return _processorArchitecture; }
	std::wstring GetFullName();
};
