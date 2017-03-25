// dllmain.h : Declaration of module class.

class CPowerExtModule : public CAtlDllModuleT< CPowerExtModule >
{
public :
	DECLARE_LIBID(LIBID_PowerExtLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_POWEREXT, "{4124BB18-CDB6-4A0B-A6FD-0B698FF03DA5}")
};

extern class CPowerExtModule _AtlModule;
