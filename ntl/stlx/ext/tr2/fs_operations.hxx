#ifndef NTL__STLX_TR2_FILESYSTEM
#error internal header
#endif

#include "../../../nt/file.hxx"
#include "../../../nt/system_error.hxx"

namespace std 
{
  namespace tr2
  {
    namespace sys
    {
      namespace filesystem
      {
        enum file_type { status_unknown, file_not_found, regular_file, directory_file,
          symlink_file, block_file, character_file, fifo_file, socket_file,
          type_unknown
        };

        /**
         *	@brief Class file_status
         *  @details A file_status object stores information about the status of a file. The internal form of the stored information is unspecified.
         *  @note The class may be extended in the future to store additional status information.
         **/
        class file_status
        {
          file_type type_;
        public:
          explicit file_status(file_type v = status_unknown)
            :type_(v)
          {}

          file_type type() const { return type_; }
          void type(file_type v) { type_ = v; }
        };

        /** Information about used and free space */
        struct space_info
        {
          uintmax_t capacity;
          uintmax_t free;
          uintmax_t available;
        };

        ///\name status functions
        namespace __
        {
          template <class Path>
          static file_status status(const Path& p, error_code& ec, bool follow_symlink = true)
          {
            ec.clear();
            using namespace ntl::nt;
            file_basic_information fbi;
            ntstatus st = NtQueryAttributesFile(const_unicode_string(p.external_file_string()), fbi);
            if(success(st)){
              file_type ft = regular_file;
              if(fbi.FileAttributes & file_attribute::directory)
                ft = directory_file;
              else if(fbi.FileAttributes & file_attribute::reparse_point)
                ft = symlink_file;
              return file_status(ft);
            }
            ec = ntl::nt::make_error_code(st);
            return file_status(st == status::object_name_not_found ? file_not_found : status_unknown);
            //file_handler f;
            //file_handler::creation_options co = file::open_for_backup_intent;
            //if(!follow_symlink)
            //  co = co | file::open_reparse_point;
            //ntstatus st = f.open(const_unicode_string(p.external_file_string()), file::read_attributes|synchronize, file::share_read|file::share_write, co);
            //if(!success(st)){
            //  ec = ntl::nt::make_error_code(st);
            //  return file_status(st == status::object_name_not_found ? file_not_found : status_unknown);
            //}

            //file_information<file_standard_information> fi(f.get());
            //if(!success(fi)){
            //  ec = make_error_code(st);
            //  return file_status(status_unknown);
            //}
            //if(fi->Directory)
            //  return file_status(directory_file);
            //else {
            //  // check, is this a link (symbolic)
            //  file_information<file_basic_information> fb(f.get());
            //  if(!success(fb)){
            //    ec = make_error_code(fb);
            //    return file_status(status_unknown);
            //  }
            //  if(fb->FileAttributes & file_attribute::reparse_point)
            //    return file_status(symlink_file);
            //  else
            //    return file_status(regular_file);
            //}
          }
        } // __
        
        template <class Path> inline file_status status(const Path& p, error_code& ec)
        {
          return __::status(p, ec, true);
        }

        template <class Path> inline file_status status(const Path& p) __ntl_throws(basic_filesystem_error<Path>)
        {
          error_code ec;
          file_status fs = status(p, ec);
          if(ec)
            __ntl_throw(basic_filesystem_error<Path>("status() failed", p, ec));
          return fs;
        }

        template <class Path> inline file_status symlink_status(const Path& p, error_code& ec)
        {
          return __::status(p, ec, false);
        }

        template <class Path> inline file_status symlink_status(const Path& p) __ntl_throws(basic_filesystem_error<Path>)
        {
          error_code ec;
          file_status fs = symlink_status(p, ec);
          if(ec)
            __ntl_throw(basic_filesystem_error<Path>("symlink_status() failed", p, ec));
          return fs;
        }

        ///\name predicate functions
        inline bool status_known(file_status s) { return s.type() != status_unknown; }
        inline bool exists(file_status s)       { return s.type() != status_unknown && s.type() != file_not_found; }
        inline bool is_regular(file_status s)   { return s.type() == regular_file; }
        inline bool is_directory(file_status s) { return s.type() == directory_file; }
        inline bool is_symlink(file_status s)   { return s.type() == symlink_file; }
        inline bool is_other(file_status s)     { return exists(s) && s.type() != regular_file && s.type() != directory_file && s.type() != symlink_file; }

        template <class Path> inline bool exists(const Path& p)       { return exists(status(p)); }
        template <class Path> inline bool is_directory(const Path& p) { return is_directory(status(p)); }
        template <class Path> inline bool is_regular(const Path& p)   { return is_regular(status(p)); }
        template <class Path> inline bool is_other(const Path& p)     { return is_other(status(p)); }
        template <class Path> inline bool is_symlink(const Path& p)   { return is_symlink(status(p)); }

        /** Determines, is the file or directory is empty */
        template <class Path> inline bool is_empty(const Path& p) __ntl_throws(basic_filesystem_error<Path>)
        {
          file_status s = status(p);
          if(!exists(s) || is_other(s)){
            bool exist = exists(s);
            __ntl_throw(basic_filesystem_error<Path>( !exist ? "file not exists" : "is_other()", p, make_error_code(!exist ? posix_error::no_such_file_or_directory : posix_error::file_exists) ));
          }
          return is_directory(s)
            ? basic_directory_iterator<Path>(p) == basic_directory_iterator<Path>() 
            : file_size(p) == 0;
        }

        /** Determines, is the given paths are the same */
        template <class Path1, class Path2>
        inline bool equivalent(const Path1& p1, const Path2& p2) __ntl_throws(basic_filesystem_error<Path1>, basic_filesystem_error<Path2>)
        {
          static_assert((is_same<typename Path1::external_string_type,typename Path2::external_string_type>::value), "shall be same type");
          file_status s1 = status(p1), s2 = status(p2);
          bool e1 = exists(s1), e2 = exists(s2);
          if((!e1 && !e2) || (is_other(s1) && is_other(s2))){
            if(!e1 || is_other(s1))
              __ntl_throw(basic_filesystem_error<Path1>( !e1 ? "file_not_exists" : "is_other()", p1, make_error_code(!e1 ? posix_error::no_such_file_or_directory : posix_error::file_exists) ));
            else
              __ntl_throw(basic_filesystem_error<Path2>( !e2 ? "file_not_exists" : "is_other()", p2, make_error_code(!e2 ? posix_error::no_such_file_or_directory : posix_error::file_exists) ));
          }
          using namespace ntl::nt;
          // TODO: try to open files
          file f1, f2;

          // compare the volumes
          volume_information<file_fs_volume_information> v1(f1.handler().get(), false), v2(f2.handler().get(), false);
          if(status::is_error(v1) || status::is_error(v2))
            return false;
          if(v1->VolumeSerialNumber != v2->VolumeSerialNumber)
            return false;

          // compare files
          file_information<file_internal_information> fi1(f1.handler().get()), fi2(f2.handler().get());
          if(!fi1 || !fi2 || fi1->IndexNumber != fi2->IndexNumber)
            return false;
          // TODO: resolve links
          return true;
        }


        ///\name attribute functions

        /** Returns the current path */
        template <class Path> inline Path current_path()
        {
          return Path::traits_type::to_internal(Path(), ntl::nt::peb::instance().ProcessParameters->CurrentDirectory.DosPath);
        }

        /** Returns the current path at the first call of initial_path() */
        template <class Path> inline const Path& initial_path()
        {
          static const Path cwd = current_path();
          return cwd;
        }

        template <class Path> inline uintmax_t file_size(const Path& p, error_code& ec = throws())
        {
          static const uintmax_t errval = 0;
          uintmax_t size = errval;
          using namespace ntl::nt;
          file_network_open_information fbi;
          ntstatus st = NtQueryFullAttributesFile(const_unicode_string(p.external_file_string()), fbi);
          if(success(st)){
            size = static_cast<uintmax_t>(fbi.EndOfFile);
          }else{
            error_code e = make_error_code(st);
            if(ec == throws())
              __ntl_throw(basic_filesystem_error<Path>("Can't get file size", p, e));
            else
              ec = e;
          }else if(ec != throws())
            ec.clear();
          return size;
        }

        template <class Path> space_info space(const Path& p);

        template <class Path> inline std::time_t last_write_time(const Path& p, error_code& ec = throws())
        {
          using namespace ntl::nt;
          if(ec != throws())
            ec.clear();
          time_t val = 0;
          file_basic_information fbi;
          ntstatus st = NtQueryAttributesFile(const_unicode_string(p.external_file_string()), fbi);
          if(success(st)){
            val = ntime2ctime(fbi.LastWriteTime);
          }else{
            error_code e = make_error_code(st);
            if(ec == throws())
              __ntl_throw(basic_filesystem_error<Path>("Can't get file time", p, e));
            else
              ec = e;
          }
          return val;
        }

        template <class Path> inline void last_write_time(const Path& p, const std::time_t new_time, error_code& ec = throws())
        {
          using namespace ntl::nt;
          file_handler f;
          ntstatus st = f.open(const_unicode_string(p.external_file_string()), file::read_attributes|file::write_attributes|synchronize, file::share_read|file::share_write, file::creation_options_default);
          if(success(st)){
            file_information<file_basic_information> fi(f.get());
            if(success(st)){
              file_basic_information bi = *fi.data();
              bi.LastWriteTime = ctime2ntime(new_time);
              file_information<file_basic_information> fis(f.get(), bi);
              st = fis;
            }
          }
          if(!success(st)){
            error_code e = make_error_code(st);
            if(ec == throws())
              __ntl_throw(basic_filesystem_error<Path>("Can't set file time", p, e));
            else
              ec = e;
          }else if(ec != throws())
            ec.clear();
        }

        ///\name operations functions
        template <class Path> bool create_directory(const Path& dp);

        template <class Path1, class Path2>
        static error_code create_hard_link(const Path1& old_fp, const Path2& new_fp, error_code& ec)
        {
          static_assert((is_same<typename Path1::external_string_type, typename Path2::external_string_type>::value), "Must be the same type");
          using namespace ntl::nt;

          // reject UNC links
          if(old_fp.is_unc() || new_fp.is_unc())
            return ec = make_error_code(status::object_path_invalid);

          ntstatus st;
          const wstring file = old_fp.external_file_string(), link = new_fp.external_file_string();

          // check is it mapped file
          // '\??\C:'
          static const wchar_t lanman_[] = L"\\Device\\LanmanRedirector\\";
          struct lanman
          {
            static bool check(const wstring& file)
            {
              if(file.size() > 6){
                wstring drive = file.substr(0, 6);
                const const_unicode_string cus(drive);
                ntl::nt::symbolic_link linko(cus);
                if(success(linko)){
                  wstring target = linko.query();
                  return !target.empty() && target.compare(0, _countof(lanman_), lanman_) == 0;
                }
              }
              return false;
            }
          };

          if(lanman::check(file) || lanman::check(link))
            return ec = make_error_code(status::object_path_invalid);

          file_handler f;
          st = f.open(const_unicode_string(file), (file::access_mask)file::delete_access|synchronize, file::share_read|file::share_write|file::share_delete,file::open_reparse_point|file::creation_options_default|file::open_for_backup_intent);
          if(success(st)){
            file_information<file_link_information> fi(f.get(), const_unicode_string(link), false);
            st = fi;
          }
          if(success(st))
            ec.clear();
          else
            ec = make_error_code(st);
          return ec;
        }
        
        template <class Path1, class Path2>
        void inline create_hard_link(const Path1& old_fp, const Path2& new_fp) __ntl_throws(basic_filesystem_error<Path1::external_string_type>)
        {
          static_assert((is_same<typename Path1::external_string_type, typename Path2::external_string_type>::value), "Must be the same type");
          error_code ec;
          create_hard_link(old_fp, new_fp, ec);
          if(ec)
            __ntl_throw(basic_filesystem_error<Path1::external_string_type>("Failed to create hard link [from:to]", old_fp.external_file_string(), new_fp.external_file_string(), ec));
        }

        template <class Path1, class Path2>
        static error_code create_symlink(const Path1& old_fp, const Path2& new_fp, error_code& ec);
        
        template <class Path1, class Path2>
        void inline create_symlink(const Path1& old_fp, const Path2& new_fp) __ntl_throws(basic_filesystem_error<Path1::external_string_type>)
        {
          static_assert((is_same<typename Path1::external_string_type, typename Path2::external_string_type>::value), "Must be the same type");
          error_code ec;
          create_symlink(old_fp, new_fp, ec);
          if(ec)
            __ntl_throw(basic_filesystem_error<Path1::external_string_type>("Failed to create symlink [from:to]", old_fp.external_file_string(), new_fp.external_file_string(), ec));
        }

        template <class Path> inline bool remove(const Path& p, error_code& ec = throws())
        {
          using namespace ntl::nt;
          ntstatus st = NtDeleteFile(const_unicode_string(p.external_file_string()));
          if(success(st)){
            if(ec != throws())
              ec.clear();
          }else{
            error_code e = make_error_code(st);
            if(ec != throws())
              ec = e;
            else
              __ntl_throw(basic_filesystem_error<Path>("Failed to remove object", p, e));
          }
          return success(st);
        }
        
        template <class Path1, class Path2>
        inline void rename(const Path1& from_p, const Path2& to_p, error_code& ec = throws())
        {
          static_assert((is_same<typename Path1::external_string_type, typename Path2::external_string_type>::value), "Must be the same type");
          if(ec != throws())
            ec.clear();
          
          // If from_p and to_p resolve to the same file, no action is taken
          if(equivalent(from_p, to_p))
            return;

          const wstring from = from_p.external_file_string(), to = to_p.external_file_string();

          using namespace ntl::nt;
          file_handler f;
          ntstatus st = f.open(const_unicode_string(from), file::read_attributes|file::delete_access|synchronize, file::share_read|file::share_write|file::share_delete, file::creation_options_default);
          if(success(st))
            st = f.rename(const_unicode_string(to))
          if(!success(st)){
            error_code e = make_error_code(st);
            if(ec == throws())
              __ntl_throw(basic_filesystem_error<Path>("Can't rename file", p, e));
            else
              ec = e;
          }
        }
        
        template <class Path1, class Path2>
        void copy_file(const Path1& from_fp, const Path2& to_fp);
        
        template <class Path> Path system_complete(const Path& p);
        
        template <class Path> Path complete(const Path& p, const Path& base = initial_path<Path>());

        ///\name convenience functions
        template <class Path> bool create_directories(const Path& p);

        template <class Path> typename Path::string_type extension(const Path& p)
        {
          return p.stem();
        }
        
        template <class Path> typename Path::string_type basename(const Path& p)
        {
          typename Path::string_type leaf = p.leaf();
          if(!leaf.empty()){
            typename Path::string_type::difference_type pos = leaf.rfind(dot<Path>::value);
            if(pos != Path::string_type::npos)
              return leaf.substr(0, pos);
          }
          return leaf;
        }

        template <class Path> Path replace_extension(const Path& p, const typename Path::string_type& new_extension)
        {
          return Path(p).replace_stem(new_extension);
        }
        ///\}
      }
    }
  }
}
