#include <win/application.hxx>
[!if NTL_RUNTIME_CRT]
#include <cstdlib>
[!endif]
using namespace ntl::win;

#pragma comment(linker, "/entry:[!output SAFE_PROJECT_NAME]Entry")
boolean __stdcall [!output SAFE_PROJECT_NAME]Entry(ntl::pe::image* self, DllMainReason reason, const void*)
{
[!if NTL_RUNTIME_CRT]
  if(reason == DllMainReason::DllProcessAttach){
    ntl::__init_crt(true);
  }else if(reason == DllMainReason::DllProcessDetach){
    ntl::__init_crt(false);
  }
[!endif]
  return true;
}
