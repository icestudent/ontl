/**\file*********************************************************************
 *                                                                     \brief
 *  Filesystem V3 library for TR2
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_FILES
#error internal header
#endif
#pragma once 

#include "../../../nt/file.hxx"
#include "../../../nt/system_error.hxx"

namespace std 
{
  namespace tr2
  {
    namespace files
    {
      /**
       *  \addtogroup tr2
       *  @{
       *  \addtogroup tr2_files
       *  @{
       *  \defgroup tr2_files_ops Filesystem v3 operations
       *  @{
       **/
      inline const path::string_type native(const path& p);

      inline path         current_path(error_code& ec = throws());
      inline void         current_path(const path& p, error_code& ec = throws());

      inline path         absolute(const path& p, const path& base);
      inline path         absolute(const path& p);

      //inline path       canonical(const path& p, const path& base = current_path());
      inline path         canonical(const path& p, error_code& ec = throws());
      inline path         canonical(const path& p, const path& base, error_code& ec = throws());


      inline void         copy(const path& from, const path& to, error_code& ec = throws());

      inline void         copy_directory(const path& from, const path& to, error_code& ec = throws());

      inline void         copy_file(const path& from, const path& to, error_code& ec = throws());
      inline void         copy_file(const path& from, const path& to, copy_option option, error_code& ec = throws());

      inline void         copy_symlink(const path& existing_symlink, const path& new_symlink, error_code& ec = throws());

      inline bool         create_directories(const path& p, error_code& ec = throws());

      inline bool         create_directory(const path& p, error_code& ec = throws());

      inline void         create_directory_symlink(const path& to, const path& new_symlink, error_code& ec = throws());

      inline void         create_hard_link(const path& to, const path& new_hard_link, error_code& ec = throws());

      inline void         create_symlink(const path& to, const path& new_symlink, error_code& ec = throws());

      inline bool         exists(file_status s) __ntl_nothrow;
      inline bool         exists(const path& p, error_code& ec = throws());

      inline bool         equivalent(const path& p1, const path& p2, error_code& ec = throws());

      inline uintmax_t    file_size(const path& p, error_code& ec = throws());

      inline uintmax_t    hard_link_count(const path& p, error_code& ec = throws());

      inline const path&  initial_path(error_code& ec = throws());

      inline bool         status_known(file_status s)     __ntl_nothrow { return s.type() != status_unknown; }
      inline bool         is_directory(file_status s)     __ntl_nothrow { return s.type() == directory_file; }
      inline bool         is_regular_file(file_status s)  __ntl_nothrow { return s.type() == regular_file; }
      inline bool         is_symlink(file_status s)       __ntl_nothrow { return s.type() == symlink_file; }
      inline bool         is_other(file_status s)         __ntl_nothrow { return exists(s) && s.type() != regular_file && s.type() != directory_file && s.type() != symlink_file; }

      inline file_status  status(const path& p, error_code& ec = throws());
      inline bool         is_empty(const path& p, error_code& ec = throws());
      inline bool         is_directory(const path& p, error_code& ec = throws())    { return is_directory(status(p)); }
      inline bool         is_other(const path& p, error_code& ec = throws())        { return is_other(status(p)); }
      inline bool         is_regular_file(const path& p, error_code& ec = throws()) { return is_regular_file(status(p)); }
      inline bool         is_symlink(const path& p, error_code& ec = throws())      { return is_symlink(status(p)); }

      inline std::time_t  last_write_time(const path& p, error_code& ec = throws());
      inline void         last_write_time(const path& p, const std::time_t new_time, error_code& ec = throws());

      inline path         read_symlink(const path& p, error_code& ec = throws());

      inline bool         remove(const path& p, error_code& ec = throws());

      inline uintmax_t    remove_all(const path& p, error_code& ec = throws());

      inline void         rename(const path& from, const path& to, error_code& ec = throws());

      inline void         resize_file(const path& p, uintmax_t size, error_code& ec = throws());

      inline space_info   space(const path& p, error_code& ec = throws());



      inline file_status  symlink_status(const path& p, error_code& ec = throws());

      inline path         system_complete(const path& p, error_code& ec = throws());

      inline path         temp_directory_path(error_code& ec = throws());

      inline path         unique_path(const path& model="%%%%-%%%%-%%%%-%%%%", error_code& ec = throws());

      /** @} tr2_files_ops */
      /** @} tr2_files */
      /** @} tr2 */
    }
  }
}
#ifndef NTL_DOC
# include "fs_ops3_impl.hxx"
#endif
