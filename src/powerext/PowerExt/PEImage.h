#pragma once

#include <windows.h>
#include <dbghelp.h>

#include <iostream>
#include <string>

#define RVA2VA(pdata, rva) ImageRvaToVa((pdata)->pNtHeader, (pdata)->pBase, (rva), NULL)

/// <summary>
/// Internal structure to hold PE image data while its loaded into memory.
/// </summary>
typedef struct
{
    HANDLE hFile;
    HANDLE hMap;
    PVOID pBase;
    PIMAGE_NT_HEADERS pNtHeader;
} LOADED_IMAGE_DATA, *PLOADED_IMAGE_DATA;

/// <summary>
/// PE (Portable Image) analyzer class. Tries to detect .NET Assemblies.
/// </summary>
class PEImage
{
protected:
	std::wstring _path;
private:
	PEImage(std::wstring path);
	~PEImage();
	void UnmapImage(PLOADED_IMAGE_DATA pImageData);
	HRESULT MapImage(PLOADED_IMAGE_DATA pImageData);
public:
	static bool IsManagedImage(std::wstring path);
};
