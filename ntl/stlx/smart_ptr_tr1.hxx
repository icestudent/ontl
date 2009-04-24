/**\file*********************************************************************
 *                                                                     \brief
 *  2.2 Smart pointers [tr.util.smartptr]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_SMARTPTRTR1
#define NTL__STLX_SMARTPTRTR1

namespace std
{
  /**\addtogroup  lib_utilities ********** 20 General utilities library [utilities]
  *@{*/
  /**\addtogroup  lib_memory *************** 20.7 Memory [memory] **********************
  *@{*/

  /// Draft Technical Report on C++ Library Extensions [N1836]
  namespace tr1
  {
    /**\defgroup lib_tr1                Technical Report 1 on C++ Library Extensions
     *@{ */
    /**\defgroup lib_tr1_util           2 General Utilities [tr.util]
     *@{ */
    /**\defgroup lib_tr1_util_smartptr  2.2 Smart pointers [tr.util.smartptr]
     *@{ */


    // [2.2.2] Class bad_weak_ptr
    /** An exception of type bad_weak_ptr is thrown by the shared_ptr constructor taking a weak_ptr. */
    class bad_weak_ptr:
      public std::exception
    {
    public:
      bad_weak_ptr()
      {}
      const char* what() __ntl_nothrow { return "tr1::bad_weak_ptr"; }
    };

    template<class T> class weak_ptr;

    // [2.2.3] Class template shared_ptr

    namespace __
    {
      template<class T>
      struct shared_ptr_data
      {
        T* p;
        long use_count, weak_count;

        explicit shared_ptr_data()
          :p(), use_count(0), weak_count(0)
        {}
        explicit shared_ptr_data(T* p)
          :p(p), use_count(1), weak_count(0)
        {}
        virtual ~shared_ptr_data() __ntl_nothrow {}
        virtual void free() __ntl_nothrow
        {
          delete p;
        }
        virtual const void* get_deleter(const type_info&) const __ntl_nothrow
        {
          return nullptr;
        }
      };

      template<class T, class D>
      struct shared_ptr_deleter
        : shared_ptr_data<T>
      {
        D deleter;

        shared_ptr_deleter(T* p, const D& d)__ntl_nothrow
          :shared_ptr_data(p), deleter(d)
        {}

        void free()__ntl_nothrow
        {
          deleter(p);
        }
        const void* get_deleter(const type_info& ti) const __ntl_nothrow
        {
          return typeid(D) == ti ? reinterpret_cast<const void*>(&deleter) : nullptr;
        }
      };

      struct shared_cast_static{};
      struct shared_cast_dynamic{};
      struct shared_cast_const{};
      template<class> struct check_shared;

      template<class T, class U>
      inline shared_ptr_data<T>* shared_data_cast(shared_ptr_data<U>* data)
      {
        return reinterpret_cast<shared_ptr_data<T>*>(data);
      }
    }

    /**
     *	@brief Class template shared_ptr [2.2.3 tr.util.smartptr.shared]
     *
     *  The shared_ptr class template stores a pointer, usually obtained via \c new. shared_ptr implements semantics of
     *  shared ownership; the last remaining owner of the pointer is responsible for destroying the object, or otherwise releasing
     *  the resources associated with the stored pointer.
     **/
    template<class T>
    class shared_ptr
    {
      typedef __::shared_ptr_data<T> shared_data;

      struct explicit_bool { int _; };
      typedef int explicit_bool::*  explicit_bool_type;

      template<class Y> friend class weak_ptr;
      template<class Y> friend class shared_ptr;
      template<class D, class T> 
      friend D* get_deleter(shared_ptr<T> const& p);

    protected:
      // *shared_ptr<void> protect
      typedef typename add_reference<typename conditional<is_void<T>::value,void*,T>::type>::type reference;

      template<class U>
      inline reference get_reference(false_type) const { return *ptr->p; }
      template<class U>
      inline reference get_reference(true_type)  const { return ptr->p;  }

      template<class Y>
      void check_shared(Y* p, const shared_ptr<T>* ptr);
    public:
      typedef T element_type;

      // [2.2.3.1] constructors
      shared_ptr()
        :ptr()
      {}

      template<class Y> explicit shared_ptr(Y* p)
      {
        ptr = new shared_data(p);
        check_shared(p, this);
      }

      template<class Y, class D> shared_ptr(Y* p, D d)
      {
        ptr = new __::shared_ptr_deleter<Y,D>(p, d);
      }

      shared_ptr(const shared_ptr& r) __ntl_nothrow
        :ptr(r.ptr)
      {
        add_ref();
      }
      template<class Y> shared_ptr(const shared_ptr<Y>& r) __ntl_nothrow
        :ptr(__::shared_data_cast<T>(r.ptr))
      {
        static_assert((is_convertible<Y*,T*>::value), "Y* shall be convertible to T*");
        add_ref();
      }

      template<class Y> explicit shared_ptr(const weak_ptr<Y>& r) __ntl_throws(bad_weak_ptr)
        :ptr()
      {
        static_assert((is_convertible<Y*,T*>::value), "Y* shall be convertible to T*");
        if(r.expired())
          __ntl_throw(bad_weak_ptr());

        ptr = __::shared_data_cast<T>(r.ptr);
        add_ref();
      }
      template<class Y> explicit shared_ptr(auto_ptr<Y>& r)__ntl_throws(bad_alloc)
        :ptr()
      {
        if(r.get()){
          ptr = new shared_data(r.get());
          r.release();
        }
      }
      template <class Y, class D>
      explicit shared_ptr(const unique_ptr<Y, D>& r)__ntl_throws(bad_alloc)
        :ptr()
      {
        if(r.get()){
          // currently unique_ptr's deleter always are reference
          ptr = new __::shared_ptr_deleter<Y,D>(r.get(), ref(r.get_deleter()));
          r.release();
        }
      }

      // [2.2.3.2] destructor
      ~shared_ptr()__ntl_nothrow
      {
        reset();
      }
      
      // [2.2.3.3] assignment
      shared_ptr& operator=(const shared_ptr& r)
      {
        if(this != &r){
          reset();
          ptr = r.ptr;
          add_ref();
        }
        return *this;
      }

      template<class Y> shared_ptr& operator=(const shared_ptr<Y>& r)
      {
        if(ptr != __::shared_data_cast<T>(r.ptr))
          shared_ptr(r).swap(*this);
        return *this;
      }
      template<class Y> shared_ptr& operator=(auto_ptr<Y>& r)
      {
        shared_ptr(r).swap(*this);
        return *this;
      }
      template <class Y, class D>
      shared_ptr& operator=(const unique_ptr<Y, D>& r)
      {
        shared_ptr(r).swap(*this);
        return *this;
      }
      
      // [2.2.3.4] modifiers
      void swap(shared_ptr& r)
      {
        std::swap(ptr, r.ptr);
      }

      void reset()
      {
        if(ptr){
          if(--ptr->use_count == 0)
            free();
          ptr = nullptr;
        }
      }

      template<class Y> void reset(Y* p)
      {
        reset();
        shared = new __::shared_ptr_data<Y>(p);
        check_shared(p, this);
      }
      
      template<class Y, class D> void reset(Y* p, D d)
      {
        reset();
        ptr = new __::shared_ptr_deleter<Y,D>(p, d);
      }
      
      // [2.2.3.5] observers
      T* get() const __ntl_nothrow
      {
        return ptr ? ptr->p : nullptr;
      }

      reference operator*() const __ntl_nothrow
      {
        //static_assert(is_void<T>::value == false, "illegal indirection");
        return get_reference<T>(is_void<T>());
      }

      T* operator->() const __ntl_nothrow
      {
        return ptr->p;
      }
      
      long use_count() const __ntl_nothrow
      {
        return ptr ? ptr->use_count : 0;
      }

      bool unique() const __ntl_nothrow
      {
        return ptr && ptr->use_count == 1;
      }

      operator explicit_bool_type() const __ntl_nothrow { return get() ? &explicit_bool::_ : 0;  }

    protected:
      template<class T, class U>
      friend bool operator<(shared_ptr<T> const& a, shared_ptr<U> const& b) __ntl_nothrow;

      template<class Y> shared_ptr(const shared_ptr<Y>& r, __::shared_cast_const) __ntl_nothrow
        :ptr(__::shared_data_cast<T>(r.ptr))
      {
        const_cast<T*>((Y*)0);
        add_ref();
      }
      template<class Y> shared_ptr(const shared_ptr<Y>& r, __::shared_cast_static) __ntl_nothrow
        :ptr(__::shared_data_cast<T>(r.ptr))
      {
        static_cast<T*>((Y*)0);
        add_ref();
      }
      template<class Y> shared_ptr(const shared_ptr<Y>& r, __::shared_cast_dynamic) __ntl_nothrow
        :ptr()
      {
        if(dynamic_cast<T*>(r.get())){
          ptr = __::shared_data_cast<T>(r.ptr);
          add_ref();
        }
      }
    protected:
      bool empty() const __ntl_nothrow { return !ptr; }
      void add_ref()
      {
        if(ptr)
          ++ptr->use_count;
      }
      void free()__ntl_nothrow
      {
        if(ptr && ptr->weak_count == 0){
          ptr->free();
          delete ptr;
          ptr = nullptr;
        }
      }
    private:
      shared_data* ptr;
    };

    // [2.2.3.6] shared_ptr comparisons
    template<class T, class U> 
    inline bool operator==(shared_ptr<T> const& a, shared_ptr<U> const& b) __ntl_nothrow
    {
      return a.get() == b.get();
    }
    template<class T, class U>
    inline bool operator!=(shared_ptr<T> const& a, shared_ptr<U> const& b) __ntl_nothrow
    {
      return a.get() != b.get();
    }
    template<class T, class U>
    inline bool operator<(shared_ptr<T> const& a, shared_ptr<U> const& b) __ntl_nothrow
    {
      return a.ptr < __::shared_data_cast<T>(b.ptr);
    }

    // [2.2.3.8] shared_ptr specialized algorithms
    template<class T> inline void swap(shared_ptr<T>& a, shared_ptr<T>& b) __ntl_nothrow
    {
      a.swap(b);
    }

    // [2.2.3.9] shared_ptr casts
    template<class T, class U>
    inline shared_ptr<T> static_pointer_cast(shared_ptr<U> const& r) __ntl_nothrow
    {
      static_cast<T*>((U*)0); // check
      return r.empty() ? shared_ptr<T>() : shared_ptr<T>(r, __::shared_cast_static());
    }
    template<class T, class U> 
    inline shared_ptr<T> dynamic_pointer_cast(shared_ptr<U> const& r) __ntl_nothrow
    {
      return r.empty() ? shared_ptr<T>() : shared_ptr<T>(r, __::shared_cast_dynamic());
    }
    template<class T, class U> 
    inline shared_ptr<T> const_pointer_cast(shared_ptr<U> const& r) __ntl_nothrow
    {
      const_cast<T*>((U*)0); // check
      return r.empty() ? shared_ptr<T>() : shared_ptr<T>(r, __::shared_cast_const());
    }

    // [2.2.3.7] shared_ptr I/O
    template<class E, class T, class Y>
    inline basic_ostream<E, T>& operator<< (basic_ostream<E, T>& os, shared_ptr<Y> const& p)
    {
      return os << p.get();
    }

    // [2.2.3.10] shared_ptr get_deleter
    template<class D, class T> 
    inline D* get_deleter(shared_ptr<T> const& p)
    {
      return !p.empty() ? reinterpret_cast<D*>(p.ptr->get_deleter(typeid(D))) : nullptr;
    }
    
    // [2.2.4] Class template weak_ptr
    /**
     *	@brief Class template weak_ptr [2.2.4 tr.util.smartptr.weak]
     *
     *  The weak_ptr class template stores a weak reference to an object that is already managed by a shared_ptr. To access
     *  the object, a weak_ptr can be converted to a shared_ptr using the member function lock.
     **/
    template<class T>
    class weak_ptr
    {
      typedef __::shared_ptr_data<T>* shared_data;

      template<class Y> friend class shared_ptr;
    public:
      typedef T element_type;
      // constructors
      weak_ptr() __ntl_nothrow
        :ptr()
      {}

      weak_ptr(weak_ptr const& r) __ntl_nothrow
        :ptr(r.ptr)
      {
        add_ref();
      }

      template<class Y> weak_ptr(weak_ptr<Y> const& r) __ntl_nothrow
        :ptr(__::shared_data_cast<T>(r.ptr))
      {
        add_ref();
      }

      template<class Y> weak_ptr(shared_ptr<Y> const& r) __ntl_nothrow
        :ptr(__::shared_data_cast<T>(r.ptr))
      {
        add_ref();
      }

      // destructor
      ~weak_ptr() __ntl_nothrow
      {
        reset();
      }

      // assignment
      weak_ptr& operator=(weak_ptr const& r) __ntl_nothrow
      {
        if(this != &r){
          reset();
          ptr = r.ptr;
        }
        return *this;
      }
      template<class Y> weak_ptr& operator=(weak_ptr<Y> const& r) __ntl_nothrow
      {
        if(ptr != __::shared_data_cast<T>(r.ptr)){
          reset();
          ptr = __::shared_data_cast<T>(r.ptr);
          add_ref();
        }
        return *this;
      }
      template<class Y> weak_ptr& operator=(shared_ptr<Y> const& r) __ntl_nothrow
      {
        if(ptr != __::shared_data_cast<T>(r.ptr)){
          reset();
          ptr = __::shared_data_cast<T>(r.ptr);
          add_ref();
        }
        return *this;
      }

      // modifiers
      void swap(weak_ptr& r) __ntl_nothrow
      {
        std::swap(ptr, r.ptr);
      }

      void reset() __ntl_nothrow
      {
        if(ptr){
          --ptr->weak_count;
          if(!ptr->use_count)
            ptr->free();
          ptr = nullptr;
        }
      }

      // observers
      long use_count() const __ntl_nothrow      { return ptr ? ptr->use_count : 0; }
      bool expired() const __ntl_nothrow        { return ptr && ptr->use_count == 0; }
      shared_ptr<T> lock() const __ntl_nothrow  { return expired() ? shared_ptr<T>() : shared_ptr<T>(*this); }

      // [2.2.4.6] weak_ptr comparison
      template<class U> 
      friend bool operator<(weak_ptr const& a, weak_ptr<U> const& b)
      {
        return ptr < reinterpret_cast<shared_data>(b.ptr);
      }


    protected:
      void add_ref()
      {
        if(ptr)
          ++ptr->weak_count;
      }
    private:
      shared_data ptr;
    };
    
    // [2.2.4.7] weak_ptr specialized algorithms
    template<class T> 
    inline void swap(weak_ptr<T>& a, weak_ptr<T>& b) __ntl_nothrow
    {
      return a.swap(b);
    }
    
    /**
     *	@brief [2.2.5] Class enable_shared_from_this
     *
     *  A class \c T can inherit from \c enable_shared_from_this<T> to inherit the \c shared_from_this member functions that
     *  obtain a shared_ptr instance pointing to \c *this.
     **/
    template<class T>
    class enable_shared_from_this
    {
      weak_ptr<T> weak_this;
      friend struct __::check_shared<T>;
    protected:
      enable_shared_from_this() __ntl_nothrow
      {}
      enable_shared_from_this(enable_shared_from_this const&) __ntl_nothrow
      {}
      enable_shared_from_this& operator=(enable_shared_from_this const&) __ntl_nothrow
      { return *this; }
      ~enable_shared_from_this() __ntl_nothrow
      {}
    public:
      /** Returns a shared_ptr<T> object that shares ownership with \c this. */
      shared_ptr<T> shared_from_this()
      {
        return shared_ptr<T>(weak_this);
      }
      shared_ptr<T const> shared_from_this() const
      {
        return shared_ptr<T const>(weak_this);
      }
    };

    namespace __
    {
      template<class T>
      struct check_shared
      {
        template<class>
        static inline void check(...) {}

        template<class Y>
        static inline void check(enable_shared_from_this<T>* p, const shared_ptr<T>& ptr)
        {
          p->weak_this = ptr;
        }
      };
    }

    template<class T>
    template<class Y>
    inline void shared_ptr<T>::check_shared(Y* p, const shared_ptr<T>* ptr)
    {
      if(p)
        __::check_shared<T>::check<Y>(p, *ptr);
    }

    /**@} lib_tr1_util_smartptr */
    /**@} lib_tr1_util */
    /**@} lib_tr1 */

  } // tr1

  /**@} lib_memory */
  /**@} lib_utilities */
} // std

#endif NTL__STLX_SMARTPTRTR1
