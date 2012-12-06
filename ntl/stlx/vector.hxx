/**\file*********************************************************************
 *                                                                     \brief
 *  Class template vector [23.2.6 lib.vector]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_VECTOR
#define NTL__STLX_VECTOR
#pragma once

#include "algorithm.hxx"
#include "memory.hxx"
#include "stdexcept_fwd.hxx"
#include "range.hxx"

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable:4127) // conditional expression is constant - clear()
# pragma warning(disable:4820) // X bytes padding added...
# pragma warning(disable:4710) // insert__blank_space(char * const,unsigned int)' : function not inlined
#endif

namespace std {

/**\defgroup  lib_containers *********** 23 Containers library [containers]
 *@{    Components to organize collections of information
 */

/**\defgroup  lib_sequence ************* 23.2 Sequence containers [sequences]
 *@{    Containers that organizes a finite %set of objects,
 *    all of the same type, into a strictly linear arrangement.
 */

/// Class template vector [23.2.6]
template <class T, class Allocator = allocator<T> >
class vector
{
    template <class, class, class> friend class basic_string;

    typedef typename
      Allocator::template rebind<T>::other        allocator;
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef           T                           value_type;
    typedef Allocator                             allocator_type;
    typedef typename  allocator::pointer          pointer;
    typedef typename  allocator::const_pointer    const_pointer;
    typedef       value_type&                     reference;
    typedef const value_type&                     const_reference;
    typedef typename  allocator::size_type        size_type;
    typedef typename  allocator::difference_type  difference_type;

    typedef pointer                               iterator;
    typedef const_pointer                         const_iterator;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    #ifndef __BCPLUSPLUS__
    static_assert((is_same<value_type, typename allocator::value_type>::value), "allocator::value_type must be the same as X::value_type");
    #endif
  private:

    void construct(size_type n, const T& value)
    {
      uninitialized_fill_n(begin_, n, value);
      end_ = begin_ + n;
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

    template <class InputIterator>
    void vector__disp(InputIterator first, InputIterator last, input_iterator_tag)
    {
      capacity_ = 0;
      begin_ = end_ = nullptr;
      while(first != last){
        push_back(*first);
        ++first;
      }
    }

    template <class ForwardIterator>
    void vector__disp(ForwardIterator first, ForwardIterator last, forward_iterator_tag)
    {
      // distance SHOULD be allways positive
      capacity_ = static_cast<size_type>(distance(first, last));
      begin_ = array_allocator.allocate(capacity_);
      construct(first, capacity_);
    }

  public:

    ///\name construct/copy/destroy [23.2.6.1]

    explicit vector(const Allocator& a = Allocator())
    : begin_(0), end_(0), capacity_(0), array_allocator(a) {}

    explicit vector(size_type n)
      :capacity_(n)
    {
      end_ = begin_ = array_allocator.allocate(n);
      // TODO: detect move constructor existance and split this implementation in two other (common and move)
#if !defined(NTL_CXX_RV) || defined(NTL_CXX_RVFIX)
      while(n--)
        array_allocator.construct(end_++, T());
#else
      while(n--)
        array_allocator.construct(end_++, forward<value_type>(T()));
#endif
    }

    vector(size_type n,
           const T& value,
           const Allocator& a  = Allocator())
    : array_allocator(a)
    {
      capacity_ = n;
      if(n){
        begin_= array_allocator.allocate(n);
        construct(n, value);
      }else{
        begin_ = end_ = nullptr;
      }
    }

    template <class InputIterator>
    vector(InputIterator first,
           InputIterator last,
           const Allocator& a = Allocator(), typename enable_if<!is_integral<InputIterator>::value>::type* =0)
    : array_allocator(a)
    {
      vector__disp(first, last, typename iterator_traits<InputIterator>::iterator_category());
    }

    vector(const vector& x)
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
    vector(const vector& x, const Allocator& a)
      :array_allocator(a)
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

    #ifdef NTL_CXX_RV
    vector(vector&& x)
      :begin_(), end_(), capacity_()
    {
      swap(x);
    }

    vector(vector&& x, const Allocator& a)
      :begin_(), end_(), capacity_(),
        array_allocator(a)
    {
      if(x.get_allocator() == a){
        swap(x);
      }else{
        // move elements using the array_allocator
        resize(x.size());
        std::move(x.begin(), x.end(), begin_);
        x.clear();
      }
    }

#ifdef NTL_STLX_RANGE
    ///\name Range extension
    template<class Iter>
    explicit vector(std::range<Iter>&& R)
      :begin_(), end_(), capacity_()
    {
      assign(forward<Range>(R));
    }
    template<class Iter>
    explicit vector(std::range<Iter>&& R, const Allocator& a)
      :begin_(), end_(), capacity_(),
      array_allocator(a)
    {
      assign(forward<Range>(R));
    }
    template<class Iter>
    vector& operator=(std::range<Iter>&& R)
    {
      assign(forward<Range>(R));
      return *this;
    }
    template<class Iter>
    void assign(std::range<Iter>&& R)
    {
      assign(__::ranged::adl_begin(R), __::ranged::adl_end(R));
    }
    template<class Iter>
    iterator insert(const_iterator position, std::range<Iter>&& R)
    {
      return insert(position, __::ranged::adl_begin(R), __::ranged::adl_end(R));
    }
    ///\}
#endif // NTL_STLX_RANGE
    #endif

    vector(initializer_list<T> il)
    {
      vector__disp(il.begin(), il.end(), forward_iterator_tag());
    }
    vector(initializer_list<T> il, const Allocator& a)
      :array_allocator(a)
    {
      vector__disp(il.begin(), il.end(), forward_iterator_tag());
    }

    __forceinline
    ~vector() __ntl_nothrow
    {
      if(begin_){
        clear();
        array_allocator.deallocate(begin_, capacity_);
      }
    }

    vector<T, Allocator>& operator=(const vector<T, Allocator>& x)
    {
      if(this != &x){
        // NOTE: Table 92 (page 677 of N2733) does not require this?
        array_allocator = x.array_allocator;
        assign(x.begin(), x.end());
      }
      return *this;
    }

    #ifdef NTL_CXX_RV
    vector<T,Allocator>& operator=(vector<T,Allocator>&& x)
    {
      if(this != &x){
        clear();
        swap(x);
      }
      return *this;
    }
    #endif

    vector& operator=(initializer_list<T> il)
    {
      assign(il);
      return *this;
    }

    template <class InputIterator>
    void assign(InputIterator first, InputIterator last, typename enable_if<!is_integral<InputIterator>::value>::type* =0)
    {
      assign__disp(first, last, typename iterator_traits<InputIterator>::iterator_category());
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

    void assign(initializer_list<T> il)
    {
      assign__disp(il.begin(), il.end(), forward_iterator_tag());
    }

    allocator_type get_allocator() const  { return static_cast<allocator_type>(array_allocator); }

    ///@}

  private:

    template <class InputIterator>
    void assign__disp(InputIterator first, InputIterator last, input_iterator_tag)
    {
      clear();
      while(first != last){
        push_back(*first);
        ++first;
      }
    }

    template <class InputIterator>
    void assign__disp(InputIterator first, InputIterator last, forward_iterator_tag)
    {
      clear();
      // distance SHOULD be allways positive
      size_type n = static_cast<size_type>(std::distance(first, last));
      if ( capacity() < n )
      {
        if(!empty())
          array_allocator.deallocate(begin_, capacity_);
        capacity_ = n;
        begin_= array_allocator.allocate(n);
      }
      construct(first, n);
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

    const_iterator          cbegin() const { return begin(); }
    const_iterator          cend()   const { return end(); }
    const_reverse_iterator  crbegin()const { return rbegin(); }
    const_reverse_iterator  crend()  const { return rend(); }

    ///\name  capacity [23.2.6.2]

    size_type size()      const { return static_cast<size_type>(end_- begin_); }
    size_type max_size()  const { return array_allocator.max_size(); }
    size_type capacity()  const { return capacity_; }
    bool      empty()     const { return begin_ == end_; }

    void resize(size_type sz)
    {
      if(sz > max_size())
        __throw_length_error(__func__": `sz` too large");
      iterator new_end = begin_ + sz;
      if(new_end < end_)
        erase(new_end, end_);
      else if(new_end > end_)
        insert__impl(end_, new_end - end_, T());
    }

    void resize(size_type sz, const T& c)
    {
      if(sz > max_size())
        __throw_length_error(__func__": `sz` too large");
      iterator new_end = begin_ + sz;
      if(new_end < end_)
        erase(new_end, end_);
      else if(new_end > end_)
        insert(end_, new_end - end_, c);
    }

    void reserve(size_type n) __ntl_throws(bad_alloc, length_error)
    {
      if(n > max_size())
        __throw_length_error(__func__": size too big");
      if ( capacity() < n ) realloc(n);
    }

    /** A non-binding request to reduce capacity() to size(). */
    void shrink_to_fit()
    {
      vector(*this).swap(*this);
    }

    ///\name  element access

    reference       operator[](size_type n)       { return *(begin_ + n); }
    const_reference operator[](size_type n) const { return *(begin_ + n); }

    const_reference at(size_type n) const __ntl_throws(out_of_range)
    {
      check_bounds(n);
      return operator[](n);
    }

    reference at(size_type n) __ntl_throws(out_of_range)
    {
      check_bounds(n);
      return operator[](n);
    }

    reference       front()       __ntl_nothrow { return *begin(); }
    const_reference front() const __ntl_nothrow { return *begin(); }
    reference       back()        __ntl_nothrow { return *(end() - 1); }
    const_reference back()  const __ntl_nothrow { return *(end() - 1); }

    ///\name data access [23.2.6.3]
    pointer       data()        __ntl_nothrow { return begin_; }
    const_pointer data() const  __ntl_nothrow { return begin_; }
    ///@}

  private:

    iterator insert__blank_space(const_iterator position, const size_type n)
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
        new_end = new_mem + difference_type(new_end - old_mem);
        //new_end += difference_type(new_mem - old_mem);        // dangerous alignment
        iterator dest = begin_ = new_mem;
        // this is safe for begin_ == 0 && end_ == 0, but keep vector() intact
        //#ifndef NTL_CXX_RV
        for ( iterator src = old_mem; src != position; ++src, ++dest )
          move(dest, src);
        //#else
        //for ( iterator src = old_mem; src != position; ++src, ++dest ){
        //  *dest = std::move(*src);
        //  array_allocator.destroy(src);
        //}
        //#endif
      }
      // move the tail. iterators are reverse - may be no realloc
      iterator r_src = end();
      iterator r_dest = end_ = new_end;
      while ( tail_size-- )
        move(--r_dest, --r_src);
      if ( old_mem ) array_allocator.deallocate(old_mem, old_capacity);
      return r_dest;
    }

    iterator insert__impl(const_iterator position, size_type n, const T& x)
    {
      iterator r_dest = insert__blank_space(position, n);
      //while ( n-- ) array_allocator.construct(--r_dest, x);
      r_dest -= n;
      uninitialized_fill_n(r_dest, n, x);
      return r_dest;
    }

    #ifdef NTL_CXX_RV
    iterator insert__impl(const_iterator position, size_type n, T&& x)
    {
      iterator r_dest = insert__blank_space(position, n);
      while ( n-- ) array_allocator.construct(--r_dest, std::move(x));
      return r_dest;
    }
    #endif

    template <class InputIterator>
    iterator insert__disp_it(iterator position, InputIterator first,
                         InputIterator last, const input_iterator_tag&)
    {
      while ( first != last ) position = insert__impl(position, 1, *first++) + 1;
      return position;
    }

    template <class InputIterator>
    iterator insert__disp_it(iterator position, InputIterator first,
                         InputIterator last, const random_access_iterator_tag &)
    {
      size_t const n = static_cast<size_type>(last - first);
      bool const insert_from_self = begin() <= &*first && &*first < end();
      iterator i = insert__blank_space(position, n);
      const difference_type disp = insert_from_self ? i - position - n : 0;
      position = i;
      while ( first != last ) array_allocator.construct(--position, (--last)[disp]);
      return i;
    }

    template <class InputIterator>
    iterator insert__disp(iterator position, InputIterator first, InputIterator last,
                      const false_type&)
    {
      return insert__disp_it(position, first, last,
                      iterator_traits<InputIterator>::iterator_category());
    }

    template <class IntegralType>
    iterator insert__disp(iterator position, IntegralType n, IntegralType x,
                      const true_type&)
    {
      return insert__impl(position, static_cast<size_type>(n),
                  static_cast<value_type>(x));
    }

  public:

    ///\name  modifiers [23.2.6.4]

    #ifdef NTL_CXX_VT
    template <class... Args> void emplace_back(Args&&... args)
    {
      emplace(end(), std::forward<Args>(args)...);
    }
    #endif

    #ifdef NTL_CXX_RV
    void push_back(T&& x)
    {
      if ( size() == capacity() ) realloc(capacity_factor());
      //*end_++ = move(x);
      array_allocator.construct(end_++, forward<value_type>(x));
    }
    #endif

    __forceinline
    void push_back(const T& x)
    {
      if ( size() == capacity() ) realloc(capacity_factor());
      array_allocator.construct(end_++, (x));
    }

    void pop_back() __ntl_nothrow { array_allocator.destroy(--end_); }

    #ifdef NTL_CXX_VT
    template <class... Args> iterator emplace(const_iterator position, Args&&... args)
    {
      iterator r_dest = insert__blank_space(position, 1);
      array_allocator.construct(--r_dest, std::forward<Args>(args)...);
      return r_dest;
    }
    #endif

    iterator insert(const_iterator position, const T& x)
    {
      return insert__impl(position, 1, x);
    }

    #ifdef NTL_CXX_RV
    iterator insert(const_iterator position, T&& x)
    {
      return insert__impl(position, 1, forward<value_type>((x)));
    }
    #endif

    iterator insert(const_iterator position, size_type n, const T& x)
    {
      return insert__impl(position, n, x);
    }

    template <class InputIterator>
    iterator insert(const_iterator position, InputIterator first, InputIterator last)
    {
      return insert__disp(&const_cast<value_type&>(*position), first, last, is_integral<InputIterator>::type());
    }

    iterator insert(const_iterator position, initializer_list<T> il)
    {
      return insert__disp(&const_cast<value_type&>(*position), il.begin(), il.end(), false_type());
    }

    __forceinline
    iterator erase(const_iterator position) __ntl_nothrow
    {
      // return erase(position, position + 1);
      iterator i = &const_cast<value_type&>(*position);
    #if 0
      array_allocator.destroy(i);
      --end_;
      do move(i, i + 1); while ( ++i != end_ );
    #else
      if(position + 1 != end()){
        #ifdef NTL_CXX_RV
          std::move
        #else
          std::copy
        #endif
        (i+1, end(), i);
      }
      array_allocator.destroy(--end_);
    #endif
      return i;
    }

    __forceinline
    iterator erase(const_iterator first, const_iterator last) __ntl_nothrow
    {
    #if 0
      for ( iterator i = last; i != first;  ) array_allocator.destroy(--i);
      iterator const tail = first;
      for ( ; last != end_; ++first, ++last ) move(first, last);
      end_ = first;
      return tail;
    #else
      // 1[000]
      // 1[000]2
      iterator first_ = &const_cast<value_type&>(*first), last_ = &const_cast<value_type&>(*last);
      if(first_ != last_){
        // destroy objects to be erased
        if ( !__::no_dtor<T>::value )
          for ( iterator i = last_; i != first_;  ) array_allocator.destroy(--i);

        #ifdef NTL_CXX_RV
        end_ = std::move(last_, end(), first_);
        #else
        end_ = std::copy(last_, end(), first_);
        #endif
      }
      return first_;
    #endif
    }

    void swap(vector<T, Allocator>& x) __ntl_nothrow
    {
      if(this != &x){
        using std::swap;
        swap(begin_, x.begin_);
        swap(end_, x.end_);
        swap(capacity_, x.capacity_);
      }
    }

    __forceinline
    void clear() __ntl_nothrow
    {
      difference_type n = end_ - begin_;
      end_ = begin_;
      if(n > 0 && !__::no_dtor<T>::value)
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

    pointer   begin_;
    pointer   end_;
    size_type capacity_;

    mutable allocator array_allocator;

    // following will broke compilation if T is incomplete type:
    //typedef __::bool_type<is_pod<T>::value || has_trivial_destructor<T>::value> no_dtor;

    void check_bounds(size_type n) const __ntl_throws(out_of_range)
    {
      if ( n >= size() ) __throw_out_of_range(__func__": no such element.");
    }

    void move(const iterator to, const iterator from) const
    {
      #ifndef NTL_CXX_RV
      array_allocator.construct(to, *from);
      #else
      array_allocator.construct(to, forward<value_type>(*from));
      #endif
      array_allocator.destroy(from);
    }

    void realloc(size_type n) __ntl_throws(bad_alloc)
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
inline bool operator==(const vector<T, Allocator>& x, const vector<T, Allocator>& y) __ntl_nothrow
{
  return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T, class Allocator>
inline bool operator< (const vector<T, Allocator>& x, const vector<T, Allocator>& y) __ntl_nothrow
{
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <class T, class Allocator>
inline bool operator!=(const vector<T, Allocator>& x, const vector<T, Allocator>& y) __ntl_nothrow
{
  return rel_ops::operator !=(x, y);
}

template <class T, class Allocator>
inline bool operator> (const vector<T, Allocator>& x, const vector<T, Allocator>& y) __ntl_nothrow
{
  return rel_ops::operator >(x, y);
}

template <class T, class Allocator>
inline bool operator>=(const vector<T, Allocator>& x, const vector<T, Allocator>& y) __ntl_nothrow
{
  return rel_ops::operator >=(x, y);
}

template <class T, class Allocator>
inline bool operator<=(const vector<T, Allocator>& x, const vector<T, Allocator>& y) __ntl_nothrow
{
  return rel_ops::operator <=(x, y);
}

///\name  Vector specialized algorithms
template <class T, class Allocator>
inline void swap(vector<T, Allocator>& x, vector<T, Allocator>& y) __ntl_nothrow { x.swap(y); }

///@}
/**@} lib_sequence */
/**@} lib_containers */

}//namespace std

#ifdef _MSC_VER
# pragma warning(pop)//disable:4820
#endif

#endif//#ifndef NTL__STLX_VECTOR
