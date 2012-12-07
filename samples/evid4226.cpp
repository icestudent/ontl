//  NTL samples library
//  EventID 4226 patch by S.T.A.S.
//
//  compile:
//      cl /Ox evid4226.cpp /Feevid4226.sys /link /align:32 /driver /machine:x86
//
//  install:
//      1. move evid4226.sys %windir%\system32\drivers\
//      2. sc create evid4226 binPath= %windir%\system32\drivers\evid4226.sys type= kernel start= auto error= ignore
//
//  manual start:
//      sc start evid4226
//  shall fail with -1 error code
//
//  uninstall:
//      1. sc delete evid4226
//      2. del %windir%\system32\drivers\evid4226.sys

#include <km/system_information.hxx>
#include <km/new.hxx>

using namespace ntl;

long __stdcall
  entry(
    void/*driver_object*/ * /*pdo*/,
    void/*const_unicode_string*/ * /*registry_path*/
    )
{
  using namespace ntl::km;
  using namespace ntl::pe;

  const system_information<system_modules_information> system_modules;
  if ( system_modules )
  {
    const rtl_process_module_information * tcpip_sys =
                                system_modules.data()->find_module("tcpip.sys");
    if ( tcpip_sys )
    {
      image * const pe = tcpip_sys->image();
      image::nt_headers * const nth = pe->get_nt_headers();

      image::optional_header32 * oh = nth->optional_header32();
      const uintptr_t code = pe->va(oh->BaseOfCode);
      const uintptr_t code_end = code + oh->SizeOfCode;

      /* EventID 4226
            mov     eax, _ActiveOpenProgressCount
            cmp     eax, _ActiveOpenProgressThreshold
            //jge     [short] XX
       */
      for ( uintptr_t i = code; i < code_end - 7; ++i )
      {
        if ( 0xA1 == *reinterpret_cast<uint8_t*>(i)
          && 0x053B == *reinterpret_cast<uint16_t*>(i+5) )
        {
          const uint32_t offset = *reinterpret_cast<uint32_t*>(i+7);
          if ( offset < pe->va(oh->BaseOfData)
            || offset > pe->va(oh->SizeOfImage) )
            continue;
          if ( 10 == *reinterpret_cast<uint32_t*>(offset) )
          {
            // RW section
            *reinterpret_cast<uint32_t*>(offset) = 65535;
            break;
          }
        }
      }

    }
  }
  return ~0;
}

#pragma comment(linker, "/entry:entry")
