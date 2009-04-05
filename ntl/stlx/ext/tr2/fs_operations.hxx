#ifndef NTL__STLX_TR2_FILESYSTEM
#error internal header
#endif

namespace std 
{
  namespace tr2
  {
    namespace sys
    {
      namespace fileystem
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
        public:
          explicit file_status(file_type v = status_unknown);

          file_type type() const;
          void type(file_type v);
        };

        /** Information about used and free space */
        struct space_info
        {
          uintmax_t capacity;
          uintmax_t free;
          uintmax_t available;
        };

        ///\name status functions
        template <class Path> file_status status(const Path& p);
        template <class Path> file_status status(const Path& p, error_code& ec);
        template <class Path> file_status symlink_status(const Path& p);
        template <class Path> file_status symlink_status(const Path& p, error_code& ec);

        ///\name predicate functions
        bool status_known( file_status s ); 
        bool exists( file_status s );
        bool is_regular( file_status s ); 
        bool is_directory( file_status s );
        bool is_symlink( file_status s );
        bool is_other( file_status s );

        template <class Path> bool exists(const Path& p);
        template <class Path> bool is_directory(const Path& p);
        template <class Path> bool is_regular(const Path& p);
        template <class Path> bool is_other(const Path& p);
        template <class Path> bool is_symlink(const Path& p);
        template <class Path> bool is_empty(const Path& p);

        template <class Path1, class Path2>
        bool equivalent(const Path1& p1, const Path2& p2);

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
