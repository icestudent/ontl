/**\file*********************************************************************
 *                                                                     \brief
 *  String classes [21.2 string.classes]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_STRING
#define NTL__STLX_STRING

#include "cwchar.hxx"
#include "cuchar.hxx"
#include "iosfwd.hxx"
/* */
#include "cstring.hxx"
#include "algorithm.hxx"
#include "iterator.hxx"
#include "memory.hxx"
#include "vector.hxx"

#include "new.hxx"

#ifndef EOF // should be moved to "stdio.hxx" ?
# define EOF -1
#endif

namespace std {

typedef ptrdiff_t streamoff;

/**\defgroup  lib_strings ************* 21 Strings library [strings]
 *@{*/

/**\defgroup  lib_char_traits ********* 21.1 Character traits [char.traits]
 *@{*/

/// 21.1 Character traits [char.traits]
/// a basis for explicit specializations.
template<class charT> struct char_traits;

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
  static int_type eof() { return static_cast<int_type>(EOF); }
};

/// 21.1.3.4 struct char_traits<wchar_t> [char.traits.specializations.wchar.t]
template<>
struct char_traits<wchar_t>
{
  typedef wchar_t   char_type;
  typedef wint_t    int_type;
  typedef streamoff off_type;
  typedef wstreampos pos_type;
  typedef mbstate_t state_type;

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
template <class charT,
          class traits    = char_traits<charT>,
          class Allocator = allocator<charT> >
class basic_string
{
    typedef vector<typename traits::char_type, Allocator> stringbuf_t;
    mutable stringbuf_t str; // mutable for c_str()

    static const charT zero_char = 0;

  ///////////////////////////////////////////////////////////////////////////
  public:

    ///\name Types

    typedef          traits                       traits_type;
    typedef typename stringbuf_t::value_type      value_type;
    typedef typename stringbuf_t::allocator_type  allocator_type;
    typedef typename stringbuf_t::size_type       size_type;
    typedef typename stringbuf_t::difference_type difference_type;
    typedef typename stringbuf_t::reference       reference;
    typedef typename stringbuf_t::const_reference const_reference;
    typedef typename stringbuf_t::pointer         pointer;
    typedef typename stringbuf_t::const_pointer   const_pointer;
    typedef typename stringbuf_t::iterator        iterator;
    typedef typename stringbuf_t::const_iterator  const_iterator;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

public:
    static const size_type npos = static_cast<size_type>(-1);

    ///\name 21.3.2 basic_string constructors and assigment operators [string.cons]

    /// 1 Effects: Constructs an object of class basic_string.
    /// Postconditions:
    /// - data() a non-null pointer that is copyable and can have 0 added to it;
    /// - size() == 0;
    /// - capacity() an unspecified value.
    explicit basic_string(const Allocator& a = Allocator()) : str(a) {/**/}


    /// 2 Effects: Constructs an object of class basic_string as indicated below.
    ///   The stored Allocator value is copied from str.get_allocator().
    /// Postconditions:
    /// - data() points at the first element of an allocated copy of the
    ///   array whose first element is pointed at by str.data();
    /// - size() == str.size();
    /// - capacity() is at least as large as size().
    __forceinline
    basic_string(const basic_string& str) : str(str.str) {/**/}

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
    basic_string(const basic_string & str,
                 size_type            pos,
                 size_type            n     = npos,
                 const Allocator &    a     = Allocator())//throw(out_of_range)
    : str(&str[pos], str.max__it(pos, n), a) {/**/}

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
    : str(assert_ptr(s), &s[n], a) {}

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
    : str(a)
    {
      assert_ptr(s);
      // small hack: copy terminating 0 to avoid case when n == 0
      size_type n = traits_type::length(s) + 1; // include '\0'
      alloc__new(n - 1); // allocates (n - 1) + 1
      str.end_ += n - 1;
      __assume(n > 0);
      while ( n-- ) traits_type::assign(str[n], s[n]);
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
    : str(n, c, a) {}

    /// 14 Effects: If InputIterator is an integral type, equivalent to
    ///   basic_string(static_cast<size_type>(begin), static_cast<value_type>(end), a)
    /// 15 Otherwise constructs a string from the values in the range [begin, end)
    ///   with Postconditions:
    /// - data() points at the first element of an allocated copy of
    ///   the sequence [begin, end);
    /// - size() == distance(begin, end);
    /// - capacity() is at least as large as size().
    template<class InputIterator>
    basic_string(InputIterator    begin,
                 InputIterator    end,
                 const Allocator& a     = Allocator())
    : str(begin, end, a) {}

    __forceinline
    basic_string(const basic_string& str, const Allocator& a)
      :str(str.str, a)
    {}

    __forceinline
    basic_string(initializer_list<charT> il, const Allocator& a = Allocator())
      :str(il, a)
    {}

    #ifdef NTL__CXX_RV
    __forceinline
    basic_string(basic_string&& str)
      :str(forward<stringbuf_t>(str.str))
    {}
    __forceinline
    basic_string(basic_string&& str, const Allocator& a)
      :str(forward<stringbuf_t>(str.str), a)
    {}
    #endif

    /// Effects: destructs string object.
    __forceinline
    ~basic_string() {}

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
    basic_string& operator=(basic_string&& str)
    {
      return this == &str ? *this : assign(move(str));
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

    ///\name  21.3.3 basic_string iterator support [string.iterators]

    /// 1 Returns: an iterator referring to the first character in the string.
    iterator                begin()         { return str.begin();   }
    const_iterator          begin()  const  { return str.cbegin();  }

    /// 2 Returns: an iterator which is the past-the-end value.
    iterator                end()           { return str.end();     }
    const_iterator          end()    const  { return str.cend();    }

    /// 3 Returns: an iterator which is semantically equivalent to reverse_iterator(end()).
    reverse_iterator        rbegin()        { return str.rbegin();  }
    const_reverse_iterator  rbegin() const  { return str.crbegin(); }

    /// 4 Returns: an iterator which is semantically equivalent to reverse_iterator(begin()).
    reverse_iterator        rend()          { return str.rend();    }
    const_reverse_iterator  rend()   const  { return str.crend();   }

    /// Returns: const iterators.
    const_iterator          cbegin()  const { return str.cbegin();  }
    const_iterator          cend()    const { return str.cend();    }
    const_reverse_iterator  crbegin() const { return str.crbegin(); }
    const_reverse_iterator  crend()   const { return str.crend();   }

    ///\name  21.3.4 basic_string capacity [string.capacity]

    /// 1 Returns: a count of the number of char-like objects currently in the string.
    size_type size()      const { return str.size();      }

    /// 2 Returns: size().
    size_type length()    const { return str.size();      }

    /// 3 Returns: The maximum size of the string.
    size_type max_size()  const { return str.max_size();  }

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
    void resize(size_type n, charT c)   { str.resize(n, c); }

    /// 8 Effects: resize(n,charT()).
    void resize(size_type n)            { str.resize(n);    }

    /// 9 Returns: the size of the allocated storage in the string.
    size_type capacity()  const { return str.capacity();  }

    /// 10 The member function reserve() is a directive that informs a basic_string
    ///   object of a planned change in size, so that it can manage the storage
    ///   allocation accordingly.
    /// 11 Effects: After reserve(), capacity() is greater or equal to the argument of reserve.
    ///\note Calling reserve() with a res_arg argument less than capacity() is
    ///   in effect a non-binding shrink request. A call with res_arg <= size()
    ///   is in effect a non-binding shrink-to-fit request.
    void reserve(size_type res_arg = 0) { str.reserve(res_arg); }

    /// 13 Remarks: shrink_to_fit is a non-binding request to reduce capacity()
    ///   to size(). \note The request is non-binding to allow latitude for
    ///   implementation-specific optimizations.
    void shrink_to_fit() 
    {
      str.shrink_to_fit();
    }

    /// 14 Effects: Behaves as if the function calls: erase(begin(), end());
    void clear()                        { str.clear();      }

    /// 15 Returns: size() == 0.
    bool empty()          const { return str.empty();     }

    ///\name  basic_string element access [21.3.5 string.access]

    /// 1 Returns: If pos < size(), returns *(begin() + pos). Otherwise,
    ///   if pos == size(), returns charT().
    const_reference operator[](size_type pos) const
    {
      if(pos < str.size())
        return str[pos];
      return zero_char;
    }

    /// 1 Returns: If pos < size(), returns *(begin() + pos).
    ///   Otherwise, the behavior is undefined.
    reference operator[](size_type pos)   { return str[pos];  }

    /// 2 Requires: pos < size()
    /// 3 Throws: out_of_range if pos >= size().
    /// 4 Returns: operator[](pos).
    const_reference at(size_type n) const { return str.at(n); }
    reference at(size_type n)             { return str.at(n); }

    /// 5 Requires: !empty()
    /// 6 Effects: Equivalent to operator[](0).
    const charT& front() const { _Assert(!empty()); return operator[](0); }
    charT& front()             { _Assert(!empty()); return operator[](0); }

    /// 7 Requires: !empty()
    /// 8 Effects: Equivalent to operator[](size() - 1).
    const charT& back() const { _Assert(!empty()); return operator[](size() - 1); }
    charT& back() { _Assert(!empty()); return operator[](size() - 1); }

    /// 21.3.6 basic_string modifiers [string.modifiers]

    ///\name  basic_string::operator+= [21.3.6.1 string::op+=]

    basic_string& operator+=(const basic_string& str) { return append(str);   }
    basic_string& operator+=(const charT* s)          { return append(s);     }
    basic_string& operator+=(charT c) { str.push_back(c); return *this; }

    basic_string& operator+=(initializer_list<charT> il) { return append(il); }


    ///\name  basic_string::append [21.3.6.2 string::append]

    basic_string& append(const basic_string& str)
    {
      this->str.insert(this->str.end(), str.begin(), str.end());
      return *this;
    }

    basic_string& append(const basic_string& str, size_type pos, size_type n)
    {
      this->str.insert(this->str.end(), str.begin() + pos, str.max__it(pos, n));
      return *this;
    }

    basic_string& append(const charT* s, size_type n)
    {
      str.insert(str.end(), s, &s[n]);
      return *this;
    }

    basic_string& append(const charT * s)
    {
      // trade off the traits_type::length(s) call for possible reallocs
      while ( !traits_type::eq(*s, charT()) ) push_back(*s++);
      return *this;
    }

    basic_string& append(size_type n, charT c)
    {
      str.insert(str.end(), n, c);
      return *this;
    }

    template<class InputIterator>
    basic_string& append(InputIterator first, InputIterator last)
    {
      str.insert(str.end(), first, last);
      return *this;
    }


    basic_string& apend(initializer_list<charT> il)
    {
      return append(il.begin(), il.end());
    }


    void push_back(charT c) { str.push_back(c); }

    ///\name  basic_string::assign [21.3.6.3 string::assign]

    basic_string& assign(const basic_string& str)
    {
      this->str.assign(str.begin(), str.end());
      return *this;
    }

    #ifdef NTL__CXX_RV
    basic_string& assign(basic_string&& str)
    {
      clear();
      swap(str);
      return *this;
    }
    #endif

    basic_string& assign(const basic_string& str, size_type pos, size_type n)
    {
      this->str.assign(&str[pos], str.max__it(pos, n));
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
      return insert(0, s);
    }

    basic_string& assign(size_type n, charT c)
    {
      this->str.assign(n, c);
      return *this;
    }

    template<class InputIterator>
    basic_string& assign(InputIterator first, InputIterator last)
    {
      this->str.assign(first, last);
      return *this;
    }


    basic_string& assign(initializer_list<charT> il)
    {
      return assign(basic_string(il));
    }


    ///\name  basic_string::insert [21.3.6.4 string::insert]

    basic_string& insert(size_type pos1, const basic_string& str)
    {
      this->str.insert(&this->str[pos1], str.begin(), str.end());
      return *this;
    }

    basic_string& insert(size_type            pos1,
                         const basic_string & str,
                         size_type            pos2,
                         size_type            n)
    {
      this->str.insert(&this->str[pos1], &str[pos2], str.max__it(pos2, n));
      return *this;
    }

    basic_string& insert(size_type pos, const charT* s, size_type n)
    {
      str.insert(&str[pos], s, &s[n]);
      return *this;
    }

    basic_string& insert(size_type pos, const charT* s)
    {
      const size_t n = traits_type::length(s);
      insert(&str[pos], s, &s[n]);
      return *this;
    }

    basic_string& insert(size_type pos, size_type n, charT c)
    {
      str.insert(&str[pos], n, c);
      return *this;
    }

    iterator insert(iterator p, charT c) { return str.insert(p, c); }

    void insert(iterator p, size_type n, charT c) { str.insert(p, n, c); }

    template<class InputIterator>
    void insert(iterator p, InputIterator first, InputIterator last)
    {
      str.insert(p, first, last);
    }

    void insert(iterator p, initializer_list<charT> il)
    {
      return str.insert(p, il.begin(), il.end());
    }

    ///\name  basic_string::erase [21.3.6.5 string::erase]

    basic_string& erase(size_type pos = 0, size_type n = npos)
    {
      str.erase(&str[pos], max__it(pos, n));
      return *this;
    }

    iterator erase(iterator position) { return str.erase(position); }

    iterator erase(iterator first, iterator last)
    {
      return str.erase(first, last);
    }

    ///\name  basic_string::replace [21.3.6.6 ib.string::replace]

    basic_string& replace(size_type pos1, size_type n1, const basic_string& str);

    basic_string& replace(size_type           pos1,
                          size_type           n1,
                          const basic_string& str,
                          size_type           pos2,
                          size_type           n2)
                          ;

    basic_string& replace(size_type     pos,
                          size_type     n1,
                          const charT * s,
                          size_type     n2)
                          ;

    basic_string& replace(size_type pos, size_type n1, const charT* s);
    basic_string& replace(size_type pos, size_type n1, size_type n2, charT c);

    basic_string& replace(iterator i1, iterator i2, const basic_string& str);
    basic_string& replace(iterator i1, iterator i2, const charT* s, size_type n);
    basic_string& replace(iterator i1, iterator i2, const charT* s);
    basic_string& replace(iterator i1, iterator i2, size_type n, charT c);

    template<class InputIterator>
    basic_string& replace(iterator i1,
                          iterator i2,
                          InputIterator j1,
                          InputIterator j2)
                          ;

    ///\name  basic_string::copy [21.3.6.7 string::copy]
    size_type copy(charT* s, size_type n, size_type pos = 0) const
    {
      const size_type tail = size() - pos;
      const size_type rlen = min(n, tail);
      for ( size_type i = 0; i != rlen; ++i )
        traits_type::assign(*s++, str[pos+i]);
      return rlen;
    }

    ///\name  basic_string::swap [21.3.6.8 string::swap]
    #ifdef NTL__CXX_RV
    void swap(basic_string&&str2) { str.swap(str2.str); }
    #else
    void swap(basic_string& str2) { str.swap(str2.str); }
    #endif

    ///\name  basic_string string operations [21.3.6 string.ops]

    const charT* c_str() const // explicit
    {
      // ensure string is null-terminated
      str.push_back(charT());
      str.pop_back();
      return str.begin();
    }

    const charT* data() const
    {
      static const charT empty;
      return /*size()*/ capacity() ? begin() : &empty;
    }

    allocator_type get_allocator() const { return str.get_allocator(); }

    ///\name   21.3.7.2 basic_string::find [string::find]

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
      for ( size_type xpos = pos; xpos + n <= size(); ++xpos )
      {
        for(size_type i = 0; i != n; ++i)
          if ( !traits_type::eq(*(begin() + xpos  + i), *(s + i)) )
            goto next_xpos;
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
      for ( size_type xpos = pos; xpos < size(); ++xpos )
        if ( traits_type::eq(*(begin() + xpos), c) )
          return xpos;
      return npos;
    }

    ///\name   basic_string::rfind [21.3.6.2 string::rfind]

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
      size_type & xpos = pos;
      if ( xpos > size() || xpos + n > size() )
        xpos = size() - n;
      while ( xpos + n > 0 )
      {
        for ( size_type i = 0; i != n; ++i )
          if ( !traits_type::eq(*(begin() + xpos + i), *(s + i)) )
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
      for ( size_type xpos = pos < size() ? pos + 1 : size(); xpos; )
        if ( traits_type::eq(*(begin() + --xpos), c) )
          return xpos;
      return npos;
    }

    /// 7 Returns: rfind(basic_string<charT,traits,Allocator>(1,c),npos).
    size_type rfind(charT c/*, size_type pos = npos*/) const
    {
      size_type xpos = size();
      while ( xpos-- )
        if ( traits_type::eq(*(begin() + xpos), c) )
          return xpos;
      return npos;
    }

    ///\name  21.3.7.4 basic_string::find_first_of [string::find.first.of]

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
      for ( size_type xpos = pos; xpos < size(); ++xpos )
        for ( size_type i = 0; i != n; ++i )
          if ( traits_type::eq(*(begin() + xpos), *(s + i)) )
            return xpos;
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
      for ( size_type xpos = pos; xpos < size(); ++xpos )
        if ( traits_type::eq(*(begin() + xpos), c) )
          return xpos;
      return npos;
    }

    ///\name  21.3.7.5 basic_string::find_last_of [string::find.last.of]

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
      size_type & xpos = pos;
      if ( xpos > size() ) xpos = size();
      while ( xpos )
      {
        for ( size_type i = 0; i != n; ++i )
          if ( traits_type::eq(*(begin() + xpos), *(s + i)) )
            return xpos;
        --xpos;
      }
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

    ///\name  21.3.7.6 basic_string::find_first_not_of [string::find.first.not.of]

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
      for ( size_type xpos = pos; xpos < size(); ++xpos )
      {
        for ( size_type i = 0; i != n; ++i )
          if ( traits_type::eq(*(begin() + xpos), *(s + i)) )
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
      for ( size_type xpos = pos; xpos < size(); ++xpos )
        if ( !traits_type::eq(*(begin() + xpos), c) )
          return xpos;
      return npos;
    }

    ///\name  21.3.7.7 basic_string::find_last_not_of [string::find.last.not.of]

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
      for ( size_type xpos = pos < size() ? pos + 1 : size(); xpos; )
      {
        --xpos;
        for ( size_type i = 0; i != n; ++i )
          if ( traits_type::eq(*(begin() + xpos), *(s + i)) )
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
      for ( size_type xpos = pos < size() ? pos + 1 : size(); xpos; )
        if ( !traits_type::eq(*(begin() + --xpos), c) )
          return xpos;
      return npos;
    }

    ///\name 21.3.7.8 basic_string::substr [string::substr]
    /// 1 Requires: pos <= size()
    /// 2 \todo Throws: out_of_range if pos > size().
    /// 3 Effects: Determines the effective length rlen of the string to copy as
    ////  the smaller of n and size() - pos.
    /// 4 Returns: basic_string<charT,traits,Allocator>(data()+pos,rlen).
    basic_string substr(size_type pos = 0, size_type n = npos) const
    {
      return basic_string(*this, pos, n);
    }

    ///\name  basic_string::compare [21.3.7.9 string::compare]

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
      return traits_type::compare(begin()+pos, str.begin(), rlen);
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
      return traits_type::compare(begin()+pos1, str.begin()+pos2, rlen);
    }

    int compare(const charT* s) const
    {
      if(!s)
        return static_cast<int>(size());
      const int r = traits_type::compare(begin(), s, size());
      // s may be longer than *this
      return r != 0 ? r : traits_type::eq(s[size()], charT()) ? r : r - 1; // r == 0 here
    }

    int compare(size_type pos, size_type n, const charT* s) const
    {
      if(!s || pos+n >= size())
        return -1;
      return traits_type::compare(begin()+pos, s, n);
    }

    int compare(size_type pos1, size_type n1, const charT* s, size_type n2) const
    {
      if(n1 != n2)
        return static_cast<int>(n1 - n2);
      else if(pos1+n1 >= size())
        return -1;
      return traits_type::compare(begin()+pos1, s, min(n1, n2));
    }

    ///\name  operator+ [21.3.8.1 string::op+]
    /// @note frends, not just non-member functions

  friend
    basic_string operator+(const basic_string& lhs, const basic_string& rhs)
    {
      basic_string<charT, traits, Allocator> sum;
      sum.alloc__new(lhs.size() + rhs.size());
      sum.append_to__reserved(lhs.begin(), lhs.end());
      sum.append_to__reserved(rhs.begin(), rhs.end());
      return sum;
    }

  friend
    basic_string operator+(const charT* lhs, const basic_string& rhs)
    {
      basic_string<charT, traits, Allocator> sum;
      sum.alloc__new(traits_type::length(lhs) + rhs.size());
      sum.append_to__reserved(lhs);
      sum.append_to__reserved(rhs.begin(), rhs.end());
      return sum;
    }

  friend
    basic_string operator+(charT lhs, const basic_string& rhs)
    {
      basic_string<charT, traits, Allocator> sum;
      sum.alloc__new(1 + rhs.size());
      sum.append_to__reserved(lhs);
      sum.append_to__reserved(rhs.begin(), rhs.end());
      return sum;
    }

  friend
    basic_string operator+(const basic_string& lhs, const charT* rhs)
    {
      basic_string<charT, traits, Allocator> sum;
      sum.alloc__new(lhs.size() + traits_type::length(rhs));
      sum.append_to__reserved(lhs.begin(), lhs.end());
      sum.append_to__reserved(rhs);
      return sum;
    }

  friend
    basic_string operator+(const basic_string& lhs, charT rhs)
    {
      basic_string<charT, traits, Allocator> sum;
      sum.alloc__new(lhs.size() + 1);
      sum.append_to__reserved(lhs.begin(), lhs.end());
      sum.push_back(rhs);
      return sum;
    }

#if !defined(NTL__STRICT_STRING) && !defined(__BCPLUSPLUS__)

  template<class String>
  friend __forceinline
    basic_string
      operator+(const String& lhs, const basic_string& rhs)
    {
      basic_string<charT, traits, Allocator> sum;
      sum.alloc__new(lhs.size() + rhs.size());
      sum.append_to__reserved(lhs.begin(), lhs.end());
      sum.append_to__reserved(rhs.begin(), rhs.end());
      return sum;
    }

  template<class String>
  friend
    basic_string
      operator+(const basic_string& lhs, const String& rhs)
    {
      basic_string<charT, traits, Allocator> sum;
      sum.alloc__new(lhs.size() + rhs.size());
      sum.append_to__reserved(lhs.begin(), lhs.end());
      sum.append_to__reserved(rhs.begin(), rhs.end());
      return sum;
    }

#endif

    ///@}

  private:

    static const charT* assert_ptr(const charT* const p)
    {
      _Assert(p);
      return p;
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
      traits_type::assign(*str.end_++, c);
    }

    void append_to__reserved(const_pointer s)
    {
      iterator i = str.end_;
      while ( *s ) traits_type::assign(*i++, *s++);
      str.end_ = i;
    }

    void append_to__reserved(const_iterator fist, const_iterator last)
    {
      iterator i = str.end_;
      while ( fist != last ) traits_type::assign(*i++, *fist++);
      str.end_ = i;
    }

    /// @note allocates n + 1 bytes, possibly optimizing c_str()
    void alloc__new(size_type n)
    {
      n = __ntl_grow_heap_block_size(n + sizeof('\0'));
      str.end_ = str.begin_ = str.array_allocator.allocate(n);
      str.capacity_ = n;
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
inline
void
  swap(basic_string<charT,traits,Allocator>& lhs,
       basic_string<charT,traits,Allocator>& rhs)
{
  lhs.swap(rhs);
}

#ifdef NTL__CXX_RV
template<class charT, class traits, class Allocator>
inline
void
swap(basic_string<charT,traits,Allocator>&& lhs,
     basic_string<charT,traits,Allocator>& rhs)
{
  lhs.swap(rhs);
}

template<class charT, class traits, class Allocator>
inline
void
swap(basic_string<charT,traits,Allocator>& lhs,
     basic_string<charT,traits,Allocator>&& rhs)
{
  lhs.swap(rhs);
}
#endif

template <class charT, class traits, class Allocator>
struct constructible_with_allocator_suffix<
  basic_string<charT, traits, Allocator> > : true_type { };


///\name  Inserters and extractors [21.3.7.9 string.io]

template<class charT, class traits, class Allocator>
basic_istream<charT,traits>& operator>>(typename __::istream<charT,traits>::type is, basic_string<charT,traits,Allocator>& str);

template<class charT, class traits, class Allocator>
basic_ostream<charT, traits>& operator<<(typename __::ostream<charT,traits>::type os, const basic_string<charT,traits,Allocator>& str);

template<class charT, class traits, class Allocator>
basic_istream<charT,traits>& getline(typename __::istream<charT,traits>::type is, basic_string<charT,traits,Allocator>& str, charT delim);

template<class charT, class traits, class Allocator>
basic_istream<charT,traits>& getline(typename __::istream<charT,traits>::type is, basic_string<charT,traits,Allocator>& str);

/** Specialization of basic_string for the \e char characters */
typedef basic_string<char>    string;
/** Specialization of basic_string for the \e wchar_t characters */
typedef basic_string<wchar_t> wstring;
/** Specialization of basic_string for the \e char16_t characters */
typedef basic_string<char16_t> u16string;
/** Specialization of basic_string for the \e char32_t characters */
typedef basic_string<char32_t> u32string;


//////////////////////////////////////////////////////////////////////////
//hash specializations for basic_string:
namespace __
{
  template<typename T> struct string_hash;

  /// basic_string<> hash implementation
  template <class charT, class traits, class Allocator>
  struct string_hash<basic_string<charT, traits, Allocator> >: unary_function<basic_string<charT, traits, Allocator>, size_t>
  {
    /// string hash calculation
    inline size_t operator()(const argument_type& str) const __ntl_nothrow
    {
      return FNVHash()(str.data(), str.length()*sizeof(charT));
    }
  };
}

template <> struct hash<std::string>: __::string_hash<std::string>{};
template <> struct hash<std::wstring>: __::string_hash<std::wstring>{};
template <> struct hash<std::u16string>: __::string_hash<std::u16string>{};
template <> struct hash<std::u32string>: __::string_hash<std::u32string>{};

///@}
/**@} lib_strings */

}//namespace std

#endif//#ifndef NTL__STLX_STRING
