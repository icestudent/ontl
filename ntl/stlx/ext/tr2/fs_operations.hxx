/**\file*********************************************************************
 *                                                                     \brief
 *  Filesystem library
 *
 ****************************************************************************
 */
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
      /**
       *  \addtogroup tr2
       *  @{
       *  \defgroup tr2_filesystem Filesystem Library
       *  @{
       *  \defgroup tr2_filesystem_ops Filesystem operations
       *  @{
       **/


        /** File object types */
        enum file_type {
          /** Unknown status which indicates an error of status() function */
          status_unknown,
          /** File or directory is not exists */
          file_not_found,
          /** It is a file */
          regular_file,
          /** It is a directory */
          directory_file,
          /** It is a symbolic link to file or directory */
          symlink_file,
          block_file,
          /** Console input or output */
          character_file,
          /** Pipe object */
          fifo_file,
          /** Socket object (not supported on NT) */
          socket_file,
          /** Pipe object */
          pipe_file = fifo_file,
          /** Mailslot object */
          mailslot_file,
          /** Kernel-mode device object */
          device_file,
          /** Unknown file type */
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

          bool operator== (const file_status& r) const { return type_ == r.type_; }
          bool operator!= (const file_status& r) const { return type_ != r.type_; }
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
          static file_status status(const Path& p, error_code& ec, bool follow_symlink = true) __ntl_nothrow
          {
            // TODO: determine console, pipe, mailslot, device, etc.
            ec.clear();
            using namespace NTL__SUBSYSTEM_NS;
            //file_basic_information fbi;
            file_network_open_information fbi;

            const object_attributes::attributes attr = object_attributes::case_insensitive|(follow_symlink ? object_attributes::none : object_attributes::openlink);
            ntstatus st = ZwQueryFullAttributesFile(object_attributes(const_unicode_string(p.external_file_string()), attr), fbi);
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

            ec = std::make_error_code(st);
            return file_status(status_unknown);
          }
        } // __
        
        /** Determines the attributes of \c p. Symlinks are resolved. */
        template <class Path> inline file_status status(const Path& p, error_code& ec) __ntl_nothrow
        {
          return __::status(p, ec, true);
        }
        
        /** Determines the attributes of \c p. Symlinks are resolved. */
        template <class Path> inline file_status status(const Path& p) __ntl_throws(basic_filesystem_error<Path>)
        {
          error_code ec;
          file_status fs = status(p, ec);
          if(ec)
            __ntl_throw(basic_filesystem_error<Path>("status() failed", p, ec));
          return fs;
        }

        /** Determines the attributes of \c p. */
        template <class Path> inline file_status symlink_status(const Path& p, error_code& ec) __ntl_nothrow
        {
          return __::status(p, ec, false);
        }

        /** Determines the attributes of \c p. */
        template <class Path> inline file_status symlink_status(const Path& p) __ntl_throws(basic_filesystem_error<Path>)
        {
          error_code ec;
          file_status fs = symlink_status(p, ec);
          if(ec)
            __ntl_throw(basic_filesystem_error<Path>("symlink_status() failed", p, ec));
          return fs;
        }

        ///\name predicate functions
        /** Is status known? */
        inline bool status_known(file_status s) { return s.type() != status_unknown; }
        /** Is file object exists? */
        inline bool exists(file_status s)       { return s.type() != status_unknown && s.type() != file_not_found; }
        /** Is it a file? */
        inline bool is_regular(file_status s)   { return s.type() == regular_file; }
        /** Is it a directory? */
        inline bool is_directory(file_status s) { return s.type() == directory_file; }
        /** Is it a symbolic link? */
        inline bool is_symlink(file_status s)   { return s.type() == symlink_file; }
        /** Is it not a file, directory or symbolic link? */
        inline bool is_other(file_status s)     { return exists(s) && s.type() != regular_file && s.type() != directory_file && s.type() != symlink_file; }

        /** Is file object exists? */
        template <class Path> inline bool exists(const Path& p)       { return exists(status(p)); }
        /** Is it a file? */
        template <class Path> inline bool is_regular(const Path& p)   { return is_regular(status(p)); }
        /** Is it a directory? */
        template <class Path> inline bool is_directory(const Path& p) { return is_directory(status(p)); }
        /** Is it a symbolic link? */
        template <class Path> inline bool is_symlink(const Path& p)   { return is_symlink(status(p)); }
        /** Is it not a file, directory or symbolic link? */
        template <class Path> inline bool is_other(const Path& p)     { return is_other(status(p)); }

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

        namespace __
        {
          template <class Path1, class Path2>
          static bool equivalent(const Path1& p1, const Path2& p2, error_code& ec)
          {
            // open files
            using namespace NTL__SUBSYSTEM_NS;
            file_handler f1, f2;
            ntstatus st = f1.open(const_unicode_string(p1.external_file_string()), file::read_attributes|synchronize, file::share_valid_flags, file::open_for_backup_intent);
            if(success(st))
              st = f2.open(const_unicode_string(p2.external_file_string()), file::read_attributes|synchronize, file::share_valid_flags, file::open_for_backup_intent);
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
        template <class Path1, class Path2>
        inline bool equivalent(const Path1& p1, const Path2& p2) __ntl_throws(basic_filesystem_error<Path1>, basic_filesystem_error<Path2>)
        {
          // check paths
          file_status s1 = status(p1), s2 = status(p2);
          bool e1 = exists(s1), e2 = exists(s2);
          if((!e1 && !e2) || (is_other(s1) && is_other(s2))){
            if(!e1 || is_other(s1))
              __ntl_throw(basic_filesystem_error<Path1>( "Failed to determine equivalence", p1, make_error_code(!e1 ? posix_error::no_such_file_or_directory : posix_error::file_exists) ));
            else
              __ntl_throw(basic_filesystem_error<Path2>( "Failed to determine equivalence", p2, make_error_code(!e2 ? posix_error::no_such_file_or_directory : posix_error::file_exists) ));
          }
          if(s1 != s2)
            return false;
          error_code ec;
          bool re = __::equivalent(p1,p2,ec);
          if(ec){
            typedef basic_path<typename Path1::external_string_type, typename Path1::traits_type> epath;
            __ntl_throw(basic_filesystem_error<epath>("Failed to determine equivalence", Path1::traits_type::to_external(p1, p1.string()), Path2::traits_type::to_external(p2, p2.string()), ec));
          }
          return re;
        }

        /** Determines, is the given paths are the same */
        template <class Path1, class Path2>
        inline bool equivalent(const Path1& p1, const Path2& p2, error_code& ec) __ntl_nothrow
        {
          static_assert((is_same<typename Path1::external_string_type,typename Path2::external_string_type>::value), "must be same type");
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


        ///\name attribute functions

        /** Returns the current path */
        template <class Path> inline Path current_path()
        {
          return Path::traits_type::to_internal(Path(), ntl::nt::peb::instance().ProcessParameters->CurrentDirectory.DosPath.get_string());
        }

        /** Sets the current path */
        template <class Path> inline void current_path(const Path& p, error_code& ec = throws())
        {
          using namespace ntl::nt;
          ntstatus st = RtlSetCurrentDirectory_U(const_unicode_string(p.external_file_string()));
          if(!success(st)){
            error_code e = make_error_code(st);
            if(&ec == &throws())
              __ntl_throw(basic_filesystem_error<Path>("Failed to set the current path", p, e));
            else ec = e;
          }else if(&ec != &throws())
            ec.clear();
        }

        /** Returns the current path at the first call of initial_path() */
        template <class Path> inline const Path& initial_path()
        {
          static const Path cwd = current_path();
          return cwd;
        }

        /** Returns the size of file, in bytes */
        template <class Path> inline uintmax_t file_size(const Path& p, error_code& ec = throws())
        {
          if(&ec != &throws())
            ec.clear();
          static const uintmax_t errval = 0;
          uintmax_t size = errval;
          using namespace NTL__SUBSYSTEM_NS;
          file_network_open_information fbi;
          ntstatus st = ZwQueryFullAttributesFile(const_unicode_string(p.external_file_string()), fbi);
          if(success(st)){
            size = static_cast<uintmax_t>(fbi.EndOfFile);
          }else{
            error_code e = make_error_code(st);
            if(&ec == &throws())
              __ntl_throw(basic_filesystem_error<Path>("Can't get file size", p, e));
            else
              ec = e;
          }
          return size;
        }

        /** Returns the space information of the file system, which \c p points to. */
        template <class Path> space_info space(const Path& p);

        /** Returns the posix time of last data modification of \c p */
        template <class Path> inline std::time_t last_write_time(const Path& p, error_code& ec = throws())
        {
          using namespace NTL__SUBSYSTEM_NS;
          if(&ec != &throws())
            ec.clear();
          time_t val = 0;
          file_network_open_information fbi;
          ntstatus st = ZwQueryFullAttributesFile(const_unicode_string(p.external_file_string()), fbi);
          if(success(st)){
            val = ntime2ctime(fbi.LastWriteTime);
          }else{
            error_code e = make_error_code(st);
            if(&ec == &throws())
              __ntl_throw(basic_filesystem_error<Path>("Can't get file time", p, e));
            else
              ec = e;
          }
          return val;
        }

        /** Returns the posix time of last data modification of \c p */
        template <class Path> inline void last_write_time(const Path& p, const std::time_t new_time, error_code& ec = throws())
        {
          using namespace NTL__SUBSYSTEM_NS;
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
            if(&ec == &throws())
              __ntl_throw(basic_filesystem_error<Path>("Can't set file time", p, e));
            else
              ec = e;
          }else if(&ec != &throws())
            ec.clear();
        }


        ///\name operations functions
        
        /** Creates a directory object, returns false if directory is already exists */
        template <class Path> inline bool create_directory(const Path& dp, error_code& ec = throws())
        {
          using namespace NTL__SUBSYSTEM_NS;
          file_handler f;
          ntstatus st = f.create(const_unicode_string(dp.external_file_string()), file::create_new, file::list_directory|synchronize, file::share_read|file::share_write, 
            file::directory_file|file::open_for_backup_intent|file::synchronous_io_nonalert, file_attribute::normal);
          if(!success(st) && st != status::object_name_collision){
            error_code e = make_error_code(st);
            if(&ec != &throws())
              ec = e;
            else
              __ntl_throw(basic_filesystem_error<Path>("Failed to create directory", dp, e));
          }else if(&ec != &throws())
            ec.clear();
          return success(st);
        }

        /** Creates a hard link, named \c new_fp, to \c old_fp */
        template <class Path1, class Path2>
        static error_code create_hard_link(const Path1& old_fp, const Path2& new_fp, error_code& ec)
        {
          static_assert((is_same<typename Path1::external_string_type, typename Path2::external_string_type>::value), "Must be the same type");
          using namespace NTL__SUBSYSTEM_NS;

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
                NTL__SUBSYSTEM_NS::symbolic_link linko(cus);
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
        
        /** Creates a hard link, named \c new_fp, to \c old_fp */
        template <class Path1, class Path2>
        void inline create_hard_link(const Path1& old_fp, const Path2& new_fp) __ntl_throws(basic_filesystem_error<Path1::external_string_type>)
        {
          static_assert((is_same<typename Path1::external_string_type, typename Path2::external_string_type>::value), "Must be the same type");
          error_code ec;
          create_hard_link(old_fp, new_fp, ec);
          if(ec)
            __ntl_throw(basic_filesystem_error<Path1::external_string_type>("Failed to create hard link [from:to]", old_fp.external_file_string(), new_fp.external_file_string(), ec));
        }

        /** Creates a symbolic link, named \c new_fp, to \c old_fp */
        template <class Path1, class Path2>
        static error_code create_symlink(const Path1& old_fp, const Path2& new_fp, error_code& ec);
        
        /** Creates a symbolic link, named \c new_fp, to \c old_fp */
        template <class Path1, class Path2>
        void inline create_symlink(const Path1& old_fp, const Path2& new_fp) __ntl_throws(basic_filesystem_error<Path1::external_string_type>)
        {
          static_assert((is_same<typename Path1::external_string_type, typename Path2::external_string_type>::value), "Must be the same type");
          error_code ec;
          create_symlink(old_fp, new_fp, ec);
          if(ec)
            __ntl_throw(basic_filesystem_error<Path1::external_string_type>("Failed to create symlink [from:to]", old_fp.external_file_string(), new_fp.external_file_string(), ec));
        }

        /** Removes the file object */
        template <class Path> inline bool remove(const Path& p, error_code& ec = throws())
        {
          using namespace NTL__SUBSYSTEM_NS;
          ntstatus st = ZwDeleteFile(const_unicode_string(p.external_file_string()));
          if(success(st)){
            if(&ec != &throws())
              ec.clear();
          }else{
            error_code e = make_error_code(st);
            if(&ec != &throws())
              ec = e;
            else
              __ntl_throw(basic_filesystem_error<Path>("Failed to remove object", p, e));
          }
          return success(st);
        }

        /** Recursively deletes the contents of \c p if it exists, then deletes file \c p itself */
        template <class Path> unsigned long remove_all(const Path& p);
        
        /** Renames the file object from \c from_p to \c to_p */
        template <class Path1, class Path2>
        inline void rename(const Path1& from_p, const Path2& to_p, error_code& ec = throws())
        {
          static_assert((is_same<typename Path1::external_string_type, typename Path2::external_string_type>::value), "Must be the same type");
          if(&ec != &throws())
            ec.clear();
          
          // If from_p and to_p resolve to the same file, no action is taken
          if(equivalent(from_p, to_p))
            return;

          const wstring from = from_p.external_file_string(), to = to_p.external_file_string();

          using namespace NTL__SUBSYSTEM_NS;
          file_handler f;
          ntstatus st = f.open(const_unicode_string(from), file::read_attributes|delete_access|synchronize, file::share_valid_flags, file::creation_options_default);
          if(success(st))
            st = f.rename(const_unicode_string(to));
          if(!success(st)){
            error_code e = make_error_code(st);
            if(&ec == &throws())
              __ntl_throw(basic_filesystem_error<Path>("Can't rename file", p, e));
            else
              ec = e;
          }
        }
        
        /** The contents and attributes of the file \c from_fp resolves to are copied to the file \c to_fp resolves to. */
        template <class Path1, class Path2>
        inline void copy_file(const Path1& from_fp, const Path2& to_fp, error_code& ec = throws())
        {
          static_assert((is_same<typename Path1::external_string_type, typename Path2::external_string_type>::value), "Must be the same type");
          if(&ec != &throws())
            ec.clear();

          // If from_p and to_p resolve to the same file, no action is taken
          if(equivalent(from_fp, to_fp))
            return;

          using namespace NTL__SUBSYSTEM_NS;
          file_handler from, to;
          ntstatus st = from.open(const_unicode_string(from_fp.external_file_string()), file::generic_read, file::share_valid_flags, file::creation_options_default);
          if(success(st)){
            st = to.create(const_unicode_string(to_fp.external_file_string()), file::overwrite_if, file::generic_write, file::share_read, file::creation_options_default);
            if(success(st)){
              static const size_t buf_size = 64*1024; // 64k
              ntl::raw_data buf(buf_size); // 64k
              int64_t size = from.size();
              while(size){
                st = from.read(buf.begin(), buf_size);
                size_t readed = from.get_io_status_block().Information;
                st = to.write(buf.begin(), static_cast<uint32_t>(readed));
                size -= readed;
              }
              if(!success(st) && to.size() == from.size())
                st = status::success;
            }
          }
          if(!success(st)){
            error_code e = make_error_code(st);
            if(&ec == &throws())
              __ntl_throw(basic_filesystem_error<Path1::external_string_type>("Failed to copy file [from:to]", from_fp.external_file_string(), to_fp.external_file_string(), e));
            else
              ec = e;
          }
        }
        
        /** Composes a complete path from \c p, using the same rules used by the operating system to resolve a path passed as the filename argument to standard library open functions. */
        template <class Path> Path system_complete(const Path& p);
        
        /** Composes a complete path from \c p and \c base */
        template <class Path> Path complete(const Path& p, const Path& base = initial_path<Path>());


        ///\name convenience functions

        /** Creates a directory. If one or more of the intermediate folders do not exist, it creates them. */
        template <class Path> static bool create_directories(const Path& p, error_code& ec)
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
          const wpath wp = p.external_file_string();
          if(!wp.has_relative_path()){
            ec = make_error_code(ntl::nt::status::object_path_invalid);
            return false;
          }

          // iterate through it
          wpath::const_iterator part = wp.cbegin(), pend = wp.cend();
          wpath q = *part;
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

        /** Creates a directory. If one or more of the intermediate folders do not exist, it creates them. */
        template <class Path> inline bool create_directories(const Path& p) __ntl_throws(basic_filesystem_error<Path>)
        {
          error_code ec;
          bool re = create_directories(p, ec);
          if(ec)
            __ntl_throw(basic_filesystem_error<Path>("Failed to create directories", p, ec));
          return re;
        }

        /** Returns the extension of \c p. \deprecated \see basic_path::extension() */
        template <class Path> inline typename Path::string_type extension(const Path& p) __ntl_nothrow
        {
          return p.extension();
        }
        
        /** Returns the base filename without extension of \c p. \deprecated \see basic_path::stem() */
        template <class Path> inline typename Path::string_type basename(const Path& p) __ntl_nothrow
        {
          typename Path::string_type leaf = p.leaf();
          if(!leaf.empty()){
            typename Path::string_type::difference_type pos = leaf.rfind(dot<Path>::value);
            if(pos != Path::string_type::npos)
              return leaf.substr(0, pos);
          }
          return leaf;
        }

        /** Replaces the extension of \c p with the \c new_extension. \deprecated \see basic_path::replace_extension() */
        template <class Path> inline Path replace_extension(const Path& p, const typename Path::string_type& new_extension) __ntl_nothrow
        {
          return Path(p).replace_stem(new_extension);
        }
        ///\}

        /** @} tr2_filesystem_ops */
        /** @} tr2_filesystem */
        /** @} tr2 */
      }
    }
  }
}
