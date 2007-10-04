/**\file*********************************************************************
 *                                                                     \brief
 *  Class template vector [23.2.4 lib.vector]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_VECTOR
#define NTL__STLX_VECTOR

#include "algorithm.hxx"
#include "iterator.hxx"
#include "memory.hxx"
//#include "stdexcept.hxx"
#include "type_traits.hxx"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127) // conditional expression is constant - clear()
#pragma warning(disable:4820) // X bytes padding added...
#pragma warning(disable:4710) // insert__blank_space(char * const,unsigned int)' : function not inlined
#endif

namespace std {

/**\defgroup  lib_containers *********** Containers library [23] ************
 *@{ *    Components to organize collections of information
 */

/**\defgroup  lib_sequence ************* Sequences [23.2] *******************
 *@{ *    Containers that organizes a finite set of objects,
 *    all of the same type, into a strictly linear arrangement.
 */

/// Class template vector [23.2.4]
template <class T, class Allocator = allocator<T> >
class vector
{
    template <class T, class traits, class Allocator> friend class basic_string;

  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef           T                           value_type;
    typedef           Allocator                   allocator_type;
    typedef typename  Allocator::pointer          pointer;
    typedef typename  Allocator::const_pointer    const_pointer;
    typedef typename  Allocator::reference        reference;
    typedef typename  Allocator::const_reference  const_reference;
    typedef typename  Allocator::size_type        size_type;
    typedef typename  Allocator::difference_type  difference_type;

    typedef pointer                               iterator;
    typedef const_pointer                         const_iterator;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  private:

    void construct(size_type n, const T& value)
    {
      iterator i = begin_;
      while ( n-- ) array_allocator.construct(i++, value);
      end_ = i;
    }
    
    template <class InputIterator>
    __forceinline
    void construct(InputIterator first, size_type n)
    {
      iterator i = begin_;
      for ( ; n--; ++first, ++i )
        array_allocator.construct(i, *first);
      end_ = i;
    }

    template <class ForwardIterator>
    void vector__disp(ForwardIterator first, ForwardIterator last,
                      const false_type&)
    {
      ///\todo specialize for InputIterator category
      // distance SHOULD be allways positive
      capacity_ = static_cast<size_type>(distance(first, last));
      begin_ = array_allocator.allocate(capacity_);
      construct(first, capacity_);
    }

    template <class IntegralType>
    void vector__disp(IntegralType n, IntegralType x, const true_type&)
    {
      capacity_ = static_cast<size_type>(n);
      begin_ = array_allocator.allocate(capacity_);
      construct(static_cast<size_type>(n), static_cast<value_type>(x));
    }

  public:

    ///\name construct/copy/destroy [23.2.4.1]

    explicit vector(const Allocator& a = Allocator())
    : array_allocator(a), begin_(0), end_(0), capacity_(0) {}

    explicit vector(      size_type n,
                    const T& value      = T(),
                    const Allocator& a  = Allocator())
    : array_allocator(a)
    {
      capacity_ = n;
      begin_= array_allocator.allocate(n);
      construct(n, value);
    }

    template <class InputIterator>
    vector(InputIterator first,
           InputIterator last,
           const Allocator& a = Allocator())
    : array_allocator(a)
    {
      vector__disp(first, last, is_integral<InputIterator>::type());
    }

    vector(const vector<T, Allocator>& x)
    : array_allocator(x.array_allocator)
    {
      capacity_ = x.size();
      if ( !capacity_ )
        begin_ = end_ = 0;
      else
      {
        begin_ = array_allocator.allocate(capacity_);
        construct(x.begin(), capacity_);
      }
    }

    __forceinline
    ~vector() throw()
    { 
      clear();
      if ( begin_ ) array_allocator.deallocate(begin_, capacity_);
    }

    vector<T, Allocator>& operator=(const vector<T, Allocator>& x)
    {
    //?  array_allocator = x.array_allocator;
      assign(x.begin(), x.end());
      return *this;
    }

    template <class InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
      assign__disp(first, last, is_integral<InputIterator>::type());
    }

    void assign(size_type n, const T& u)
    {
      clear();
      if ( capacity() < n )
      {
        array_allocator.deallocate(begin_, capacity_);
        capacity_ = n;
        begin_= array_allocator.allocate(n);
      }
      construct(n, u);
    }

    allocator_type get_allocator() const  { return array_allocator; }

    ///@}

  private:

    template <class InputIterator>
    void assign__disp(InputIterator first, InputIterator last,
                      const false_type&)
    {
      clear();
      // distance SHOULD be allways positive
      size_type n = static_cast<size_type>(std::distance(first, last));
      if ( capacity() < n )
      {
        array_allocator.deallocate(begin_, capacity_);
        capacity_ = n;
        begin_= array_allocator.allocate(n);
      }
      construct(first, n);
    }

    template <class IntegralType>
    void assign__disp(IntegralType n, IntegralType x, const true_type&)
    {
      assign(static_cast<size_type>(n), static_cast<value_type>(x));
    }

  public:

    ///\name  iterators

    iterator                begin()       { return begin_; }
    const_iterator          begin() const { return begin_; }
    iterator                end()         { return end_; }
    const_iterator          end()   const { return end_; }
    reverse_iterator        rbegin()      { return reverse_iterator(end_); }
    const_reverse_iterator  rbegin() const
      { return const_reverse_iterator(end_); }
    reverse_iterator        rend()       { return reverse_iterator(begin_); }
    const_reverse_iterator  rend() const 
      { return const_reverse_iterator(begin_); }

    ///\name  capacity [23.2.4.2]

    size_type size()      const { return static_cast<size_type>(end_- begin_); }
    size_type max_size()  const { return array_allocator.max_size(); }
    size_type capacity()  const { return capacity_; }
    bool      empty()     const { return begin_ == end_; }

    void resize(size_type sz, T c = T())
    {
      iterator new_end = begin_ + sz;
      while ( new_end < end_ ) pop_back();
      if    ( new_end > end_ ) insert(end_, new_end - end_, c);
    }

    void reserve(size_type n) throw(bad_alloc) //throw(length_error)
    { 
      if ( capacity() < n ) realloc(n);
    }

    ///\name  element access

    reference       operator[](size_type n)       { return *(begin_ + n); }
    const_reference operator[](size_type n) const { return *(begin_ + n); }

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

    reference       front()       throw() { return *begin(); }
    const_reference front() const throw() { return *begin(); }
    reference       back()        throw() { return *(end() - 1); }
    const_reference back()  const throw() { return *(end() - 1); }

    ///@}

  private:

    iterator insert__blank_space(const iterator position, const size_type n)
    {
      const size_type old_capacity = capacity_;
      iterator old_mem = 0;
      iterator new_end = end_ + n;
      // a hint to compiler - may be it'll eliminate  while ( tail_size-- ) ...
      difference_type tail_size = end_ - position;
      // realloc the first part if needed
      if ( capacity_ < end_- begin_ + n )
      {
        old_mem = begin_;
        capacity_ = n + capacity_factor();
        const iterator new_mem = array_allocator.allocate(capacity_);
        new_end += new_mem - old_mem;
        iterator dest = begin_ = new_mem;
        // this is safe for begin_ == 0 && end_ == 0, but keep vector() coherent
        for ( iterator src = old_mem; src != position; ++src, ++dest )
          move(dest, src);
      }
      // move the tail. iterators are reverse - may be no realloc
      iterator r_src = end();
      iterator r_dest = end_ = new_end;
      while ( tail_size-- )
        move(--r_dest, --r_src);      
      if ( old_mem ) array_allocator.deallocate(old_mem, old_capacity);
      return r_dest;
    }
    
    iterator insert__impl(iterator position, size_type n, const T& x)
    {
      iterator r_dest = insert__blank_space(position, n);
      while ( n-- ) array_allocator.construct(--r_dest, x);
      return r_dest;
    }

    template <class InputIterator>
    void insert__disp_it(iterator position, InputIterator first,
                         InputIterator last, const input_iterator_tag&)
    {
      while ( first != last ) position = insert__impl(position, 1, *first++) + 1;
    }

    template <class InputIterator>
   void insert__disp_it(iterator position, InputIterator first,
                         InputIterator last, const random_access_iterator_tag &)
    {
      position = insert__blank_space(position, static_cast<size_type>(last - first));
      while ( first != last ) array_allocator.construct(--position, *--last);
    }

    template <class InputIterator>
    void insert__disp(iterator position, InputIterator first, InputIterator last,
                      const false_type&)
    {
      insert__disp_it(position, first, last,
                      iterator_traits<InputIterator>::iterator_category());
    }

    template <class IntegralType>
    void insert__disp(iterator position, IntegralType n, IntegralType x,
                      const true_type&)
    {
      insert__impl(position, static_cast<size_type>(n),
                  static_cast<value_type>(x));
    }

  public:

    ///\name  modifiers [23.2.4.3]

    __forceinline
    void push_back(const T& x)
    {
      if ( size() == capacity() ) realloc(capacity_factor());
      array_allocator.construct(end_++, x);
    }

    void pop_back() throw() { array_allocator.destroy(--end_); }

    iterator insert(iterator position, const T& x)
    {
      return insert__impl(position, 1, x);
    }

    void insert(iterator position, size_type n, const T& x)
    {
      insert__impl(position, n, x);
    }

    template <class InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last)
    {
      insert__disp(position, first, last, is_integral<InputIterator>::type());
    }

    iterator erase(iterator position) throw()
    { 
      return erase(position, position + 1);
    }

    iterator erase(iterator first, iterator last) throw()
    {
      for ( iterator i = last; i != first;  ) array_allocator.destroy(--i);
      iterator const tail = first;
      for ( ; last != end_; ++first, ++last ) move(first, last);
      end_ = first;
      return tail;
    }

    void swap(vector<T, Allocator>& x) throw()
    {
      std::swap(begin_, x.begin_);
      std::swap(end_, x.end_);
      std::swap(capacity_, x.capacity_);
    }

    void clear() throw()
    {
      difference_type n = end_ - begin_;
      end_ = begin_;
      while ( n ) array_allocator.destroy(begin_ + --n);
    }

    ///@}
    
  ///////////////////////////////////////////////////////////////////////////
  private:

  #if 0
  // non-standard extension
  friend
    const vector<T, Allocator>
      make_vector(pointer first, pointer last, const Allocator& a = Allocator())
    {
      return const vector(first, last, true, a);
    }

  friend
    const vector<T, Allocator>
      make_vector(const_pointer first, const_pointer last,
                  const Allocator& a = Allocator())
    {
      return const vector(first, last, true, a);
    }

    vector(pointer first, size_type n, bool, const Allocator& a = Allocator())
    : array_allocator(a), begin_(first), end_(last), capacity(last - first) {}
  //end extension
  #endif

    pointer         begin_;
    pointer         end_;
    size_type       capacity_;

    mutable allocator_type  array_allocator;

    // "stdexcept.hxx" includes this header
    // hack: MSVC doesn't look inside function body
    void check_bounds(size_type n) const throw(bad_alloc)//throw(out_of_range)
    {
      if ( n > size() ) do__throw(out_of_range);
    }

    void move(const iterator to, const iterator from) const
    {
      array_allocator.construct(to, *from);
      array_allocator.destroy(from);
    }

    void realloc(size_type n) throw(bad_alloc)
    {
      const iterator new_mem = array_allocator.allocate(n);
      const size_type old_capacity = capacity_;
      capacity_ = n;
      iterator dest = new_mem;
      // this is safe for begin_ == 0 && end_ == 0, but keep vector() coherent
      for ( iterator src = begin_; src != end_; ++src, ++dest )
        move(dest, src);
      if ( begin_ ) array_allocator.deallocate(begin_, old_capacity);
      begin_ = new_mem;
      end_ = dest;
    }

    //  + 8/2 serves two purposes:
    //    capacity_ may be 0;
    //    for smal capacity_ values reallocation will be more efficient
    //      2,  4,  8,  16,  32,  64, 128, 256, 512
    //      8, 24, 56, 120, 248, 504, 
    size_type capacity_factor() const { return (capacity_ + 4) * 2; }

};//class vector

///\name  Vector comparisons

template <class T, class Allocator>
inline
bool operator==(const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  throw()
{
  return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T, class Allocator>
inline
bool operator< (const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  throw()
{
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <class T, class Allocator>
inline
bool operator!=(const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  throw()
{
  return ! (x == y);
}

template <class T, class Allocator>
inline
bool operator> (const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  throw()
{
  return y < x;
}

template <class T, class Allocator>
inline
bool operator>=(const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  throw()
{
  return ! (x < y);
}

template <class T, class Allocator>
inline
bool operator<=(const vector<T, Allocator>& x, const vector<T, Allocator>& y)
  throw()
{
  return ! (y < x);
}

///\name  Vector specialized algorithms
template <class T, class Allocator>
inline
void swap(vector<T, Allocator>& x, vector<T, Allocator>& y) throw()
{
  x.swap(y);
}

///@}
/**@} lib_sequence */
/**@} lib_containers */

}//namespace std

#ifdef _MSC_VER
#pragma warning(pop)//disable:4820
#endif

#endif//#ifndef NTL__STLX_VECTOR
