/**\file*********************************************************************
 *                                                                     \brief
 *  Class template unordered_map [unord.map]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_UNORDERED_MAP
#define NTL__STLX_UNORDERED_MAP
#pragma once

#include "stdexcept_fwd.hxx"
#include "ext/hashtable.hxx"
#include "range.hxx"

namespace std {

/**\defgroup  lib_containers *********** 23 Containers library [containers]
  *@{    Components to organize collections of information
  **/

/**\defgroup  lib_unord **************** 23.4 Unordered associative containers [unord]
  *@{    Provide an ability for fast retrieval of data based on keys.
  **/

  // 23.4.1, class template unordered_map:
  template <class Key,
            class T,
            class Hash = hash<Key>,
            class Pred = std::equal_to<Key>,
            class Alloc = std::allocator<std::pair<const Key, T> >
            >
  class unordered_map;

  // 23.4.2, class template unordered_multimap:
  template <class Key,
            class T,
            class Hash = hash<Key>,
            class Pred = std::equal_to<Key>,
            class Alloc = std::allocator<std::pair<const Key, T> >
            >
  class unordered_multimap;
  

  /**
   *	@brief 23.4.1 Class template unordered_map [unord.map]
   *  
   *  An unordered_map is an unordered associative container that supports unique keys
   *  (an unordered_map contains at most one of each key value) and that associates values of another type \c mapped_type with the keys.
   **/
  template <class Key, class T, class Hash, class Pred, class Allocator>
  class unordered_map:
    public std::ext::hashtable::chained_hashtable<Key,T,Hash,Pred,Allocator, true, true>
  {
    typedef std::ext::hashtable::chained_hashtable<Key,T,Hash,Pred,Allocator, true, true> base;
  public:

    ///\name types
    typedef Key                     key_type;
    typedef std::pair<const Key, T> value_type;
    typedef T                       mapped_type;

    typedef Hash                    hasher;
    typedef Pred                    key_equal;
    typedef Allocator               allocator_type;
#ifdef NTL_DOC
    typedef typename allocator_type::pointer          pointer;
    typedef typename allocator_type::const_pointer    const_pointer;
    typedef typename allocator_type::reference        reference;
    typedef typename allocator_type::const_reference  const_reference;
#endif

  public:
    using base::insert;

    ///\name construct/destroy/copy

    /** Constructs an empty unordered_map using the specified hash function, key equality function, and allocator, and using at least \c n buckets.
      If \c n is not provided, the number of buckets is implementation defined. 
      max_load_factor() returns 1.0. */
    explicit unordered_map(size_type n = initial_count, const hasher& hf = hasher(), const key_equal& eql = key_equal(), const allocator_type& a = allocator_type())
      :base(n,hf,eql,a)
    {}

    /** Constructs an empty unordered_map using the specified hash function, key equality function, and allocator, and using at least \c n buckets.
        Then inserts elements from the range <tt>[f, l)</tt>. max_load_factor() returns 1.0. */
    template <class InputIterator>
    unordered_map(InputIterator first, InputIterator last,
                  size_type n = initial_count, const hasher& hf = hasher(), const key_equal& eql = key_equal(), const allocator_type& a = allocator_type())
      :base(n,hf,eql,a)
    {
      insert(first, last);
    }

    /** Constructs a copy of unordered_map */
    unordered_map(const unordered_map& r)
      :base(static_cast<const base&>(r))
    {}

    /** Constructs an empty unordered_map which holds the specified %allocator and default hash and key equality functions */
    unordered_map(const Allocator& a)
      :base(initial_count, hasher(), key_equal(), a)
    {}

    /** Constructs a copy of unordered_map using the specified %allocator */
    unordered_map(const unordered_map& r, const Allocator& a)
      :base(r,a)
    {}

#ifdef NTL_CXX_RV
    /** Transfers the contents of unordered_map */
    unordered_map(unordered_map&& r)
      :base(forward<base>(r))
    {}

    unordered_map(unordered_map&& r, const Allocator& a)
      :base(forward<base>(r), a)
    {}
    
    /** Transfers the contents of unordered_map */
    unordered_map& operator=(unordered_map&& r)
    {
      base::operator=(r);
      return *this;
    }

#ifdef NTL_STLX_RANGE
    ///\name Range extension
    template<class Iter>
    explicit unordered_map(std::range<Iter>&& R, size_type n = initial_count, const hasher& hf = hasher(), const key_equal& eql = key_equal(), const allocator_type& a = allocator_type())
      :base(n,hf,eql,a)
    {
      insert(forward<Range>(R));
    }
    template<class Iter>
    unordered_map& operator=(std::range<Iter>&& R)
    {
      clear();
      insert(forward<Range>(R));
      return *this;
    }
    template<class Iter>
    void insert(std::range<Iter>&& R)
    {
      insert(__::ranged::adl_begin(R), __::ranged::adl_end(R));
    }
    ///\}
#endif // NTL_STLX_RANGE
#endif
    /** Constructs an unordered_map using the specified initializer %list and provided hash and key equality functions */
    unordered_map(initializer_list<value_type> il,
                  size_type n = initial_count, const hasher& hf = hasher(), const key_equal& eql = key_equal(), const allocator_type& a = allocator_type())
      :base(n,hf,eql,a)
    {
      insert(il.begin(), il.end());
    }
    /** destructor */
    ~unordered_map()
    {}

    /** Copies elements from the \c r */
    unordered_map& operator=(const unordered_map& r)
    {
      base::operator=(r);
      return *this;
    }

    /** Copies elements from the initializer %list */
    unordered_map& operator=(initializer_list<value_type> il)
    {
      clear();
      insert(il.begin(), il.end());
      return *this;
    }
    
    /** Returns a copy of used %allocator */
    allocator_type get_allocator() const { return allocator_type(nalloc); }

#ifdef NTL_DOC
    ///\name size and capacity
    /** Returns \c true if container is empty */
    bool empty() const;
    /** Returns the number of stored elements */
    size_type size() const;
    /** Returns the maximum possible number of stored elements */
    size_type max_size() const;

    ///\name iterators
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
#endif

    ///\name modifiers
#ifdef NTL_CXX_VT
    template <class... Args> pair<iterator, bool> emplace(Args&&... args);
    template <class... Args> iterator emplace_hint(const_iterator position, Args&&... args);
#endif

#ifdef NTL_DOC
    /** Inserts value if container doesn't have element with the specified key.
        @return Pair of iterator, which points to the given element, and flag, which indicates whether the insertion takes place */
    std::pair<iterator, bool> insert(const value_type& obj);

    /** Inserts value if container doesn't have element with the specified key.
        @return iterator, which points to the inserted element */
    iterator insert(const_iterator hint, const value_type& obj);
    
    /** Inserts sequence of elements */
    template <class InputIterator> void insert(InputIterator first, InputIterator last);

    /** Inserts sequence of elements of initializer list */
    void insert(initializer_list<value_type>);

    /** Erases element in specified position. 
        @return position of following element if any */
    iterator erase(const_iterator position);

    /** Erases element with the given key.
        @return Number of erased elements (maximum 1) */
    size_type erase(const key_type& k);

    /** Erases all elements in range <tt>[first, last)</tt>
        @return position of following element if any */
    iterator erase(const_iterator first, const_iterator last);

    /** Erases all elements of container */
    void clear();

    /** Swap contents of container with \c r */
    void swap(unordered_map& r);

    ///\name observers

    /** Returns hash function */
    hasher hash_function() const;

    /** Returns key equality predicate */
    key_equal key_eq() const;

    ///\name lookup

    /** Returns an iterator pointing to an element with the given key */
    iterator find(const key_type& k);

    /** Returns an iterator pointing to an element with the given key */
    const_iterator find(const key_type& k) const;

    /** Returns the number of elements with the given key (maximum 1) */
    size_type count(const key_type& k) const;
    
    /** Returns a range containing all elements with keys equivalent to given (maximum 1) */
    std::pair<iterator, iterator> equal_range(const key_type& k);

    /** Returns a range containing all elements with keys equivalent to given (maximum 1) */
    std::pair<const_iterator, const_iterator> equal_range(const key_type& k) const;
#endif

    /** If the unordered_map does not already contain an element with the given key, inserts a default mapped value the value with the specified key */
    mapped_type& operator[](const key_type& k)
    {
      iterator i = find(k);
      if(i == end())
        i = insert(value_type(k, mapped_type())).first;
      return i->second;
    }

    /** Returns a reference to \c x.second, where \c x is the unique element whose key is equivalent to \c k. */
    mapped_type& at(const key_type& k) __ntl_throws(out_of_range)
    {
      iterator i = find(k);
      if(i == end())
        __throw_out_of_range("specified key isn't exists in the hash map");
      return i->second;
    }

    /** Returns a reference to \c x.second, where \c x is the unique element whose key is equivalent to \c k. */
    const mapped_type& at(const key_type& k) const __ntl_throws(out_of_range)
    {
      const_iterator i = find(k);
      if(i == end())
        __throw_out_of_range("specified key isn't exists in the hash map");
      return i->second;
    }

#ifdef NTL_DOC

    ///\name bucket interface

    /** Returns the number of buckets. */
    size_type bucket_count() const;
    /** Returns an upper bound on the number of buckets that container might ever contain. */
    size_type max_bucket_count() const;
    /** Returns the number of elements in the \c n<sup>th</sup> bucket. */
    size_type bucket_size(size_type n);
    /** Returns the index of the bucket in which elements with keys equivalent to the given would be found, if any such element existed. */
    size_type bucket(const key_type& k) const;

    /** Returns an iterator pointing to the first element of \c n<sup>th</sup> bucket. */
    local_iterator begin(size_type n);
    /** Returns an iterator pointing to the first element of \c n<sup>th</sup> bucket. */
    const_local_iterator begin(size_type n) const;
    /** Returns an iterator pointing to the last element of \c n<sup>th</sup> bucket. */
    local_iterator end(size_type n);
    /** Returns an iterator pointing to the last element of \c n<sup>th</sup> bucket. */
    const_local_iterator end(size_type n) const;
    
    ///\name hash policy

    /** Returns the average number of elements per bucket. */
    float load_factor() const;
    /** Returns a positive number that the container attempts to keep the load factor less than or equal to. 
        The container automatically increases the number of buckets as necessary to keep the load factor below this number. */
    float max_load_factor() const;
    /** Changes the container's maximum load load factor, using \c z as a hint. */
    void max_load_factor(float z);

    /** Invalidates hash table */
    void rehash(size_type n);
    ///\}
#endif
  };



  /**
   *	@brief 23.4.2 Class template unordered_multimap [unord.multimap]
   *
   *  An unordered_multimap is an unordered associative container that supports equivalent keys 
   *  (an unordered_multimap may contain multiple copies of each key value) and that associates values of another type \c mapped_type with the keys.
   **/
  template <class Key, class T, class Hash, class Pred, class Allocator>
  class unordered_multimap:
    public std::ext::hashtable::chained_hashtable<Key,T,Hash,Pred,Allocator, true, false>
  {
    typedef std::ext::hashtable::chained_hashtable<Key,T,Hash,Pred,Allocator, true, false> base;
  public:

    ///\name types
    typedef Key                     key_type;
    typedef std::pair<const Key, T> value_type;
    typedef T                       mapped_type;

    typedef Hash                    hasher;
    typedef Pred                    key_equal;
    typedef Allocator               allocator_type;
#ifdef NTL_DOC
    typedef typename allocator_type::pointer          pointer;
    typedef typename allocator_type::const_pointer    const_pointer;
    typedef typename allocator_type::reference        reference;
    typedef typename allocator_type::const_reference  const_reference;
#endif

  public:
    using base::insert;

    ///\name construct/destroy/copy

    /** Constructs an empty unordered_multimap using the specified hash function, key equality function, and allocator, and using at least \c n buckets.
      If \c n is not provided, the number of buckets is implementation defined. 
      max_load_factor() returns 1.0. */
    explicit unordered_multimap(size_type n = initial_count, const hasher& hf = hasher(), const key_equal& eql = key_equal(), const allocator_type& a = allocator_type())
      :base(n,hf,eql,a)
    {}

    /** Constructs an empty unordered_multimap using the specified hash function, key equality function, and allocator, and using at least \c n buckets.
        Then inserts elements from the range <tt>[f, l)</tt>. max_load_factor() returns 1.0. */
    template <class InputIterator>
    unordered_multimap(InputIterator first, InputIterator last,
                  size_type n = initial_count, const hasher& hf = hasher(), const key_equal& eql = key_equal(), const allocator_type& a = allocator_type())
      :base(n,hf,eql,a)
    {
      insert(first, last);
    }

    /** Constructs a copy of unordered_multimap */
    unordered_multimap(const unordered_multimap& r)
      :base(static_cast<const base&>(r))
    {}

    /** Constructs an empty unordered_multimap which holds the specified %allocator and default hash and key equality functions */
    unordered_multimap(const Allocator& a)
      :base(initial_count, hasher(), key_equal(), a)
    {}

    /** Constructs a copy of unordered_multimap using the specified %allocator */
    unordered_multimap(const unordered_multimap& r, const Allocator& a)
      :base(r,a)
    {}

#ifdef NTL_CXX_RV
    /** Transfers the contents of unordered_multimap */
    unordered_multimap(unordered_multimap&& r)
      :base(forward<base>(r))
    {}

    unordered_multimap(unordered_multimap&& r, const Allocator& a)
      :base(forward<base>(r), a)
    {}
    
    /** Transfers the contents of unordered_multimap */
    unordered_multimap& operator=(unordered_multimap&& r)
    {
      base::operator=(r);
      return *this;
    }

#ifdef NTL_STLX_RANGE
    ///\name Range extension
    template<class Iter>
    unordered_multimap(std::range<Iter>&& R, size_type n = initial_count, const hasher& hf = hasher(), const key_equal& eql = key_equal(), const allocator_type& a = allocator_type())
      :base(n,hf,eql,a)
    {
      insert(forward<Range>(R));
    }
    template<class Iter>
    unordered_multimap& operator=(std::range<Iter>&& R)
    {
      clear();
      insert(forward<Range>(R));
      return *this;
    }
    template<class Iter>
    void insert(std::range<Iter>&& R)
    {
      insert(__::ranged::adl_begin(R), __::ranged::adl_end(R));
    }
    ///\}
#endif // NTL_STLX_RANGE
#endif
    /** Constructs an unordered_multimap using the specified initializer %list and provided hash and key equality functions */
    unordered_multimap(initializer_list<value_type> il,
                  size_type n = initial_count, const hasher& hf = hasher(), const key_equal& eql = key_equal(), const allocator_type& a = allocator_type())
      :base(n,hf,eql,a)
    {
      insert(il.begin(), il.end());
    }
    /** destructor */
    ~unordered_multimap()
    {}

    /** Copies elements from the \c r */
    unordered_multimap& operator=(const unordered_multimap& r)
    {
      base::operator=(r);
      return *this;
    }

    /** Copies elements from the initializer %list */
    unordered_multimap& operator=(initializer_list<value_type> il)
    {
      clear();
      insert(il.begin(), il.end());
      return *this;
    }
    
    /** Returns a copy of used %allocator */
    allocator_type get_allocator() const { return allocator_type(nalloc); }

#ifdef NTL_DOC
    ///\name size and capacity
    /** Returns \c true if container is empty */
    bool empty() const;
    /** Returns the number of stored elements */
    size_type size() const;
    /** Returns the maximum possible number of stored elements */
    size_type max_size() const;

    ///\name iterators
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
#endif

    ///\name modifiers
#ifdef NTL_CXX_VT
    template <class... Args> pair<iterator, bool> emplace(Args&&... args);
    template <class... Args> iterator emplace_hint(const_iterator position, Args&&... args);
#endif

#ifdef NTL_DOC
    /** Inserts value if container doesn't have element with the specified key.
        @return Pair of iterator, which points to the given element, and flag, which indicates whether the insertion takes place */
    std::pair<iterator, bool> insert(const value_type& obj);

    /** Inserts value if container doesn't have element with the specified key.
        @return iterator, which points to the inserted element */
    iterator insert(const_iterator hint, const value_type& obj);
    
    /** Inserts sequence of elements */
    template <class InputIterator> void insert(InputIterator first, InputIterator last);

    /** Inserts sequence of elements of initializer list */
    void insert(initializer_list<value_type>);

    /** Erases element in specified position. 
        @return position of following element if any */
    iterator erase(const_iterator position);

    /** Erases element with the given key.
        @return Number of erased elements */
    size_type erase(const key_type& k);

    /** Erases all elements in range <tt>[first, last)</tt>
        @return position of following element if any */
    iterator erase(const_iterator first, const_iterator last);

    /** Erases all elements of container */
    void clear();

    /** Swap contents of container with \c r */
    void swap(unordered_multimap& r);

    ///\name observers

    /** Returns hash function */
    hasher hash_function() const;

    /** Returns key equality predicate */
    key_equal key_eq() const;

    ///\name lookup

    /** Returns an iterator pointing to an element with the given key */
    iterator find(const key_type& k);

    /** Returns an iterator pointing to an element with the given key */
    const_iterator find(const key_type& k) const;

    /** Returns the number of elements with the given key */
    size_type count(const key_type& k) const;
    
    /** Returns a range containing all elements with keys equivalent to given */
    std::pair<iterator, iterator> equal_range(const key_type& k);

    /** Returns a range containing all elements with keys equivalent to given */
    std::pair<const_iterator, const_iterator> equal_range(const key_type& k) const;

    ///\name bucket interface

    /** Returns the number of buckets. */
    size_type bucket_count() const;
    /** Returns an upper bound on the number of buckets that container might ever contain. */
    size_type max_bucket_count() const;
    /** Returns the number of elements in the \c n<sup>th</sup> bucket. */
    size_type bucket_size(size_type n);
    /** Returns the index of the bucket in which elements with keys equivalent to the given would be found, if any such element existed. */
    size_type bucket(const key_type& k) const;

    /** Returns an iterator pointing to the first element of \c n<sup>th</sup> bucket. */
    local_iterator begin(size_type n);
    /** Returns an iterator pointing to the first element of \c n<sup>th</sup> bucket. */
    const_local_iterator begin(size_type n) const;
    /** Returns an iterator pointing to the last element of \c n<sup>th</sup> bucket. */
    local_iterator end(size_type n);
    /** Returns an iterator pointing to the last element of \c n<sup>th</sup> bucket. */
    const_local_iterator end(size_type n) const;
    
    ///\name hash policy

    /** Returns the average number of elements per bucket. */
    float load_factor() const;
    /** Returns a positive number that the container attempts to keep the load factor less than or equal to. 
        The container automatically increases the number of buckets as necessary to keep the load factor below this number. */
    float max_load_factor() const;
    /** Changes the container's maximum load load factor, using \c z as a hint. */
    void max_load_factor(float z);

    /** Invalidates hash table */
    void rehash(size_type n);
    ///\}
#endif
  };

  template <class Key, class T, class Hash, class Pred, class Alloc>
  inline void swap(unordered_map<Key, T, Hash, Pred, Alloc>& x, unordered_map<Key, T, Hash, Pred, Alloc>& y) { x.swap(y); }

  template <class Key, class T, class Hash, class Pred, class Alloc>
  inline void swap(unordered_multimap<Key, T, Hash, Pred, Alloc>& x, unordered_multimap<Key, T, Hash, Pred, Alloc>& y) { x.swap(y); }

  /**@} lib_unord */
  /**@} lib_containers */

}//namespace std
#endif // NTL__STLX_UNORDERED_MAP
