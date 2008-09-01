/**\file*********************************************************************
 *                                                                     \brief
 *  Class template map [lib.map]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_MAP
#define NTL__STLX_MAP

#include "stdexcept.hxx"
#include "functional.hxx"
#include "memory.hxx"
#include "utility.hxx"
#include "rbtree.hxx"

namespace std {

/**\defgroup  lib_containers *********** Containers library [23] ************
  *@{ *    Components to organize collections of information
  **/

/**\defgroup  lib_associative ********** Associative containers [23.3] ******
  *@{ *    Provide an ability for fast retrieval of data based on keys.
  **/

/// [23.3.1 lib.map]
/// supports unique keys (contains at most one of each key value)
  template
    <
    class Key,
    class T,
    class Compare,
    class Allocator
    >
  class map;

  namespace detail
  {
    template <class Key,
              class T,
              class Compare,
              class Allocator
              >
    class value_compare:
      public binary_function<pair<const Key, T>, pair<const Key, T>, bool>
    {
    public:
      typedef pair<const Key, T> value_type;

      value_compare(const value_compare& x)
        :comp(x.comp)
      {}

      __forceinline
      bool operator()(const value_type& x, const value_type& y) const
      {
        return comp(x.first, y.first);
      }


    friend class std::map<Key, T, Compare, Allocator>;
    protected:
      Compare comp;
      value_compare(Compare c) : comp(c) {}
      value_compare();
      value_compare& operator=(const value_compare&);
    };
  } // detail

template <class Key,
          class T,
          class Compare = less<Key>,
          class Allocator = allocator<pair<const Key, T> > >
class map:
  protected tree::rb_tree::rb_tree<pair<const Key, T>, detail::value_compare<Key, T, Compare, Allocator>, Allocator>
{
  ///////////////////////////////////////////////////////////////////////////
  typedef tree::rb_tree::rb_tree<pair<const Key, T>, detail::value_compare<Key, T, Compare, Allocator>, Allocator> tree_type;
  typedef tree_type::node node;
  public:

    ///\name  types
    typedef Key                                   key_type;
    typedef T                                     mapped_type;
    typedef pair<const Key, T>                    value_type;
    typedef Compare                               key_compare;

    typedef detail::value_compare
      <Key, T, Compare, Allocator>                value_compare;

    typedef Allocator                             allocator_type;
    typedef typename  Allocator::reference        reference;
    typedef typename  Allocator::const_reference  const_reference;

    typedef typename tree_type::iterator          iterator;
    typedef typename tree_type::const_iterator    const_iterator;

    typedef typename tree_type::reverse_iterator        reverse_iterator;
    typedef typename tree_type::const_reverse_iterator  const_reverse_iterator;

    typedef typename  Allocator::size_type        size_type;
    typedef typename  Allocator::difference_type  difference_type;
    typedef typename  Allocator::pointer          pointer;
    typedef typename  Allocator::const_pointer    const_pointer;


    ///\name 23.3.1.1 construct/copy/destroy:
    explicit map(const Compare& comp = Compare(), const Allocator& a = Allocator())
      :val_comp_(comp), tree_type(val_comp_, a)
    {}

    template <class InputIterator>
    map(InputIterator   first,
        InputIterator   last,
        const Compare&  comp = Compare(),
        const Allocator& a = Allocator())
        :val_comp_(comp), tree_type(val_comp_, a)
    {
      insert(first, last);
    }

    map(const map<Key, T, Compare, Allocator> & x)
      :val_comp_(x.val_comp_), tree_type(val_comp_, x.get_allocator())
    {}

#ifdef NTL__CXX
    map(map<Key,T,Compare,Allocator>&& x);
#endif
    map(const Allocator&);
    map(const map&, const Allocator&);
#ifdef NTL__CXX
    map(map&&, const Allocator&);
#endif

    ~map(){}

    map<Key, T, Compare, Allocator>& operator=(const map<Key, T, Compare, Allocator> & x)
    {
      if(this != &x)
        val_comp_ = x.val_comp_;
      return *this;
    }
#ifdef NTL__CXX
    map<Key,T,Compare,Allocator>& operator=(map<Key,T,Compare,Allocator>&& x);
#endif

    using tree_type::get_allocator;
    using tree_type::begin;
    using tree_type::rbegin;
    using tree_type::cbegin;
    using tree_type::crbegin;
    using tree_type::end;
    using tree_type::rend;
    using tree_type::cend;
    using tree_type::crend;

    using tree_type::empty;
    using tree_type::size;
    using tree_type::max_size;

    using tree_type::erase;
    using tree_type::clear;
    using tree_type::insert;

    // 23.3.1.2 element access:
    T& operator[](const key_type& x)
    {
      iterator iter = find(x);
      if(iter == end())
        iter = insert(value_type(x, mapped_type())).first;
      return iter->second;
    }

#ifdef NTL__CXX
    T& operator[](key_type&& x);
#endif

    T& at(const key_type& x) __ntl_throws(out_of_range)
    {
      iterator iter = find(x);
      if(iter == end())
        __ntl_throw(out_of_range(__FUNCTION__));
      return iter->second;
    }

    const T& at(const key_type& x) const __ntl_throws(out_of_range)
    {
      const_iterator iter = find(x);
      if(iter == end())
        __ntl_throw(out_of_range(__FUNCTION__));
      return iter->second;
    }

    // modifiers:
#ifdef NTL__CXX
    template <class... Args> pair<iterator, bool> emplace(Args&&... args);
    template <class... Args> iterator emplace(const_iterator position, Args&&... args);
    template <class P> pair<iterator, bool> insert(P&& x);
    template <class P>
    iterator insert(const_iterator position, P&&);
#endif
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last)
    {
      while(first != last){
        insert(*first);
        ++first;
      }
    }

    size_type erase(const key_type& x)
    {
      if(empty())
        return 0;
      iterator val = find(x);
      return val == end() ? 0 : (erase(val), 1);
    }

#ifdef NTL__CXX
    void swap(map<Key,T,Compare,Allocator>&&);
#else
    void swap(map<Key,T,Compare,Allocator>& x)
    {
      tree_type::swap(x);
      swap(val_comp_, x.val_comp_);
    }
#endif

    // observers:
    key_compare key_comp() const { return val_comp_.comp; }
    value_compare value_comp() const { return val_comp_; }

    // 23.3.1.3 map operations:
    iterator find(const key_type& x)
    {
      node* p = tree_type::root_;
      while(p){
        if(val_comp_.comp(x, p->elem.first))
          p = p->u.s.left;
        else if(val_comp_.comp(p->elem.first, x))
          p = p->u.s.right;
        else
          return tree_type::make_iterator(p);
      }
      return end();
    }

    const_iterator find(const key_type& x) const
    {
      return const_cast<map*>(this)->find(x);
    }

    size_type count(const key_type& x) const
    {
      return find(x) != end() ? 1 : 0;
    }

    iterator        lower_bound(const key_type& x)        { return equal_range(x)->second; }
    const_iterator  lower_bound(const key_type& x) const  { return equal_range(x)->second; }
    iterator        upper_bound(const key_type& x)        { return equal_range(x)->first;  }
    const_iterator  upper_bound(const key_type& x) const  { return equal_range(x)->first;  }

    pair<iterator,iterator> equal_range(const key_type& x)
    {
      // find a node with value which are equal or nearest to the x
      node* p = root_;
      while(p){
        if(val_comp_(x, p->elem)){
          if(p->left){
            p = p->left;
          }else{
            iterator re(p, this);
            return make_pair(re, re); // is a closest nodes
          }
        }else if(val_comp_(p->elem, x)) // greater
          p = p->right;
        else
          return make_pair(iterator(p, this), iterator(next(p), this));
      }
      iterator re(NULL, this);
      return make_pair(re, re);
    }

    pair<const_iterator,const_iterator> equal_range(const key_type& x) const
    {
      return equal_range(x);
    }

    friend bool operator==(const map<Key,T,Compare,Allocator>& x,
      const map<Key,T,Compare,Allocator>& y)
    {
      return static_cast<const tree_type&>(x) == static_cast<const tree_type&>(y);
    }

  private:
    value_compare val_comp_;

}; //class template map


#if 0
template <class Key, class T, class Compare, class Allocator>
bool operator==(const map<Key,T,Compare,Allocator>& x,
                const map<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
bool operator< (const map<Key,T,Compare,Allocator>& x,
                const map<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
bool operator!=(const map<Key,T,Compare,Allocator>& x,
                const map<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
bool operator> (const map<Key,T,Compare,Allocator>& x,
                const map<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
bool operator>=(const map<Key,T,Compare,Allocator>& x,
                const map<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
bool operator<=(const map<Key,T,Compare,Allocator>& x,
                const map<Key,T,Compare,Allocator>& y);
#endif

template <class Key, class T, class Compare, class Allocator>
void swap(map<Key,T,Compare,Allocator>& x,
          map<Key,T,Compare,Allocator>& y)
{
  x.swap(y);
}

#ifdef NTL__CXX
template <class Key, class T, class Compare, class Allocator>
void swap(map<Key,T,Compare,Allocator&& x, map<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
void swap(map<Key,T,Compare,Allocator& x, map<Key,T,Compare,Allocator>&& y);

#endif

template <class Key, class T, class Compare, class Alloc>
struct constructible_with_allocator_suffix< map<Key, T, Compare, Alloc> >
  :true_type
{};

/// [23.3.2 lib.multimap]
#if 0
template <class Key, class T, class Compare = less<Key>, class Allocator = allocator<pair<const Key, T> > >
class multimap;

template <class Key, class T, class Compare, class Allocator>
bool operator==(const multimap<Key,T,Compare,Allocator>& x,
                const multimap<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
bool operator< (const multimap<Key,T,Compare,Allocator>& x,
                const multimap<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
bool operator!=(const multimap<Key,T,Compare,Allocator>& x,
                const multimap<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
bool operator> (const multimap<Key,T,Compare,Allocator>& x,
                const multimap<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
bool operator>=(const multimap<Key,T,Compare,Allocator>& x,
                const multimap<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
bool operator<=(const multimap<Key,T,Compare,Allocator>& x,
                const multimap<Key,T,Compare,Allocator>& y);

template <class Key, class T, class Compare, class Allocator>
void swap(multimap<Key,T,Compare,Allocator>& x,
          multimap<Key,T,Compare,Allocator>& y);
#endif
///@}
/**@} lib_associative */
/**@} lib_containers */

}//namespace std

#endif//#ifndef NTL__STLX_MAP
