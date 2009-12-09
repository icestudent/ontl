/**\file*********************************************************************
 *                                                                     \brief
 *  String classes [21.2 string.classes]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_STRING
#define NTL__STLX_STRING
#pragma once

#include "cuchar.hxx"
#ifndef NTL__STLX_IOSFWD
# include "iosfwd.hxx"
#endif
#ifndef NTL__STLX_MEMORY
#include "memory.hxx"
#endif
#ifndef NTL__STLX_FUNCTIONAL
# include "functional.hxx" // for hash<>
#endif
#ifndef NTL__STLX_ALGORITHM
# include "algorithm.hxx"  // for min/max
#endif
#ifndef NTL__EXT_NUMERIC_CONVERSIONS
# include "ext/numeric_conversions.hxx"
#endif

#ifndef NTL__STLX_STDEXCEPT_FWD
# include "stdexcept_fwd.hxx"
#endif

#ifndef NTL__STDLIB
# include "../stdlib.hxx"
#endif

#ifndef EOF // should be moved to "stdio.hxx" ?
# define EOF -1
#endif

namespace std {

typedef int64_t streamoff;

/**\defgroup  lib_strings ************* 21 Strings library [strings]
 *@{*/

/**\defgroup  lib_char_traits ********* 21.1 Character traits [char.traits]
 *@{*/

/// 21.1 Character traits [char.traits]
/// a basis for explicit specializations.
template<class charT> struct char_traits;

#ifdef __ICL
#pragma warning(disable:2259) // non-pointer conversion from "std::uint_least16_t" to "unsigned char" may lose significant bits
#endif

///\name 21.1.3 char_traits specializations [char.traits.specializations]

/// 21.1.3.1 struct char_traits<char> [char.traits.specializations.char]
template<>
struct char_traits<char>
{
  typedef char      char_type;
  typedef int       int_type;
  typedef streamoff off_type;
  typedef streampos pos_type;
  typedef mbstate_t state_type;

  static void assign(char_type& c1, const char_type& c2) { c1 = c2; }
  static bool eq(const char_type& c1, const char_type& c2) { return c1 == c2; }
  static bool lt(const char_type& c1, const char_type& c2) { return c1 < c2; }
  static int compare(const char_type* s1, const char_type* s2, size_t n)
    { return strncmp(s1, s2, n); }
  static size_t length(const char_type* s) { return strlen(s); }
  static const char_type* find(const char_type* s, size_t n, const char_type& a)
    { while ( n-- ) if ( a != *s ) ++s; else return s; return 0; }
  static char_type* move(char_type* dst, const char_type* src, size_t n)
    { return reinterpret_cast<char_type*>(memmove(dst, src, n)); }
  static char_type* copy(char_type* dst, const char_type* src, size_t n)
    { return reinterpret_cast<char_type*>(memcpy(dst, src, n)); }
  static char_type* assign(char_type* s, size_t n, char_type a)
    { for ( char_type * p = s; n; --n, ++p ) *p = a; return s; }
  static int_type not_eof(const int_type& c) { return eof() != c ? c : 0; }
  static char_type to_char_type(const int_type& c)
    { return static_cast<char_type>(static_cast<unsigned char>(c)); }
  static int_type to_int_type(const char_type& c)
    { return static_cast<unsigned char>(c); }
  static bool eq_int_type(const int_type& c1, const int_type& c2)
    { return c1 == c2; }
  static int_type eof() { return EOF; }
};

/// 21.1.3.2 struct char_traits<char16_t> [char.traits.specializations.char16_t]
template<>
struct char_traits<char16_t>
{
  typedef char16_t        char_type;
  typedef uint_least16_t  int_type;
  typedef streamoff       off_type;
  typedef u16streampos    pos_type;
  typedef mbstate_t       state_type;

  static void assign(char_type& c1, const char_type& c2) { c1 = c2; }
  static bool eq(const char_type& c1, const char_type& c2) { return c1 == c2; }
  static bool lt(const char_type& c1, const char_type& c2) { return c1 < c2; }

  static int compare(const char_type* s1, const char_type* s2, size_t n)
  { return wcsncmp(reinterpret_cast<const wchar_t*>(s1), reinterpret_cast<const wchar_t*>(s2), n); }
  static size_t length(const char_type* s) { return wcslen(reinterpret_cast<const wchar_t*>(s)); }
  static const char_type* find(const char_type* s, size_t n, const char_type& a)
  { while ( n-- ) if ( a != *s ) ++s; else return s; return 0; }
  static char_type* move(char_type* dst, const char_type* src, size_t n)
  { return reinterpret_cast<char_type*>(memmove(dst, src, n*sizeof(char_type))); }
  static char_type* copy(char_type* dst, const char_type* src, size_t n)
  { return reinterpret_cast<char_type*>(memcpy(dst, src, n*sizeof(char_type))); }
  static char_type* assign(char_type* s, size_t n, char_type a)
  { for ( char_type * p = s; n; --n, ++p ) *p = a; return s; }

  static int_type not_eof(const int_type& c) { return eof() != c ? c : 0U; }
  static char_type to_char_type(const int_type& c)
  { return static_cast<char_type>(static_cast<unsigned char>(c)); }
  static int_type to_int_type(const char_type& c)
  { return static_cast<unsigned char>(c); }
  static bool eq_int_type(const int_type& c1, const int_type& c2)
  { return c1 == c2; }
  static int_type eof() { return static_cast<int_type>(EOF); }
};


/// 21.1.3.3 struct char_traits<char32_t> [char.traits.specializations.char16_t]
template<>
struct char_traits<char32_t>
{
  typedef char32_t        char_type;
  typedef uint_least32_t  int_type;
  typedef streamoff       off_type;
  typedef u32streampos    pos_type;
  typedef mbstate_t       state_type;

  static void assign(char_type& c1, const char_type& c2) { c1 = c2; }
  static bool eq(const char_type& c1, const char_type& c2) { return c1 == c2; }
  static bool lt(const char_type& c1, const char_type& c2) { return c1 < c2; }

  static int compare(const char_type* s1, const char_type* s2, size_t n)
  { return memcmp(s1, s2, n*sizeof(char_type)); }
  static size_t length(const char_type* s)
  {
    size_t n = 0;
    while(*s++)
      n++;
    return n;
  }
  static const char_type* find(const char_type* s, size_t n, const char_type& a)
  { while ( n-- ) if ( a != *s ) ++s; else return s; return 0; }
  static char_type* move(char_type* dst, const char_type* src, size_t n)
  { return reinterpret_cast<char_type*>(memmove(dst, src, n*sizeof(char_type))); }
  static char_type* copy(char_type* dst, const char_type* src, size_t n)
  { return reinterpret_cast<char_type*>(memcpy(dst, src, n*sizeof(char_type))); }
  static char_type* assign(char_type* s, size_t n, char_type a)
  { for ( char_type * p = s; n; --n, ++p ) *p = a; return s; }

  static int_type not_eof(const int_type& c) { return eof() != c ? c : 0; }
  static char_type to_char_type(const int_type& c)
  { return static_cast<char_type>(static_cast<unsigned char>(c)); }
  static int_type to_int_type(const char_type& c)
  { return static_cast<unsigned char>(c); }
  static bool eq_int_type(const int_type& c1, const int_type& c2)
  { return c1 == c2; }
  static int_type eof() { return static_cast<int_type>(EOF); }
};
#ifdef __ICL
#pragma warning(default:2259)
#endif

/// 21.1.3.4 struct char_traits<wchar_t> [char.traits.specializations.wchar.t]
template<>
struct char_traits<wchar_t>
{
  typedef wchar_t     char_type;
  typedef wint_t      int_type;
  typedef streamoff   off_type;
  typedef wstreampos  pos_type;
  typedef mbstate_t   state_type;

  static void assign(char_type& c1, const char_type& c2) { c1 = c2; }
  static bool eq(const char_type& c1, const char_type& c2) { return c1 == c2; }
  static bool lt(const char_type& c1, const char_type& c2) { return c1 < c2; }
  static int compare(const char_type* s1, const char_type* s2, size_t n)
    { return wcsncmp(s1, s2, n); }
  static size_t length(const char_type* s) { return wcslen(s); }
  static const char_type* find(const char_type* s, size_t n, const char_type& a)
    { while ( n-- ) if ( a != *s ) ++s; else return s; return 0; }
  static char_type* move(char_type* dst, const char_type* src, size_t n)
    { return reinterpret_cast<char_type*>(memmove(dst, src, n * sizeof(char_type))); }
  static char_type* copy(char_type* dst, const char_type* src, size_t n)
    { return reinterpret_cast<char_type*>(memcpy(dst, src, n * sizeof(char_type))); }
  static char_type* assign(char_type* s, size_t n, char_type a)
    { for ( char_type * p = s; n; --n, ++p ) *p = a; return s; }
  static int_type not_eof(const int_type& c)
    { return eof() != c ? c : static_cast<int_type>(0); }
  static char_type to_char_type(const int_type& c)
    { return static_cast<char_type>(c); }
  static int_type to_int_type(const char_type& c)
    { return static_cast<int_type>(c); }
  static bool eq_int_type(const int_type& c1, const int_type& c2)
    { return c1 == c2; }
  static int_type eof() { return WEOF; }
};
///\}
/**@} lib_char_traits */

/**
 *  @brief 21.3 Class template basic_string [basic.string]
 *
 *  - The class template basic_string describes objects that can store a sequence
 *    consisting of a varying number of arbitrary char-like objects with
 *    the first element of the sequence at position zero. Such a sequence is
 *    also called a "string" if the type of the char-like objects that it holds
 *    is clear from context. In the rest of this clause, the type of the char-like
 *    objects held in a basic_string object is designated by charT.
 *  - The member functions of basic_string use an object of the Allocator class
 *    passed as a template parameter to allocate and free storage for the contained
 *    char-like objects. (Allocator::value_type must name the same type as charT (21.3.1))
 *  - The class template basic_string conforms to the requirements for a Sequence
 *    Container (23.1.3), for a Reversible Container (23.1), and for
 *    an Allocator-aware container (92). The iterators supported by basic_string
 *    are random access iterators (24.1.5).
 *  - In all cases, <tt>size() <= capacity()</tt>.
 *  - The functions described in this clause can report two kinds of errors,
 *    each associated with an exception type:
 *    - a length error is associated with exceptions of type length_error (19.1.4);
 *    - an out-of-range error is associated with exceptions of type out_of_range (19.1.5).
 *    .
 *  .
 * \note 21.3.1/3
 *    The char-like objects in a basic_string object shall be stored contiguously.
 *    That is, for any basic_string object s,
 *    the identity <tt>&*(s.begin() + n) == &*s.begin() + n</tt>
 *    shall hold for all values of \e n such that <tt>0 <= n < s.size()</tt>.
 **/
  template <class charT, class traits, class Allocator>
  class basic_string
  {
    typedef allocator_traits<Allocator> allocator_traits;
    typedef typename allocator_traits::size_type size_type_;

    /** because Requires: s shall not be a null pointer [21.4.2/9] 
    and http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-closed.html#466 **/
    basic_string(nullptr_t, const Allocator& a = Allocator()) __deleted; // string(nullptr)
    basic_string(int, const Allocator& a = Allocator()) __deleted;       // string(0)
    basic_string(nullptr_t, size_type_, const Allocator& a = Allocator()) __deleted;
    //basic_string(int, size_type_, const Allocator& a = Allocator()) __deleted; // icl can't resolve string(1, '1') and string(0, 0)

    ///////////////////////////////////////////////////////////////////////////
  public:
    ///\name Types
    typedef          traits                            traits_type;
    typedef typename allocator_traits::value_type      value_type;
    typedef typename allocator_traits::allocator_type  allocator_type;
    typedef typename allocator_traits::size_type       size_type;
    typedef typename allocator_traits::difference_type difference_type;
    typedef typename allocator_traits::reference       reference;
    typedef typename allocator_traits::const_reference const_reference;
    typedef typename allocator_traits::pointer         pointer;
    typedef typename allocator_traits::const_pointer   const_pointer;
    
    typedef pointer         iterator;
    typedef const_pointer   const_iterator;

    typedef std::reverse_iterator<iterator>            reverse_iterator;
    typedef std::reverse_iterator<const_iterator>      const_reverse_iterator;

  private:
    static const charT zero_char = 0;
    size_type length_, capacity_;
    charT* buffer_;
    Allocator alloc;
  public:
    static const size_type npos = static_cast<size_type>(-1);

    ///\name 21.4.2 basic_string constructors and assigment operators [string.cons]

    /// 1 Effects: Constructs an object of class basic_string.
    /// Postconditions:
    /// - data() a non-null pointer that is copyable and can have 0 added to it;
    /// - size() == 0;
    /// - capacity() an unspecified value.
    explicit basic_string(const Allocator& a = Allocator())
      :alloc(a), length_(), capacity_(), buffer_()
    {}


    /// 2 Effects: Constructs an object of class basic_string as indicated below.
    ///   The stored Allocator value is copied from str.get_allocator().
    /// Postconditions:
    /// - data() points at the first element of an allocated copy of the
    ///   array whose first element is pointed at by str.data();
    /// - size() == str.size();
    /// - capacity() is at least as large as size().
    __forceinline
    basic_string(const basic_string& str)
      :alloc(str.alloc), length_(), capacity_(), buffer_()
    {
      append(str);
    }

    /// 4 Requires: pos <= str.size()
    /// 5 \todo Throws: out_of_range if pos > str.size().
    /// 6 Effects: Constructs an object of class basic_string and determines
    ///   the effective length rlen of the initial string value as the smaller
    ///   of n and str.size() - pos.
    /// Postconditions:
    /// - data() points at the first element of an allocated copy of rlen
    ///   consecutive elements of the string controlled by str beginning
    ///   at position pos;
    /// - size() == rlen;
    /// - capacity() is at least as large as size().
    basic_string(const basic_string& str, size_type pos, size_type n = npos, const Allocator& a = Allocator()) __ntl_throws(out_of_range)
      :alloc(a), length_(), capacity_(), buffer_()
    {
      if(pos > str.size()){
        __throw_out_of_range(__func__": invalid `pos`");
        return;
      }
      if(!str.empty() && pos < str.size())
        append(str, pos, n);
    }

    /// 7 Requires: s shall not be a null pointer and n < npos.
    /// 8 Effects: Constructs an object of class basic_string and determines
    ///   its initial string value from the array of charT of length n whose
    ///   first element is designated by s.
    /// Postconditions:
    /// - data() points at the first element of an allocated copy of the
    ///   array whose first element is pointed at by s;
    /// - size() == n;
    /// - capacity() is at least as large as size().
    basic_string(const charT* s, size_type n, const Allocator& a = Allocator())
      :alloc(a), length_(), capacity_(), buffer_()
    {
      if(!assert_pos(n) || !assert_ptr(s)) return;
      append(s, n);
    }

    /// 9 Requires: s shall not be a null pointer.
    /// 10 Effects: Constructs an object of class basic_string and determines
    ///   its initial string value from the array of charT of length traits::length(s)
    ///   whose first element is designated by s.
    /// Postconditions:
    /// - data() points at the first element of an allocated copy of the
    ///   array whose first element is pointed at by s;
    /// - size() == traits::length(s);
    /// - capacity() is at least as large as size().
    basic_string(const charT* s, const Allocator& a = Allocator())
      :alloc(a), length_(), capacity_(), buffer_()
    {
      if(!assert_ptr(s)) return;
      append(s);
    }

    /// 12 Requires: n < npos
    /// 13 Effects: Constructs an object of class basic_string and determines
    ///   its initial string value by repeating the charlikeobject c for all
    ///   n elements.
    /// Postconditions:
    /// - data() points at the first element of an allocated array of
    ///   n elements, each storing the initial value c;
    /// - size() = n;
    /// - capacity() is at least as large as size().
    basic_string(size_type n, charT c, const Allocator& a = Allocator())
      :alloc(a), length_(), capacity_(), buffer_()
    {
      assert_pos(n);
      if(n < npos)
        append(n, c);
    }

    /// 14 Effects: If InputIterator is an integral type, equivalent to
    ///   basic_string(static_cast<size_type>(begin), static_cast<value_type>(end), a)
    /// 15 Otherwise constructs a string from the values in the range [begin, end)
    ///   with Postconditions:
    /// - data() points at the first element of an allocated copy of
    ///   the sequence [begin, end);
    /// - size() == distance(begin, end);
    /// - capacity() is at least as large as size().
    template<class InputIterator>
    basic_string(InputIterator begin, InputIterator end, const Allocator& a = Allocator(), typename enable_if<!is_integral<InputIterator>::value>::type* =0)
      :alloc(a), length_(), capacity_(), buffer_()
    {
      append(begin, end);
    }

    __forceinline
      basic_string(const basic_string& str, const Allocator& a)
      :alloc(str.alloc), length_(), capacity_(), buffer_()
    {
      if(!str.empty())
        append(str);
    }

    __forceinline
      basic_string(initializer_list<charT> il, const Allocator& a = Allocator())
      :alloc(a), length_(), capacity_(), buffer_()
    {
      append(il.begin(), il.end());
    }

#ifdef NTL__CXX_RV
    __forceinline
      basic_string(basic_string&& str)
      :alloc(str.alloc), length_(), capacity_(), buffer_()
    {
      swap(str);
    }
    basic_string(basic_string&& str, const Allocator& a);
#endif

    /// Effects: destructs string object.
    __forceinline
    ~basic_string()
    {
      if(buffer_){
        allocator_traits::deallocate(alloc, buffer_, capacity_);
        #ifdef NTL__DEBUG
        length_ = 0;
        #endif
      }
    }

    /// 18 Effects: If *this and str are not the same object, modifies *this as designated below:
    /// - data() points at the first element of an allocated copy of the
    ///   array whose first element is pointed at by str.data()
    /// - size() == str.size();
    /// - capacity() is at least as large as size().
    /// 19 If *this and str are the same object, the member has no effect.
    /// 20 Returns: *this
    __forceinline
    basic_string& operator=(const basic_string& str)
    {
      return this == &str ? *this : assign(str);
    }

#ifdef NTL__CXX_RV
    __forceinline
    basic_string& operator=(basic_string&& str) __ntl_nothrow
    {
      return this == &str ? *this : assign(forward<basic_string>(str));
    }
#endif

    __forceinline
    basic_string& operator=(initializer_list<charT> il)
    {
      return *this = basic_string(il);
    }


    /// 25 Returns: *this = basic_string<charT,traits,Allocator>(s).
    /// 26 Remarks: Uses traits::length().
    basic_string& operator=(const charT* s) { return assign(s);     }

    /// 27 Returns: *this = basic_string<charT,traits,Allocator>(1,c).
    basic_string& operator=(charT c)        { return assign(1, c);  }

    ///\name  21.4.3 basic_string iterator support [string.iterators]

    /// 1 Returns: an iterator referring to the first character in the string.
    iterator                begin()         { return buffer_;   }
    const_iterator          begin()  const  { return buffer_;  }

    /// 2 Returns: an iterator which is the past-the-end value.
    iterator                end()           { return buffer_+length_;     }
    const_iterator          end()    const  { return buffer_+length_;    }

    /// 3 Returns: an iterator which is semantically equivalent to reverse_iterator(end()).
    reverse_iterator        rbegin()        { return reverse_iterator(buffer_+length_);  }
    const_reverse_iterator  rbegin() const  { return const_reverse_iterator(buffer_+length_); }

    /// 4 Returns: an iterator which is semantically equivalent to reverse_iterator(begin()).
    reverse_iterator        rend()          { return reverse_iterator(buffer_);    }
    const_reverse_iterator  rend()   const  { return const_reverse_iterator(buffer_);   }

    /// Returns: const iterators.
    const_iterator          cbegin()  const { return buffer_;  }
    const_iterator          cend()    const { return buffer_+length_;    }
    const_reverse_iterator  crbegin() const { return const_reverse_iterator(buffer_+length_); }
    const_reverse_iterator  crend()   const { return const_reverse_iterator(buffer_);   }

    ///\name  21.4.4 basic_string capacity [string.capacity]

    /// 1 Returns: a count of the number of char-like objects currently in the string.
    size_type size() const __ntl_nothrow { return length_;}

    /// 2 Returns: size().
    size_type length()    const { return length_; }

    /// 3 Returns: The maximum size of the string.
    size_type max_size()  const { return allocator_traits::max_size(alloc); }

    /// 5 Requires: n <= max_size()
    /// 6 Throws: length_error if n > max_size().
    /// 7 Effects: Alters the length of the string designated by *this as follows:
    /// — If n <= size(), the function replaces the string designated by *this
    ///   with a string of length n whose elements are a copy of the initial
    ///   elements of the original string designated by *this.
    /// — If n > size(), the function replaces the string designated by *this
    ///   with a string of length n whose first size() elements are a copy of
    ///   the original string designated by *this, and whose remaining elements
    ///   are all initialized to c.
    void resize(size_type n, charT c) __ntl_throws(length_error)
    {
      if(n > max_size()){
        __throw_length_error(__func__": n > max_size()");
        return;
      }
      if(n > capacity_){
        reserve(n);
        memset( buffer_+length_, c, (n-length_)*sizeof(charT) );
      }
      length_ = n;
    }

    /// 8 Effects: resize(n,charT()).
    void resize(size_type n) { resize(n, charT()); }

    /// 9 Returns: the size of the allocated storage in the string.
    size_type capacity()  const { return capacity_;  }

    /// 10 The member function reserve() is a directive that informs a basic_string
    ///   object of a planned change in size, so that it can manage the storage
    ///   allocation accordingly.
    /// 11 Effects: After reserve(), capacity() is greater or equal to the argument of reserve.
    ///\note Calling reserve() with a res_arg argument less than capacity() is
    ///   in effect a non-binding shrink request. A call with res_arg <= size()
    ///   is in effect a non-binding shrink-to-fit request.
    void reserve(size_type n = 0)
    {
      if(n > max_size()){
        __throw_length_error(__func__": res_arg > max_size()");
        return;
      }
      if(n <= length_){
        shrink_to_fit();
      }else if(n != capacity_){
        grow_buffer(n, length_);
      }
    }

    /// 13 Remarks: shrink_to_fit is a non-binding request to reduce capacity()
    ///   to size(). \note The request is non-binding to allow latitude for
    ///   implementation-specific optimizations.
    void shrink_to_fit() 
    {
      if(length_){
        pointer buf = allocator_traits::allocate(alloc, length_);
        traits_type::copy(buf, buffer_, length_);
        allocator_traits::deallocate(alloc, buffer_, capacity_);
        buffer_ = buf;
        capacity_ = length_;
      }else if(capacity_){
        allocator_traits::deallocate(alloc, buffer_, capacity_);
        buffer_ = nullptr;
        capacity_ = 0;
      }
    }

    /// 14 Effects: Behaves as if the function calls: erase(begin(), end());
    void clear()
    {
      length_ = 0;
    }

    /// 15 Returns: size() == 0.
    bool empty() const { return length_ == 0; }

    ///\name  basic_string element access [21.4.5 string.access]

    /// 1 Returns: If pos < size(), returns *(begin() + pos). Otherwise,
    ///   if pos == size(), returns charT().
    const_reference operator[](size_type pos) const __ntl_nothrow
    {
      if(pos < length_)
        return buffer_[pos];
      return zero_char;
    }

    /// 1 Returns: If pos < size(), returns *(begin() + pos).
    ///   Otherwise, the behavior is undefined.
    reference operator[](size_type pos) __ntl_nothrow
    {
      if(pos < length_)
        return buffer_[pos];
      return const_cast<charT&>(zero_char);
    }

    /// 2 Requires: pos < size()
    /// 3 Throws: out_of_range if pos >= size().
    /// 4 Returns: operator[](pos).
    const_reference at(size_type pos) const __ntl_throws(out_of_range)
    {
      if(pos >= length_)
        __throw_out_of_range(__func__": pos > size()");
      return operator[](pos);
    }
    reference at(size_type pos)             __ntl_throws(out_of_range)
    {
      if(pos >= length_)
        __throw_out_of_range(__func__": pos > size()");
      return operator[](pos);
    }

    /// 5 Requires: !empty()
    /// 6 Effects: Equivalent to operator[](0).
    const charT& front() const { assert(!empty()); return operator[](0); }
    charT& front()             { assert(!empty()); return operator[](0); }

    /// 7 Requires: !empty()
    /// 8 Effects: Equivalent to operator[](size() - 1).
    const charT& back() const { assert(!empty()); return operator[](length_ - 1); }
    charT& back() { assert(!empty()); return operator[](length_ - 1); }

    /// 21.4.6 basic_string modifiers [string.modifiers]

    ///\name  basic_string::operator+= [21.4.6.1 string::op+=]

    basic_string& operator+=(const basic_string& str) { return append(str);   }
    basic_string& operator+=(const charT* s)          { return append(s);     }
    basic_string& operator+=(charT c) { push_back(c); return *this; }

    basic_string& operator+=(initializer_list<charT> il) { return append(il); }


    ///\name  basic_string::append [21.4.6.2 string::append]

    basic_string& append(const basic_string& str)
    {
      if(!str.empty())
        replace_impl(length_,0,str.buffer_,str.length_);
      return *this;
    }

    basic_string& append(const basic_string& str, size_type pos, size_type n)
    {
      if(pos > str.size()){
        __throw_out_of_range(__func__": invalid `pos`");
      }else if(!str.empty()){
        replace_impl(length_,0,str.buffer_,str.length_,pos,n);
      }
      return *this;
    }

    basic_string& append(const charT* s, size_type n)
    {
      return replace_impl(length_,0,s,n);
    }

    basic_string& append(const charT * s)
    {
      return replace_impl(length_,0,s,static_cast<size_type>(traits::length(s)));
    }

    basic_string& append(size_type n, charT c)
    {
      insert(end(), n, c);
      return *this;
    }

    template<class InputIterator>
    typename enable_if<!is_integral<InputIterator>::value, basic_string&>::type append(InputIterator first, InputIterator last)
    {
      if(first != last)
        insert(end(), first, last);
      return *this;
    }


    basic_string& append(initializer_list<charT> il)
    {
      return insert(end(), il.begin(), il.end());
    }


    void push_back(charT c)
    {
      insert(buffer_+length_, 1, c);
    }

    ///\name  basic_string::assign [21.4.6.3 string::assign]

    basic_string& assign(const basic_string& str)
    {
      if(this != &str){
        clear();
        if(!str.empty())
          insert(0, str);
      }
      return *this;
    }

#ifdef NTL__CXX_RV
    basic_string& assign(basic_string&& rstr)
    {
      if(this != &rstr){
        clear();
        swap(rstr);
      }
      return *this;
    }
#endif

    basic_string& assign(const basic_string& str, size_type pos, size_type n)
    {
      if(pos > str.length_){
        __throw_out_of_range(__func__": invalid `pos`");
      }else if(!str.empty()){
        size_type len = str.length_;  // insert from self protection
        clear();                      // can set str.length() to 0 if &str == this
        replace_impl(0,0,str.buffer_,len,pos,n);
      }
      return *this;
    }

    basic_string& assign(const charT* s, size_type n)
    {
      clear();
      return insert(0, s, n);
    }

    basic_string& assign(const charT* s)
    {
      clear();
      return append(s);
    }

    basic_string& assign(size_type n, charT c)
    {
      clear(); insert(begin(), n, c);
      return *this;
    }

    template<class InputIterator>
    typename enable_if<!is_integral<InputIterator>::value, basic_string&>::type assign(InputIterator first, InputIterator last)
    {
      clear();
      if(first != last)
        insert(begin(), first, last);
      return *this;
    }


    basic_string& assign(initializer_list<charT> il)
    {
      return clear(), insert(begin(), il.begin(), il.end());
    }


    ///\name  basic_string::insert [21.4.6.4 string::insert]

    basic_string& insert(size_type pos1, const basic_string& str)
    {
      if(!str.empty())
        insert(pos1, str, 0, npos);
      return *this;
    }

    basic_string& insert(size_type pos1, const basic_string& str, size_type pos2, size_type n)
    {
      if(pos1 > length_ || pos2 > str.length_){
        __throw_out_of_range(__func__": invalid `pos`");
      }
      if(!str.empty() && pos1 <= length_ && pos2 <= str.length_)
        replace_impl(pos1, 0, str.buffer_, str.length_, pos2, n);
      return *this;
    }

    basic_string& insert(size_type pos, const charT* s, size_type n)
    {
      if(pos > length_){
        __throw_out_of_range(__func__": invalid `pos`");
      }
      if(pos <= length_)
        replace_impl(pos, 0, s, n);
      return *this;
    }

    basic_string& insert(size_type pos, const charT* s)
    {
      if(pos > length_){
        __throw_out_of_range(__func__": invalid `pos`");
      }
      if(pos <= length_)
        replace_impl(pos, 0, s, traits_type::length(s));
      return *this;
    }

    basic_string& insert(size_type pos, size_type n, charT c)
    {
      if(pos > length_){
        __throw_out_of_range(__func__": invalid `pos`");
      }
      if(pos <= length_)
        replace(pos, 0, n, c);
      return *this;
    }

    iterator insert(iterator p, charT c) { return insert(p, 1, c); }

    iterator insert(iterator p, size_type n, charT c)
    {
      assert(p >= buffer_ && p <= buffer_+length_);
      if(n == 0) return p;

      const size_type pos = static_cast<size_type>(p-buffer_);
      if(length_ + n + 1 > capacity_)
        reserve(length_+n+1);
      charT* pc = buffer_+pos;
      p = pc;
      if(pos < length_)
        traits_type::move(pc+n, pc, length_-pos);
      length_ = length_ + n;
      traits_type::assign(pc, n, c);
      #ifdef NTL__DEBUG
      assert(length_ < capacity_);
      buffer_[length_] = zero_char;
      #endif
      return p;
    }

    template<class InputIterator>
    typename enable_if<!is_integral<InputIterator>::value, iterator>::type insert(iterator p, InputIterator first, InputIterator last)
    {
      assert(p >= begin() && p <= end());
      return replace_it(p-buffer_,p-buffer_, first, last, iterator_traits<InputIterator>::iterator_category());
    }

    iterator insert(iterator p, initializer_list<charT> il)
    {
      return insert(p, il.begin(), il.end());
    }


    ///\name  basic_string::erase [21.4.6.5 string::erase]

    basic_string& erase(size_type pos = 0, size_type n = npos)
    {
      if(pos > length_){
        __throw_out_of_range(__func__": invalid `pos`");
      }else if(length_){
        const size_type xlen = min(n, length_-pos);
        if(xlen < length_){
          const size_type rlen = length_-(pos+xlen);
          if(rlen)
            traits_type::move(buffer_+pos, buffer_+pos+xlen, rlen);
        }
        length_ -= xlen;
        #ifdef NTL__DEBUG
        assert(length_ < capacity_);
        buffer_[length_] = zero_char;
        #endif
      }
      return *this;
    }

    iterator erase(iterator position)
    {
      size_type pos = position-buffer_;
      if(position >= buffer_ && pos < length_){
        traits_type::move(position, position+1, length_-pos);
        length_--; pos++;
        #ifdef NTL__DEBUG
        assert(length_ < capacity_);
        buffer_[length_] = zero_char;
        #endif
        if(pos < length_)
          return position;
      }
      return end();
    }

    iterator erase(iterator first, iterator last)
    {
      assert(last > first && first >= buffer_ && first < buffer_+length_ && last >= buffer_ && last < buffer_+length_);
      const size_type pos = first-buffer_, len = buffer_+length_-last;
      if(first >= buffer_ && pos < length_){
        traits_type::move(first, last, len);
        length_ -= len;
        last -= len;
        #ifdef NTL__DEBUG
        assert(length_ < capacity_);
        buffer_[length_] = zero_char;
        #endif
        return last;
      }
      return end();
    }

    ///\name  basic_string::replace [21.4.6.6 ib.string::replace]

    basic_string& replace(size_type pos1, size_type n1, const basic_string& str)
    {
      return replace_impl(pos1, n1, str.data(), str.length());
    }

    basic_string& replace(size_type pos1, size_type n1, const basic_string& str, size_type pos2, size_type n2)
    {
      return replace_impl(pos1,n1,str.data(),str.length(),pos2,n2);
    }

    basic_string& replace(size_type pos, size_type n1, const charT* s, size_type n2)
    {
      return replace_impl(pos, n1, s, traits_type::length(s), 0, n2);
    }

    basic_string& replace(size_type pos, size_type n1, const charT* s)
    {
      return replace_impl(pos,n1,s,traits_type::length(s));
    }

    basic_string& replace(size_type pos, size_type n1, size_type n2, charT c)
    {
      return replace(pos, n1, basic_string(n2,c));
    }

    basic_string& replace(iterator i1, iterator i2, const basic_string& str)
    {
      return replace_impl(i1-begin(),i2-begin(),str.data(),str.length());
    }

    basic_string& replace(iterator i1, iterator i2, const charT* s, size_type n)
    {
      return replace_impl(i1-begin(),i2-begin(),s,n);
    }

    basic_string& replace(iterator i1, iterator i2, const charT* s)
    {
      return replace_impl(i1-begin(),i2-begin(),s,traits_type::length(s));
    }

    basic_string& replace(iterator i1, iterator i2, size_type n, charT c)
    {
      return replace(i1,i2,basic_string(n,c));
    }

    template<class InputIterator>
    typename enable_if<!is_integral<InputIterator>::value, basic_string&>::type
    replace(iterator i1, iterator i2, InputIterator j1, InputIterator j2)
    {
      assert(i1 >= begin() && i1 <= end() && i2 >= begin() && i2 <= end());
      replace_it(i1-buffer_, i2-i1, j1, j2, iterator_traits<InputIterator>::iterator_category());
      return *this;
    }

  private:
    template<class RandomIterator>
    iterator replace_it(size_type pos, size_type n, RandomIterator first, RandomIterator last, random_access_iterator_tag)
    {
      const size_type rlen = distance(first,last), xlen = min(n, length_-pos);
      if(rlen == 0 && xlen == 0)
        return end();

      RandomIterator src = first;
      if(rlen > xlen){                      // expand
        const size_type space = length_ + (rlen-xlen);

        const const_pointer pfirst = reinterpret_cast<const_pointer>(&*first);
        const bool from_self = capacity_ && pfirst >= buffer_ && pfirst < buffer_+capacity_;
        size_type first_pos = 0;
        if(from_self)
          first_pos = static_cast<size_type>(distance(static_cast<const_pointer>(buffer_), pfirst)); // always positive

        if(space+1 > capacity_){
          reserve(space);
          if(from_self)
            src = ntl::brute_cast<RandomIterator>(buffer_) + first_pos;
        }
        if(length_){
          if(from_self){
            if(first_pos >= pos)
              advance(src, rlen-xlen);
            else if(first_pos < pos && first_pos+rlen > pos){
              // splitted
              basic_string tmp(first, last);
              return replace_it(pos, n, tmp.begin(), tmp.end(), iterator_traits<RandomIterator>::iterator_category());
            }
          }
          traits_type::move(buffer_+pos+rlen, buffer_+pos+xlen, length_-pos);
        }
      }
      if(rlen){                             // replace
        n = rlen;
        for(charT* p = buffer_+pos; n!= 0; n--, ++p, ++src)
          traits_type::assign(*p, *src);
      }
      if((length_ || rlen) && xlen > rlen && xlen != length_)  // collapse
        traits_type::move(buffer_+pos+rlen, buffer_+pos+xlen, length_-pos);
      length_ += rlen - xlen;

      #ifdef NTL__DEBUG
      assert(length_ < capacity_);
      buffer_[length_] = zero_char;
      #endif
      return buffer_ + pos;
    }

    template<class InputIterator>
    iterator replace_it(size_type pos, size_type n, InputIterator first, InputIterator last, input_iterator_tag)
    {
      size_type xpos = pos, rlen = 0, xlen = min(n, length_-pos), xend = length_-pos;
      const bool have_tail = length_ && pos < length_;
      while(first != last){
        if(pos >= capacity_){
          grow_buffer(capacity_ + 1, max(pos, length_));
        }
        value_type c = *first;
        assert(first != last);     // istreambuf_iterator workaround isn't need (was fixed)
        if(have_tail)
          traits_type::move(buffer_+pos+1, buffer_+pos, xend++);
        traits_type::assign(buffer_[pos++], c);
        ++rlen;
        ++first;
      }
      if(rlen == 0 && xlen == 0)
        return end();
      length_ += rlen - xlen;
      #ifdef NTL__DEBUG
      assert(length_ < capacity_);
      buffer_[length_] = zero_char;
      #endif
      return buffer_+xpos;
    }

    basic_string& replace_impl(size_type pos1, size_type n1, const charT* str, size_type len, size_type pos2 = 0, size_type n2 = npos)// __ntl_throws(out_of_range, length_error)
    {
      if(!str) return *this;
      if(pos1 > length_ || pos2 > len){
        __throw_out_of_range(__func__": invalid position given");
        return *this;
      }

      // dest size, src size
      const size_type xlen = min<size_type>(n1, length_-pos1), rlen = min<size_type>(n2, len-pos2);
      if(xlen == 0 && rlen == 0)
        return *this;

      if(length_ - xlen >= max_size()-rlen){
        __throw_length_error(__func__": too large size");
        return *this;
      }

      const_pointer s = str;
      if(rlen > xlen){                      // expand
        const size_type res = length_ + (rlen-xlen);
        const bool from_self = capacity_ && str >= buffer_ && str < buffer_+capacity_;
        difference_type selfpos = from_self ? str - buffer_ : 0;

        if(res+1 > capacity_){
          reserve(res+1);
          if(from_self)
            s = buffer_ + selfpos;
        }
        if(length_){
          // replace from self?
          if(from_self){
            if(selfpos+pos2 >= pos1)  // moved
              pos2 += rlen-xlen;
            else if(s+pos2 < buffer_+pos1 && s+pos2+rlen > buffer_+pos1){
              // splitted part
              basic_string tmp(s+pos2, rlen);
              return replace_impl(pos1, n1, tmp.c_str(), tmp.length(), 0, rlen);
            }
          }
          if(pos1 < length_)
            traits_type::move(buffer_+pos1+rlen, buffer_+pos1+xlen, length_-pos1);
        }
      }

      if(rlen)                              // replace
        traits_type::copy(buffer_+pos1, s+pos2, rlen);
      
      if((length_ || rlen) && xlen > rlen && xlen != length_)  // collapse to non-empty string
        traits_type::move(buffer_+pos1+rlen, buffer_+pos1+xlen, length_-pos1);

      length_ += rlen - xlen;
      #ifdef NTL__DEBUG
      assert(length_ < capacity_);
      buffer_[length_] = zero_char;
      #endif
      return *this;
    }
  public:

    ///\name  basic_string::copy [21.4.6.7 string::copy]
    size_type copy(charT* s, size_type n, size_type pos = 0) const// __ntl_throws(out_of_range)
    {
      if(pos > size()){
        __throw_out_of_range(__func__": invalid `pos`");
        return 0;
      }
      const size_type tail = size() - pos;
      const size_type rlen = min(n, tail);
      traits_type::copy(s, buffer_+pos, rlen);
      return rlen;
    }

    ///\name  basic_string::swap [21.4.6.8 string::swap]
    void swap(basic_string& str) __ntl_nothrow
    {
      if(this == &str) return;
      using std::swap;
      swap(buffer_, str.buffer_);
      swap(length_, str.length_);
      swap(capacity_, str.capacity_);
    }

    ///\name  basic_string string operations [21.4.6 string.ops]

    const charT* c_str() const  __ntl_nothrow
    {
      // ensure string is null-terminated
      if(!capacity_)
        return &zero_char;
      if(length_ < capacity_) {
        buffer_[length_] = zero_char;
      } else{
        const_cast<basic_string*>(this)->push_back(zero_char);
        const_cast<basic_string*>(this)->length_--;
      }
      return buffer_;
    }

    const charT* data() const __ntl_nothrow
    {
      return length_ ? buffer_ : &zero_char;
    }

    allocator_type get_allocator() const { return alloc; }

    ///\name   21.4.7.2 basic_string::find [string::find]

    /// 1 Effects: Determines the lowest position xpos, if possible, such that
    ///   both of the following conditions obtain:
    /// — pos <= xpos and xpos + str.size() <= size();
    /// — traits::eq(at(xpos+I), str.at(I)) for all elements I of the string
    ///   controlled by str.
    /// 2 Returns: xpos if the function can determine such a value for xpos.
    ///   Otherwise, returns npos.
    /// 3 Remarks: Uses traits::eq().
    size_type find(const basic_string& str, size_type pos = 0) const
    {
      return find(str.begin(), pos, str.size());
    }

    /// 4 Returns: find(basic_string<charT,traits,Allocator>(s,n),pos).
    size_type find(const charT* s, size_type pos, size_type n) const
    {
      size_type cursize = size();
      if(pos > cursize || pos+n > cursize) return npos;
      const charT* const beg = begin();
      for ( size_type xpos = pos; xpos + n <= cursize; ++xpos )
      {
        for(size_type i = 0; i != n; ++i){
          if ( !traits_type::eq(*(beg + xpos  + i), *(s + i)) )
            goto next_xpos;
        }
        return xpos;
next_xpos:;
      }
      return npos;
    }

    /// 5 Returns: find(basic_string<charT,traits,Allocator>(s),pos).
    /// 6 Remarks: Uses traits::length().
    size_type find(const charT* s, size_type pos = 0) const
    {
      return find(s, pos, traits_type::length(s));
    }

    /// 7 Returns: find(basic_string<charT,traits,Allocator>(1,c),pos).
    size_type find(charT c, size_type pos = 0) const
    {
      const charT* const beg = begin();
      for ( size_type xpos = pos; xpos < size(); ++xpos )
        if ( traits_type::eq(*(beg + xpos), c) )
          return xpos;
      return npos;
    }

    ///\name   basic_string::rfind [21.4.6.2 string::rfind]

    /// 1 Effects: Determines the highest position xpos, if possible, such that
    ///   both of the following conditions obtain:
    ///   — xpos <= pos and xpos + str.size() <= size();
    ///   — traits::eq(at(xpos+I), str.at(I)) for all elements I of the string
    ///   controlled by str.
    /// 2 Returns: xpos if the function can determine such a value for xpos.
    ///   Otherwise, returns npos.
    /// 3 Remarks: Uses traits::eq().
    size_type rfind(const basic_string& str, size_type pos = npos) const
    {
      return rfind(str.begin(), pos, str.size());
    }

    /// 4 Returns: rfind(basic_string<charT,traits,Allocator>(s,n),pos).
    size_type rfind(const charT* s, size_type pos, size_type n) const
    {
      if(!n) return min(pos,size());
      size_type & xpos = pos;
      if ( xpos > size() || xpos + n > size() )
        xpos = size() - n;
      const charT* const beg = begin();
      while ( xpos + n > 0 )
      {
        for ( size_type i = 0; i != n; ++i )
          if ( !traits_type::eq(*(beg + xpos + i), *(s + i)) )
            goto next_xpos;
        return xpos;
next_xpos:
        --xpos;
      }
      return npos;
    }

    /// 5 Returns: rfind(basic_string<charT,traits,Allocator>(s),pos).
    /// 6 Remarks: Uses traits::length().
    size_type rfind(const charT* s, size_type pos = npos) const
    {
      return rfind(s, pos, traits_type::length(s));
    }

    /// 7 Returns: rfind(basic_string<charT,traits,Allocator>(1,c),pos).
    size_type rfind(charT c, size_type pos/* = npos*/) const
    {
      ///\note  Standard claims the use of at() member function, but
      ///       we stick to an exception-safe way
      const charT* const beg = begin();
      for ( size_type xpos = pos < size() ? pos + 1 : size(); xpos; )
        if ( traits_type::eq(*(beg + --xpos), c) )
          return xpos;
      return npos;
    }

    /// 7 Returns: rfind(basic_string<charT,traits,Allocator>(1,c),npos).
    size_type rfind(charT c/*, size_type pos = npos*/) const
    {
      size_type xpos = size();
      const charT* const beg = begin();
      while ( xpos-- )
        if ( traits_type::eq(*(beg + xpos), c) )
          return xpos;
      return npos;
    }

    ///\name  21.4.7.4 basic_string::find_first_of [string::find.first.of]

    /// 1 Effects: Determines the lowest position xpos, if possible, such that
    ///   both of the following conditions obtain:
    ///   — pos <= xpos and xpos < size();
    ///   — traits::eq(at(xpos), str.at(I)) for some element I of the string
    ///     controlled by str.
    /// 2 Returns: xpos if the function can determine such a value for xpos.
    ///   Otherwise, returns npos.
    /// 3 Remarks: Uses traits::eq().
    size_type find_first_of(const basic_string& str, size_type pos = 0) const
    {
      return find_first_of(str.begin(), pos, str.size());
    }

    /// 4 Returns: find_first_of(basic_string<charT,traits,Allocator>(s,n),pos).
    size_type find_first_of(const charT* s, size_type pos, size_type n) const
    {
      const charT* const beg = begin();
      for ( size_type xpos = pos; xpos < length_; ++xpos ){
        for ( size_type i = 0; i != n; ++i ) {
          if ( traits_type::eq(*(beg + xpos), *(s + i)) )
            return xpos;
        }
      }
      return npos;
    }

    /// 5 Returns: find_first_of(basic_string<charT,traits,Allocator>(s),pos).
    /// 6 Remarks: Uses traits::length().
    size_type find_first_of(const charT* s, size_type pos = 0) const
    {
      return find_first_of(s, pos, traits_type::length(s));
    }

    /// 7 Returns: find_first_of(basic_string<charT,traits,Allocator>(1,c),pos).
    size_type find_first_of(charT c, size_type pos = 0) const
    {
      const charT* const beg = begin();
      for ( size_type xpos = pos; xpos < length_; ++xpos )
        if ( traits_type::eq(*(beg + xpos), c) )
          return xpos;
      return npos;
    }

    ///\name  21.4.7.5 basic_string::find_last_of [string::find.last.of]

    /// 1 Effects: Determines the highest position xpos, if possible, such that
    ///   both of the following conditions obtain:
    /// — xpos <= pos and xpos < size();
    /// — traits::eq(at(xpos), str.at(I)) for some element I of the string
    ///   controlled by str.
    /// 2 Returns: xpos if the function can determine such a value for xpos.
    ///   Otherwise, returns npos.
    /// 3 Remarks: Uses traits::eq().
    size_type find_last_of(const basic_string& str, size_type pos = npos) const
    {
      return find_last_of(str.begin(), pos, str.size());
    }

    /// 4 Returns: find_last_of(basic_string<charT,traits,Allocator>(s,n),pos).
    size_type find_last_of(const charT* s, size_type pos, size_type n) const
    {
      if(!n || !length_)
        return npos;

      size_type xpos = min(pos, length_-1);
      const charT* const beg = begin();
      do
      {
        for ( size_type i = 0; i != n; ++i ){
          if ( traits_type::eq(*(beg + xpos), *(s + i)) )
            return xpos;
        }
      }while(xpos--);
      return npos;
    }

    /// 5 Returns: find_last_of(basic_string<charT,traits,Allocator>(s),pos).
    /// 6 Remarks: Uses traits::length().
    size_type find_last_of(const charT* s, size_type pos = npos) const
    {
      return find_last_of(s, pos, traits_type::length(s));
    }

    /// 7 Returns: find_last_of(basic_string<charT,traits,Allocator>(1,c),pos).
    size_type find_last_of(charT c, size_type pos = npos) const
    {
      return rfind(c, pos);
    }

    ///\name  21.4.7.6 basic_string::find_first_not_of [string::find.first.not.of]

    /// 1 Effects: Determines the lowest position xpos, if possible, such that
    ///   both of the following conditions obtain:
    /// — pos <= xpos and xpos < size();
    /// — traits::eq(at(xpos), str.at(I)) for no element I of the string
    ///   controlled by str.
    /// 2 Returns: xpos if the function can determine such a value for xpos.
    ///   Otherwise, returns npos.
    /// 3 Remarks: Uses traits::eq().
    size_type find_first_not_of(const basic_string& str, size_type pos = 0) const
    {
      return find_first_not_of(str.begin(), pos, str.size());
    }

    /// 4 Returns: find_first_not_of(basic_string<charT,traits,Allocator>(s,n),pos).
    size_type find_first_not_of(const charT* s, size_type pos, size_type n) const
    {
      const charT* const beg = begin();
      for ( size_type xpos = pos; xpos < size(); ++xpos )
      {
        for ( size_type i = 0; i != n; ++i )
          if ( traits_type::eq(*(beg + xpos), *(s + i)) )
            goto next_xpos;
        return xpos;
next_xpos:;
      }
      return npos;
    }

    /// 5 Returns: find_first_not_of(basic_string<charT,traits,Allocator>(s),pos).
    /// 6 Remarks: Uses traits::length().
    size_type find_first_not_of(const charT* s, size_type pos = 0) const
    {
      return find_first_not_of(s, pos, traits_type::length(s));
    }

    /// 7 Returns: find_first_not_of(basic_string<charT,traits,Allocator>(1,c),pos).
    size_type find_first_not_of(charT c, size_type pos = 0) const
    {
      const charT* const beg = begin();
      for ( size_type xpos = pos; xpos < size(); ++xpos )
        if ( !traits_type::eq(*(beg + xpos), c) )
          return xpos;
      return npos;
    }

    ///\name  21.4.7.7 basic_string::find_last_not_of [string::find.last.not.of]

    /// 1 Effects: Determines the highest position xpos, if possible, such that
    ///   both of the following conditions obtain:
    /// — xpos <= pos and xpos < size();
    /// — traits::eq(at(xpos), str.at(I)) for no element I of the string
    ///   controlled by str.
    /// 2 Returns: xpos if the function can determine such a value for xpos.
    ///   Otherwise, returns npos.
    /// 3 Remarks: Uses traits::eq().
    size_type find_last_not_of(const basic_string& str, size_type pos = npos) const
    {
      return find_last_not_of(str.begin(), pos, str.size());
    }

    /// 4 Returns: find_last_not_of(basic_string<charT,traits,Allocator>(s,n),pos).
    size_type find_last_not_of(const charT* s, size_type pos, size_type n) const
    {
      const charT* const beg = begin();
      for ( size_type xpos = pos < size() ? pos + 1 : size(); xpos; )
      {
        --xpos;
        for ( size_type i = 0; i != n; ++i )
          if ( traits_type::eq(*(beg + xpos), *(s + i)) )
            goto next_xpos;
        return xpos;
next_xpos:;
      }
      return npos;
    }

    /// 5 Returns: find_last_not_of(basic_string<charT,traits,Allocator>(s),pos).
    /// 6 Remarks: Uses traits::length().
    size_type find_last_not_of(const charT* s, size_type pos = npos) const
    {
      return find_last_not_of(s, pos, traits_type::length(s));
    }

    /// 7 Returns: find_last_not_of(basic_string<charT,traits,Allocator>(1,c),pos).
    size_type find_last_not_of(charT c, size_type pos = npos) const
    {
      const charT* const beg = begin();
      for ( size_type xpos = pos < size() ? pos + 1 : size(); xpos; )
        if ( !traits_type::eq(*(beg + --xpos), c) )
          return xpos;
      return npos;
    }

    ///\name 21.4.7.8 basic_string::substr [string::substr]
    /// 1 Requires: pos <= size()
    /// 2 \todo Throws: out_of_range if pos > size().
    /// 3 Effects: Determines the effective length rlen of the string to copy as
    ////  the smaller of n and size() - pos.
    /// 4 Returns: basic_string<charT,traits,Allocator>(data()+pos,rlen).
    basic_string substr(size_type pos = 0, size_type n = npos) const __ntl_throws(out_of_range)
    {
      if(pos > size()){
        __throw_out_of_range(__func__": invalid `pos`");
        return basic_string();
      }
      return basic_string(*this, pos, n);
    }

    ///\name  basic_string::compare [21.4.7.9 string::compare]

    int compare(const basic_string& str) const
    {
      const size_t rlen = std::min(size(), str.size());
      if(!rlen)
        return static_cast<int>(size() - str.size());
      const int r = traits_type::compare(begin(), str.begin(), rlen);
      return r != 0 ? r : static_cast<int>(size() - str.size());
    }

    int compare(size_type pos, size_type n, const basic_string& str) const
    {
      if(pos+n >= size())
        return -1;
      const size_t rlen = std::min(n, str.size());
      if(!rlen)
        return static_cast<int>(n - str.size());
      const int r = traits_type::compare(begin()+pos, str.begin(), rlen);
      return r != 0 ? r : static_cast<int>(n - str.size());
    }

    int compare(size_type pos1, size_type n1, const basic_string& str, size_type pos2, size_type n2) const
    {
      if(pos1+n1 >= size())
        return -1;
      else if(pos2+n2 >= str.size())
        return 1;
      const size_t rlen = std::min(n1, n2);
      if(!rlen)
        return static_cast<int>(n1 - n2);
      const int r = traits_type::compare(begin()+pos1, str.begin()+pos2, rlen);
      return r != 0 ? r : static_cast<int>(n1 - n2);
    }

    int compare(const charT* s) const
    {
      if(!s)
        return static_cast<int>(size());
      const int r = traits_type::compare(begin(), s, size());
      if(r) return r;
      // s may be longer than *this
      const size_type rlen = traits_type::length(s);
      return static_cast<int>(size() - rlen);
      //return r != 0 ? r : traits_type::eq(s[size()], zero_char) ? r : r - 1; // r == 0 here
    }

    int compare(size_type pos, size_type n, const charT* s) const
    {
      if(!s || pos+n >= size())
        return -1;
      const int r = traits_type::compare(begin()+pos, s, n);
      if(r) return r;
      const size_type rlen = traits_type::length(s);
      return static_cast<int>(size() - rlen);
    }

    int compare(size_type pos1, size_type n1, const charT* s, size_type n2) const
    {
      if(n1 != n2)
        return static_cast<int>(n1 - n2);
      else if(pos1+n1 >= size())
        return -1;
      const int r = traits_type::compare(begin()+pos1, s, min(n1, n2));
      return r != 0 ? r : static_cast<int>(n1 - n2);
    }

    ///\name  operator+ [21.4.8.1 string::op+]
    /// @note frends, not just non-member functions

    friend
      basic_string operator+(const basic_string& lhs, const basic_string& rhs)
    {
      basic_string<charT, traits, Allocator> sum;
      sum.alloc__new(lhs.size() + rhs.size());
      sum.append_to__reserved(lhs.begin(), lhs.end());
      sum.append_to__reserved(rhs.begin(), rhs.end());
      return move(sum);
    }

    friend
      basic_string operator+(const charT* lhs, const basic_string& rhs)
    {
      basic_string<charT, traits, Allocator> sum;
      sum.alloc__new(traits_type::length(lhs) + rhs.size());
      sum.append_to__reserved(lhs);
      sum.append_to__reserved(rhs.begin(), rhs.end());
      return move(sum);
    }

    friend
      basic_string operator+(charT lhs, const basic_string& rhs)
    {
      basic_string<charT, traits, Allocator> sum;
      sum.alloc__new(1 + rhs.size());
      sum.append_to__reserved(lhs);
      sum.append_to__reserved(rhs.begin(), rhs.end());
      return move(sum);
    }

    friend
      basic_string operator+(const basic_string& lhs, const charT* rhs)
    {
      basic_string<charT, traits, Allocator> sum;
      sum.alloc__new(lhs.size() + traits_type::length(rhs));
      sum.append_to__reserved(lhs.begin(), lhs.end());
      sum.append_to__reserved(rhs);
      return move(sum);
    }

    friend
      basic_string operator+(const basic_string& lhs, charT rhs)
    {
      basic_string<charT, traits, Allocator> sum;
      sum.alloc__new(lhs.size() + 1);
      sum.append_to__reserved(lhs.begin(), lhs.end());
      sum.push_back(rhs);
      return move(sum);
    }

#ifdef NTL__CXX_RV
    friend
      basic_string&& operator+(basic_string&& lhs, const basic_string& rhs)
    {
      return move(lhs.append(rhs));
    }
    friend
      basic_string&& operator+(const basic_string& lhs, basic_string&& rhs)
    {
      return move(rhs.insert(0, lhs));
    }
    friend
      basic_string&& operator+(basic_string&& lhs, basic_string&& rhs)
    {
      if(rhs.size() <= lhs.capacity()-lhs.size()
        || rhs.capacity()-rhs.size() < lhs.size())
        return move(lhs.append(rhs));
      else
        return move(rhs.insert(0,lhs));
    }
    friend
      basic_string&& operator+(const charT* lhs, basic_string&& rhs)
    {
      return move(rhs.insert(0, lhs));
    }
    friend
      basic_string&& operator+(charT lhs, basic_string&& rhs)
    {
      return move(rhs.insert(0, lhs));
    }
    friend
      basic_string&& operator+(basic_string&& lhs, const charT* rhs)
    {
      return move(lhs.append(rhs));
    }
    friend
      basic_string&& operator+(basic_string&& lhs, charT rhs)
    {
      return move(lhs.append(1, rhs));
    }
#endif

    ///@}

  private:

    static const charT* assert_ptr(const charT* const p)
    {
    #if !STLX__USE_EXCEPTIONS
      assert(p);
    #endif
      return p;
    }

    static bool assert_pos(const size_type pos)
    {
    #if !STLX__USE_EXCEPTIONS
      assert(pos < npos);
    #endif
      return pos < npos;
    }

    const_iterator max__it(size_type pos, size_type n) const
    {
      return n < str.size() - pos ? str.begin() + pos + n : str.end();
    }

    iterator max__it(size_type pos, size_type n)
    {
      return n < str.size() - pos ? str.begin() + pos + n : str.end();
    }

    void append_to__reserved(charT c)
    {
      traits_type::assign(buffer_[length_++], c);
    }

    void append_to__reserved(const_pointer s)
    {
      iterator i = buffer_+length_;
      while ( *s ) traits_type::assign(*i++, *s++);
      length_ = i-buffer_;
    }

    void append_to__reserved(const_iterator fist, const_iterator last)
    {
      iterator i = buffer_+length_;
      while ( fist != last ) traits_type::assign(*i++, *fist++);
      length_ = i-buffer_;
    }

    void grow_buffer(size_type new_size, size_type length)
    {
      const size_type n = static_cast<size_type>(__ntl_grow_heap_block_size(new_size + 1));
      if(!(n < new_size)) // overflow
        new_size = n;
      charT* buf = allocator_traits::allocate(alloc, new_size);
      if(!buf) return;
      if(length)
        traits::copy(buf, buffer_, length);
      if(buffer_)
        allocator_traits::deallocate(alloc, buffer_, capacity_);
      capacity_ = new_size;
      buffer_ = buf;
    }

    /// @note allocates n + 1 bytes, possibly optimizing c_str()
    void alloc__new(size_type n)
    {
      capacity_ = __ntl_grow_heap_block_size(n + sizeof('\0'));
      buffer_ = allocator_traits::allocate(alloc, capacity_);
    }

  };//class basic_string



// 21.3.7 basic_string non-member functions [lib.string.nonmembers]

///\name  operator== [21.3.8.2 string::operator==]

template<class charT, class traits, class Allocator>
inline
bool
  operator==(const basic_string<charT,traits,Allocator>& lhs,
             const basic_string<charT,traits,Allocator>& rhs)
{
  return lhs.compare(rhs) == 0;
}

template<class charT, class traits, class Allocator>
inline
bool
  operator==(const charT* lhs,
             const basic_string<charT,traits,Allocator>& rhs)
{
  return rhs.compare(lhs) == 0;
}

template<class charT, class traits, class Allocator>
inline
bool
  operator==(const basic_string<charT,traits,Allocator>& lhs,
             const charT* rhs)
{
  return lhs.compare(rhs) == 0;
}

///\name  operator!= [21.3.8.3 string::op!=]

template<class charT, class traits, class Allocator>
inline
bool
  operator!=(const basic_string<charT,traits,Allocator>& lhs,
             const basic_string<charT,traits,Allocator>& rhs)
{
  return ! (lhs == rhs);
}

template<class charT, class traits, class Allocator>
inline
bool
  operator!=(const charT* lhs, const basic_string<charT,traits,Allocator>& rhs)
{
  return ! (lhs == rhs);
}

template<class charT, class traits, class Allocator>
inline
bool
  operator!=(const basic_string<charT,traits,Allocator>& lhs, const charT* rhs)
{
  return ! (lhs == rhs);
}

///\name  operator< [21.3.8.4 string::op<]

template<class charT, class traits, class Allocator>
inline
bool
  operator< (const basic_string<charT,traits,Allocator>& lhs,
             const basic_string<charT,traits,Allocator>& rhs)
{
  return lhs.compare(rhs) < 0;
}

template<class charT, class traits, class Allocator>
inline
bool
  operator< (const basic_string<charT,traits,Allocator>& lhs, const charT* rhs)
{
  return lhs.compare(rhs) < 0;
}

template<class charT, class traits, class Allocator>
inline
bool
  operator< (const charT* lhs, const basic_string<charT,traits,Allocator>& rhs)
{
  return 0 < rhs.compare(lhs);
}

///\name  operator> [21.3.8.5 string::op>]

template<class charT, class traits, class Allocator>
inline
bool
  operator> (const basic_string<charT,traits,Allocator>& lhs,
             const basic_string<charT,traits,Allocator>& rhs)
{
  return lhs.compare(rhs) > 0;
}

template<class charT, class traits, class Allocator>
inline
bool
  operator> (const charT* lhs, const basic_string<charT,traits,Allocator>& rhs)
{
  return 0 > rhs.compare(lhs);
}

template<class charT, class traits, class Allocator>
inline
bool
  operator> (const basic_string<charT,traits,Allocator>& lhs, const charT* rhs)
{
  return lhs.compare(rhs) > 0;
}

///\name  operator<= [21.3.8.6 string::op<=]

template<class charT, class traits, class Allocator>
inline
bool
  operator<=(const basic_string<charT,traits,Allocator>& lhs,
             const basic_string<charT,traits,Allocator>& rhs)
{
  return lhs.compare(rhs) <= 0;
}

template<class charT, class traits, class Allocator>
inline
bool
  operator<=(const basic_string<charT,traits,Allocator>& lhs, const charT* rhs)
{
  return lhs.compare(rhs) <= 0;
}

template<class charT, class traits, class Allocator>
inline
bool
  operator<=(const charT* lhs, const basic_string<charT,traits,Allocator>& rhs)
{
  return 0 <= rhs.compare(lhs);
}

///\name  operator>= [21.3.8.7 string::op>=]

template<class charT, class traits, class Allocator>
inline
bool
  operator>=(const basic_string<charT,traits,Allocator>& lhs,
             const basic_string<charT,traits,Allocator>& rhs)
{
  return lhs.compare(rhs) >= 0;
}

template<class charT, class traits, class Allocator>
inline
bool
  operator>=(const basic_string<charT,traits,Allocator>& lhs, const charT* rhs)
{
  return lhs.compare(rhs) >= 0;
}

template<class charT, class traits, class Allocator>
inline
bool
  operator>=(const charT* lhs, const basic_string<charT,traits,Allocator>& rhs)
{
  return 0 >= rhs.compare(lhs);
}


///\name  swap [21.3.8.8 string.special]
template<class charT, class traits, class Allocator>
inline void swap(basic_string<charT,traits,Allocator>&  lhs, basic_string<charT,traits,Allocator>&  rhs) { lhs.swap(rhs); }

template <class charT, class traits, class Allocator>
struct constructible_with_allocator_suffix<
  basic_string<charT, traits, Allocator> > : true_type { };


///\name  Inserters and extractors [21.3.7.9 string.io]

template<class charT, class traits, class Allocator>
basic_istream<charT,traits>& operator>>(basic_istream<charT,traits>& is, basic_string<charT,traits,Allocator>& str);

template<class charT, class traits, class Allocator>
basic_ostream<charT, traits>& operator<<(basic_ostream<charT,traits>& os, const basic_string<charT,traits,Allocator>& str);

template<class charT, class traits, class Allocator>
basic_istream<charT,traits>& getline(basic_istream<charT,traits>& is, basic_string<charT,traits,Allocator>& str, charT delim);

template<class charT, class traits, class Allocator>
basic_istream<charT,traits>& getline(basic_istream<charT,traits>& is, basic_string<charT,traits,Allocator>& str);


///\name basic_string typedef names

/** Specialization of basic_string for the \e char characters */
typedef basic_string<char>    string;
/** Specialization of basic_string for the \e wchar_t characters */
typedef basic_string<wchar_t> wstring;
/** Specialization of basic_string for the \e char16_t characters */
typedef basic_string<char16_t> u16string;
/** Specialization of basic_string for the \e char32_t characters */
typedef basic_string<char32_t> u32string;


///\name 21.5 Numeric Conversions [string.conversions]

int stoi(const string& str, size_t *idx = 0, int base = 10);
long stol(const string& str, size_t *idx = 0, int base = 10);
unsigned long stoul(const string& str, size_t *idx = 0, int base = 10);
long long stoll(const string& str, size_t *idx = 0, int base = 10);
unsigned long long stoull(const string& str, size_t *idx = 0, int base = 10);

float stof(const string& str, size_t *idx = 0);
double stod(const string& str, size_t *idx = 0);
long double stold(const string& str, size_t *idx = 0);

string to_string(long long val);
string to_string(unsigned long long val);
string to_string(long double val);

int stoi(const wstring& str, size_t *idx = 0, int base = 10);
long stol(const wstring& str, size_t *idx = 0, int base = 10);
unsigned long stoul(const wstring& str, size_t *idx = 0, int base = 10);
long long stoll(const wstring& str, size_t *idx = 0, int base = 10);
unsigned long long stoull(const wstring& str, size_t *idx = 0, int base = 10);

float stof(const wstring& str, size_t *idx = 0);
double stod(const wstring& str, size_t *idx = 0);
long double stold(const wstring& str, size_t *idx = 0);

wstring to_wstring(long long val);
wstring to_wstring(unsigned long long val);
wstring to_wstring(long double val);

namespace __
{
  template<typename T>
  inline T stoi(const char* str, size_t length, size_t* idx, int base)
  {
    size_t l;
    typedef typename conditional<(sizeof(T) > sizeof(long)), unsigned long long, unsigned long>::type storage_type;
    storage_type value;
    ntl::numeric::convresult re = ntl::numeric::str2num<storage_type, typename make_signed<storage_type>::type>(value, str, length, base, numeric_limits<T>::__max, numeric_limits<T>::__min, &l);
    if(idx) *idx = l;
    if(re <= ntl::numeric::conv_result::bad_format){
  #if STLX__USE_EXCEPTIONS
      __throw_invalid_argument("stoi: no conversion could be performed");
  #else
      _assert_msg("stoi: no conversion could be performed");
  #endif
    }else if(re == ntl::numeric::conv_result::overflow){
  #if STLX__USE_EXCEPTIONS
      __throw_out_of_range("stoi: converted value is outside the range of representable values");
  #else
      _assert_msg("stoi: converted value is outside the range of representable values");
  #endif
    }
    return static_cast<T>(value);
  }
}

inline int stoi(const string& str, size_t *idx, int base)
{
  return __::stoi<int>(str.c_str(), str.length(), idx, base);
}

inline long stol(const string& str, size_t *idx, int base)
{
  return __::stoi<long>(str.c_str(), str.length(), idx, base);
}

inline unsigned long stoul(const string& str, size_t *idx, int base)
{
  return __::stoi<unsigned long>(str.c_str(), str.length(), idx, base);
}

inline long long stoll(const string& str, size_t *idx, int base)
{
  return __::stoi<long long>(str.c_str(), str.length(), idx, base);
}

inline unsigned long long stoull(const string& str, size_t *idx, int base)
{
  return __::stoi<unsigned long long>(str.c_str(), str.length(), idx, base);
}

inline int stoi(const wstring& str, size_t *idx, int base)
{
  char buf[ntl::numeric::max_number_size];
  const wchar_t* p = str.c_str();
  const size_t len = min(str.length(), ntl::numeric::max_number_size);
  for(size_t i = 0; i != len; i++, p++)
    buf[i] = static_cast<char>(*p);
  return __::stoi<int>(buf, len, idx, base);
}

inline long stol(const wstring& str, size_t *idx, int base)
{
  char buf[ntl::numeric::max_number_size];
  const wchar_t* p = str.c_str();
  const size_t len = min(str.length(), ntl::numeric::max_number_size);
  for(size_t i = 0; i != len; i++, p++)
    buf[i] = static_cast<char>(*p);
  return __::stoi<long>(buf, len, idx, base);
}

inline unsigned long stoul(const wstring& str, size_t *idx, int base)
{
  char buf[ntl::numeric::max_number_size];
  const wchar_t* p = str.c_str();
  const size_t len = min(str.length(), ntl::numeric::max_number_size);
  for(size_t i = 0; i != len; i++, p++)
    buf[i] = static_cast<char>(*p);
  return __::stoi<unsigned long>(buf, len, idx, base);
}

inline long long stoll(const wstring& str, size_t *idx, int base)
{
  char buf[ntl::numeric::max_number_size];
  const wchar_t* p = str.c_str();
  const size_t len = min(str.length(), ntl::numeric::max_number_size);
  for(size_t i = 0; i != len; i++, p++)
    buf[i] = static_cast<char>(*p);
  return __::stoi<long long>(buf, len, idx, base);
}

inline unsigned long long stoull(const wstring& str, size_t *idx, int base)
{
  char buf[ntl::numeric::max_number_size];
  const wchar_t* p = str.c_str();
  const size_t len = min(str.length(), ntl::numeric::max_number_size);
  for(size_t i = 0; i != len; i++, p++)
    buf[i] = static_cast<char>(*p);
  return __::stoi<unsigned long long>(buf, len, idx, base);
}


//////////////////////////////////////////////////////////////////////////
inline string to_string(long long val)
{
  string s(ntl::numeric::max_number_size, '\0');
  size_t len;
  ntl::numeric::itoa(val, s.begin(), s.size(), 10, &len);
  s.resize(len);
  return s;
}

inline string to_string(unsigned long long val)
{
  string s(ntl::numeric::max_number_size, '\0');
  size_t len;
  ntl::numeric::itoa(val, s.begin(), s.size(), 10, &len);
  s.resize(len);
  return s;
}

inline wstring to_wstring(long long val)
{
  wstring s(ntl::numeric::max_number_size, '\0');
  size_t len;
  ntl::numeric::itow(val, s.begin(), s.size(), 10, &len);
  s.resize(len);
  return s;
}

inline wstring to_wstring(unsigned long long val)
{
  wstring s(ntl::numeric::max_number_size, '\0');
  size_t len;
  ntl::numeric::itow(val, s.begin(), s.size(), 10, &len);
  s.resize(len);
  return s;
}

// eliminate to_string(-1) ambiguity
inline string to_string(int val) { return to_string(static_cast<long long>(val)); }
inline string to_string(unsigned int val) { return to_string(static_cast<unsigned long long>(val)); }
inline wstring to_wstring(int val) { return to_wstring(static_cast<long long>(val)); }
inline wstring to_wstring(unsigned int val) { return to_wstring(static_cast<unsigned long long>(val)); }

///\}

//////////////////////////////////////////////////////////////////////////
//hash specializations for basic_string:
namespace __
{
  template<typename T> struct string_hash;

  /// basic_string<> hash implementation
  template <class charT, class traits, class Allocator>
  struct string_hash<basic_string<charT, traits, Allocator> >:
    unary_function<basic_string<charT, traits, Allocator>, size_t>
  {
    /// string hash calculation
    inline size_t operator()(const basic_string<charT, traits, Allocator>& str) const __ntl_nothrow // 
    {
      return FNVHash()(str.data(), str.length()*sizeof(charT));
    }
  };
}

template <> struct hash<std::string>: __::string_hash<std::string>{};
template <> struct hash<std::wstring>: __::string_hash<std::wstring>{};
template <> struct hash<std::u16string>: __::string_hash<std::u16string>{};
template <> struct hash<std::u32string>: __::string_hash<std::u32string>{};



//////////////////////////////////////////////////////////////////////////
// 
#if STLX__USE_EXCEPTIONS
std::__::exstring::exstring(const std::string& r)
{
  len = r.length();
  if(len == 0){
    msg = 0;
  }else{
    msg = new char[len+1];
    char* const dest = const_cast<char*>(msg);
    r.copy(dest, len);
    dest[len] = 0;
  }
}
#endif

///@}
/**@} lib_strings */

}//namespace std

#endif//#ifndef NTL__STLX_STRING
