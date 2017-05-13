#pragma once

#include <string>
#include <iomanip>
#include <windows.h>
#include <comdef.h>
#include <sstream>


class HResultDecoder
{
public:
	HResultDecoder(HRESULT hr)
	{
		_com_error comError(hr);
		_errorCode = comError.WCode();
		_hexErrorCode = ToHexString<WORD>(_errorCode);
		_errorMessage = std::wstring(comError.ErrorMessage());
	}

	WORD GetErrorCode() const { return _errorCode; }
	std::wstring GetHexErrorCode() const { return _hexErrorCode; }
	std::wstring GetErrorMessage() const { return _errorMessage; }


private:
	template<typename T>
	static std::wstring ToHexString(T i)
	{
		std::wstringstream builder;
		builder << _T("0x")
			<< std::setfill(_T('0')) << std::setw(sizeof(T) * 2)
			<< std::hex << i;
		return builder.str();
	}

	WORD _errorCode;
	std::wstring _hexErrorCode;
	std::wstring _errorMessage;
};
