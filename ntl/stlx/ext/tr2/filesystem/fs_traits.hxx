/**\file*********************************************************************
 *                                                                     \brief
 *  Filesystem library
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_FILESYSTEM3
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
      namespace filesystem3
      {
        class directory_entry;

        namespace path_traits
        {
          typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_type;

          template<class Source>
          struct is_pathable: false_type{};
          template<> struct is_pathable<char*>: true_type{};
          template<> struct is_pathable<const char*>: true_type{};
          template<> struct is_pathable<wchar_t*>: true_type{};
          template<> struct is_pathable<const wchar_t*>: true_type{};
          template<> struct is_pathable<std::string>: true_type{};
          template<> struct is_pathable<std::wstring>: true_type{};
          template<> struct is_pathable<std::vector<char> >: true_type{};
          template<> struct is_pathable<std::vector<wchar_t> >: true_type{};
          template<> struct is_pathable<nt::native_string<char> >: true_type{};
          template<> struct is_pathable<nt::native_string<wchar_t> >: true_type{};
          template<> struct is_pathable<directory_entry>: true_type{};

          inline void convert(const char* from, const char* from_end, std::wstring& to, const codecvt_type& cvt);
          inline void convert(const wchar_t* from, const wchar_t* from_end, std::string& to, const codecvt_type& cvt);

          inline void convert(const char* from, const char* from_end, std::string& to, const codecvt_type&) { to.assign(from, from_end); }
          inline void convert(const wchar_t* from, const wchar_t* from_end, std::wstring& to, const codecvt_type&) { to.assign(from, from_end); }


        }

      }
    }
  }
}
