#include "StdAfx.h"
#include "StrongName.h"
#include "HexEncoder.h"

/// <summary>
/// Initializes a new instance of the StrongName class with the specified assembly file path.
/// </summary>
/// <param name="path">The path of the Assembly.</param>
StrongName::StrongName(std::wstring path)
{
	TCHAR wszFilePath[MAX_PATH];
	PBYTE ppbStrongNameToken = 0; // Allocated by StrongNameTokenFromAssemblyEx API
	ULONG pcbStrongNameToken = 0;  
	PBYTE ppbPublicKeyBlob = 0;   // Allocated by StrongNameTokenFromAssemblyEx API
	ULONG pcbPublicKeyBlob = 0;

	_publicKey = _T("");
	_publicKeyToken = _T("");

	memset(wszFilePath, '\0', MAX_PATH);

	HMODULE hModule = LoadLibrary(_T("mscoree"));
	if (hModule)
	{    
		StrongNameTokenFromAssemblyEx pStrongNameTokenFromAssemblyEx = 
			(StrongNameTokenFromAssemblyEx)GetProcAddress(hModule, "StrongNameTokenFromAssemblyEx");
		
		StrongNameFreeBuffer pStrongNameFreeBuffer = 
			(StrongNameFreeBuffer)GetProcAddress(hModule, "StrongNameFreeBuffer");
		
		StrongNameErrorInfo pStrongNameErrorInfo = 
			(StrongNameErrorInfo)GetProcAddress(hModule, "StrongNameErrorInfo");

		if (pStrongNameTokenFromAssemblyEx != NULL && pStrongNameFreeBuffer != NULL) 
		{
			wcscpy_s(wszFilePath, MAX_PATH, path.c_str());

			if (pStrongNameTokenFromAssemblyEx(wszFilePath, 
					&ppbStrongNameToken, 
					&pcbStrongNameToken, 
					&ppbPublicKeyBlob, 
					&pcbPublicKeyBlob))
			{
				_publicKey = HexEncoder::Encode(ppbPublicKeyBlob, pcbPublicKeyBlob);
				std::wcout << _T("Public Key:") << std::endl << _publicKey << std::endl;
				_publicKeyToken = HexEncoder::Encode(ppbStrongNameToken, pcbStrongNameToken);
				std::wcout << _T("Public Key Token:") << std::endl << _publicKeyToken << std::endl;
				
				// Don't forget to free the memory allocated by StrongNameTokenFromAssemblyEx API
				pStrongNameFreeBuffer(ppbStrongNameToken);
				pStrongNameFreeBuffer(ppbPublicKeyBlob);
			}
			else
			{
				std::wcout << _T("Error: StrongNameTokenFromAssemblyEx() failed.") << std::endl;
				std::wcout << _T("StrongNameErrorInfo() returned: ") << pStrongNameErrorInfo() << std::endl;
			}
		}
		else
		{
			std::wcout << _T("Error: Failed to acquire pointers for StrongNameTokenFromAssemblyEx and StrongNameFreeBuffer methods.") << std::endl;
		}

		FreeLibrary(hModule);
	}
	else
	{
		throw std::runtime_error("Error: LoadLibrary() failed.");
	}
}

/// <summary>
/// StrongName destructor.
/// </summary>
StrongName::~StrongName()
{
}

/// <summary>
/// Get the Public Key for the Assembly.
/// </summary>
/// <returns>Returns Public Key of the Assembly</returns>.
std::wstring StrongName::GetPublicKey()
{
	return _publicKey;
}

/// <summary>
/// Get the Public Key Token for the Assembly.
/// </summary>
/// <returns>Returns Public Key Token of the Assembly</returns>.
std::wstring StrongName::GetPublicKeyToken()
{
	return _publicKeyToken;
}
