/**\file*********************************************************************
 *                                                                     \brief
 *  String classes [21.2 lib.string.classes]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_STRING
#define NTL__STLX_STRING

#include "cwchar.hxx"
#include "iosfwd.hxx"
/* */
#include "cstring.hxx"
#include "algorithm.hxx"
#include "iterator.hxx"
#include "memory.hxx"
//#include "stdexcept.hxx"
#include "vector.hxx"

#ifndef EOF // should be removed to "stdio.hxx" ?
#define EOF -1
#endif

namespace std {

typedef ptrdiff_t streamoff;

/**\defgroup  lib_strings ************* Strings library [21] ****************
 *@{*/

/**\defgroup  lib_char_traits ********* Character traits [21.1] *************
 *@{*/

/// Character traits [21.1]
template<class charT> struct char_traits;

///char_traits specializations [21.1.3]
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

///char_traits specializations [21.1.3]
template<>
struct char_traits<wchar_t>
{
  typedef wchar_t   char_type;
  typedef wint_t    int_type;
  typedef streamoff off_type;
  typedef streampos pos_type;
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

/**@} lib_char_traits
 */

/// Class template basic_string [lib.basic.string]
///\note 
template <class charT,
          class traits    = char_traits<charT>,
          class Allocator = allocator<charT> >
class basic_string
{
    //  N2315 21.3.1/3: 
    //    The char-like objects in a basic_string object shall be stored contiguously.
    //    That is, for any basic_string object s,
    //    the identity &*(s.begin() + n) == &*s.begin() + n
    //    shall hold for all values of n such that 0 <= n < s.size().
    typedef vector<typename traits::char_type, Allocator> stringbuf_t;
    mutable stringbuf_t str; // mutable for c_str()

  ///////////////////////////////////////////////////////////////////////////
  public:

    // types:

    typedef typename traits                       traits_type;
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

    static const size_type npos = static_cast<size_type>(-1);

    ///\name  basic_string constructors [21.3.2 lib.string.cons]

    explicit basic_string(const Allocator& a = Allocator()) : str(a) {/**/}

    __forceinline
    basic_string(const basic_string& str) : str(str.str) {/**/}

    basic_string(const basic_string & str,
                 size_type            pos,
                 size_type            n     = npos,
                 const Allocator &    a     = Allocator())
    // commented - "stdexcept.hxx" includes this header
    //  throw(out_of_range)
    : str(&str[pos], str.max__it(pos, n), a) {/**/}

    basic_string(const charT* s, size_type n, const Allocator& a = Allocator())
    : str(s, &s[n], a) {}

    basic_string(const charT* s, const Allocator& a = Allocator())
    : str(a)
    { 
      // small hack: copy terminating 0 to avoid case when n == 0
      size_type n = traits_type::length(s) + 1; // include '\0'
      alloc__new(n - 1); // allocates (n - 1) + 1
      str.end_ += n - 1;
      __assume(n > 0);
      while ( n-- ) traits_type::assign(str[n], s[n]);
    }

    basic_string(size_type n, charT c, const Allocator& a = Allocator())
    : str(n, c, a) {}

    template<class InputIterator>
    basic_string(InputIterator    begin,
                 InputIterator    end,
                 const Allocator& a     = Allocator())
    : str(begin, end, a) {}

    __forceinline
    ~basic_string() {}

    basic_string& operator=(const basic_string& str)
    {
      return this == &str ? *this : assign(str);
    }

    basic_string& operator=(const charT* s) { return assign(s);     }
    basic_string& operator=(charT c)        { return assign(1, c);  }

    ///\name  basic_string iterator support [21.3.3 lib.string.iterators]

    iterator                begin()         { return str.begin();   }
    const_iterator          begin()  const  { return str.cbegin();   }
    iterator                end()           { return str.end();     }
    const_iterator          end()    const  { return str.cend();     }
    reverse_iterator        rbegin()        { return str.rbegin();  }
    const_reverse_iterator  rbegin() const  { return str.crbegin();  }
    reverse_iterator        rend()          { return str.rend();    }
    const_reverse_iterator  rend()   const  { return str.crend();    }

    const_iterator          cbegin()  const  { return begin();   }
    const_iterator          cend()    const  { return end();     }
    const_reverse_iterator  crbegin() const  { return rbegin();  }
    const_reverse_iterator  crend()   const  { return rend();    }

    ///\name  basic_string capacity [21.3.4 lib.string.capacity]

    size_type size()      const { return str.size();      }
    size_type length()    const { return str.size();      }
    size_type max_size()  const { return str.max_size();  }
    size_type capacity()  const { return str.capacity();  }
    bool empty()          const { return str.empty();     }

    void resize(size_type n, charT c)   { str.resize(n, c); }
    void resize(size_type n)            { str.resize(n);    }
    void reserve(size_type res_arg = 0) { str.reserve();    }
    void clear()                        { str.clear();      }

    ///\name  basic_string element access [21.3.5 lib.string.access]

    const_reference operator[](size_type pos) const
    {
      static const charT zero_char;// = charT();
      return pos < str.size() ? *(begin() + pos) : zero_char;
    }

    reference operator[](size_type pos)   { return str[pos];  }
    const_reference at(size_type n) const { return str.at(n); }
    reference at(size_type n)             { return str.at(n); }

    // 21.3.6 basic_string modifiers [lib.string.modifiers]

    ///\name  basic_string::operator+= [21.3.6.1 lib.string::op+=]

    basic_string& operator+=(const basic_string& str) { return append(str);   }
    basic_string& operator+=(const charT* s)          { return append(s);     }
    basic_string& operator+=(charT c) { push_back(c); return *this; }

#ifndef NTL__STRICT_STRING

  template<class String>
    basic_string& operator+=(const String& str)
    { 
      return append(str.data(), str.size());
    }

#endif


    ///\name  basic_string::append [21.3.6.2 lib.string::append]

    basic_string& append(const basic_string& str)
    {
      this->str.insert(this->str.end(), str.begin(), str.end());
      return *this;
    }

    basic_string& append(const basic_string& str, size_type pos, size_type n)
    {
      this->str.insert(this->str.end(), str.max__it(pos, n));
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

    void push_back(charT c) { str.push_back(c); }

    ///\name  basic_string::assign [21.3.6.3 lib.string::assign]

    basic_string& assign(const basic_string& str)
    {
      this->str.assign(str.begin(), str.end());
      return *this;
    }

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

    ///\name  basic_string::insert [21.3.6.4 lib.string::insert]

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

    ///\name  basic_string::erase [21.3.6.5 lib.string::erase]

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

    ///\name  basic_string::copy [21.3.6.7 lib.string::copy]
    size_type copy(charT* s, size_type n, size_type pos = 0) const
    {
      const size_type tail = size() - pos;
      const size_type rlen = min(n, tail);
      for ( size_type i = 0; i != rlen; ++i )
        traits_type::assign(*s++, str[pos+i]);
      return rlen;
    }

    ///\name  basic_string::swap [21.3.6.8 lib.string::swap]
    void swap(basic_string& str2) { str.swap(str2.str); }

    ///\name  basic_string string operations [21.3.6 lib.string.ops]

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

    ///\name   basic_string::find [21.3.6.1 lib.string::find]

    size_type find(const basic_string& str, size_type pos = 0) const
    {
      for ( size_type xpos = pos; xpos + str.size() <= size(); ++xpos )
      {
        for ( size_type i = 0; i != str.size(); ++i )
          if ( !traits_type::eq(*(begin() + xpos + i), *(str.begin() + i)) ) ///\note at() by Standard
            goto next_xpos;
        return xpos;
      next_xpos:;
      }
      return npos;
    }
    
    size_type find(const charT* s, size_type pos, size_type n) const;
    size_type find(const charT* s, size_type pos = 0) const;
    
    size_type find(charT c, size_type pos = 0) const
    {
      for ( size_type xpos = pos; xpos < size(); ++xpos )
        if ( traits_type::eq(*(begin() + xpos), c) )
          return xpos;
      return npos;
    }

    ///\name   basic_string::rfind [21.3.6.2 lib.string::rfind]

    size_type rfind(const basic_string& str, size_type pos = npos) const;
    size_type rfind(const charT* s, size_type pos, size_type n) const;
    size_type rfind(const charT* s, size_type pos = npos) const;

    size_type rfind(charT c/*, size_type pos = npos*/) const
    {
      size_type xpos = size();
      while ( xpos-- )
        if ( traits_type::eq(str[xpos], c) ) return xpos;
      return npos;
    }

    size_type rfind(charT c, size_type pos/* = npos*/) const
    {
      ///\note  Standard claims the use of at() member function, but
      ///       we stick to an exception-safe way
      size_type & xpos = pos;
      if ( xpos > size() ) xpos = size();
      while ( xpos )
        if ( traits_type::eq(str[xpos], c) ) return xpos;
        else --xpos;
      return npos;
    }

    ///\name  basic_string::find_first_of [21.3.6.3 lib.string::find.first.of]

    size_type find_first_of(const basic_string& str, size_type pos = 0) const;
    size_type find_first_of(const charT* s, size_type pos, size_type n) const;
    size_type find_first_of(const charT* s, size_type pos = 0) const;
    size_type find_first_of(charT c, size_type pos = 0) const;

    ///\name  basic_string::find_last_of [21.3.6.4 lib.string::find.last.of]

    size_type find_last_of (const basic_string& str, size_type pos = npos) const;
    size_type find_last_of (const charT* s, size_type pos, size_type n) const;
    size_type find_last_of (const charT* s, size_type pos = npos) const;
    size_type find_last_of (charT c, size_type pos = npos) const;

    ///\name  basic_string::find_first_not_of [21.3.6.5 lib.string::find.first.not.of]

    size_type find_first_not_of(const basic_string& str, size_type pos = 0) const;
    size_type find_first_not_of(const charT* s, size_type pos, size_type n) const;
    size_type find_first_not_of(const charT* s, size_type pos = 0) const;
    size_type find_first_not_of(charT c, size_type pos = 0) const;

    ///\name  basic_string::find_last_not_of [21.3.6.6 lib.string::find.last.not.of]

    size_type find_last_not_of (const basic_string& str, size_type pos = npos) const;
    size_type find_last_not_of (const charT* s, size_type pos, size_type n) const;
    size_type find_last_not_of (const charT* s, size_type pos = npos) const;
    size_type find_last_not_of (charT c, size_type pos = npos) const;

    ///\name  basic_string::substr [21.3.6.7 lib.string::substr]
    basic_string substr(size_type pos = 0, size_type n = npos) const
    {
      return basic_string(*this, pos, n);
    }

    ///\name  basic_string::compare [21.3.6.8 lib.string::compare]

    int compare(const basic_string& str) const
    {
      const int r = traits_type::compare(begin(), str.begin(),
                                            std::min(size(), str.size()));
      return r != 0 ? r : size() - str.size();   
    }

    int compare(size_type pos1, size_type n1, const basic_string& str) const;
    int compare(size_type pos1, size_type n1, const basic_string& str, size_type pos2, size_type n2) const;

    int compare(const charT* s) const
    {
      const int r = traits_type::compare(begin(), s, size());
      // s may be longer than *this
      return r != 0 ? r : traits_type::eq(s[size()], charT()) ? r : r - 1; // r == 0 here
    }

    int compare(size_type pos1, size_type n1, const charT* s) const;
    int compare(size_type pos1, size_type n1, const charT* s, size_type n2) const;

    ///\name  operator+ [21.3.7.1 lib.string::op+]
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
      sum.push_back(rhs)
      return sum;
    }

#ifndef NTL__STRICT_STRING

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
      sum.append_to__reserved(rhs.begin(), rhs.end())
      return sum;
    }

#endif

    ///@}

  private:

    iterator max__it(size_type pos, size_type n) const
    {
      return n < str.size() - pos ? str.begin() + pos + n : str.end();
    }

    void append_to__reserved(charT c)
    {
      traits_type::assign(str.end_++, c);
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

#if 0

    basic_string append_to__reserved(const basic_string & str)
    {
      iterator to = this->str.end_;
      for ( const_iterator from = str.begin(); from != str.end(); ) 
        traits_type::assign(*to++, *from++);
      this->str.end_ = to;
      return *this;
    }

#endif

    /// @note allocates n + 1 bytes, possibly optimizing c_str()
    void alloc__new(size_type n)
    {
      str.end_ = str.begin_ = str.array_allocator.allocate(n + sizeof('\0'));
      str.capacity_ = n + 1;
    }

};//class basic_string

// 21.3.7 basic_string non-member functions [lib.string.nonmembers]

///\name  operator== [21.3.7.2 lib.string::operator==]

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

///\name  operator!= [21.3.7.3 lib.string::op!=]

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

///\name  operator< [21.3.7.4 lib.string::op<]

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

///\name  operator> [21.3.7.5 lib.string::op>]

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

///\name  operator<= [21.3.7.6 lib.string::op<=]

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

///\name  operator>= [21.3.7.7 lib.string::op>=]

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

///\name  swap [21.3.7.8 lib.string.special]
template<class charT, class traits, class Allocator>
inline
void
  swap(basic_string<charT,traits,Allocator>& lhs,
       basic_string<charT,traits,Allocator>& rhs)
{
  lhs.swap(rhs);
}

///\name  Inserters and extractors [21.3.7.9 lib.string.io]

template<class charT, class traits, class Allocator>
basic_istream<charT,traits>&
  operator>>(basic_istream<charT,traits>& is, 
             basic_string<charT,traits,Allocator>& str);

template<class charT, class traits, class Allocator>
basic_ostream<charT, traits>&
  operator<<(basic_ostream<charT, traits>& os,
             const basic_string<charT,traits,Allocator>& str);

template<class charT, class traits, class Allocator>
basic_istream<charT,traits>&
  getline(basic_istream<charT,traits>& is,
          basic_string<charT,traits,Allocator>& str, charT delim);

template<class charT, class traits, class Allocator>
basic_istream<charT,traits>&
  getline(basic_istream<charT,traits>& is, 
          basic_string<charT,traits,Allocator>& str);

typedef basic_string<char>    string;
typedef basic_string<wchar_t> wstring;


#if 0

#if CHAR_MIN == 0
typedef signed char utf8char_t;
#else
typedef unsigned char utf8char_t;
#endif

template<>
struct char_traits<utf8char_t> : char_traits<char> {};
typedef basic_string<utf8char_t>    utf8string;

#endif

///@}
/**@} lib_strings */

}//namespace std

#endif//#ifndef NTL__STLX_STRING
