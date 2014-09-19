/**\file*********************************************************************
 *                                                                     \brief
 *  [N3594] String join
 *
 ****************************************************************************
 */
#ifndef NTL__EXT_JOIN
#define NTL__EXT_JOIN
#pragma once

#include "../type_traits.hxx"
#include "../string_ref.hxx"
#include "../sstream.hxx"

namespace std
{
  /**
   *	@brief Default Formatter
   *	
   *	The default formatter will be able to format the following types:
   *	* std::string
   *  * std::string_ref
   *  * const char*
   *  * char*
   *  * All primitive types such as int, char, float, double, bool, etc.
   **/
  struct join_formatter
  {
    void operator()(std::string& o, const std::string_ref& n) const
    {
      o.append(n.data(), n.size());
    }
    void operator()(std::string& o, const char n) const
    {
      o.append(1, n);
    }
    template<typename Number>
    typename std::enable_if<std::is_arithmetic<Number>::value, void>::type operator()(std::string& o, Number n) const
    {
      o.append(std::to_string(n));
    }
  };

  /** Formatter that can format any numeric type that is compatible with std::to_string */
  struct number_formatter
  {
    template<typename Number>
    typename std::enable_if<std::is_arithmetic<Number>::value, void>::type operator()(std::string& o, Number n) const
    {
      o.append(std::to_string(n));
    }
  };

  /** Stream-based formatter for iterators */
  template <class InputIterator>
  inline std::string join(InputIterator first, InputIterator last, const std::string_ref& sep)
  {
    std::ostringstream o;

    if(first != last) {
      o << *first;
      ++first;
    }
    while(first != last) {
      o << sep;
      o << *first;
      ++first;
    }
    return o.str();
  }

  /** Stream-based formatter for container */
  template <class Range>
  inline std::string join(const Range& range, const std::string_ref& sep)
  {
    return join(std::begin(range), std::end(range), sep);
  }

  // Range and Formatter
  template <class InputIterator, typename Formatter>
  inline std::string sjoin(InputIterator first, InputIterator last, const std::string_ref& sep, Formatter format)
  {
    std::string o;
    o.reserve(128);

    if(first != last) {
      format(o, *first);
      ++first;
    }
    while(first != last) {
      o.append(sep.data(), sep.size());
      format(o, *first);
      ++first;
    }
    return std::move(o);
  }


  // Range (a default formatter is used)
  template <class InputIterator>
  inline std::string sjoin(InputIterator first, InputIterator last, const std::string_ref& sep)
  {
    return join(first, last, sep, join_formatter());
  }

  template <class Range, typename Formatter>
  inline std::string sjoin(const Range& range, const std::string_ref& sep, Formatter format)
  {
    return sjoin(std::begin(range), std::end(range), sep, format);
  }
}
#endif // NTL__EXT_JOIN
