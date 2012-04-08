#ifndef NTL__STLX_TR2_FILES
#error internal header
#endif
#pragma once

#include <nt/environ.hxx>

namespace std 
{
  namespace tr2
  {
    namespace files
    {
      namespace __
      {
        inline bool is_error(error_code& ec)
        {
          return &ec != &throws() && ec;
        }

        inline bool handle_status(error_code& holder, ntl::nt::ntstatus st, error_code& e, bool& throwable)
        {
          throwable = &holder == &throws();
          const bool success = ntl::nt::success(st);
          if(success){
            if(!throwable)
              holder.clear();
          }else{
            const error_code e = make_error_code(st);
            if(!throwable)
              holder = e;
          }
          return success;
        }

        inline bool throw_files_error(error_code& holder, ntl::nt::ntstatus st, const char* msg)
        {
          bool throwable; error_code e;
          const bool success = handle_status(holder, st, e, throwable);
          if(throwable)
            __ntl_throw(filesystem_error(msg, e));
          return success;
        }
        inline bool throw_files_error(error_code& holder, ntl::nt::ntstatus st, const char* msg, const path& p1)
        {
          bool throwable; error_code e;
          const bool success = handle_status(holder, st, e, throwable);
          if(throwable)
            __ntl_throw(filesystem_error(msg, p1, e));
          return success;
        }
        inline bool throw_files_error(error_code& holder, ntl::nt::ntstatus st, const char* msg, const path& p1, const path& p2)
        {
          bool throwable; error_code e;
          const bool success = handle_status(holder, st, e, throwable);
          if(throwable)
            __ntl_throw(filesystem_error(msg, p1, p2, e));
          return success;
        }
      }

      const path::string_type native(const path& p)
      {
        return absolute(p).native();
      }



      path current_path(error_code& ec/* = throws()*/)
      {
        if(&ec != &throws())
          ec.clear();
        return ntl::nt::peb::instance().ProcessParameters->CurrentDirectory.DosPath.get_string();
      }

      void current_path(const path& p, error_code& ec/* = throws()*/)
      {
        using namespace ntl::nt;
        ntstatus st = RtlSetCurrentDirectory_U(const_unicode_string(native(p)));
        __::throw_files_error(ec, st, "Failed to set current path", p);
      }

      path absolute(const path& p)
      {
        path rp(p);
        rp.make_absolute();
        return std::move(rp);
      }

      path absolute(const path& p, const path& base)
      {
        const path abs_base(base.is_absolute() ? base : absolute(base));
        if(p.empty())
          return absolute(abs_base);
        const path p_root_name (p.root_name());
        const path base_root_name (abs_base.root_name());
        const path p_root_directory (p.root_directory());
        if (!p_root_name.empty())  // p.has_root_name()
        {
          if (p_root_directory.empty())  // !p.has_root_directory()
            return absolute(p_root_name / abs_base.root_directory() / abs_base.relative_path() / p.relative_path());
        } else if (!p_root_directory.empty()){  // p.has_root_directory()
          return absolute(base_root_name / p);
        } else {
          return absolute(abs_base / p);
        }
        return absolute(p);  // p.is_absolute() is true
      }

      void copy(const path& from, const path& to, error_code& ec/* = throws()*/)
      {
        file_status s(symlink_status(from, ec));
        if(__::is_error(ec))
          return;
        if(is_symlink(s))
          copy_symlink(from, to, ec);
        else if(is_directory(s))
          copy_directory(from, to, ec);
        else if(is_regular_file(s))
          copy_file(from, to, copy_option::fail_if_exists, ec);
        else
          __::throw_files_error(ec, ntl::nt::status::file_invalid, "Unknown path to copy", from);
      }

      void copy_file(const path& from, const path& to, error_code& ec /*= throws()*/)
      {
        copy_file(from, to, copy_option::fail_if_exists, ec);
      }

      bool exists(file_status s) __ntl_nothrow
      {
        return s.type() != status_unknown && s.type() != file_not_found;
      }
      bool exists(const path& p, error_code& ec /*= throws()*/)
      {
        return exists(status(p));
      }

      uintmax_t file_size(const path& p, error_code& ec /*= throws()*/)
      {
        using namespace NTL__SUBSYSTEM_NS;
        file_network_open_information fbi;
        ntstatus st = ZwQueryFullAttributesFile(const_unicode_string(native(p)), fbi);
        return !__::throw_files_error(ec, st, "Can't get file size", p) ? 0 : static_cast<uintmax_t>(fbi.EndOfFile);
      }

      std::time_t last_write_time(const path& p, error_code& ec /*= throws()*/)
      {
        using namespace NTL__SUBSYSTEM_NS;
        file_network_open_information fbi;
        ntstatus st = ZwQueryFullAttributesFile(const_unicode_string(native(p)), fbi);
        return !__::throw_files_error(ec, st, "Can't get last write time", p) ? 0 : ntime2ctime(fbi.LastWriteTime);
      }

      void last_write_time(const path& p, const std::time_t new_time, error_code& ec /*= throws()*/)
      {
        using namespace NTL__SUBSYSTEM_NS;
        file_handler f;
        ntstatus st = f.open(const_unicode_string(native(p)), file::read_attributes|file::write_attributes|synchronize, file::share_read|file::share_write, file::creation_options_default);
        if(success(st)){
          file_information<file_basic_information> fi(f.get());
          if(success(st)){
            file_basic_information bi = *fi.data();
            bi.LastWriteTime = ctime2ntime(new_time);
            file_information<file_basic_information> fis(f.get(), bi);
            st = fis;
          }
        }
        __::throw_files_error(ec, st, "Can't write file time", p);
      }

      const path&  initial_path(error_code& ec /*= throws()*/)
      {
        if(&ec != &throws())
          ec.clear();
        static const path initial = current_path(ec);
        return initial;
      }

      path temp_directory_path(error_code& ec /*= throws()*/)
      {
        using namespace ntl::nt;
        environment env;
        unicode_string temp;
        ntstatus st = env.query(L"TMP", temp);
        if(!success(st))
          st = env.query(L"TEMP", temp);
        __::throw_files_error(ec, st, "Can't get temp path");
        return path(temp);
      }

      path unique_path(const path& model, error_code& ec)
      {
        static const char hex[] = "0123456789abcdef";
        union
        {
          int val;
          char buf[sizeof(int)];
        } rand;
        size_t used = 0;
        const size_t max = sizeof(rand)*2;
        path::string_type s = model.wstring();
        for(path::string_type::iterator i = s.begin(), e = s.end(); i != e; ++i){
          if(*i != '%')
            continue;
          if(used == 0)
            rand.val = std::rand();
          int c = rand.buf[used/2];
          c >>= 4* (used++ & 1);
          *i = hex[c & 0x0F];
          if(used == max)
            used = 0;
        }
        return s;
      }

      ///\name status functions
      namespace __
      {
        static file_status status(const path& p, ntl::nt::ntstatus& st, bool follow_symlink = true) __ntl_nothrow
        {
          // TODO: determine console, pipe, mailslot, device, etc.
          using namespace NTL__SUBSYSTEM_NS;
          //file_basic_information fbi;
          file_network_open_information fbi;
          const object_attributes::attributes attr = object_attributes::case_insensitive|(follow_symlink ? object_attributes::none : object_attributes::openlink);
          st = ZwQueryFullAttributesFile(object_attributes(const_unicode_string(native(p)), attr), fbi);
          if(success(st)){
            file_type ft = regular_file;
            if(fbi.FileAttributes & file_attribute::directory)
              ft = directory_file;
            else if(fbi.FileAttributes & file_attribute::reparse_point)
              ft = symlink_file;
            return file_status(ft);
          }
          if(st == status::object_name_not_found || status::object_path_not_found)
            return file_status(file_not_found);
          return file_status(status_unknown);
        }
      } // __

      file_status status(const path& p, error_code& ec) __ntl_nothrow
      {
        ntl::nt::ntstatus st;
        const file_status fst = __::status(p, st, true);
        __::throw_files_error(ec, st, "status()", p);
        return fst;
      }

      file_status symlink_status(const path& p, error_code& ec) __ntl_nothrow
      {
        ntl::nt::ntstatus st;
        const file_status fst = __::status(p, st, true);
        __::throw_files_error(ec, st, "symlink_status()", p);
        return fst;
      }


    } // files
  } // tr2
}
