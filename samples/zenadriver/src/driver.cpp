//  The source is distributed under the terms of GNU General Public License
//  http://www.gnu.org/copyleft/gpl.html

#define NTL__POOL_TAG 'ZADR'

#define NTL__DEBUG_2K

#include "config.h"

#include <algorithm>
#include <array>
#include <list>

#include <km/new.hxx>
#include <km/driver_object.hxx>
#include <km/file.hxx>
#include <km/image.hxx>
#include <km/string.hxx>
#include <km/system_information.hxx>
#include <km/process.hxx>

#include <nt/registry.hxx>
#include <nt/service.hxx>


#include <km/debug.hxx>


using namespace std;
using namespace ntl;
using namespace ntl::km;

#include "utils.hxx"


// userland pass data here...
///\todo  fix constant size problem
static const char shared_data[shared_data_size] = ZENADRIVER_SIGNATURE_SHARED_DATA;

// ...and we are rearranging it to speed up PE images fitering
static list<wstring> * white;

// boot group is filtered during startup, however carantined later
static list<string> * boot_black;


// this function assumes shared_data[] is holding MULTI_SZ-like data
static inline
bool build_whitelist()
{
  if ( white ) return true;
  white = new list<wstring>;
  if ( !white ) return false;

  const wchar_t * str_begin = 0;
  const wchar_t * str_end = 0;
  for ( const wchar_t * p = reinterpret_cast<const wchar_t*>(shared_data);
        p != reinterpret_cast<const wchar_t*>(shared_data + shared_data_size);
        ++p )
  {
#if 0
    // check the upper bound
    if ( pe::image::this_module()->va<char*>(
      pe::image::this_module()->get_nt_headers()->OptionalHeader32.SizeOfImage)
      <= p )
    {
      return false;
    }
#endif

    if ( !str_begin ) str_begin = p;

    if ( *p == L'\0' )
    {
      str_end = p;
      // null-string at the end
      if ( str_begin == str_end ) break;
      white->push_back(wstring(str_begin, str_end - str_begin));
      if ( equal_filenames(white->back(), const_ansi_string("dump_atapi.sys"))
        || equal_filenames(white->back(), const_ansi_string("dump_WMILIB.SYS")))
      {
        const wstring::size_type und = white->back().rfind('_');
        white->back().erase(und-4, 5);
      }
      dbg::trace.printf("\t ZenADriver whitelist : %ws\n", white->back().c_str());
      str_begin = str_end + 1;
    }
  }
  return !white->empty();
} 


static
ntstatus patch_entry(pe::image * img)
{
  if ( !img->get_dos_header()->is_valid() )
    return status::invalid_image_not_mz;
  pe::image::nt_headers * const nth = img->get_nt_headers();
  const uint32_t entry = nth->OptionalHeader32.AddressOfEntryPoint;
  if ( !entry ) return status::driver_entrypoint_not_found;
  if ( uintptr_t(entry) >= nth->OptionalHeader32.SizeOfImage )
    return status::invalid_image_win_32;

  mdl * const pmdl = new(img, nth->OptionalHeader32.SizeOfImage) mdl;
  if ( !pmdl ) return status::insufficient_resources;
  pmdl->build_for_non_paged_pool();
  pe::image * const km_map = pe::image::bind(pmdl->map_locked_pages(KernelMode));
  char * p = km_map->va<char*>(entry);
  memcpy(p, "\x83\xC8\xFF\xC2\x08\x00", 6); // or eax, -1 : ret 8
  pmdl->unmap_locked_pages(km_map);
  delete pmdl;
  return status::success;
}


static
bool carantine_file(const const_unicode_string & filename)
{
#if 0
  return true;
#else

  file f(filename);
  raw_data filedata(f.get_data());
  f.erase();

  wstring new_name( filename.get_string() );
  
  // special case for NTFS data streams
  wstring::size_type colon = new_name.rfind(L':');
  if ( colon != wstring::npos ) new_name[colon] = '_';

  ///\todo basic_string::replace
  new_name.erase(0, 1+new_name.rfind(L'\\'));
  new_name.insert(0, L"\\??\\"ZENADRIVER_FOLDER_NAME L"\\");

  file backup(new_name,
              file::supersede,
              file::write_data | file::write_attributes | synchronize,
              file::share_read);
  return backup && backup.write(filedata.begin(), filedata.size());
#endif
}


///\note ImageNotifyRoutine is called after boot stage, it is safe to use file I/O
static inline
void rename_pending_files()
{
  if ( boot_black )
  {
    file_handler dir;
    dir.create(L"\\??\\"ZENADRIVER_FOLDER_NAME, file::open_if, file::all_access,
                file::share_none, file::directory_file | file::synchronous_io_nonalert);
      
    for ( list<string>::iterator it = boot_black->begin();
          it != boot_black->end();
          ++it )
    {
      wstring filename(it->begin(), it->end());
      build_full_driver_path(filename);
      carantine_file(const_unicode_string(filename));
    }
    delete boot_black;
    boot_black = 0;
  }
}


// filter out previously invisible drivers
///\note  guarded by MmSystemLoadLock (?)
void __stdcall
  on_image_load(
    unicode_string *  full_imagename,
    legacy_handle     /*process_id*/,
    image_info *      imginfo)
{
  if ( imginfo->SystemModeImage && imginfo->ImageBase > highest_user_address() )
  {
    rename_pending_files();

    // a small hack for videodrivers
    bool in_csrss = false;  
    const kprocess * const cp = kprocess::get_current();
    if ( cp )
    {
      static const char * image_filename;
      static const char system[] = "System";
      const char * const p = reinterpret_cast<const char*>(cp);
      if ( !image_filename )
        image_filename = search(p, p + 0x260, system, &system[sizeof(system)]); ///\note if not found it is strange but safe
      if ( strcmp(image_filename, "csrss.exe") )
        in_csrss = true;
    }
    
    // drivers are blacklisted by default
    bool allowed = false;

    // search through the list of visible drivers
    if ( white )
    {
      for ( list<wstring>::iterator it = white->begin();
            it != white->end();
            ++it )
      {
        if ( !equal_filenames(*it, *full_imagename) )
          continue;
        allowed = true;
        if ( !in_csrss ) white->erase(it);
        break;
      }
    }
    
    if ( in_csrss && equal_filenames(*full_imagename, const_ansi_string("vga.dll")) )
      allowed = true;

    dbg::trace.printf("\t ZenADriver : %ws \t%s\n", full_imagename->begin(),
                      allowed ? "" : "*** black ***");

    // move a suspicious file to the carantine folder
    if ( !allowed )
    {
      carantine_file(*full_imagename);
      patch_entry(imginfo->ImageBase);
    }
  }
}


static
bool set_load_image_notify_routine(load_image_notify_routine_t * pfn, bool remove)
{
  const system_information<system_modules_information> system_modules;
  if ( !system_modules ) return false;
  const rtl_process_module_information * nt = 
            system_modules->find_module(system_modules_information::module0());
  if ( !nt ) return false;

  array<char, sizeof("PsRemoveLoadImageNotifyRoutine")> fname;
  char * p = fname.begin();
  *p++ = 'P'; *p++ = 's';
  if ( remove )
  {
    *p++ = 'R'; *p++ = 'e'; *p++ = 'm'; *p++ = 'o'; *p++ = 'v'; *p++ = 'e';
  }
  else
  {
    *p++ = 'S'; *p++ = 'e'; *p++ = 't';
  }
  *p++ = 'L'; *p++ = 'o'; *p++ = 'a'; *p++ = 'd'; *p++ = 'I'; *p++ = 'm';
  *p++ = 'a'; *p++ = 'g'; *p++ = 'e'; *p++ = 'N'; *p++ = 'o'; *p++ = 't';
  *p++ = 'i'; *p++ = 'f'; *p++ = 'y'; *p++ = 'R'; *p++ = 'o'; *p++ = 'u';
  *p++ = 't'; *p++ = 'i'; *p++ = 'n'; *p++ = 'e'; *p = '\0';
  
  set_remove_load_image_notify_routine_t * f = nt->ImageBase->
            find_export<set_remove_load_image_notify_routine_t>(fname.begin());
  if ( !f ) return false;
  return nt::success(f(pfn));
}


void __stdcall
  on_driver_unload(driver_object *)
{
  dbg::trace("\t ZenADriver : unload\n");
  set_load_image_notify_routine(on_image_load, true);
  delete white;
  delete boot_black;
}


bool filter_boot_drivers()
{
  if ( !build_whitelist() ) return false;

  boot_black = new list<string>;
  if ( !boot_black ) return false;

  if ( !get_active_drivers_list(*boot_black) )
  {
    dbg::trace("\t ZenADriver : NtQuerySystemInformation\n");
    return false;
  }

  // remove duplicates from both lists
  for ( list<string>::iterator a_it = boot_black->begin();
        a_it != boot_black->end(); )
  {
    for ( list<wstring>::iterator w_it = white->begin();
          w_it != white->end();
          ++w_it )
    {
      if ( !equal_filenames(*a_it, *w_it) )
        continue;
      white->erase(w_it);
      a_it = boot_black->erase(a_it);
      goto next_a_it;
    }
    ++a_it;
    next_a_it:;
  }

  // patch drivers to disallow run
  if ( !boot_black->empty() )
  {
    const system_information<system_modules_information> active_drivers;
    if ( !active_drivers ) return false;
    for ( list<string>::iterator it = boot_black->begin();
          it != boot_black->end(); )
    {
      string::size_type const fn = it->rfind('\\');
      pe::image * const img =
        active_drivers->find_module(it->c_str() + (string::npos == fn ? 0 : fn+1))->ImageBase;
      if ( img == pe::image::this_module() )
        it = boot_black->erase(it);
      else
      {
        dbg::trace.printf("\t ZenADriver : %s \t*** black ***\n", (*it).begin());
        patch_entry(img);
        ++it;
      }
    }
  }
  return true;
}


long __stdcall
  entry( 
    driver_object *         pdo, 
    const_unicode_string *  registry_path
    )
{
  nt::key scm_entry(*registry_path);
  scm_entry.set(L"Start", nt::service::disabled);
  
  pdo->DriverUnload = on_driver_unload;

  filter_boot_drivers();

  if ( !set_load_image_notify_routine(on_image_load, false) )
  {
    return status::unsuccessful;
    dbg::trace("\t ZenADriver : failed\n");
  }
  dbg::trace("\t ZenADriver : active\n");
  return status::success;
}


#pragma comment(linker, "/entry:entry")
#pragma comment(linker, "/nodefaultlib:libcmt")
#pragma comment(linker, "/nodefaultlib:oldnames")

