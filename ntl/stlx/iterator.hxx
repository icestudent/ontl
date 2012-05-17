/**\file*********************************************************************
 *                                                                     \brief
 *  Iterators library [24 lib.iterators]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_ITERATOR
#define NTL__STLX_ITERATOR
#pragma once

#ifndef NTL__STLX_CSTDDEF
#include "cstddef.hxx"
#endif
#ifndef NTL__STLX_IOSFWD
#include "iosfwd.hxx"
#endif
#ifndef NTL__STLX_EXCPTDEF
#include "excptdef.hxx"
#endif

namespace std
{

/**\addtogroup  lib_iterators ********** 24 Iterators library [iterators]
 *
 *    Components that C++ programs may use to perform iterations over
 *    \link lib_containers containers\endlink,
 *    \link lib_input_output streams\endlink,
 *    and \link lib_stream_buffers stream buffers\endlink.
 *@{
 */

// primitives [24.3]

/**\addtogroup  lib_std_iterator_tags ** 24.3.3 Standard iterator tags [std.iterator.tags]
 *@{*/
struct input_iterator_tag                                             {};
struct output_iterator_tag                                            {};
struct forward_iterator_tag       : public input_iterator_tag         {};
struct bidirectional_iterator_tag : public forward_iterator_tag       {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};
/**@}*/

/**\addtogroup lib_std_iterator_traits ** 24.3.1 Iterator traits [iterator.traits]
  *@{*/
template<class Iterator>
struct iterator_traits
{
  typedef typename Iterator::difference_type    difference_type;
  typedef typename Iterator::value_type         value_type;
  typedef typename Iterator::pointer            pointer;
  typedef typename Iterator::reference          reference;
  typedef typename Iterator::iterator_category  iterator_category;
};

template<class T>
struct iterator_traits<T*>
{
  typedef ptrdiff_t                   difference_type;
  typedef T                           value_type;
  typedef T *                         pointer;
  typedef T &                         reference;
  typedef random_access_iterator_tag  iterator_category;
};

template<class T>
struct iterator_traits<const T*>
{
  typedef ptrdiff_t                   difference_type;
  typedef       T                     value_type;
  typedef const T *                   pointer;
  typedef const T &                   reference;
  typedef random_access_iterator_tag  iterator_category;
};
/**@}*/

/**\addtogroup lib_std_iterator ** 24.3.2 Basic iterator [iterator.basic]
 *@{*/
template<class Category, class T, class Distance = ptrdiff_t,
         class Pointer = T*, class Reference = T&>
struct iterator
{
  typedef T         value_type;
  typedef Distance  difference_type;
  typedef Pointer   pointer;
  typedef Reference reference;
  typedef Category  iterator_category;
};
/**@}*/

/**\addtogroup lib_std_iterator_operations ** 24.3.4 Iterator operations [iterator.operations]
*@{*/
namespace __
{
template<class InputIterator, class Distance>
static inline
void
  advance(InputIterator& i, Distance n, const input_iterator_tag &)
{
  advance(i, n, forward_iterator_tag());
}

template<class ForwardIterator, class Distance>
static inline
void
  advance(ForwardIterator& i, Distance n, const forward_iterator_tag &)
{
  for ( ; 0 < n ; --n ) ++i;
}

template<class BidirectionalIterator, class Distance>
static inline
void
  advance(BidirectionalIterator& i, Distance n,
            const bidirectional_iterator_tag &)
{
  if ( 0 < n ) advance(i, n, forward_iterator_tag());
  else  for ( ; n < 0; ++n ) --i;
}

template<class RandomIterator, class Distance>
static inline
void
  advance(RandomIterator& i, Distance n, const random_access_iterator_tag &)
{
  i += n;
}

template <class InputIterator>
static inline
typename iterator_traits<InputIterator>::difference_type
  distance(const InputIterator& first, const InputIterator& last,
             const input_iterator_tag &)
{
  return distance(first, last, forward_iterator_tag());
}

template <class ForwardIterator>
static inline
typename iterator_traits<ForwardIterator>::difference_type
  distance(const ForwardIterator& first, const ForwardIterator& last,
           const forward_iterator_tag &)
{
  ForwardIterator from(first), to(last);
  typename iterator_traits<ForwardIterator>::difference_type d = 0;
  for ( ; from != to; ++from ) ++d;
  return d;
}

template <class BidirectionalIterator>
static inline
typename iterator_traits<BidirectionalIterator>::difference_type
  distance(const BidirectionalIterator& first,
           const BidirectionalIterator& last,
           const bidirectional_iterator_tag &)
{
  // NOTE: is it required? bidirectional_iterator_tag is convertible to the forward_iterator_tag
  return distance(first, last, forward_iterator_tag());
}

template <class RandomIterator>
static inline
typename iterator_traits<RandomIterator>::difference_type
  distance(const RandomIterator& first, const RandomIterator& last,
           const random_access_iterator_tag &)
{
  return last - first;
}

} //namespace __

//\name  24.3.4 Iterator operations [iterator.operations]

template<class AnyIterator, class Distance>
inline
void
  advance(AnyIterator& i, Distance n)
{
  __::advance(i, n, iterator_traits<AnyIterator>::iterator_category());
}

template <class AnyIterator>
inline
typename iterator_traits<AnyIterator>::difference_type
  distance(AnyIterator first, AnyIterator last)
{
  return __::distance(first, last,
                          iterator_traits<AnyIterator>::iterator_category());
}

template <class InputIterator>
inline
InputIterator
  next(InputIterator x, typename iterator_traits<InputIterator>::difference_type n = 1)
{
  advance(x, n);
  return x;
}

template <class InputIterator>
inline
InputIterator
  prev(InputIterator x, typename iterator_traits<InputIterator>::difference_type n = 1)
{
  advance(x, -n);
  return x;
}

/**@} lib_std_iterator_operations */


/**\addtogroup lib_std_predef_iterators ** 24.5 Iterator adaptors [predef.iterators]
 *@{*/

/// 24.5.1 Reverse iterators [lib.reverse.iterators]
/// Class template reverse_iterator is an iterator adaptor that iterates
/// from the end of the sequence defined by its underlying iterator
/// to the beginning of that sequence.
/// The fundamental relation between a reverse iterator and
/// its corresponding iterator i is established by the identity:
/// &*(reverse_iterator(i)) == &*(i - 1).
template <class Iterator>
class reverse_iterator
: public iterator<typename iterator_traits<Iterator>::iterator_category,
                  typename iterator_traits<Iterator>::value_type,
                  typename iterator_traits<Iterator>::difference_type,
                  typename iterator_traits<Iterator>::pointer,
                  typename iterator_traits<Iterator>::reference>
{
  public:

    typedef Iterator                                            iterator_type;
    typedef typename iterator_traits<Iterator>::difference_type difference_type;
    typedef typename iterator_traits<Iterator>::reference       reference;
    typedef typename iterator_traits<Iterator>::pointer         pointer;

    reverse_iterator() {}
    explicit reverse_iterator(Iterator x) : current(x) {}

    template<class U> friend class reverse_iterator;
    
    template <class U>
    reverse_iterator(const reverse_iterator<U>& u) : current(u.current) {}
    template <class U>
    reverse_iterator& operator=(const reverse_iterator<U>& u)
    {
      current = u.current;
      return *this;
    }

    Iterator  base()        const { return current; }
    ///\warning see N3092 24.5.1.3.4: This operation must use an auxiliary member variable rather than a temporary variable to avoid returning a reference that persists beyond the lifetime of its associated iterator.
    ///\todo make this to { return *current; }
    reference operator*()   const { Iterator tmp = current; return *--tmp; }
    pointer   operator->()  const { return &(operator*()); }

    reverse_iterator& operator++()  { --current; return *this; }
    reverse_iterator operator++(int)
      { reverse_iterator tmp = *this; --current; return tmp; }
    reverse_iterator& operator--()  { ++current; return *this; }
    reverse_iterator operator--(int)
      { reverse_iterator tmp = *this; ++current; return tmp; }

    reverse_iterator operator+ (difference_type n) const
    {
      return reverse_iterator( current - n );
    }

    reverse_iterator& operator+=(difference_type n)
    {
      current -= n;
      return *this;
    }

    reverse_iterator operator- (difference_type n) const
    {
      return reverse_iterator( current + n );
    }

    reverse_iterator& operator-=(difference_type n)
    {
      current += n;
      return *this;
    }

    reference operator[](difference_type n) const
    {
      return current[-n-1];
    }

  template<typename Iterator2>
  friend
    bool operator==(const reverse_iterator<Iterator>& x,
                    const reverse_iterator<Iterator2>& y)
      { return x.current == y.base(); }

  template<typename Iterator2>
  friend
    bool operator!=(const reverse_iterator<Iterator>& x,
                    const reverse_iterator<Iterator2>& y)
      { return x.current != y.base(); }

  template<typename Iterator2>
  friend
    bool operator< (const reverse_iterator<Iterator>& x,
                    const reverse_iterator<Iterator2>& y)
      { return x.current > y.base(); }

  template<typename Iterator2>
  friend
    bool operator> (const reverse_iterator<Iterator>& x,
                    const reverse_iterator<Iterator2>& y)
      { return x.current < y.base(); }

  template<typename Iterator2>
  friend
    bool operator>=(const reverse_iterator<Iterator>& x,
                    const reverse_iterator<Iterator2>& y)
      { return x.current <= y.base(); }

  template<typename Iterator2>
  friend
    bool operator<=(const reverse_iterator<Iterator>& x,
                    const reverse_iterator<Iterator2>& y)
      { return x.current >= y.base(); }

#if defined(NTL_CXX_AUTORET) && 0
  // isn't used anymore
  template<typename Iterator2>
  friend auto operator-(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator2>& y) -> decltype(y.current-x.current)
  { return y.base() - x.current; }
#else
  template<typename Iterator2>
  friend
    typename reverse_iterator<Iterator>::difference_type
      operator-(const reverse_iterator<Iterator>& x,
                const reverse_iterator<Iterator2>& y)
      { return y.base() - x.current; }
#endif
  friend inline
    reverse_iterator<Iterator>
      operator+(typename reverse_iterator<Iterator>::difference_type n,
                const reverse_iterator<Iterator>& x)
      {
        return reverse_iterator(x.current - n);
      }

  protected:
    Iterator current;
};//class reverse_iterator


/// 24.5.2.1 Class template back_insert_iterator [lib.back.insert.iterator]
template <class Container>
class back_insert_iterator
: public iterator<output_iterator_tag, void, void, void, void>
{
  public:
    typedef Container container_type;

    explicit back_insert_iterator(Container & x) : container(&x) {}

    back_insert_iterator<Container>&
      operator=(typename Container::const_reference value)
    {
      container->push_back(value);
      return *this;
    }
    #ifdef NTL_CXX_RV
    back_insert_iterator<Container>&
      operator=(typename Container::value_type&& value)
    {
      container->push_back(std::move(value));
      return *this;
    }
    #endif

    back_insert_iterator<Container>& operator*()     { return *this; }
    back_insert_iterator<Container>& operator++()    { return *this; }
    back_insert_iterator<Container>  operator++(int) { return *this; }

  protected:
    Container * container;
};

/// 24.5.2.2.5 back_inserter [lib.back.inserter]
template <class Container> inline
back_insert_iterator<Container>
back_inserter(Container& x) { return back_insert_iterator<Container>( x ); }


/// 24.5.2.3 Class template front_insert_iterator [lib.front.insert.iterator]
template <class Container>
class front_insert_iterator
: public iterator<output_iterator_tag, void, void, void, void>
{
  public:
    typedef Container container_type;

    explicit front_insert_iterator(Container& x) : container(&x) {}

    front_insert_iterator<Container>&
      operator=(typename Container::const_reference value)
    {
      container->push_front(value);
      return *this;
    }
    #ifdef NTL_CXX_RV
    front_insert_iterator<Container>&
      operator=(typename Container::value_type&& value)
    {
      container->push_front(std::move(value));
      return *this;
    }
    #endif

    front_insert_iterator<Container>& operator*()     { return *this; }
    front_insert_iterator<Container>& operator++()    { return *this; }
    front_insert_iterator<Container>  operator++(int) { return *this; }

  protected:
    Container * container;
};

/// 24.5.2.4.5 front_inserter [lib.front.inserter]
template <class Container>
inline
front_insert_iterator<Container>
  front_inserter(Container& x)
{
  return front_insert_iterator<Container>( x );
}


/// 24.5.2.5 Class template insert_iterator [lib.insert.iterator]
template <class Container>
class insert_iterator
: public iterator<output_iterator_tag, void, void, void, void>
{
  public:
    typedef Container container_type;

    insert_iterator(Container& x, typename Container::iterator i)
    : container(&x), iter(i) {}

    insert_iterator<Container>&
      operator=(typename Container::const_reference value)
    {
      iter = container->insert(iter, value);
      ++iter;
      return *this;
    }
    #ifdef NTL_CXX_RV
    insert_iterator<Container>&
      operator=(typename Container::value_type&& value)
    {
      iter = container->insert(iter, std::move(value));
      ++iter;
      return *this;
    }
    #endif

    insert_iterator<Container>& operator*()     { return *this; }
    insert_iterator<Container>& operator++()    { return *this; }
    insert_iterator<Container>& operator++(int) { return *this; }

  protected:
    Container * container;
    typename Container::iterator iter;
};

/// 24.5.2.6.5 inserter [lib.inserter]
template <class Container, class Iterator>
inline
insert_iterator<Container>
  inserter(Container& x, Iterator i)
{
  return insert_iterator<Container>( x, typename Container::iterator( i ) );
}

/// 24.5.3.1 Class template move_iterator [move.iterator]
#ifdef NTL_CXX_RV
template <class Iterator>
class move_iterator
{
public:
  typedef Iterator iterator_type;
  typedef Iterator pointer;
  typedef typename iterator_traits<Iterator>::difference_type   difference_type;
  typedef typename iterator_traits<Iterator>::value_type        value_type;
  typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
  typedef value_type&& reference;

  move_iterator()
    :current()
  {}

  explicit move_iterator(Iterator i)
    :current(i)
  {}

  template <class U> move_iterator(const move_iterator<U>& u)
    :current(u.current)
  {}
  template <class U> move_iterator& operator=(const move_iterator<U>& u)
  {
    current = u.current;
    return *this;
  }
  iterator_type base() const { return current; }
  reference operator*() const{ return std::move(*current); }
  pointer operator->() const { return current; }
  move_iterator& operator++(){ ++current; return *this; }
  move_iterator operator++(int){ move_iterator tmp = *this; ++current; return tmp; }
  move_iterator& operator--(){ --current; return *this; }
  move_iterator operator--(int){ move_iterator tmp = *this; --current; return tmp; }
  move_iterator operator+(difference_type n) const { return move_iterator(current+n); }
  move_iterator& operator+=(difference_type n)     { current += n; return *this; }
  move_iterator operator-(difference_type n) const { return move_iterator(current-n); }
  move_iterator& operator-=(difference_type n)     { current -= n; return *this; }
  reference operator[](difference_type n) const    { return std::move(current[n]); }

  template<typename Iterator2>
  friend bool operator==(const move_iterator<Iterator>& x, const move_iterator<Iterator2>& y)
  { return x.current == y.current; }

  template<typename Iterator2>
  friend bool operator!=(const move_iterator<Iterator>& x, const move_iterator<Iterator2>& y)
  { return x.current != y.current; }

  template<typename Iterator2>
  friend bool operator< (const move_iterator<Iterator>& x, const move_iterator<Iterator2>& y)
  { return x.current < y.current; }

  template<typename Iterator2>
  friend bool operator> (const move_iterator<Iterator>& x, const move_iterator<Iterator2>& y)
  { return x.current > y.current; }

  template<typename Iterator2>
  friend bool operator>=(const move_iterator<Iterator>& x, const move_iterator<Iterator2>& y)
  { return x.current >= y.current; }

  template<typename Iterator2>
  friend bool operator<=(const move_iterator<Iterator>& x, const move_iterator<Iterator2>& y)
  { return x.current <= y.current; }

#if defined(NTL_CXX_AUTORET) && 0
  template<typename Iterator2>
  friend auto operator-(const move_iterator<Iterator>& x, const move_iterator<Iterator2>& y) -> decltype(y.current-x.current)
  { return y.current - x.current; }
#else
  template<typename Iterator2>
  friend typename move_iterator<Iterator>::difference_type operator-(const move_iterator<Iterator>& x, const move_iterator<Iterator2>& y)
  { return y.current - x.current; }
#endif
  friend inline move_iterator<Iterator> operator+(typename move_iterator<Iterator>::difference_type n, const move_iterator<Iterator>& x)
  {
    return move_iterator(x.current + n);
  }

private:
  Iterator current;
};

template <class Iterator>
inline move_iterator<Iterator> make_move_iterator(const Iterator& i)
{
  return move_iterator<Iterator>(i);
}

#endif // move iterator

/**@}*/


/**\addtogroup lib_istream_iterator ** 24.5 Stream iterators [stream.iterators]
*@{*/

/// 24.5.1 Class template istream_iterator [lib.istream.iterator]
template <class T,
          class charT = char,
          class traits = char_traits<charT>,
          class Distance = ptrdiff_t>
class istream_iterator
: public iterator<input_iterator_tag, T, Distance, const T*, const T&>
  {
    typedef istream_iterator<T, charT, traits, Distance> this_type;

  public:
    typedef charT                         char_type;
    typedef traits                        traits_type;
    typedef basic_istream<charT, traits>  istream_type;

    istream_iterator()                : in_stream(0) {}
    istream_iterator(istream_type& s) : in_stream(&s) { read(); }
    //istream_iterator(const this_type& x)
    //: in_stream(x.in_stream), value(x.value) {}

  #ifdef NTL_CXX_EF
    istream_iterator(const this_type& x) __default;
    ~istream_iterator() __default;
  #endif

    const T&   operator* () const { return value; }
    const T*   operator->() const { return &(operator*()); }
    this_type& operator++()       { read(); return *this; }
    this_type  operator++(int)    { this_type tmp = *this; read(); return tmp; }

  friend
    bool operator==(const this_type& x, const this_type& y)
      { return x.in_stream == y.in_stream; }

  friend
    bool operator!=(const this_type& x, const this_type& y)
      { return x.in_stream != y.in_stream; }

  private:
    istream_type *  in_stream;
    T               value;

    void read()
    {
      // The result of operator* on an end of stream is not defined.
      if ( (*in_stream >> value).fail() ) in_stream = 0;
    }
};

/// 24.5.2 Class template ostream_iterator [lib.ostream.iterator]
#ifdef __ICL
#pragma warning(disable:444) // [remark] destructor for base class "std::iterator<std::output_iterator_tag, void, void, void, void>" (declared at line 74) is not virtual
#endif
template <class T,
          class charT = char,
          class traits = char_traits<charT> >
class ostream_iterator
: public iterator<output_iterator_tag, void, void, void, void>
{
  typedef ostream_iterator<T, charT, traits> this_type;

  public:
    typedef charT                         char_type;
    typedef traits                        traits_type;
    typedef basic_ostream<charT, traits>  ostream_type;

    ostream_iterator(ostream_type& s) : out_stream(&s), delim(0) {}
    ostream_iterator(ostream_type& s, const charT* delimiter)
    : out_stream(&s), delim(delimiter) {}
    ostream_iterator(const this_type& x)
    : out_stream(x.out_stream), delim(x.delim) {}
    ~ostream_iterator() {}

    ostream_iterator<T,charT,traits>& operator=(const T& value)
    {
      *out_stream << value;
      if ( delim ) *out_stream << delim;
      return *this;
    }

    ostream_iterator<T,charT,traits>& operator*()     { return *this; }
    ostream_iterator<T,charT,traits>& operator++()    { return *this; }
    ostream_iterator<T,charT,traits>& operator++(int) { return *this; }

  private:
    ostream_type *  out_stream;
    const char *    delim;
};//class ostream_iterator
#ifdef __ICL
#pragma warning(default:444)
#endif

/// 24.5.3 Class template istreambuf_iterator [lib.istreambuf.iterator]
template<class charT, class traits
                                    #ifndef NTL__STLX_IOSFWD
                                    = char_traits<charT>
                                    #endif
                                    >
class istreambuf_iterator
: public iterator<input_iterator_tag,
                  charT,
                  typename traits::off_type,
                  charT*,
                  charT&>
{
    typedef istreambuf_iterator<charT, traits>  this_type;

  public:
    typedef charT                           char_type;
    typedef traits                          traits_type;
    typedef typename traits::int_type       int_type;
    typedef basic_streambuf<charT, traits>  streambuf_type;
    typedef basic_istream<charT, traits>    istream_type;

    struct proxy
    {
        charT             operator*() { return c; }
        streambuf_type *  sbuf()      { return sbuf_; }

    private:
        streambuf_type *  sbuf_;
        charT             c;
        proxy(charT c, streambuf_type* sbuf) : c(c), sbuf_(sbuf) {}
        friend proxy istreambuf_iterator::operator++(int);
    };

    constexpr istreambuf_iterator()                   __ntl_nothrow : sbuf_(0) {}
    istreambuf_iterator(istream_type& s)    __ntl_nothrow : sbuf_(s.rdbuf()) {}
    istreambuf_iterator(streambuf_type* s)  __ntl_nothrow : sbuf_(s) {}
    istreambuf_iterator(const proxy& p)     __ntl_nothrow : sbuf_(p.sbuf()) {}

    charT operator*() const
    {
      // The result of operator*() on an end of stream is undefined.
      int_type c = sbuf_->sgetc();
      if ( ! traits::not_eof(c) ) sbuf_ = 0;
      return traits::to_char_type(c);
    }

    this_type& operator++() { if(sbuf_) sbuf_->sbumpc(); return *this; }

    proxy operator++(int) { return proxy( traits::to_char_type(sbuf_->sbumpc()), sbuf_ ); }

    bool equal(const istreambuf_iterator& b) const
    {
      return at_end() == b.at_end();
    }

  friend
    bool operator==(const this_type& a, const this_type& b)
      { return a.equal(b); }

  friend
    bool operator!=(const this_type& a, const this_type& b)
      { return ! a.equal(b); }

  private:
    bool at_end() const
    {
      return !(sbuf_ && traits_type::not_eof(sbuf_->sgetc()));
    }

  private:
    mutable streambuf_type * sbuf_;
};

/// 24.5.4 Class template ostreambuf_iterator [lib.ostreambuf.iterator]
template <class charT, class traits
                                    #ifndef NTL__STLX_IOSFWD
                                    = char_traits<charT>
                                    #endif
                                    >
class ostreambuf_iterator
: public iterator<output_iterator_tag, void, void, void, void>
{
    typedef ostreambuf_iterator<charT, traits>  this_type;

  public:
    typedef charT                           char_type;
    typedef traits                          traits_type;
    typedef basic_streambuf<charT, traits>  streambuf_type;
    typedef basic_ostream<charT, traits>    ostream_type;

    ostreambuf_iterator(ostream_type& s)    __ntl_nothrow : sbuf_(s.rdbuf()) {}
    ostreambuf_iterator(streambuf_type* s)  __ntl_nothrow : sbuf_(s) {}

    ostreambuf_iterator& operator=(charT c)
    {
      if ( ! failed() && ! traits::not_eof(sbuf_->sputc(c)) ) sbuf_ = 0;
      return *this;
    }

    ostreambuf_iterator& operator*()      { return *this; }
    ostreambuf_iterator& operator++()     { return *this; }
    ostreambuf_iterator& operator++(int)  { return *this; }
    bool failed() const __ntl_nothrow           { return 0 == sbuf_; }

  private:
    streambuf_type * sbuf_;
    bool             failed_;
};
/**@}*/

///\name 24.6.5 range access [iterator.range]
// NOTE: available when any of the containers header are included.

#ifdef NTL_CXX_AUTORET
  template <class C> inline auto begin(C& c)      -> decltype(c.begin())  { return c.begin(); }
  template <class C> inline auto begin(const C& c)-> decltype(c.begin())  { return c.begin(); }
  template <class C> inline auto end(C& c)        -> decltype(c.end())    { return c.end();   }
  template <class C> inline auto end(const C& c)  -> decltype(c.end())    { return c.end();   }
#else
  template <class C> inline typename C::iterator        begin(C& c)       { return c.begin(); }
  template <class C> inline typename C::const_iterator  begin(const C& c) { return c.begin(); }
  template <class C> inline typename C::iterator        end(C& c)         { return c.end();   }
  template <class C> inline typename C::const_iterator  end(const C& c)   { return c.end();   }
#endif

  template <class T, size_t N> inline T* begin  (T (&array)[N])           { return array;     }
  template <class T, size_t N> inline T* end    (T (&array)[N])           { return array+N;   }

///\}

/**@} lib_iterators */

}//namespace std

#endif//#ifndef NTL__STLX_ITERATOR
