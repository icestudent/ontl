/**\file*********************************************************************
 *                                                                     \brief
 *  Class template set [23.3.3 lib.list]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_SET
#define NTL__STLX_SET

#include "algorithm.hxx"
#include "iterator.hxx"
#include "memory.hxx"

#include "rbtree.hxx"

namespace std {

/**\addtogroup  lib_containers ********* Containers library [23] ************
 *@{*/

/**\addtogroup  lib_sequence *********** Assotiative [23.3] *******************
 *@{*/

  /// Class template set [23.3.3]
  template <class Key, class Compare = less<Key>, class Allocator = allocator<Key> >
  class set:
    public tree::rbtree::rbtree<Key, Compare, Allocator>
  {
    typedef tree::rbtree::rbtree<Key, Compare, Allocator> tree_type;
  public:
    // types:
    typedef Key key_type;
    typedef Key value_type;
    typedef Compare key_compare;
    typedef Compare value_compare;
    typedef Allocator allocator_type;
    typedef typename Allocator::reference reference;
    typedef typename Allocator::const_reference const_reference;
    typedef tree_type::iterator iterator;
    typedef tree_type::const_iterator const_iterator;
    typedef typename Allocator::size_type size_type;
    typedef typename Allocator::difference_type difference_type;
    typedef typename Allocator::pointer pointer;
    typedef typename Allocator::const_pointer const_pointer;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    // 23.3.3.1 construct/copy/destroy:
    explicit set(const Compare& comp = Compare(),
      const Allocator& a = Allocator())
      :tree_type(comp, a)
    {}
      
    template <class InputIterator>
    set(InputIterator first, InputIterator last,
      const Compare& comp = Compare(), const Allocator& a = Allocator())
      :tree_type(first, last, comp, a)
    {}

    set(const set<Key,Compare,Allocator>& x)
    {}

    //set(set<Key,Compare,Allocator>&& x);
    
    set(const Allocator&);
    
    set(const set& x, const Allocator& a)
    {}

    //set(set&&, const Allocator&);
    
    ~set()
    {}

    set<Key,Compare,Allocator>& operator= (const set<Key,Compare,Allocator>& x)
    {}

    //set<Key,Compare,Allocator>& operator=
    //  (set<Key,Compare,Allocator>&& x);

    //allocator_type get_allocator() const;

    // iterators:
    //iterator begin();
    //const_iterator begin() const;
    //iterator end();
    //const_iterator end() const;
    //reverse_iterator rbegin();
    //const_reverse_iterator rbegin() const;
    //reverse_iterator rend();
    //const_reverse_iterator rend() const;
    //const_iterator cbegin() const;
    //const_iterator cend() const;
    //const_reverse_iterator crbegin() const;
    //const_reverse_iterator crend() const;

    // capacity:
    //bool empty() const;
    //size_type size() const;
    //size_type max_size() const;

    // modifiers:
    //template <class... Args> pair<iterator, bool> emplace(Args&&... args);
    //template <class... Args> iterator emplace(const_iterator position, Args&&... args);
    //pair<iterator,bool> insert(const value_type& x);
    //pair<iterator,bool> insert(value_type&& x);
    //iterator insert(const_iterator position, const value_type& x);
    //iterator insert(const_iterator position, value_type&& x);
    //template <class InputIterator>
    //void insert(InputIterator first, InputIterator last);
    //iterator erase(const_iterator position);
    //size_type erase(const key_type& x);
    //iterator erase(const_iterator first, const_iterator last);
    //void swap(set<Key,Compare,Allocator>&);
    //void clear();

    // observers:
    key_compare key_comp() const { return tree_type::value_comp(); }
    value_compare value_comp() const;

    // set operations:
    //iterator find(const key_type& x);
    //const_iterator find(const key_type& x) const;
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
        if(elem_less(x, p->elem)){
          if(p->left){
            p = p->left;
          }else{
            iterator re(p, this);
            return make_pair(re, re); // is a closest nodes
          }
        }else if(elem_greater(x, p->elem))
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
  };

  //template <class Key, class Compare, class Allocator>
  //bool operator==(const set<Key,Compare,Allocator>& x,
  //  const set<Key,Compare,Allocator>& y);
  //template <class Key, class Compare, class Allocator>
  //bool operator< (const set<Key,Compare,Allocator>& x,
  //  const set<Key,Compare,Allocator>& y);
  //template <class Key, class Compare, class Allocator>
  //bool operator!=(const set<Key,Compare,Allocator>& x,
  //  const set<Key,Compare,Allocator>& y);
  //template <class Key, class Compare, class Allocator>
  //bool operator> (const set<Key,Compare,Allocator>& x,
  //  const set<Key,Compare,Allocator>& y);
  //template <class Key, class Compare, class Allocator>
  //bool operator>=(const set<Key,Compare,Allocator>& x,
  //  const set<Key,Compare,Allocator>& y);
  //template <class Key, class Compare, class Allocator>
  //bool operator<=(const set<Key,Compare,Allocator>& x,
  //  const set<Key,Compare,Allocator>& y);

  // specialized algorithms:
  template <class Key, class Compare, class Allocator>
  void swap(set<Key,Compare,Allocator>& x, set<Key,Compare,Allocator>& y)
  {
    x.swap(y);
  }

  //template <class Key, class Compare, class Allocator>
  //void swap(set<Key,Compare,Allocator&& x, set<Key,Compare,Allocator>& y);
  //template <class Key, class Compare, class Allocator>
  //void swap(set<Key,Compare,Allocator& x, set<Key,Compare,Allocator>&& y);

  template <class Key, class Compare, class Alloc>
  struct constructible_with_allocator_suffix<
    set<Key, Compare, Alloc> >
    : true_type { };

  ///////////////////////////////////////////////////////////////////////////

} //namespace std

#endif //#ifndef NTL__STLX_SET
