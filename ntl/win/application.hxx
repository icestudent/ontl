/**\file*********************************************************************
 *                                                                     \brief
 *  Win32 Application support
 *
 ****************************************************************************
 */
#ifndef NTL__WIN_APPLICATION
#define NTL__WIN_APPLICATION
#pragma once

#include "windef.hxx"
#include "handle.hxx"
#include "pe/image.hxx"
#include "stlx/iterator.hxx"
#include "stlx/vector.hxx"
#include "stlx/string_ref.hxx"

namespace ntl {
namespace win {
/**\addtogroup  winapi_types_support *** Win32 API support library **********
 *@{*/

#ifndef NTL_NO_AUTOLINK
# pragma comment(lib,    "kernel32.lib")
#endif


NTL_EXTERNAPI
char * __stdcall
  GetCommandLineA();

NTL_EXTERNAPI
wchar_t * __stdcall
  GetCommandLineW();

NTL_EXTERNAPI
uint32_t __stdcall
  GetLastError();

NTL_EXTERNAPI
void __stdcall
  SetLastError(
    uint32_t dwErrCode
    );


typedef pe::image * hinstance;

/**\addtogroup  application ****** Application support library **************
*@{*/

template<class charT>
class application: noncopyable
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef charT char_type;

    static hinstance instance()
    {
      return pe::image::this_module();
    }

    struct startupinfo
    {
      uint32_t        cb;
      char_type *     lpReserved;
      char_type *     lpDesktop;
      char_type *     lpTitle;
      uint32_t        dwX;
      uint32_t        dwY;
      uint32_t        dwXSize;
      uint32_t        dwYSize;
      uint32_t        dwXCountChars;
      uint32_t        dwYCountChars;
      uint32_t        dwFillAttribute;

//      uint32_t        dwFlags;
      uint32_t        useshowwindow     : 1;
      uint32_t        usesize           : 1;
      uint32_t        useposition       : 1;
      uint32_t        usecountchars     : 1;

      uint32_t        usefillattribute  : 1;
      uint32_t        runfullscreen     : 1;
      uint32_t        forceonfeedback   : 1;
      uint32_t        forceofffeedback  : 1;

      uint32_t        usestdhandles     : 1;
      uint32_t        usehotkey         : 1;
      uint32_t                          : 1;
      uint32_t        titleshortcut     : 1;

      uint16_t        wShowWindow;
      uint16_t        cbReserved2;
      uint8_t *       lpReserved2;
      legacy_handle   hStdInput;
      legacy_handle   hStdOutput;
      legacy_handle   hStdError;

      void inline get();
    };


    struct command_line
    {
      //const char_type * argv[128];
      //typedef const char_type* value_type;
      // 
      typedef std::basic_string_ref<char_type> value_type;
      std::vector<value_type> argv;
      int               argc;

      typedef value_type * iterator;
      typedef value_type const * const_iterator;
      typedef std::reverse_iterator<iterator> reverse_iterator;
      typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

      iterator begin() { return argv.begin(); }
      iterator end()   { return argv.end(); }

      const_iterator begin() const { return argv.cbegin(); }
      const_iterator end() const   { return argv.cend(); }

      const_iterator cbegin() const{ return begin(); }
      const_iterator cend() const  { return end(); }

      reverse_iterator        rbegin()        { return reverse_iterator(end()); }
      const_reverse_iterator  rbegin() const  { return const_reverse_iterator(end()); }
      reverse_iterator        rend()          { return reverse_iterator(begin()); }
      const_reverse_iterator  rend()   const  { return const_reverse_iterator(begin()); }

      const_reverse_iterator  crbegin()const { return rbegin(); }
      const_reverse_iterator  crend()  const { return rend(); }

      // random access
      const value_type operator[](int argno)
      {
        return argv[argno];
      }

      int size() const { return argc; }
      bool empty() const { return false; }

      static inline char_type * get();

    public:
      /**
       *	@brief command line option
       *  @code Usage:
       *  
       **/
      struct option
      {
        typedef std::basic_string_ref<char_type> string_ref;
        
        string_ref name;

        template<size_t N>
        option(const char_type (&name)[N])
          :name(name, N-1)
        {}
        option(const char_type* name, size_t len)
          :name(name, len)
        {}
        option(const string_ref& name)
          :name(name)
        {}

        //bool operator()(const char_type* opt) const;
        bool operator()(const string_ref& opt) const
        {
          return opt.starts_with(name) && (opt[name.length()] == '\0' || opt[name.length()] == '=');
        }

        string_ref value(const string_ref& opt) const
        {
          return (opt.starts_with(name) && opt[name.length()] == '=')
            ? string_ref(&opt[name.length()+1]) : string_ref();
        }
      };


      command_line(bool parse_default = true)
        :argc(0)
      {
        if(parse_default)
          parse();
      }

      int parse(char_type * p = get())
      {
        argc = 0;
        // self
        char_type* beg;
        if(*p == '"'){
          //argv[argc++] = ++p;
          beg = ++p;
          while(*p && *p != '"') p++;
          if(!*p) return -1;
          argv.push_back(value_type(beg, p-beg));
          *p++ = '\0';
        }else{
          //argv[argc++] = p;
          beg = p;
          while(*p < '\0' || *p > ' ') p++;
          argv.push_back(value_type(beg, p-beg));
          if(*p) *p++ = '\0';
        }
        // arguments
        while(*p){
          while(*p > '\0' && *p <= ' ') p++;
          if(!*p) break;
          if(*p == '"'){
            //argv[argc++] = ++p;
            beg = ++p;
            while(*p && *p != '"') p++;
            if(!*p) return -1;
            argv.push_back(value_type(beg, p-beg));
            *p++ = '\0';
          }else{
            //argv[argc++] = p;
            beg = p;
            while(*p < '\0' || *p > ' ') p++;
            argv.push_back(value_type(beg, p-beg));
            if(*p) *p++ = '\0';
          }
        }
        return argc = static_cast<int>(argv.size());
      }

    };

    static uint32_t last_error()
    {
      return GetLastError();
    }

};


template<>
inline char* application<char>::command_line::get()
{
  return GetCommandLineA();
}

template<>
inline wchar_t* application<wchar_t>::command_line::get()
{
  return GetCommandLineW();
}

//template<>
//inline bool application<char>::command_line::option::operator()(const char* opt) const
//{
//  const size_t len = std::strlen(opt);
//  return std::strncmp(opt, name.first, name.second-1) == 0 && 
//    ( len == name.second-1 || (len > name.second && opt[name.second-1] == '=') );
//}
//template<>
//inline bool application<wchar_t>::command_line::option::operator()(const wchar_t* opt) const
//{
//  const size_t len = std::wcslen(opt);
//  return std::wcsncmp(opt, name.first, name.second-1) == 0 && 
//    ( len == name.second-1 || (len > name.second && opt[name.second-1] == '=') );
//}


/**@} application */

typedef application<char>::startupinfo    startupinfoa;
typedef application<wchar_t>::startupinfo startupinfow;


NTL_EXTERNAPI
void __stdcall GetStartupInfoA(startupinfoa *);

NTL_EXTERNAPI
void __stdcall GetStartupInfoW(startupinfow *);

template<>
inline void startupinfoa::get()
{
  GetStartupInfoA(this);
}

template<>
inline void startupinfow::get()
{
  GetStartupInfoW(this);
}
/**@} winapi_types_support */
}//namespace win
}//namespace ntl

#endif//#ifndef NTL__WIN_APPLICATION
