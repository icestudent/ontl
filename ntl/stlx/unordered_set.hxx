/**\file*********************************************************************
 *                                                                     \brief
 *  Class template unordered_set [unord.map]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_UNORDERED_SET
#define NTL__STLX_UNORDERED_SET

#include "stdexcept.hxx"
#include "functional.hxx"
#include "memory.hxx"
#include "utility.hxx"
#include "ext/hashtable.hxx"

namespace std {

/**\defgroup  lib_containers *********** 23 Containers library [containers] ************
  *@{    Components to organize collections of information
  **/

/**\defgroup  lib_unord **************** 23.4 Unordered associative containers [unord]
  *@{    Provide an ability for fast retrieval of data based on keys.
  **/

  // 23.4.3, class template unordered_set:
  template <class Value,
            class Hash = hash<Value>,
            class Pred = std::equal_to<Value>,
            class Alloc = std::allocator<Value> >
  class unordered_set;

  // 23.4.4, class template unordered_multiset:
  template <class Value,
            class Hash = hash<Value>,
            class Pred = std::equal_to<Value>,
            class Alloc = std::allocator<Value> >
  class unordered_multiset;


  /**
   *	@brief 23.4.3 Class template unordered_set [unord.set]
   *
   *  An unordered_set is an unordered associative container that supports unique keys
   *  (an unordered_set contains at most one of each key value) and in which the elements' keys are the elements themselves.
   **/
  template <class Value, class Hash, class Pred, class Allocator>
  class unordered_set:
    public std::ext::hashtable::chained_hashtable<Value,Value,Hash,Pred,Allocator, false, true>
  {
    typedef std::ext::hashtable::chained_hashtable<Value,Value,Hash,Pred,Allocator, false, true> base;
  public:

    ///\name types
    typedef Value                   key_type;
    typedef Value                   value_type;

    typedef Hash                    hasher;
    typedef Pred                    key_equal;
    typedef Allocator               allocator_type;
#ifdef __DOXYGEN__
    typedef typename allocator_type::pointer          pointer;
    typedef typename allocator_type::const_pointer    const_pointer;
    typedef typename allocator_type::reference        reference;
    typedef typename allocator_type::const_reference  const_reference;
#endif

  public:
    ///\name construct/destroy/copy

    /** Constructs an empty unordered_set using the specified hash function, key equality function, and allocator, and using at least \c n buckets.
      If \c n is not provided, the number of buckets is implementation defined. 
      max_load_factor() returns 1.0. */
    explicit unordered_set(size_type n = initial_count, const hasher& hf = hasher(), const key_equal& eql = key_equal(), const allocator_type& a = allocator_type())
      :base(n,hf,eql,a)
    {}

    /** Constructs an empty unordered_set using the specified hash function, key equality function, and allocator, and using at least \c n buckets.
        Then inserts elements from the range <tt>[f, l)</tt>. max_load_factor() returns 1.0. */
    template <class InputIterator>
    unordered_set(InputIterator first, InputIterator last,
                  size_type n = initial_count, const hasher& hf = hasher(), const key_equal& eql = key_equal(), const allocator_type& a = allocator_type())
      :base(n,hf,eql,a)
    {
      insert(first, last);
    }

    /** Constructs a copy of unordered_set */
    unordered_set(const unordered_set& r)
      :base(static_cast<const base&>(r))
    {}

    /** Constructs an empty unordered_set which holds the specified %allocator and default hash and key equality functions */
    unordered_set(const Allocator& a)
      :base(initial_count, hasher(), key_equal(), a)
    {}

    /** Constructs a copy of unordered_set using the specified %allocator */
    unordered_set(const unordered_set& r, const Allocator& a)
      :base(r,a)
    {}

#ifdef NTL__CXX_RV
    /** Transfers the contents of unordered_set */
    unordered_set(unordered_set&& r)
      :base(forward<base>(r))
    {}

    unordered_set(unordered_set&& r, const Allocator& a);
    
    /** Transfers the contents of unordered_set */
    unordered_set& operator=(unordered_set&& r)
    {
      base::operator=(r);
      return *this;
    }
#endif
    /** Constructs an unordered_set using the specified initializer %list and provided hash and key equality functions */
    unordered_set(initializer_list<value_type> il,
                  size_type n = initial_count, const hasher& hf = hasher(), const key_equal& eql = key_equal(), const allocator_type& a = allocator_type())
      :base(n,hf,eql,a)
    {
      insert(il.begin(), il.end());
    }
    /** destructor */
    ~unordered_set()
    {}

    /** Copies elements from the \c r */
    unordered_set& operator=(const unordered_set& r)
    {
      base::operator=(r);
      return *this;
    }

    /** Copies elements from the initializer %list */
    unordered_set& operator=(initializer_list<value_type> il)
    {
      clear();
      insert(il.begin(), il.end());
      return *this;
    }
    
    /** Returns a copy of used %allocator */
    allocator_type get_allocator() const { return allocator_type(nalloc); }

#ifdef __DOXYGEN__
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
#ifdef NTL__CXX_VT
    template <class... Args> pair<iterator, bool> emplace(Args&&... args);
    template <class... Args> iterator emplace_hint(const_iterator position, Args&&... args);
#endif

#ifdef __DOXYGEN__
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
    void swap(unordered_set& r);

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
   *	@brief 23.4.4 Class template unordered_multiset [unord.multiset]
   *
   *  An unordered_multiset is an unordered associative container that supports equivalent keys
   *  (an unordered_multiset may contain multiple copies of the same key value) and in which each element's key is the element itself.
   **/
  template <class Value, class Hash, class Pred, class Allocator>
  class unordered_multiset:
    public std::ext::hashtable::chained_hashtable<Value,Value,Hash,Pred,Allocator, false, false>
  {
    typedef std::ext::hashtable::chained_hashtable<Value,Value,Hash,Pred,Allocator, false, false> base;
  public:

    ///\name types
    typedef Value                   key_type;
    typedef Value                   value_type;

    typedef Hash                    hasher;
    typedef Pred                    key_equal;
    typedef Allocator               allocator_type;
#ifdef __DOXYGEN__
    typedef typename allocator_type::pointer          pointer;
    typedef typename allocator_type::const_pointer    const_pointer;
    typedef typename allocator_type::reference        reference;
    typedef typename allocator_type::const_reference  const_reference;
#endif

  public:
    ///\name construct/destroy/copy

    /** Constructs an empty unordered_multiset using the specified hash function, key equality function, and allocator, and using at least \c n buckets.
      If \c n is not provided, the number of buckets is implementation defined. 
      max_load_factor() returns 1.0. */
    explicit unordered_multiset(size_type n = initial_count, const hasher& hf = hasher(), const key_equal& eql = key_equal(), const allocator_type& a = allocator_type())
      :base(n,hf,eql,a)
    {}

    /** Constructs an empty unordered_multiset using the specified hash function, key equality function, and allocator, and using at least \c n buckets.
        Then inserts elements from the range <tt>[f, l)</tt>. max_load_factor() returns 1.0. */
    template <class InputIterator>
    unordered_multiset(InputIterator first, InputIterator last,
                  size_type n = initial_count, const hasher& hf = hasher(), const key_equal& eql = key_equal(), const allocator_type& a = allocator_type())
      :base(n,hf,eql,a)
    {
      insert(first, last);
    }

    /** Constructs a copy of unordered_multiset */
    unordered_multiset(const unordered_multiset& r)
      :base(static_cast<const base&>(r))
    {}

    /** Constructs an empty unordered_multiset which holds the specified %allocator and default hash and key equality functions */
    unordered_multiset(const Allocator& a)
      :base(initial_count, hasher(), key_equal(), a)
    {}

    /** Constructs a copy of unordered_multiset using the specified %allocator */
    unordered_multiset(const unordered_multiset& r, const Allocator& a)
      :base(r,a)
    {}

#ifdef NTL__CXX_RV
    /** Transfers the contents of unordered_multiset */
    unordered_multiset(unordered_multiset&& r)
      :base(forward<base>(r))
    {}

    unordered_multiset(unordered_multiset&& r, const Allocator& a);
    
    /** Transfers the contents of unordered_multiset */
    unordered_multiset& operator=(unordered_multiset&& r)
    {
      base::operator=(r);
      return *this;
    }
#endif
    /** Constructs an unordered_multiset using the specified initializer %list and provided hash and key equality functions */
    unordered_multiset(initializer_list<value_type> il,
                  size_type n = initial_count, const hasher& hf = hasher(), const key_equal& eql = key_equal(), const allocator_type& a = allocator_type())
      :base(n,hf,eql,a)
    {
      insert(il.begin(), il.end());
    }
    /** destructor */
    ~unordered_multiset()
    {}

    /** Copies elements from the \c r */
    unordered_multiset& operator=(const unordered_multiset& r)
    {
      base::operator=(r);
      return *this;
    }

    /** Copies elements from the initializer %list */
    unordered_multiset& operator=(initializer_list<value_type> il)
    {
      clear();
      insert(il.begin(), il.end());
      return *this;
    }
    
    /** Returns a copy of used %allocator */
    allocator_type get_allocator() const { return allocator_type(nalloc); }

#ifdef __DOXYGEN__
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
#ifdef NTL__CXX_VT
    template <class... Args> pair<iterator, bool> emplace(Args&&... args);
    template <class... Args> iterator emplace_hint(const_iterator position, Args&&... args);
#endif

#ifdef __DOXYGEN__
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
    void swap(unordered_multiset& r);

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

  template <class Value, class Hash, class Pred, class Alloc>
  inline void swap(unordered_set<Value, Hash, Pred, Alloc>& x, unordered_set<Value, Hash, Pred, Alloc>& y) { x.swap(y); }

  template <class Value, class Hash, class Pred, class Alloc>
  inline void swap(unordered_multiset<Value, Hash, Pred, Alloc>& x, unordered_multiset<Value, Hash, Pred, Alloc>& y) { x.swap(y); }

  /**@} lib_unord */
  /**@} lib_containers */
}//namespace std
#endif // NTL__STLX_UNORDERED_SET
