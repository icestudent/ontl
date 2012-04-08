/**\file*********************************************************************
 *                                                                     \brief
 *  Filesystem V3 library for TR2
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_FILES
#define NTL__STLX_TR2_FILES
#pragma once

#include "../../iosfwd.hxx"
#include "../../stdstring.hxx"
#include "../../system_error.hxx"
#include "../../algorithm.hxx"
#include "../../ctime.hxx"

#ifndef NTL_NO_LOCALES
#include "../../locale.hxx"
#endif

#include "../../type_traits.hxx"

// implementation
#include "../../../nt/string.hxx"

#ifndef NTL_SUBSYSTEM_KM
# include "../../../nt/new.hxx"
#else
# include "../../../km/new.hxx"
#endif

namespace std
{
  /// C++ Standard Library Technical Report 2
  namespace tr2
  {
    /**\defgroup tr2 Technical Report 2 on C++ Library Extensions
     *@{*/

    /// Filesystem V3
    namespace files
    {
      /**
       *  \addtogroup tr2
       *  @{
       *  \defgroup tr2_files Filesystem V3 Library
       *  @{
       **/

      class path;

      inline void swap(path& lhs, path& rhs);

      //bool lexicographical_compare(path::iterator first1, path::iterator last1, path::iterator first2, path::iterator last2);
      inline std::size_t hash_value(const path& p);

      inline bool operator==(const path& lhs, const path& rhs);
      inline bool operator!=(const path& lhs, const path& rhs);
      inline bool operator< (const path& lhs, const path& rhs);
      inline bool operator<=(const path& lhs, const path& rhs);
      inline bool operator> (const path& lhs, const path& rhs);
      inline bool operator>=(const path& lhs, const path& rhs);

      inline path operator/ (const path& lhs, const path& rhs);

      template <class Char, class Traits>
      inline std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, const path& p);

      template <class Char, class Traits>
      inline std::basic_istream<Char, Traits>& operator>>(std::basic_istream<Char, Traits>& is, path& p);

      //std::ostream&  operator<<( std::ostream& os, const path& p );
      //std::wostream& operator<<( std::wostream& os, const path& p );
      //std::istream&  operator>>( std::istream& is, path& p );
      //std::wistream& operator>>( std::wistream& is, path& p );

      class filesystem_error;      
      class directory_entry;

      class directory_iterator;

      class recursive_directory_iterator;


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
        uintmax_t available; // free space available to a non-privileged process
      };

      enum perms
      {
        no_perms,
        owner_read, owner_write, owner_exe, owner_all,
        group_read, group_write, group_exe, group_all,
        others_read, others_write, others_exe, others_all, all_all,
        set_uid_on_exe, set_gid_on_exe, sticky_bit,
        perms_mask, perms_not_known,
        add_perms, remove_perms, symlink_perms
      };

      enum class copy_option
      {
        none,
        fail_if_exists = none,
        overwrite_if_exists
      };

      enum class symlink_option
      {
        none,
        no_recurse = none,
        recurse
      };


    } // files

    /** @} tr2_files */
    /** @} tr2 */
  } // tr2
}// std

#include "filesystem/fs_path.hxx"
#include "filesystem/fs_operations_v3.hxx"
#include "filesystem/fs_directory_v3.hxx"
#endif
