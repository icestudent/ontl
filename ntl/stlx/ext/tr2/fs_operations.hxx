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
          inline file_status status(const Path& p, error_code& ec, bool follow_symlink = true)
          {
            using namespace ntl::nt;
            file_handler f;
            file_handler::creation_options co = file::open_for_backup_intent;
            if(!follow_symlink)
              co = co | file::open_reparse_point;
            ntstatus st = f.open(const_unicode_string(p.external_file_string()), file::read_attributes|synchronize, file::share_read|file::share_write, co);
            if(!success(st)){
              ec = ntl::nt::make_error_code(st);
              return file_status(st == status::object_name_not_found ? file_not_found : status_unknown);
            }

            file_information<file_standard_information> fi(f.get());
            if(!success(fi)){
              ec = make_error_code(st);
              return file_status(status_unknown);
            }
            if(fi->Directory)
              return file_status(directory_file);
            else {
              // check, is this a link (symbolic)
              file_information<file_basic_information> fb(f.get());
              if(!success(fb)){
                ec = make_error_code(fb);
                return file_status(status_unknown);
              }
              if(fb->FileAttributes & file_attribute::reparse_point)
                return file_status(symlink_file);
              else
                return file_status(regular_file);
            }
          }

        }
        
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

        template <class Path> inline file_status symlink_status(const Path& p)
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
        template <class Path> Path current_path();
        template <class Path> const Path& initial_path();
        template <class Path> uintmax_t file_size(const Path& p);
        template <class Path> space_info space(const Path& p);
        template <class Path> std::time_t last_write_time(const Path& p);
        template <class Path>
        void last_write_time(const Path& p, const std::time_t new_time);

        ///\name operations functions
        template <class Path> bool create_directory(const Path& dp);
        template <class Path1, class Path2>
        void create_hard_link(const Path1& old_fp, const Path2& new_fp);
        template <class Path1, class Path2>
        error_code create_hard_link(const Path1& old_fp, const Path2& new_fp, error_code& ec);
        template <class Path1, class Path2>
        void create_symlink(const Path1& old_fp, const Path2& new_fp);
        template <class Path1, class Path2>
        error_code create_symlink(const Path1& old_fp, const Path2& new_fp, error_code& ec);
        template <class Path> bool remove(const Path& p);
        template <class Path1, class Path2>
        void rename(const Path1& from_p, const Path2& to_p);
        template <class Path1, class Path2>
        void copy_file(const Path1& from_fp, const Path2& to_fp);
        template <class Path> Path system_complete(const Path& p);
        template <class Path> Path complete(const Path& p, const Path& base=initial_path<Path>());

        ///\name convenience functions
        template <class Path> bool create_directories(const Path& p);
        template <class Path> typename Path::string_type extension(const Path& p);
        template <class Path> typename Path::string_type basename(const Path& p);
        template <class Path> Path replace_extension(const Path& p, const typename Path::string_type& new_extension);
        ///\}
      }
    }
  }
}
