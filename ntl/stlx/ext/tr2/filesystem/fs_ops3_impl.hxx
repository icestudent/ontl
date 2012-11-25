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
            e = make_error_code(st);
            if(!throwable)
              holder = e;
          }
          return success;
        }

        inline bool throw_files_error(error_code& holder, ntl::nt::ntstatus st, const char* msg)
        {
          (void)msg;
          bool throwable; error_code e;
          const bool success = handle_status(holder, st, e, throwable);
          if(!success && throwable)
            __ntl_throw(filesystem_error(msg, e));
          return success;
        }
        inline bool throw_files_error(error_code& holder, ntl::nt::ntstatus st, const char* msg, const path& p1)
        {
          (void)msg; (void)p1;
          bool throwable; error_code e;
          const bool success = handle_status(holder, st, e, throwable);
          if(!success && throwable)
            __ntl_throw(filesystem_error(msg, p1, e));
          return success;
        }
        inline bool throw_files_error(error_code& holder, ntl::nt::ntstatus st, const char* msg, const path& p1, const path& p2)
        {
          (void)msg; (void)p1; (void)p2;
          bool throwable; error_code e;
          const bool success = handle_status(holder, st, e, throwable);
          if(!success && throwable)
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
        return exists(status(p, ec));
      }

      uintmax_t file_size(const path& p, error_code& ec /*= throws()*/)
      {
        using namespace NTL_SUBSYSTEM_NS;
        file_network_open_information fbi;
        ntstatus st = ZwQueryFullAttributesFile(const_unicode_string(native(p)), fbi);
        return !__::throw_files_error(ec, st, "Can't get file size", p) ? 0 : static_cast<uintmax_t>(fbi.EndOfFile);
      }

      std::time_t last_write_time(const path& p, error_code& ec /*= throws()*/)
      {
        using namespace NTL_SUBSYSTEM_NS;
        file_network_open_information fbi;
        ntstatus st = ZwQueryFullAttributesFile(const_unicode_string(native(p)), fbi);
        return !__::throw_files_error(ec, st, "Can't get last write time", p) ? 0 : ntime2ctime(fbi.LastWriteTime);
      }

      void last_write_time(const path& p, const std::time_t new_time, error_code& ec /*= throws()*/)
      {
        using namespace NTL_SUBSYSTEM_NS;
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
        ec.clear();
        return s;
      }

      ///\name status functions
      namespace __
      {
        static file_status status(const path& p, ntl::nt::ntstatus& st, bool follow_symlink = true) __ntl_nothrow
        {
          // TODO: determine console, pipe, mailslot, device, etc.
          using namespace NTL_SUBSYSTEM_NS;
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
          if(st == status::object_name_not_found || status::object_path_not_found){
            st = status::success;
            return file_status(file_not_found);
          }
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

      namespace __
      {
        static bool create_directories(const path& p, error_code& ec)
        {
          ec.clear();
          if(p.empty())
            return false;

          // check path
          file_status fst = status(p, ec);
          if(!ec && exists(fst)){
            if(is_directory(fst))
              return false;       // directory already exists
            ec = make_error_code(posix_error::not_a_directory);
          }
          if(ec)
            return false;

          // convert to external
          const path wp = native(p);
          if(!wp.has_relative_path()){
            ec = make_error_code(ntl::nt::status::object_path_invalid);
            return false;
          }

          // iterate through it
          path::const_iterator part = wp.cbegin(), pend = wp.cend();
          path q = *part;
          for(++part; !ec && part != pend; ++part){
            q /= *part;
            fst = status(q, ec);
            if(ec) break;
            if(is_directory(fst))
              continue;
            if(!exists(fst))
              create_directory(q, ec);
            else
              ec = make_error_code(posix_error::not_a_directory);
          }
          return !ec;
        }
      } // impl

      bool create_directories(const path& p, error_code& ec)
      {
        error_code e;
        const bool re = __::create_directories(p, e);
        const bool throwable = &ec == &throws();
        if(!throwable)
          ec = e;
        else if(!re)
          __ntl_throw(filesystem_error("create_directories()", p, e));
        return re;
      }

      bool create_directory(const path& dp, error_code& ec)
      {
        using namespace NTL_SUBSYSTEM_NS;
        file_handler f;
        ntstatus st = f.create(const_unicode_string(native(dp)), file::create_new, file::list_directory|synchronize, file::share_read|file::share_write, 
          file::directory_file|file::open_for_backup_intent|file::synchronous_io_nonalert, file_attribute::normal);
        const bool throwable = &ec == &throws();
        if(!success(st) && st != status::object_name_collision){
          error_code e = make_error_code(st);
          if(!throwable)
            ec = e;
          else
            __ntl_throw(filesystem_error("create_directory", dp, e));
        }else if(!throwable)
          ec.clear();
        return success(st);
      }

      namespace __
      {
        static bool equivalent(const path& p1, const path& p2, error_code& ec)
        {
          // open files
          using namespace NTL_SUBSYSTEM_NS;
          file_handler f1, f2;
          ntstatus st = f1.open(const_unicode_string(native(p1)), file::read_attributes|synchronize, file::share_valid_flags, file::open_for_backup_intent);
          if(success(st))
            st = f2.open(const_unicode_string(native(p2)), file::read_attributes|synchronize, file::share_valid_flags, file::open_for_backup_intent);
          ec.clear();
          if(success(st)){
            // compare the volumes
            volume_information<file_fs_volume_information> v1(f1.get(), false), v2(f2.get(), false);
            if(status::is_error(v1) || status::is_error(v2)){
              ec = make_error_code( status::is_error(v1) ? v1.operator ntstatus() : v2.operator ntstatus() );
              return false;
            }
            if(v1->VolumeSerialNumber != v2->VolumeSerialNumber)
              return false;

            // compare files
            file_information<file_internal_information> fi1(f1.get()), fi2(f2.get());
            if(!fi1 || !fi2){
              ec = make_error_code( status::is_error(fi1) ? fi1.operator ntstatus() : fi2.operator ntstatus() );
              return false;
            }
            if(fi1->IndexNumber != fi2->IndexNumber)
              return false;
            // TODO: resolve links
            return true;
          }
          ec = make_error_code(st);
          return false;
        }
      }

      /** Determines, is the given paths are the same */
      inline bool equivalent(const path& p1, const path& p2) __ntl_throws(filesystem_error, filesystem_error)
      {
        // check paths
        file_status s1 = status(p1), s2 = status(p2);
        bool e1 = exists(s1), e2 = exists(s2);
        if((!e1 && !e2) || (is_other(s1) && is_other(s2))){
          if(!e1 || is_other(s1))
            __ntl_throw(filesystem_error( "Failed to determine equivalence", p1, make_error_code(!e1 ? posix_error::no_such_file_or_directory : posix_error::file_exists) ));
          else
            __ntl_throw(filesystem_error( "Failed to determine equivalence", p2, make_error_code(!e2 ? posix_error::no_such_file_or_directory : posix_error::file_exists) ));
        }
        if(s1 != s2)
          return false;
        error_code ec;
        bool re = __::equivalent(p1,p2,ec);
        if(ec){
          __ntl_throw(filesystem_error("Failed to determine equivalence", p1, p2, ec));
        }
        return re;
      }

      /** Determines, is the given paths are the same */
      inline bool equivalent(const path& p1, const path& p2, error_code& ec) __ntl_nothrow
      {
        if(&ec == &throws())
          return equivalent(p1,p2);

        // check paths
        file_status s1 = status(p1), s2 = status(p2);
        bool e1 = exists(s1), e2 = exists(s2);
        if((!e1 && !e2) || (is_other(s1) && is_other(s2))){
          if(!e1 || is_other(s1))
            ec = make_error_code(!e1 ? posix_error::no_such_file_or_directory : posix_error::file_exists);
          else
            ec = make_error_code(!e2 ? posix_error::no_such_file_or_directory : posix_error::file_exists);
          return false;
        }
        return __::equivalent(p1,p2,ec);
      }

      inline void rename(const path& from, const path& to, error_code& ec)
      {
        if(&ec != &throws())
          ec.clear();

        // If from_p and to_p resolve to the same file, no action is taken
        if(equivalent(from, to))
          return;

        using namespace NTL_SUBSYSTEM_NS;
        file_handler f;
        ntstatus st = f.open(const_unicode_string(native(from)), file::read_attributes|delete_access|synchronize, file::share_valid_flags, file::creation_options_default);
        if(success(st))
          st = f.rename(const_unicode_string(native(to)));
        if(!success(st)){
          error_code e = make_error_code(st);
          if(&ec == &throws())
            __ntl_throw(filesystem_error("Can't rename file", from, to, e));
          else
            ec = e;
        }
      }

    } // files
  } // tr2
}
