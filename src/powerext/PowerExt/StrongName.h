#pragma once

#include <windows.h>
#include <iostream>
#include <string>

typedef BOOLEAN  (__stdcall *StrongNameTokenFromAssemblyEx)(LPCWSTR, BYTE**, ULONG*, BYTE**, ULONG*);
typedef VOID (__stdcall *StrongNameFreeBuffer)(BYTE*);
typedef HRESULT (__stdcall *StrongNameErrorInfo)();

/// <summary>
/// StrongName class tries to use the StrongName APIs to find out the Public Key and Public Key token.
/// </summary>
class StrongName
{
protected:
	std::wstring _publicKey;
	std::wstring _publicKeyToken;
public:
	StrongName(std::wstring path);
	~StrongName();
	std::wstring GetPublicKey();
	std::wstring GetPublicKeyToken();
};
