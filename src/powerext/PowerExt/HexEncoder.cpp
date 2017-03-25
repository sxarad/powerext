#include "StdAfx.h"
#include "HexEncoder.h"

/// <summary>
/// Initializes a new instance of the HexEncoder.
/// </summary>
HexEncoder::HexEncoder()
{
}

/// <summary>
/// HexEncoder destructor.
/// </summary>
HexEncoder::~HexEncoder()
{
}

/// <summary>
/// Converts a BYTE buffer into a HEX encoded string.
/// </summary>
/// <param name="buffer">The BYTE buffer to encode.</param>
/// <param name="size">The size of the BYTE buffer.</param>
/// <returns>Returns the HEX encoded value as string.</returns>
std::wstring HexEncoder::Encode(PBYTE buffer, size_t size)
{
	std::wstringstream str;
	str.setf(std::ios_base::hex, std::ios::basefield);
	str.fill(_T('0'));

	for (size_t i = 0; i < size; ++i)
	{
		str << std::hex << std::setw(2) << (unsigned short)buffer[i];
	}

	return str.str(); 
}
