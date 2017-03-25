#include "StdAfx.h"
#include "PEImage.h"

/// <summary>
/// Initializes a new instance of the PEImage class with the specified assembly file path.
/// PE stands for Portable Executable. All Windows executables and .NET Assemblies are 
/// PE files.
/// </summary>
/// <param name="path">The path of the Assembly.</param>
PEImage::PEImage(std::wstring path)
{
	_path = path;
}

/// <summary>
/// PEImage destructor.
/// </summary>
PEImage::~PEImage()
{
}

/// <summary>
/// Static method to check if a particular file is a value .NET Assembly or not.
/// </summary>
/// <param name="path">The path of the Assembly.</param>
/// <returns>Returns true if the file is a valid .NET Assembly; false otherwise.</returns>
bool PEImage::IsManagedImage(std::wstring path)
{
	bool isManaged = false;

	PEImage instance(path);
	
	LOADED_IMAGE_DATA imageData;
    HRESULT hr = instance.MapImage(&imageData);
    
    if (FAILED(hr))
    {
		throw std::exception("Error: Failed to check image.", (int)hr);
    }

    PIMAGE_DATA_DIRECTORY pDataDirectories = NULL;
    
    switch (imageData.pNtHeader->OptionalHeader.Magic)
    {
        case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
            pDataDirectories = reinterpret_cast<PIMAGE_NT_HEADERS32>(
                imageData.pNtHeader)->OptionalHeader.DataDirectory;
            break;

        case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            pDataDirectories = reinterpret_cast<PIMAGE_NT_HEADERS64>(
                imageData.pNtHeader)->OptionalHeader.DataDirectory;
            break;
        
        default:
		    instance.UnmapImage(&imageData);    
			throw std::runtime_error("Error: Unrecognized Magic value in NT header.");
    }
    
    // Make sure there is a CLR header
    _ASSERTE(pDataDirectories != NULL);

    if (0 == pDataDirectories[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].VirtualAddress ||
        0 == pDataDirectories[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].Size)
    {
	    instance.UnmapImage(&imageData);    
		throw std::runtime_error("Error: Invalid or missing CLR header values.");
    }

    // Get the CLR header
    PIMAGE_COR20_HEADER pCorHeader = reinterpret_cast<PIMAGE_COR20_HEADER>(
        RVA2VA(&imageData, pDataDirectories[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].VirtualAddress));
    
    if (NULL == pCorHeader)
    {
	    instance.UnmapImage(&imageData);    
		throw std::runtime_error("Error: Invalid or missing CLR header values.");
    }
	else
	{
		isManaged = true;
	}

	// Clean-up
    instance.UnmapImage(&imageData);    

	return isManaged;
}

/// <summary>
/// Unmaps and unloads an already mapped image from memory.
/// </summary>
/// <param name="pImageData">Infomration about the loaded PE image.</param>
void PEImage::UnmapImage(PLOADED_IMAGE_DATA pImageData)
{
	if (NULL != pImageData->pBase)
        UnmapViewOfFile(pImageData->pBase);
    
    if (NULL != pImageData->hMap)
        CloseHandle(pImageData->hMap);
        
    if (INVALID_HANDLE_VALUE != pImageData->hFile)
        CloseHandle(pImageData->hFile);
}

/// <summary>
/// Maps and loads a PE image from file system into memory.
/// </summary>
/// <param name="pImageData">Infomration about the loaded PE image.</param>
/// <returns>Returns HRESULT value indicating success or failure.</returns>
HRESULT PEImage::MapImage(PLOADED_IMAGE_DATA pImageData)
{
	memset(pImageData, '\0', sizeof(LOADED_IMAGE_DATA));
    HRESULT hr = S_FALSE;

    pImageData->hFile = CreateFile(_path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (pImageData->hFile != INVALID_HANDLE_VALUE)
    {
        if (NULL != (pImageData->hMap = CreateFileMapping(pImageData->hFile, NULL, PAGE_READONLY, 0, 0, NULL)))
        {
            if (NULL != (pImageData->pBase = MapViewOfFile(pImageData->hMap, FILE_MAP_READ, 0, 0, 0)))
            {
                if (NULL != (pImageData->pNtHeader = ImageNtHeader(pImageData->pBase)))
				{
                    return S_OK;
				}
                else
				{
					std::wcout << _T("Error: ImageNtHeader() failed.") << std::endl;
				}
            }
			else
			{
				std::wcout << _T("Error: MapViewOfFile() failed.") << std::endl;
			}
        }
		else
		{
			std::wcout << _T("Error: CreateFileMapping() failed.") << std::endl;
		}
    }
	else
	{
		std::wcout << _T("Error: CreateFile() failed.") << std::endl;
	}

	if (hr != S_OK)
	{
		hr = HRESULT_FROM_WIN32(ERROR_BAD_FORMAT);
		std::wcout << _T("Error: HRESULT = ") << hr << std::endl;
	}
    
    UnmapImage(pImageData);
    
	return hr;
}
