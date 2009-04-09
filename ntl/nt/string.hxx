/**\file*********************************************************************
 *                                                                     \brief
 *  NT native string types
 *
 ****************************************************************************
 */

#ifndef NTL__NT_STRING
#define NTL__NT_STRING

#include "basedef.hxx"
#include "../stdexcept"
#include "../string"
#include "../type_traits"
#include "../basedef.hxx"
#include "../stlx/cstdint.hxx"
#include "../stlx/array.hxx"
#include "../stlx/functional.hxx"

namespace ntl {
namespace nt {

/**\addtogroup  native_types_support *** NT Types support library ***********
 *@{*/

template <class charT,
          class traits    = std::char_traits<typename std::remove_const<charT>::type>,
          class Allocator = std::allocator<charT> >
class native_string
{

  ///////////////////////////////////////////////////////////////////////////
  public:

    ///\name  native_string types:

    typedef typename  traits                      traits_type;
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

  explicit
    native_string(
      typename std::conditional<std::is_const<charT>::value,
        const std::basic_string<value_type>&, std::basic_string<value_type>&>::type str)
    : length_(static_cast<size_type>(str.size() * sizeof(value_type))),
      maximum_length_(length_/* + sizeof(value_type)*/),
      buffer_(str.begin())
    {/**/}

 friend class native_string;

#if 0
    native_string(const native_string<value_type>& str)
    : length_(str.length_),
      maximum_length_(str.maximum_length_),
      buffer_(str.buffer_)
    {/**/}
#endif

    native_string(charT* s, size_t n)
    : length_(size_type(n) * sizeof(value_type)),
      maximum_length_(size_type(n) * sizeof(value_type)),
      buffer_(s)
    {/**/}

    template<uint16_t Size>
    native_string(const value_type (&str)[Size])
    : length_((Size - 1) * sizeof(value_type)),
      maximum_length_(length_ + sizeof(value_type)),
      buffer_(&str[0])
    {/**/}

    template<uint16_t Size>
    native_string(value_type (&str)[Size])
    : length_((Size - 1) * sizeof(value_type)),
      maximum_length_(length_ + sizeof(value_type)),
      buffer_(&str[0])
    {/**/}

    template<size_t Size>
    native_string(std::array<charT, Size>& str)
      : length_((Size - 1) * sizeof(value_type)),
      maximum_length_(length_ + sizeof(value_type)),
      buffer_(str.data())
    {/**/}

    ///\name  native_string connversions

    std::basic_string<value_type>
      get_string() const
    {
      return std::basic_string<value_type>(begin(), size());
    }

    operator
      const native_string<typename std::add_const<charT>::type, traits>&() const
    {
      return *reinterpret_cast<const native_string<std::add_const<charT>::type, traits>*>(this);
    }

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


/**@} native_types_support */

/** @defgroup native_string_support String support routines
 *@{*/

///\name String-integer conversion
NTL__EXTERNAPI ntstatus __stdcall RtlIntegerToUnicodeString(uint32_t Value, uint32_t Base, unicode_string& String);
NTL__EXTERNAPI ntstatus __stdcall RtlInt64ToUnicodeString(uint64_t Value, uint32_t Base, unicode_string& String);
NTL__EXTERNAPI ntstatus __stdcall RtlUnicodeStringToInteger(const const_unicode_string& String, uint32_t Base, uint32_t& Value);

///\name String conversion
NTL__EXTERNAPI ntstatus __stdcall RtlUnicodeStringToAnsiString(ansi_string& DestinationString, const const_unicode_string& SourceString, bool AllocateDestination);
NTL__EXTERNAPI uint32_t __stdcall RtlUnicodeStringToAnsiSize(const const_unicode_string& SourceString);

NTL__EXTERNAPI ntstatus __stdcall RtlAnsiStringToUnicodeString(unicode_string& DestinationString, const const_ansi_string& SourceString, bool AllocateDestination);
NTL__EXTERNAPI uint32_t __stdcall RtlAnsiStringToUnicodeSize(const const_ansi_string& SourceString);

NTL__EXTERNAPI ntstatus __stdcall RtlMultiByteToUnicodeSize(uint32_t& BytesInUnicodeString, const char* MultiByteString, uint32_t BytesInMultiByteString);
NTL__EXTERNAPI ntstatus __stdcall RtlUnicodeToMultiByteSize(uint32_t& BytesInMultiByteString, const wchar_t* UnicodeString, uint32_t BytesInUnicodeString);
NTL__EXTERNAPI ntstatus __stdcall RtlMultiByteToUnicodeN(wchar_t* UnicodeString, uint32_t MaxBytesInUnicodeString, uint32_t* BytesInUnicodeString __optional, const char* MultiByteString, uint32_t BytesInMultiByteString);
NTL__EXTERNAPI ntstatus __stdcall RtlUnicodeToMultiByteN(char* MultiByteString, uint32_t MaxBytesInMultiByteString, uint32_t* BytesInMultiByteString __optional, const wchar_t* UnicodeString, uint32_t BytesInUnicodeString);

NTL__EXTERNAPI wchar_t __stdcall RtlAnsiCharToUnicodeChar(unsigned char**);

__forceinline wchar_t __fastcall widen(char c)
{
  unsigned char* pc = reinterpret_cast<unsigned char*>(&c);
  return RtlAnsiCharToUnicodeChar(&pc);
}

///\name String case mapping conversion
NTL__EXTERNAPI wchar_t __stdcall RtlUpcaseUnicodeChar(wchar_t);
NTL__EXTERNAPI wchar_t __stdcall RtlDowncaseUnicodeChar(wchar_t);
NTL__EXTERNAPI ntstatus __stdcall RtlUpcaseUnicodeString(unicode_string& DestinationString, const_unicode_string const& SourceString, bool AllocateDestination);
NTL__EXTERNAPI void __stdcall RtlUpperString(ansi_string& DestinationString, const_ansi_string const& SourceString);


///\name String comparation
NTL__EXTERNAPI int32_t __stdcall RtlCompareUnicodeStrings(const wchar_t* String1, size_t String1Length, const wchar_t* String2, size_t String2Length);
NTL__EXTERNAPI int32_t __stdcall RtlCompareUnicodeString (const const_unicode_string& String1, const const_unicode_string& String2, bool CaseInSensitive);
NTL__EXTERNAPI int32_t __stdcall RtlCompareString (const const_ansi_string& String1, const const_ansi_string& String2, bool CaseInSensitive);
NTL__EXTERNAPI bool    __stdcall RtlEqualUnicodeString(const const_unicode_string& String1, const const_unicode_string& String2);
NTL__EXTERNAPI bool    __stdcall RtlEqualString(const const_ansi_string& String1, const const_ansi_string& String2);

namespace hash_string_algorithm
{
  enum type {
    def,
    x65599,
    invalid = -1
  };
}
NTL__EXTERNAPI ntstatus __stdcall RtlHashUnicodeString(const const_unicode_string& String, bool CaseInSensitive, hash_string_algorithm::type HashAlgorithm, uint32_t& HashValue);

inline uint32_t hash_string(const const_unicode_string& String, bool CaseInSensetive = true, hash_string_algorithm::type Algorithm = hash_string_algorithm::def)
{
  uint32_t hash = 0;
  RtlHashUnicodeString(String, CaseInSensetive, Algorithm, hash);
  return hash;
}

///\name String allocation
NTL__EXTERNAPI void __stdcall RtlFreeAnsiString(ansi_string& UnicodeString);
NTL__EXTERNAPI void __stdcall RtlFreeUnicodeString(unicode_string& UnicodeString);
///\}

/** @} native_string_support */

}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_STRING
