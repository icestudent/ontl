/**\file*********************************************************************
 *                                                                     \brief
 *  N2315 27.6.2 Output streams [output.streams]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_SSTREAM
#define NTL__STLX_SSTREAM

namespace std {

/**\addtogroup  lib_input_output ******* [27] Input/output library **********
 *@{*/

/**\addtogroup  lib_string_streams ***** [27.7] String-based streams ********
 *@{*/
  template <class charT, class traits, class Allocator>
  class basic_stringbuf;

  typedef basic_stringbuf<char> stringbuf;
  typedef basic_stringbuf<wchar_t> wstringbuf;
  
  template <class charT, class traits, class Allocator>
  class basic_istringstream;
  
  typedef basic_istringstream<char> istringstream;
  typedef basic_istringstream<wchar_t> wistringstream;
  
  template <class charT, class traits, class Allocator>
  class basic_ostringstream;
  
  typedef basic_ostringstream<char> ostringstream;
  typedef basic_ostringstream<wchar_t> wostringstream;
  
  template <class charT, class traits, class Allocator>
  class basic_stringstream;
  
  typedef basic_stringstream<char> stringstream;
  typedef basic_stringstream<wchar_t> wstringstream;

  /**@} lib_string_streams */
  /**@} lib_input_output */
} // std

#endif // NTL__STLX_SSTREAM
