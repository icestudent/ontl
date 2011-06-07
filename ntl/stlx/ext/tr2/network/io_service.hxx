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

namespace std { namespace tr2 { namespace sys {

  class io_service;
  class invalid_service_owner;
  class service_already_exists;

  template<class Service> Service& use_service(io_service&);
  template<class Service> void add_service(io_service&, Service*);
  template<class Service> bool has_service(io_service&);

  // default handler hook functions:
  inline void* io_handler_allocate(size_t s, ...)    { return ::operator new(s);    }
  inline void io_handler_deallocate(void* p, size_t, ...)   { ::operator delete(p); }

  template<class F> 
  inline void io_handler_invoke(F f, ...) { f(); }

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

    template<class Handler, typename A1>
    inline binder1<Handler, A1> bind_handler(const Handler& f, const A1& a1)
    {
      return binder1<Handler, A1>(f, a1);
    }
  }} // __

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
   *	@brief 5.3.3. Class io_service
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
     *	@brief 5.3.5. Class io_service::id
     *  @details The class io_service::id provides identification of services, and is used as an index for service lookup.
     **/
    class id: noncopyable
    {
    public:
      id();
    };

    ///\name constructors/destructor:
    io_service(){svcNo = 0;}
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
    typedef forward_list<service_node> services_t;
    services_t services;
    size_t svcNo;

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
      type_index key(typeid(Service));
      services_t::const_iterator it = find_if(services.cbegin(), services.cend(), finder(key));
      return it != services.cend();
    }
    template<class Service> Service& do_use_service()
    {
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
   *	Service objects may be explicitly added to an io_service using the function template add_service<Service>(). If the 
   *  Service is already present, the service_already_exists exception is thrown. If the owner of the service is not the same 
   *  object as the io_service parameter, the invalid_service_owner exception is thrown. 
   **/
   template<class Service> inline void add_service(io_service& ios, Service* svc) { ios.do_add_service<Service>(svc); }
   template<class Service> inline bool has_service(io_service& ios) { return ios.do_has_service<Service>(); }
   template<class Service> inline Service& use_service(io_service& ios) { return ios.do_use_service<Service>(); }


  /**
   *	@brief 5.3.4. Class io_service::service
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

  inline io_service::~io_service()
  {
    for(services_t::iterator it = services.begin(), end = services.end(); it != end; ++it, svcNo--){
      assert(svcNo == it->id);
      it->val->shutdown_service();
      delete it->val;
    }
    assert(svcNo == 0);
  }

  /**
   *	@brief 5.3.6 Class io_service::work
   *  @details An object of class io_service::work represents a unit of unfinished work for an io_service. 
   **/
  class io_service::work
  {
    io_service& ios;
  public:
    // constructors/destructor:
    explicit work(io_service& ios)
      :ios(ios)
    {}
    work(const work& other)
      :ios(other.ios)
    {}
    ~work(); // if last, stop ios

    // members:
    io_service& get_io_service() { return ios; }
  private:
    void operator=(const work&) __deleted;
  };

  /**
   *	@brief 5.3.7 Class io_service::strand
   *  @details An object of class io_service::strand may be used to prevent concurrent invocation of handlers.
   *
   *  A \e strand is defined as a strictly sequential invocation of event handlers (i.e. no concurrent invocation). Use of strands allows 
   *  execution of code in a multithreaded program without the need for explicit locking (e.g. using mutexes). 
   **/
  class io_service::strand:
    noncopyable
  {
    io_service& ios;
  public:
    // constructors/destructor:
    explicit strand(io_service& ios)
      :ios(ios)
    {}
    ~strand();

    // members:
    io_service& get_io_service() { return ios; }

    template<class CompletionHandler>
    void dispatch(CompletionHandler handler);
    
    template<class CompletionHandler>
    void post(CompletionHandler handler);
    
    //template<class Handler>
    //unspecified wrap(Handler handler);
  private:
    strand(const strand&) __deleted;
    void operator=(const strand&) __deleted;
  };

  } // sys
 } // tr2
} // std
#endif // NTL__STLX_TR2_IOSERVICE
