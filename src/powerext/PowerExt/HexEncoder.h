#pragma once

#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

/// <summary>
/// Simple HEX encoder class to encode BYTE array to HEX string.
/// </summary>
class HexEncoder
{
private:
	HexEncoder();
	~HexEncoder();
public:
	static std::wstring Encode(PBYTE buffer, size_t size);
};
