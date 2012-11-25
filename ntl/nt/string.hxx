/**\file*********************************************************************
 *                                                                     \brief
 *  NT native string types
 *
 ****************************************************************************
 */
#ifndef NTL__NT_STRING
#define NTL__NT_STRING
#pragma once

#include "basedef.hxx"
#ifndef NTL__STLX_STDEXCEPT
#include "../stlx/stdexcept_fwd.hxx"
#endif
#include "../stlx/array.hxx"
#ifndef NTL__STLX_FUNCTIONAL
#include "../stlx/functional.hxx"
#endif
#ifndef NTL__STLX_STRING
# include "../stlx/stdstring.hxx"
#endif

namespace ntl {
namespace nt {


/**\addtogroup  native_types_support *** NT Types support library ***********
 *@{*/

  /**
   *	@brief Native string allocator
   *  @details Allocator for native strings which serves several purposes:
   *  * make std::basic_string binary compatible with nt::native_string
   *  * use the same string memory management routines as the OS does.
   **/
  template<class charT>
  class string_allocator:
    public std::allocator<charT>
  {
  public:
    typedef uint16_t size_type;
    typedef typename std::allocator<charT>::pointer pointer;

    __noalias __forceinline charT* __restrict allocate(size_type n, std::allocator<void>::const_pointer = 0)
    {
      return reinterpret_cast<charT*>(heap::alloc(process_heap(), n*sizeof(value_type)));
    }

    __noalias __forceinline void deallocate(pointer p, size_type)
    {
      heap::free(process_heap(), p);
    }

    size_type max_size() const __ntl_nothrow { return static_cast<size_type>(std::numeric_limits<size_type>::max() / sizeof(value_type)); }
  };


template <class charT,
          class traits    = std::char_traits<typename std::remove_const<charT>::type>,
          class Allocator = string_allocator<charT> >
class native_string
{
  template<class argT>
  struct arg_select
  {

  };

  ///////////////////////////////////////////////////////////////////////////
  public:

    ///\name  native_string types:

    typedef           traits                      traits_type;
    typedef typename  traits::char_type           value_type;
    typedef           Allocator                   allocator_type;
    typedef charT *       pointer;
    typedef const charT * const_pointer;
    typedef charT &       reference;
    typedef const charT & const_reference;
    typedef typename  std::uint16_t               size_type;
    typedef typename  Allocator::difference_type  difference_type;
    typedef pointer                               iterator;
    typedef const_pointer                         const_iterator;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    static const size_type npos = static_cast<size_type>(-1);

    //STATIC_ASSERT((std::is_same<pointer, charT*>::value));

    ///\name  native_string constructors

    native_string()
    : length_(), maximum_length_(), buffer_()
    {/**/}

  //explicit
    native_string(
      typename std::conditional<std::is_const<charT>::value,
        const std::basic_string<value_type>&, std::basic_string<value_type>&>::type str)
    : length_(static_cast<size_type>(str.size() * sizeof(value_type))),
      maximum_length_(length_/* + sizeof(value_type)*/),
      buffer_(str.begin())
    {/**/}

 friend class native_string;

#if 0 // issue13 fix
    native_string(const native_string<value_type>& str)
    : length_(str.length_),
      maximum_length_(str.maximum_length_),
      buffer_(str.buffer_)
    {/**/}
#endif

    // construct from pointer, template required to lower priority
    template<typename Pointer>
    native_string(const Pointer& str, typename std::enable_if<std::is_same<Pointer,pointer>::value>::type* =0)
    : length_(size_type(traits::length(str)) * sizeof(value_type)),
      maximum_length_(length_),
      buffer_(str)
    {/**/}

    native_string(charT* s, size_t n)
    : length_(size_type(n) * sizeof(value_type)),
      maximum_length_(size_type(n) * sizeof(value_type)),
      buffer_(s)
    {/**/}

    // construct from char array
    template<uint16_t Size>
    native_string(const value_type (&str)[Size])
    : length_((Size - 1) * sizeof(value_type)),
      maximum_length_(length_ + sizeof(value_type)),
      buffer_(&str[0])
    {/**/}

    // if we are const, calculate length for non-const args
    template<uint16_t Size>
    native_string(value_type (&str)[Size])
      : length_(
                sizeof(value_type) *
                  (std::is_const<charT>::value ? traits::length(str) : (Size-1))
                ),
      maximum_length_(Size*sizeof(value_type)),
      buffer_(&str[0])
    {/**/}

    template<size_t Size>
    native_string(std::array<charT, Size>& str)
      : length_((Size - 1) * sizeof(value_type)),
      maximum_length_(length_ + sizeof(value_type)),
      buffer_(str.data())
    {/**/}

    // if we are const, accept all args; non-const with length calculation
    // if we are mutable, accept only non-const, with calculation
    //template<argT, uint16_t Size>
    //native_string(argT (&str)[Size], typename std::enable_if<std::is_same<value_type, typename std::remove_cv<argT>::type>::value)
    //: length_((Size - 1) * sizeof(value_type)),
    //  maximum_length_(length_ + sizeof(value_type)),
    //  buffer_(&str[0])
    //{/**/}

    ///\name  native_string connversions

    std::basic_string<value_type> get_string() const
    {
      return std::basic_string<value_type>(begin(), size());
    }

    template<class CustomAllocator>
    std::basic_string<value_type, traits_type, CustomAllocator> get_string() const
    {
      return std::basic_string<value_type, traits_type, CustomAllocator>(begin(), size());
    }

    template<template <typename> class CustomAllocator>
    std::basic_string<value_type, traits_type, CustomAllocator<value_type>> get_string() const
    {
      return std::basic_string<value_type, traits_type, CustomAllocator<value_type>>(begin(), size());
    }

///\warning this solves issue13
#ifndef __ICL
    operator
      const native_string<typename std::add_const<charT>::type, traits>&() const
    {
      return *reinterpret_cast<const native_string<std::add_const<charT>::type, traits>*>(this);
    }
#endif

    ///\name  native_string iterator support

    iterator                begin()         { return buffer_; }
    const_iterator          begin()  const  { return buffer_; }
    iterator                end()           { return &buffer_[length_ / sizeof(value_type)]; }
    const_iterator          end()    const  { return &buffer_[length_ / sizeof(value_type)]; }

    reverse_iterator        rbegin()      { return reverse_iterator(&buffer_[length_ / sizeof(value_type)]); }
    const_reverse_iterator  rbegin() const { return const_reverse_iterator(&buffer_[length_ / sizeof(value_type)]); }
    reverse_iterator        rend()       { return reverse_iterator(buffer_); }
    const_reverse_iterator  rend() const { return const_reverse_iterator(buffer_); }

    const_iterator          cbegin()  const { return begin(); }
    const_iterator          cend()    const { return end();   }
    const_reverse_iterator  crbegin() const { return rbegin();}
    const_reverse_iterator  crend()   const { return rend();  }

    ///\name  native_string capacity

    size_type size()      const { return length_ / sizeof(value_type); }
    size_type length()    const { return length_ / sizeof(value_type); }
    size_type max_size()  const { return maximum_length_ / sizeof(value_type); }
    size_type capacity()  const { return maximum_length_ / sizeof(value_type); }
    bool empty()          const { return length_ == 0; }

    ///\name  native_string element access

    const_reference operator[](size_type pos) const
    {
#if 0
      static const charT nullchar;
      return pos < size() ? buffer_[pos] : nullchar;
#else
      return buffer_[pos];
#endif
    }

    reference operator[](size_type pos)   { return buffer_[pos];  }

    const_reference at(size_type n) const
    {
      check_bounds(n);
      return operator[](n);
    }

    reference at(size_type n)
    {
      check_bounds(n);
      return operator[](n);
    }

    ///\name  native_string::swap
    void swap(native_string& str)
    {
      std::swap(length_, str.length_);
      std::swap(maximum_length_, str.maximum_length_);
      std::swap(buffer_, str.buffer_);
    }

    ///\name  native_string string operations
    const charT * data() const
    {
#if 0
      static const charT empty;
      return begin() ? begin() : &empty;
#else
      return begin();
#endif
    }

    ///\name  compare
    int compare(const native_string& str) const;

    ///\name  operator==
    friend
      bool operator==(const native_string& lhs, const native_string& rhs)
      {
        return lhs.length_== rhs.length_
            && std::equal(lhs.buffer_, &lhs.buffer_[lhs.length_/sizeof(charT)], rhs.buffer_);
      }

    ///\name  operator!=
    friend
      bool operator!=(const native_string& lhs, const native_string& rhs)
      {
        return ! (lhs == rhs);
      }

    ///}

  private:
    uint16_t  length_;
    uint16_t  maximum_length_;
    charT   * buffer_;

    void check_bounds(size_type n) const __ntl_throws(std::out_of_range)
    {
      if ( n > size() ) __ntl_throw (std::out_of_range(__FUNCTION__));
    }
    const native_string& operator=(const native_string & s);


};//class native_string


typedef native_string<wchar_t>        unicode_string;
typedef native_string<const wchar_t>  const_unicode_string;

typedef native_string<char>           ansi_string;
typedef native_string<const char>     const_ansi_string;

template<typename char_type>
struct raw_native_string
{
  uint16_t    length;
  uint16_t    max_length;
  char_type*  buffer;
};

typedef raw_native_string<char>     raw_ansi_string;
typedef raw_native_string<wchar_t>  raw_unicode_string;


/**@} native_types_support */

/** @defgroup native_string_support String support routines
 *@{*/

///\name String-integer conversion
NTL_EXTERNAPI ntstatus __stdcall RtlIntegerToUnicodeString(uint32_t Value, uint32_t Base, unicode_string& String);
NTL_EXTERNAPI ntstatus __stdcall RtlInt64ToUnicodeString(uint64_t Value, uint32_t Base, unicode_string& String);
NTL_EXTERNAPI ntstatus __stdcall RtlUnicodeStringToInteger(const const_unicode_string& String, uint32_t Base, uint32_t& Value);

///\name String conversion
NTL_EXTERNAPI ntstatus __stdcall RtlUnicodeStringToAnsiString(ansi_string& DestinationString, const const_unicode_string& SourceString, bool AllocateDestination);
NTL_EXTERNAPI uint32_t __stdcall RtlUnicodeStringToAnsiSize(const const_unicode_string& SourceString);

NTL_EXTERNAPI ntstatus __stdcall RtlAnsiStringToUnicodeString(unicode_string& DestinationString, const const_ansi_string& SourceString, bool AllocateDestination);
NTL_EXTERNAPI uint32_t __stdcall RtlAnsiStringToUnicodeSize(const const_ansi_string& SourceString);

NTL_EXTERNAPI ntstatus __stdcall RtlMultiByteToUnicodeSize(uint32_t& BytesInUnicodeString, const char* MultiByteString, uint32_t BytesInMultiByteString);
NTL_EXTERNAPI ntstatus __stdcall RtlUnicodeToMultiByteSize(uint32_t& BytesInMultiByteString, const wchar_t* UnicodeString, uint32_t BytesInUnicodeString);
NTL_EXTERNAPI ntstatus __stdcall RtlMultiByteToUnicodeN(wchar_t* UnicodeString, uint32_t MaxBytesInUnicodeString, uint32_t* BytesInUnicodeString __optional, const char* MultiByteString, uint32_t BytesInMultiByteString);
NTL_EXTERNAPI ntstatus __stdcall RtlUnicodeToMultiByteN(char* MultiByteString, uint32_t MaxBytesInMultiByteString, uint32_t* BytesInMultiByteString __optional, const wchar_t* UnicodeString, uint32_t BytesInUnicodeString);

NTL_EXTERNAPI wchar_t __stdcall RtlAnsiCharToUnicodeChar(unsigned char**);

__forceinline wchar_t __fastcall widen(char c)
{
  unsigned char* pc = reinterpret_cast<unsigned char*>(&c);
  return RtlAnsiCharToUnicodeChar(&pc);
}

///\name String case mapping conversion
NTL_EXTERNAPI wchar_t __stdcall RtlUpcaseUnicodeChar(wchar_t);
NTL_EXTERNAPI ntstatus __stdcall RtlUpcaseUnicodeString(unicode_string& DestinationString, const_unicode_string const& SourceString, bool AllocateDestination);
NTL_EXTERNAPI ntstatus __stdcall RtlDowncaseUnicodeString(unicode_string& DestinationString, const_unicode_string const& SourceString, bool AllocateDestination);
NTL_EXTERNAPI void __stdcall RtlUpperString(ansi_string& DestinationString, const_ansi_string const& SourceString);

#ifndef NTL_SUBSYSTEM_KM
NTL_EXTERNAPI wchar_t __stdcall RtlDowncaseUnicodeChar(wchar_t);
#else
extern "C" inline wchar_t __stdcall RtlDowncaseUnicodeChar(wchar_t c)
{
  wchar_t dc = c + 'a'-'A';
  unicode_string udc(&dc,1);
  const const_unicode_string uc(&c,1);
  RtlDowncaseUnicodeString(udc, uc, false);
  return dc;
}
#endif


///\name String comparation
NTL_EXTERNAPI int32_t __stdcall RtlCompareUnicodeStrings(const wchar_t* String1, size_t String1Length, const wchar_t* String2, size_t String2Length);
NTL_EXTERNAPI int32_t __stdcall RtlCompareUnicodeString (const const_unicode_string& String1, const const_unicode_string& String2, bool CaseInSensitive);
NTL_EXTERNAPI int32_t __stdcall RtlCompareString (const const_ansi_string& String1, const const_ansi_string& String2, bool CaseInSensitive);
NTL_EXTERNAPI bool    __stdcall RtlEqualUnicodeString(const const_unicode_string& String1, const const_unicode_string& String2);
NTL_EXTERNAPI bool    __stdcall RtlEqualString(const const_ansi_string& String1, const const_ansi_string& String2);

namespace hash_string_algorithm
{
  enum type {
    def,
    x65599,
    invalid = -1
  };
}
NTL_EXTERNAPI ntstatus __stdcall RtlHashUnicodeString(const const_unicode_string& String, bool CaseInSensitive, hash_string_algorithm::type HashAlgorithm, uint32_t& HashValue);

inline uint32_t hash_string(const const_unicode_string& String, bool CaseInSensetive = true, hash_string_algorithm::type Algorithm = hash_string_algorithm::def)
{
  uint32_t hash = 0;
  RtlHashUnicodeString(String, CaseInSensetive, Algorithm, hash);
  return hash;
}

///\name String allocation
NTL_EXTERNAPI void __stdcall RtlFreeAnsiString(ansi_string& UnicodeString);
NTL_EXTERNAPI void __stdcall RtlFreeUnicodeString(unicode_string& UnicodeString);
///\}

/** @} native_string_support */

}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_STRING
