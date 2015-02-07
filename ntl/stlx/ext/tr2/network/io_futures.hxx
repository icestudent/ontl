/**\file*********************************************************************
 *                                                                     \brief
 *  I/O Service Futures
 *
 ****************************************************************************
 */
#pragma once

#include "../../../future.hxx"

namespace std { namespace tr2 { namespace sys {

  template<class Allocator>
  class use_future_t;

  namespace __
  {
    //////////////////////////////////////////////////////////////////////////
    template<class Arg>
    class future_handler
    {
    public:
      template<class Alloc>
      explicit future_handler(const use_future_t<Alloc>& f)
        : promise(std::make_shared< std::promise<Arg> >())
      {}

      void operator()(Arg t) const
      {
        promise->set_value(t);
      }

      void operator()(const std::error_code& ec, Arg t) const
      {
        if(!ec)
          promise->set_value(t);
        else
          promise->set_error(ec);
      }

      mutable std::shared_ptr< std::promise<Arg> > promise;
    };

    
    template<>
    class future_handler<void>
    {
    public:
      template<class A>
      explicit future_handler(const use_future_t<A>& f)
        : promise(std::make_shared< std::promise<void> >())
      {}

      void operator()() const
      {
        promise->set_value();
      }

      void operator()(const std::error_code& ec) const
      {
        if(!ec)
          promise->set_value();
        else
          promise->set_error(ec);
      }

      mutable std::shared_ptr< std::promise<void> > promise;
    };


    // Handler itself
    template<class Handler, class Arg = void>
    struct handler_type
    {
      typedef Handler type;
    };

    // Use future handler
    template<class Allocator, class HandlerArg>
    struct handler_type<use_future_t<Allocator>, HandlerArg>
    {
      typedef future_handler<HandlerArg> type;
    };
  }

  // generic async result w/o futures
  template<class Handler, class Arg = void>
  struct async_result
  {
    typedef void type;

    explicit async_result(Handler&)
    {}

    void get() {}
  };


  // async future result
  template<class Arg>
  struct async_result< __::future_handler<Arg> >
  {
    typedef __::future_handler<Arg> Handler;
    typedef std::future<Arg> type;

    explicit async_result(Handler& handler)
    {
      value = handler.promise->get_future();
    }

    type get() { return std::move(value); }

  private:
    std::future<Arg> value;
  };


  // Generic completion handler
  template<class F, class Arg = void>
  class CompleteHandler
  {
  public:
    typedef typename __::handler_type<F,Arg>::type Handler;
    typedef typename async_result<Handler>::type type;

    explicit CompleteHandler(F f)
      : handler(f)
      , result(handler)
    {}

    Handler handler;
    async_result<Handler> result;
  };


  // Read completion handler
  template<class F>
  class ReadHandler:
    public CompleteHandler<F, size_t>
  {
  public:
    explicit ReadHandler(F f)
      : CompleteHandler(f)
    {}
  };


  // Write completion handler
  template<class F>
  class WriteHandler:
    public CompleteHandler<F, size_t>
  {
  public:
    explicit WriteHandler(F f)
      : CompleteHandler(f)
    {}
  };



  //template<class Handler>
  //typename sys::ReadHandler<Handler>::type async_receive(Handler handler)
  //{
  //  sys::ReadHandler<Handler> wrap(handler);
  //  service_receive(wrap.handler);
  //  return wrap.result.get();
  //}

  template<class F, class T>
  inline void io_handler_invoke(F& f, __::future_handler<T>* h)
  {
    std::shared_ptr< std::promise<T> > promise(h->promise);
    __ntl_try {
      f();
    }
    __ntl_catch(...) {
      promise->set_exception(std::current_exception());
    }
  }

  template<class F, class T>
  inline void io_handler_invoke(const F& f, __::future_handler<T>* h)
  {
    std::shared_ptr< std::promise<T> > promise(h->promise);
    __ntl_try {
      f();
    }
    __ntl_catch(...) {
      promise->set_exception(std::current_exception());
    }
  }

}}}
