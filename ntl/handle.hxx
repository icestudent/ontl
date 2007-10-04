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

    explicit basic_handle(X h = X())  throw() : h(h) {}

    basic_handle(basic_handle & a)  throw() : h(a.release()) {}

    basic_handle & operator=(basic_handle & a) throw()
    {
      reset(a.release());
      return *this;
    }

    ~basic_handle() throw() { if ( get() ) Delete(get()); }

    bool is_valid() const { return Validate(get()); }

    X get() const throw() { return h; }
    X release()   throw() { X tmp = get(); set(0); return tmp; }

    basic_handle duplicate() const throw()
    { 
      return basic_handle( Duplicate(get()) );
    }

    void reset(X h = 0) throw()
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
