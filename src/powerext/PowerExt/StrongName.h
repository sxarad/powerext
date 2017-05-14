#pragma once

#include <windows.h>
#include <string>

typedef BOOLEAN  (__stdcall *StrongNameTokenFromAssemblyEx)(LPCWSTR, BYTE**, ULONG*, BYTE**, ULONG*);
typedef VOID (__stdcall *StrongNameFreeBuffer)(BYTE*);
typedef HRESULT (__stdcall *StrongNameErrorInfo)();

/// <summary>
/// StrongName class tries to use the StrongName APIs to find out the Public Key and Public Key token.
/// </summary>
class StrongName
{
public:
	StrongName(std::wstring publicKey, std::wstring publicKeyToken);
	StrongName(std::wstring path);
	~StrongName();
	std::wstring GetPublicKey() const;
	std::wstring GetPublicKeyToken() const;
private:
	std::wstring _publicKey;
	std::wstring _publicKeyToken;
};
