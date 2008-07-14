/**\file*********************************************************************
 *                                                                     \brief
 *  Handles support
 *
 ****************************************************************************
 */


#ifndef NTL__HANDLE
#define NTL__HANDLE

//#include <functional>
#include "cstdint"
#include "stlx/exception.hxx"

namespace ntl {


namespace aux { 
template<typename T> __forceinline bool is_valid(T t) { return t != 0; }
}

/// handle RAII wrapper
template <class X,
          void(*Delete)(X),
          X(*Duplicate)(X),
          bool(*Validate)(X) = aux::is_valid>
class basic_handle
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef X element_type;

    explicit basic_handle(X h = X())  __ntl_nothrow : h(h) {}

    basic_handle(basic_handle & a)  __ntl_nothrow : h(a.release()) {}

    basic_handle & operator=(basic_handle & a) __ntl_nothrow
    {
      reset(a.release());
      return *this;
    }

    ~basic_handle() __ntl_nothrow { if ( get() ) Delete(get()); }

    bool is_valid() const { return Validate(get()); }

    X get() const __ntl_nothrow { return h; }
    X release()   __ntl_nothrow { X tmp = get(); set(0); return tmp; }

    basic_handle duplicate() const __ntl_nothrow
    { 
      return basic_handle( Duplicate(get()) );
    }

    void reset(X h = 0) __ntl_nothrow
    { 
      if ( get() && get() != h ) Delete(get());
      set(h);
    }

  ///////////////////////////////////////////////////////////////////////////
  private:
  
    X h;
    void set(X h) { this->h = h; }
};


}//namespace ntl

#endif//#ifndef NTL__HANDLE
