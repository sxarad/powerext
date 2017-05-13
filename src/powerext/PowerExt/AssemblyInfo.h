#pragma once

#include <string>


class AssemblyInfo
{
public:
	AssemblyInfo() : _isOk(false)
	{
	}

	AssemblyInfo(std::wstring name, std::wstring version, std::wstring culture, 
		std::wstring publicKey, std::wstring publicKeyToken, 
		std::wstring processorArchitecture) :
			_isOk(true),
			_name(name),
			_version(version),
			_culture(culture),
			_publicKey(publicKey),
			_publicKeyToken(publicKeyToken),
			_processorArchitecture(processorArchitecture)
	{
	}

	bool IsOk() const { return _isOk; }
	std::wstring Name() const { return _name; }
	std::wstring Version() const { return _version; }
	std::wstring Culture() const { return _culture; }
	std::wstring PublicKey() const { return _publicKey; }
	std::wstring PublicKeyToken() const { return _publicKeyToken; }
	std::wstring ProcessorArchitecture() const { return _processorArchitecture; }

private:
	bool _isOk;
	std::wstring _name;
	std::wstring _version;
	std::wstring _culture;
	std::wstring _publicKey;
	std::wstring _publicKeyToken;
	std::wstring _processorArchitecture;
};
