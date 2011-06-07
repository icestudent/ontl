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


namespace ntl {
namespace win {
/**\addtogroup  winapi_types_support *** Win32 API support library **********
 *@{*/

#ifndef NTL__NO_AUTOLINK
# pragma comment(lib,    "kernel32.lib")
#endif


NTL__EXTERNAPI
char * __stdcall
  GetCommandLineA();

NTL__EXTERNAPI
wchar_t * __stdcall
  GetCommandLineW();

NTL__EXTERNAPI
uint32_t __stdcall
  GetLastError();

NTL__EXTERNAPI
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
      const char_type * argv[128];
      int               argc;

      typedef const char_type * * iterator;
      typedef const char_type* const * const_iterator;
      typedef std::reverse_iterator<iterator> reverse_iterator;
      typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

      iterator begin() { return &argv[0]; }
      iterator end()   { return &argv[argc]; }

      const_iterator begin() const { return &argv[0]; }
      const_iterator end() const   { return &argv[argc]; }

      const_iterator cbegin() const{ return begin(); }
      const_iterator cend() const  { return end(); }

      reverse_iterator        rbegin()        { return reverse_iterator(end()); }
      const_reverse_iterator  rbegin() const  { return const_reverse_iterator(end()); }
      reverse_iterator        rend()          { return reverse_iterator(begin()); }
      const_reverse_iterator  rend()   const  { return const_reverse_iterator(begin()); }

      const_reverse_iterator  crbegin()const { return rbegin(); }
      const_reverse_iterator  crend()  const { return rend(); }

      // random access
      const char_type* operator[](int argno)
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
        std::pair<const char_type*, size_t> name;

        template<size_t N>
        option(const char_type (&name)[N])
          :name(name, N)
        {}

        bool operator()(const char_type* opt) const;
        bool operator()(const std::basic_string<char_type>& opt) const { return opt.compare(0, name.second-1, name.first) == 0; }

        const char_type* value(const char_type* opt) const
        {
          static const char_type null = '\0';
          return opt[name.second-1] == '=' ? opt + name.second : &null;
        }
        std::basic_string<char_type> value(const std::basic_string<char_type>& opt) const
        {
          return name.second < opt.length() && opt[name.second-1] == '=' ? opt.substr(name.second) : std::basic_string<char_type>();
        }
      };


      command_line()
      {
        parse();
      }

      int parse(char_type * p = get())
      {
        argc = 0;
        // self
        if(*p == '"'){
          argv[argc++] = ++p;
          while(*p && *p != '"') p++;
          if(!*p) return -1;
          *p++ = '\0';
        }else{
          argv[argc++] = p;
          while(*p < '\0' || *p > ' ') p++;
          if(*p) *p++ = '\0';
        }
        // arguments
        while(*p){
          while(*p > '\0' && *p <= ' ') p++;
          if(!*p) break;
          if(*p == '"'){
            argv[argc++] = ++p;
            while(*p && *p != '"') p++;
            if(!*p) return -1;
            *p++ = '\0';
          }else{
            argv[argc++] = p;
            while(*p < '\0' || *p > ' ') p++;
            if(*p) *p++ = '\0';
          }
        }
        return argc;
      }

    };

    static uint32_t last_error()
    {
      return GetLastError();
    }

};


template<>
char *
application<char>::command_line::get()
{
  return GetCommandLineA();
}

template<>
wchar_t *
application<wchar_t>::command_line::get()
{
  return GetCommandLineW();
}

template<>
bool application<char>::command_line::option::operator()(const char* opt) const
{
  return std::strncmp(opt, name.first, name.second-1) == 0;
}
template<>
bool application<wchar_t>::command_line::option::operator()(const wchar_t* opt) const
{
  return std::wcsncmp(opt, name.first, name.second-1) == 0;
}


/**@} application */

typedef application<char>::startupinfo    startupinfoa;
typedef application<wchar_t>::startupinfo startupinfow;


NTL__EXTERNAPI
void __stdcall GetStartupInfoA(startupinfoa *);

NTL__EXTERNAPI
void __stdcall GetStartupInfoW(startupinfow *);

template<>
void startupinfoa::get()
{
  GetStartupInfoA(this);
}

template<>
void startupinfow::get()
{
  GetStartupInfoW(this);
}
/**@} winapi_types_support */
}//namespace win
}//namespace ntl

#endif//#ifndef NTL__WIN_APPLICATION
