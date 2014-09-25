/**\file*********************************************************************
 *                                                                     \brief
 *  Basic I/O Services
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_IOSERVICE
#define NTL__STLX_TR2_IOSERVICE
#pragma once

#include "../../../atomic.hxx"
#include "io_service_fwd.hxx"
#include "../../../forward_list.hxx"
#include "../../../typeindex.hxx"
#include "../../../mutex.hxx"

namespace std { namespace tr2 { namespace sys {

  namespace iocp
  {
    class iocp_service;
  }

  class io_service;
  class strand_service;
  class invalid_service_owner;
  class service_already_exists;

  template<class Service> Service& use_service(io_service&);
  template<class Service> void add_service(io_service&, Service*);
  template<class Service> bool has_service(io_service&);

  ///\name default handler hook functions (note: handler passed as pointer to ...)
  inline void* io_handler_allocate(size_t s, ...)           { return ::operator new(s);    }
  inline void  io_handler_deallocate(void* p, size_t, ...)  { return ::operator delete(p); }

  template<class F> 
  inline void io_handler_invoke(F& f, ...) { f(); }

  template<class F> 
  inline void io_handler_invoke(F const& f, ...) { f(); }
  ///\}

  template<class IoObjectService>
  class basic_io_object;

  namespace __ { namespace sys
  {
    template<class Handler, typename A1>
    struct binder1
    {
      binder1(const Handler& handler, const A1& a1)
        :handler(handler), a1(a1)
      {}

      void operator()()
      {
        handler(static_cast<const A1&>(a1));
      }
      void operator()() const
      {
        handler(a1);
      }
    private:
      Handler handler;
      A1 a1;
    };

    template<class Handler, typename A1, typename A2>
    struct binder2
    {
      binder2(const Handler& handler, const A1& a1, const A2& a2)
        :handler(handler), a1(a1), a2(a2)
      {}

      void operator()()
      {
        handler(static_cast<const A1&>(a1), static_cast<const A2&>(a2));
      }
      void operator()() const
      {
        handler(a1, a2);
      }
    private:
      Handler handler;
      A1 a1;
      A2 a2;
    };
  } // __::sys

  template<class Handler, typename A1>
  inline sys::binder1<Handler, A1> bind_handler(const Handler& f, const A1& a1)
  {
    return sys::binder1<Handler, A1>(f, a1);
  }

  template<class Handler, typename A1, typename A2>
  inline sys::binder2<Handler, A1, A2> bind_handler(const Handler& f, const A1& a1, const A2& a2)
  {
    return sys::binder2<Handler, A1, A2>(f, a1, a2);
  }
} // __ ns

  ///\name service access exceptions
  class service_already_exists:
    public logic_error
  {
  public:
    service_already_exists()
      :logic_error("io service already exists")
    {}
  };
  class invalid_service_owner:
    public logic_error
  {
  public:
    invalid_service_owner()
      :logic_error("invalid io service owner")
    {}
  };
  ///\}



  /**
   *  @brief 5.3.3. Class io_service
   *  @details Class io_service implements an extensible, type-safe, polymorphic set of <i>IO services, indexed by service/type</i>.
   *  An object of class io_service must be initialised before I/O objects such as sockets, resolvers and timers can be used. These I/O objects 
   *  are distinguished by having constructors that accept an \c io_service& parameter.
   **/
  class io_service:
    noncopyable
  {
  public:
    // types:
    class service;
    class id;
    class work;
    class strand;

    /**
     *  @brief 5.3.5. Class io_service::id
     *  @details The class io_service::id provides identification of services, and is used as an index for service lookup.
     **/
    class id: noncopyable
    {
    public:
      id();
    };

    ///\name constructors/destructor:
    io_service();
    ~io_service();

    ///\name members:
    size_t run(error_code& ec = throws())
    {
      size_t n = 0;
      while(run_one(ec))
        ++n;
      return n;
    }
    
    size_t run_one(error_code& ec = throws());
    
    size_t poll(error_code& ec = throws())
    {
      size_t n = 0;
      while(poll_one(ec))
        ++n;
      return n;
    }
    
    size_t poll_one(error_code& ec = throws());

    void stop();
    void reset();
    
    template<class CompletionHandler>
    void dispatch(CompletionHandler handler);
    
    template<class CompletionHandler>
    void post(CompletionHandler handler);
    
    //template<class Handler>
    //unspecified wrap(Handler handler);
    ///\}
  protected:
    struct service_node
    {
      type_index  key;
      service*    val;
      io_service* owner;
      size_t      id;
    };
    typedef std::unique_lock<std::recursive_mutex> guard;
    typedef forward_list<service_node> services_t;
    typedef iocp::iocp_service service_implementation_type;
    
    mutable std::recursive_mutex lock;
    services_t services;
    size_t svcNo;

    service_implementation_type& impl;

    struct finder:
      unary_function<service_node, bool>
    {
      type_index key;
      finder(type_index key)
        :key(key)
      {}
      result_type operator()(const service_node& node) const
      {
        return node.key == key;
      }
    };
    template<class Service> friend Service& use_service(io_service&);
    template<class Service> friend void add_service(io_service&, Service*);
    template<class Service> friend bool has_service(io_service&);

    template<class Service> void do_add_service(Service* svc)
    {
      guard _(lock);
      type_index key(typeid(Service));
      services_t::const_iterator it = find_if(services.cbegin(), services.cend(), finder(key));
      if(it != services.cend()){
        __ntl_throw(service_already_exists());
        return;
      }
      service_node node = {key, svc, this, ++svcNo};
      services.push_front(node);
    }
    template<class Service> bool do_has_service() const
    {
      guard _(lock);
      type_index key(typeid(Service));
      services_t::const_iterator it = find_if(services.cbegin(), services.cend(), finder(key));
      return it != services.cend();
    }
    template<class Service> Service& do_use_service()
    {
      guard _(lock);
      type_index key(typeid(Service));
      services_t::const_iterator it = find_if(services.cbegin(), services.cend(), finder(key));
      if(it != services.cend()){
        if(it->owner != this)
          __ntl_throw(invalid_service_owner());
        return static_cast<Service&>(*it->val);
      }
      Service* svc = new Service(*this);
      service_node node = {key, svc, this, ++svcNo};
      services.push_front(node);
      return *svc;
    }

    
  private:
    io_service(const io_service&) __deleted;
    void operator=(const io_service&) __deleted;
  };



  /**
   *  Service objects may be explicitly added to an io_service using the function template add_service<Service>(). If the 
   *  Service is already present, the service_already_exists exception is thrown. If the owner of the service is not the same 
   *  object as the io_service parameter, the invalid_service_owner exception is thrown. 
   **/
   template<class Service> inline void add_service(io_service& ios, Service* svc) { ios.do_add_service<Service>(svc); }
   template<class Service> inline bool has_service(io_service& ios) { return ios.do_has_service<Service>(); }
   template<class Service> inline Service& use_service(io_service& ios) { return ios.do_use_service<Service>(); }


  /**
   *  @brief 5.3.4. Class io_service::service
   *  @details Base class for services
   **/
  class io_service::service:
    noncopyable
  {
    io_service& ios;

  public:
    io_service& get_io_service() { return ios; }

  protected:
    service(io_service& owner)
      :ios(owner)
    {}

    virtual ~service()
    {}

  private:
    /** A service's shutdown_service member function must cause all copies of user-defined handler objects that are held by the 
      service to be destroyed.  */
    virtual void shutdown_service() = 0;
    friend io_service::~io_service();

    service(const service&) __deleted;
    void operator=(const service&) __deleted;
  };


  //////////////////////////////////////////////////////////////////////////
  /**
   *  @brief 5.3.6 Class io_service::work
   *  @details An object of class io_service::work represents a unit of unfinished work for an io_service. 
   **/
  class io_service::work
  {
    service_implementation_type& ios;
  public:
    // constructors/destructor:
    explicit work(io_service& ios);
    work(const work& other);
    ~work(); // if last, stop ios

    // members:
    io_service& get_io_service();

  private:
    void operator=(const work&) __deleted;
  };


  /**
   *  @brief 5.3.7 Class io_service::strand
   *  @details An object of class io_service::strand may be used to prevent concurrent invocation of handlers.
   *
   *  A \e strand is defined as a strictly sequential invocation of event handlers (i.e. no concurrent invocation). Use of strands allows 
   *  execution of code in a multithreaded program without the need for explicit locking (e.g. using mutexes). 
   **/
  class io_service::strand:
    noncopyable
  {
    class strand_service;
    class strand_impl;
    strand_service& service;
    strand_impl* impl;
  public:
    // constructors/destructor:
    explicit strand(io_service& ios);
    ~strand();

    // members:
    io_service& get_io_service();

    template<class CompletionHandler>
    void dispatch(CompletionHandler handler);
    
    template<class CompletionHandler>
    void post(CompletionHandler handler);
    
    //template<class Handler>
    //unspecified wrap(Handler handler);
    
    bool running_in_this_thread() const;

  private:
    strand(const strand&) __deleted;
    void operator=(const strand&) __deleted;
  };

  } // sys
 } // tr2
} // std

#include "iocp/iocp_service.hxx"

//////////////////////////////////////////////////////////////////////////
namespace std { namespace tr2 { namespace sys {

  inline io_service::io_service()
    : svcNo(0)
    , impl(use_service<service_implementation_type>(*this))
  {}

  inline io_service::~io_service()
  {
    for(services_t::iterator it = services.begin(), end = services.end(); it != end; ++it, svcNo--) {
      assert(it->owner == this);
      assert(svcNo == it->id);
      it->val->shutdown_service();
      delete it->val;
    }
    assert(svcNo == 0);
  }

  inline size_t io_service::run_one(error_code& ec /* = throws() */)
  {
    error_code e;
    size_t re = impl.run_one(e);
    return throw_system_error(e, ec), re;
  }

  inline size_t io_service::poll_one(error_code& ec /* = throws() */)
  {
    error_code e;
    size_t re = impl.poll_one(e);
    return throw_system_error(e, ec), re;
  }

  inline void io_service::stop()
  {
    impl.stop();
  }

  inline void io_service::reset()
  {
    impl.reset();
  }

  template<class CompletionHandler>
  inline void io_service::dispatch(CompletionHandler handler)
  {
    impl.dispatch(std::forward<CompletionHandler>(handler));
  }

  template<class CompletionHandler>
  inline void io_service::post(CompletionHandler handler)
  {
    impl.post(std::forward<CompletionHandler>(handler));
  }


  //////////////////////////////////////////////////////////////////////////
  inline io_service::work::work(io_service& ios)
    : ios(use_service<service_implementation_type>(ios))
  {
    this->ios.work_started();
  }

  inline io_service::work::work(const work& r)
    : ios(r.ios)
  {
    this->ios.work_started();
  }

  inline io_service::work::~work()
  {
    this->ios.work_finished();
  }

  inline io_service& io_service::work::get_io_service()
  {
    return ios.get_io_service();
  }


  //////////////////////////////////////////////////////////////////////////
  class io_service::strand::strand_impl:
    public __::async_operation
  {
  public:
    strand_impl();

  //private:
    std::mutex mutex;
    std::vector<async_operation*> waiting, ready;
    bool locked;
  };

  class io_service::strand::strand_service:
    public io_service::service
  {
    typedef __::async_operation async_operation;
    typedef service_implementation_type iocp_service;

    typedef std::unique_lock<std::mutex> scoped_lock;
    static const size_t max_count = 193;
  public:
    typedef strand_impl* implementation_type;

    explicit strand_service(io_service& ios)
      : service(ios)
      , iocp(use_service<service_implementation_type>(ios))
      , salt()
      , self_id()
    {}

    void shutdown_service()
    {
      std::vector<async_operation*> ops;
      scoped_lock lock(mutex);
      for(size_t i = 0; i < max_count; i++) {
        if(strand_impl* impl = objects[i].get()) {
          ops.insert(ops.end(), impl->waiting.begin(), impl->waiting.end());
          ops.insert(ops.end(), impl->ready.begin(), impl->ready.end());
        }
      }
    }

    void construct(implementation_type& impl)
    {
      scoped_lock lock(mutex);
      size_t index = ++salt % max_count;

      if(!objects[index])
        objects[index].reset(new strand_impl());
      impl = objects[index].get();
    }

    template<class CompletionHandler>
    void dispatch(implementation_type& impl, CompletionHandler handler);

    template<class CompletionHandler>
    void post(implementation_type& impl, CompletionHandler handler)
    {
      typedef __::completion_operation<CompletionHandler> op;
      typename op::ptr p (handler);

      do_post(impl, p.op);

      p.release();
    }

    //template<class Handler>
    //unspecified wrap(Handler handler);

    bool running_in_this_thread(const implementation_type&) const
    {
      return ntl::nt::this_thread::id() == self_id;
    }

  protected:

    struct scoped_complete
    {
      strand_impl* impl;
      service_implementation_type* owner;
      strand_service* self;

      ~scoped_complete()
      {
        impl->mutex.lock();
        impl->ready.insert(impl->ready.end(), impl->waiting.begin(), impl->waiting.end());
        const bool more = impl->locked == !impl->ready.empty();
        impl->mutex.unlock();

        if(more)
          owner->post_immediate_completion(impl);

        self->self_id = nullptr;
      }
    };

    void do_post(implementation_type& impl, async_operation* op)
    {
      impl->mutex.lock();
      if(impl->locked) {
        impl->waiting.push_back(op);
        impl->mutex.unlock();
      } else {
        impl->locked = true;
        impl->mutex.unlock();
        impl->ready.push_back(op);
        iocp.post_immediate_completion(op);
      }
    }

    static void do_complete(iocp_service* owner, async_operation* base, const error_code& ec, size_t /*transferred*/)
    {
      if(!owner)
        return;

      strand_impl* impl = static_cast<implementation_type>(base);

      strand_service& self = use_service<strand_service>(owner->get_io_service());
      self.self_id = ntl::nt::this_thread::id();

      scoped_complete scoped = {impl, owner, &self};

      while(!impl->ready.empty()) {
        async_operation* op = impl->ready.back();
        impl->ready.pop_back();
        op->complete(*owner, ec, 0);
      }
    }
  private:
    friend class strand_impl;
    service_implementation_type& iocp;
    std::mutex mutex;
    ntl::nt::legacy_handle self_id;
    std::unique_ptr<strand_impl> objects[max_count];
    size_t salt;
  };


  inline io_service::strand::strand(io_service& ios)
    : service(use_service<strand_service>(ios))
  {
    service.construct(impl);
  }

  inline io_service::strand::~strand()
  {}

  inline io_service& io_service::strand::get_io_service()
  {
    return service.get_io_service();
  }

  template<class CompletionHandler>
  inline void io_service::strand::dispatch(CompletionHandler handler)
  {
    return service.dispatch(impl, handler);
  }

  template<class CompletionHandler>
  inline void io_service::strand::post(CompletionHandler handler)
  {
    return service.post(impl, handler);
  }

  //template<class Handler>
  //unspecified wrap(Handler handler);

  inline bool io_service::strand::running_in_this_thread() const
  {
    return service.running_in_this_thread(impl);
  }

  inline io_service::strand::strand_impl::strand_impl()
    : async_operation(&strand_service::do_complete)
    , locked(false)
  {}


}}}


#endif // NTL__STLX_TR2_IOSERVICE
