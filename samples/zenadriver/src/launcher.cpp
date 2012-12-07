/**\file*********************************************************************
 *                                                                     \brief
 *  ZenADriver is a simple but effective tool to reveal
 *  hidden drivers commonly used by rootkits novadays.
 *
 *  \author     S.T.A.S.  http://stasm.org
 *
 *  \version    0.26
 *  \date       23 Nov 2007
 *
 *  The source is distributed under the terms of GNU General Public License
 *  http://www.gnu.org/copyleft/gpl.html
 *
 *
 *  Revision history:
 *
 *    0.10    prototype
 *    0.20  + the SCM database filtering
 *    0.21  + the video drivers whitelist is retrieved from the regystry
 *    0.22  + command line key to manually `carantine' files
 *    0.23  + import parsing to check modules dependencies
 *    0.24    minor sources cleanups
 *    0.25  * drivers are pathed to return an error
 *    0.26  * if imege file not found it is blacklisted
 *
 ****************************************************************************
 */

#include "config.h"

#include <winapp.hxx>
#include <functional>
#include <list>
#include <nt/new.hxx>
#include <nt/system_information.hxx>
#include <pe/image.hxx>
#include <nt/file.hxx>
#include <nt/registry.hxx>
#include <nt/service.hxx>


bool const default_silent   = false;
bool const default_verbose  = false;//true;//
#pragma warning(disable:4127) // conditional expression is constant, see above


using namespace std;
using namespace ntl;
using namespace ntl::nt;

#include "utils.hxx"


class zenadriver : public winapp
{

    void about()
    {
      write("ZenADriver v0.26  stealth drivers revealer. Build on "__DATE__"\n"
            "Copyright (C) 2007 S.T.A.S. All rights reserved.\n\n"
            "alpha version - do not distribute.\n\n");
    }

    void write_help()
    {
      write("\ncommand line keys:\n"
            "  silent ------------- suppress output\n"
            "  verbose ------------ show loadable drivers\n"
            "  carantine:filename - force delete on reboot\n");
    }

  ///////////////////////////////////////////////////////////////////////////
  public:

    __forceinline
    zenadriver()
    : silent(default_silent),
      verbose_output(default_verbose)
    {
      if ( !silent && !parse_args() )
      {
        use_console = true;
        acquire_console();
        write_help();
        sleep<seconds>(5);
        return;
      }
      use_console = not silent;
      acquire_console();
      about();

      write("WARNING! This software might cause OS malfunction.\n"
            "Make shure system restore point is created before proceed.\n\n"
            "Press:\n"
            "(Q)uit.\n"
            "(P)roceed. OS will be rebooted. See C:\\ZenADriver folder for scan results.\n");

      // inkey();

      proceed();
     
      sleep<minutes>(5);
      //  console::free();

    }

  ///////////////////////////////////////////////////////////////////////////
  protected:

    bool silent;
    bool use_console;
    bool verbose_output;

    list<wstring> carantine;

    template<typename T>
    __forceinline
    unsigned write(T msg, bool verbose = true)
    {
      return use_console && verbose ? console::write(msg) : 0;
    }

    template<typename stringContainer>
    __forceinline
    void write_container(const stringContainer & c, bool verbose = true)
    {
      for ( stringContainer::const_iterator it = c.begin(); verbose && it != c.end(); ++it )
      {
        write(*it);
        write('\n');
      }
    }

    __forceinline
    bool parse_args()
    {
      command_line cmdl;
      int parsed = 0;
      for ( command_line::iterator it = cmdl.begin(); it != cmdl.end(); ++it )
      {
        ++parsed; // assume no parse errors
        if ( not default_silent && !strcmp(*it, "silent") )
          silent = true;
        else
        if ( not default_verbose && !strcmp(*it, "verbose") )
          verbose_output = true;
        else
        if ( !strncmp(*it, "carantine:", sizeof("carantine:")-1) )
        {
          string filename(*it + sizeof("carantine:")-1);
          carantine.push_back(wstring());
          copy(filename.begin(), filename.end(), back_inserter(carantine.back()));
        }
        else
        if ( it != cmdl.begin() )  // invalid argument?
          --parsed;
      }
      return parsed == cmdl.size();
    }

    bool acquire_console()
    {
      return use_console ? console::attach() || console::alloc() : true/*no console, no error*/;
    }


    bool proceed()
    {
      list<string> active_drivers;
      if ( !get_active_drivers_list(active_drivers) )
      {
        write("fatal error: QuerySystemInformation\n");
        return false;
      }
      write("\nActive system services:\n", verbose_output);
      write_container(active_drivers, verbose_output);

      list<wstring> runnable_drivers;
      get_runnable_drivers_list(runnable_drivers);
      write("\nRunnable system services:\n", verbose_output);
      write_container(runnable_drivers, verbose_output);

      list<wstring> video_drivers;
      get_video_drivers_list(video_drivers);
      copy(video_drivers.begin(), video_drivers.end(), back_inserter(runnable_drivers));
      write("\nLoadable video drivers:\n", verbose_output);
      write_container(video_drivers, verbose_output);
     
      write("\nCarantine:\n", verbose_output);
      write_container(carantine, verbose_output);

      write('\n');

      static const const_unicode_string drv_path(L"\\SystemRoot\\System32\\drivers\\zenadriver.sys");

      // images for the imports parsing
      list<raw_data> img_cache;

      for ( list<string>::iterator a_it = active_drivers.begin();
            a_it != active_drivers.end(); )
      {
        for ( list<wstring>::iterator r_it = carantine.begin();
              r_it != carantine.end();
              ++r_it )
        {
          if ( equal_filenames(*a_it, *r_it) )
          {
            carantine.erase(r_it);
            goto carantine_it;
          }
        }

        ///\todo  Ntbootdd.sys - may be loaded by ntldr

        // KMD disables itself, so it may be blacklisted
        if ( equal_filenames(*a_it, drv_path) ) goto next_a_it;

        if ( equal_filenames(*a_it, const_ansi_string("ntdll.dll")) ) goto next_a_it;

        ///\see build_whitelist
        if ( equal_filenames(*a_it, const_ansi_string("dump_atapi.sys")) ) goto next_a_it;
        if ( equal_filenames(*a_it, const_ansi_string("dump_WMILIB.sys")) ) goto next_a_it;
        if ( equal_filenames(*a_it, const_ansi_string("hiber_WMILIB.sys")) ) goto next_a_it;

        {
          img_cache.push_back(raw_data());
          wstring filename(a_it->begin(), a_it->end());
          build_full_driver_path(filename);
          if ( !load_image(filename, img_cache.back()) )
          {
//            write("warning: file `");
//            write(a_it->c_str());
//            write("' not found\n");
            img_cache.back().clear();
            goto carantine_it;
          }
        }

        // check against the registry database
        for ( list<wstring>::iterator r_it = runnable_drivers.begin();
              r_it != runnable_drivers.end();
              ++r_it )
        {
          if ( equal_filenames(*a_it, *r_it) )
          {
            runnable_drivers.erase(r_it);
            goto next_a_it;
          }
        }

        // an image may be loaded due to the imports resolving
        for ( list<raw_data>::reverse_iterator imp_it = img_cache.rbegin();
              imp_it != img_cache.rend();
              ++imp_it )
        {
          if ( imp_it->empty() ) continue;
          pe::image * const img = pe::image::bind(imp_it->begin());
          if ( img->find_import_entry(a_it->c_str()) )
            goto next_a_it;
        }

        if ( equal_filenames(*a_it, const_ansi_string("watchdog.sys")) ) goto next_a_it;

        // ntos
        if ( equal_filenames(*a_it, const_ansi_string("ntoskrnl.exe")) ) goto next_a_it;
        if ( equal_filenames(*a_it, const_ansi_string("ntkrnlpa.exe")) ) goto next_a_it;
        if ( equal_filenames(*a_it, const_ansi_string("ntkrnlmp.exe")) ) goto next_a_it;
        if ( equal_filenames(*a_it, const_ansi_string("ntkrpamp.exe")) ) goto next_a_it;

        carantine_it:
//        if ( verbose_output )
        {
          write(*a_it);
          write("\t blacklisted\n");
        }
        a_it = active_drivers.erase(a_it);
        continue;
        next_a_it: ++a_it;
      }

      for_each
      (
        active_drivers.begin(),
        active_drivers.end(),
        bind2nd(mem_fun_ref(&string::push_back), '\0')
      );
      string active_drivers_list;
      for_each
      (
        active_drivers.begin(),
        active_drivers.end(),
        bind1st(mem_fun1_t<string&, string, const string&>(&string::append),
                &active_drivers_list)
      );

      if ( build_driver_image(active_drivers_list)
        && save_driver_image(drv_path)
        && register_driver(drv_path) )
      {
        write("\nReboot OS to continue...\n");
        return true;
      }
      return false;
    }

    __forceinline
    bool build_driver_image(const string & active_drivers_list)
    {
      if ( active_drivers_list.size() > shared_data_size )
      {
        write("fatal error: no memory\n");
        return false;
      }
      extern unsigned char zenadriver_sys[];
      extern unsigned zenadriver_sys_size;      
      static const char signature[] = ZENADRIVER_SIGNATURE_SHARED_DATA;
      wchar_t * const shared_data = reinterpret_cast<wchar_t*>(
                search(&zenadriver_sys[0], &zenadriver_sys[zenadriver_sys_size],
                       &signature[0], &signature[sizeof(signature)]));
      copy(active_drivers_list.begin(), active_drivers_list.end(), shared_data);
      pe::image::bind(zenadriver_sys)->checksum(true);
      return true;
    }
    
    template<class String>
    __forceinline
    bool save_driver_image(const String& drv_path)
    {
      file drv_img(drv_path,
                  file::supersede/*overwrite_if*/,
                  file::write_data | file::write_attributes | synchronize,
                  file::share_none);
      if ( !drv_img )
      {
        write("fatal error: CreateFile\n");
        return false;
      }
      extern unsigned char zenadriver_sys[];
      extern unsigned zenadriver_sys_size;      
      if ( !drv_img.write(zenadriver_sys, zenadriver_sys_size) )
      {
        write("fatal error: WriteFile\n");
        return false;
      }
      return true;
    }

    template<class String>
    __forceinline
    bool register_driver(const String& drv_path)
    {
      wstring my_service_group(const_unicode_string(L"ZenADriver").get_string());
      key group_order;
      if ( !group_order.open(const_unicode_string(L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Control\\ServiceGroupOrder"),
                              key::query_value | key::set_value) )
      {
        write("fatal error: OpenKey\n");
        return false;
      }
      wstring group_order_list;
      if ( !group_order.query(L"List", group_order_list) )
      {
        write("fatal error: QueryValueKey\n");
        return false;
      }
      if ( !equal(my_service_group.begin(), my_service_group.end(),
                  group_order_list.begin()) )
      {
        write("ZenADriver runs the first time\n");
      }
      else
      {
//        write("ZenADriver is back again\n");
        const wstring::size_type sgend = group_order_list.find(L'\0');
        if ( sgend == my_service_group.size() )
          my_service_group += L'_';
        group_order_list.erase(0, sgend + 1);
      }
      my_service_group.push_back(L'\0');
      group_order_list.insert(group_order_list.begin(),
                            my_service_group.begin(), my_service_group.end());
      if ( !group_order.set(L"List", group_order_list, key::reg_multi_sz) )
      {
        write("fatal error: SetValueKey 1\n");
        return false;
      }
      if ( !group_order.flush() )
      {
        write("fatal error: FlushKey 1\n");
        return false;
      }

      key autorun;
//      const uint32_t tag = uint32_t(-1);
      if ( !autorun.create(const_unicode_string(L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\"
                            ZENADRIVER_SERVICE_NAME),
                            key::set_value) )
      {
        write("fatal error: CreateKey\n");
        return false;
      }
      if ( !autorun.set(L"ErrorControl", nt::service::error_severe)
        || !autorun.set(L"Start", nt::service::boot_start)
        || !autorun.set(L"Group", my_service_group)
        || !autorun.set(L"ImagePath", drv_path)
//        || !autorun.set(L"Tag", tag)
        || !autorun.set(L"Type", nt::service::kernel_driver) )
      {
        write("fatal error: SetValueKey 2\n");
        return false;
      }
      if ( !autorun.flush() )
      {
        write("fatal error: FlushKey 2\n");
        return false;
      }
      return true;
    }

};


int winapp::main()
{
  zenadriver();
  return 0;
}

