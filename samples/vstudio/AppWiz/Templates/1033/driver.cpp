#include <km/basedef.hxx>
#include <km/driver_object.hxx>
#include <km/new.hxx>
[!if NTL_RUNTIME_CRT]
#include <cstdlib>
[!endif]
using namespace ntl;
using namespace ntl::km;

static void __stdcall DrvUnload(ntl::km::driver_object*)
{
[!if NTL_RUNTIME_CRT]
	// Cleanup CRT
	ntl::__init_crt(false);
[!endif]
}

#pragma comment(linker, "/entry:DrvEntry")
ntstatus DrvEntry(ntl::km::driver_object* DriverObject, ntl::km::const_unicode_string* /*RegistryPath*/)
{
[!if NTL_RUNTIME_CRT]
	// Initialize runtime. 
	ntl::__init_crt(true);
[!endif]
  DriverObject->DriverUnload = DrvUnload;

	return status::success;
}
