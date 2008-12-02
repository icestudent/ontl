/**\file*********************************************************************
 *                                                                     \brief
 *  18.6 Type identification [support.rtti]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_TYPEINFO
#define NTL__STLX_TYPEINFO

#ifndef STLX__USE_RTTI
  #ifdef _MSC_VER
    #ifdef _CPPRTTI
      #ifdef NTL_KM
        #pragma message("Kernel mode RTTI support was not implemented yet")
        #define STLX__USE_RTTI 0
      #else
        #define STLX__USE_RTTI 1
      #endif
    #else
      #define STLX__USE_RTTI 0
    #endif
  #else
    #error define STLX__USE_RTTI
  #endif
#endif

#include "cstring.hxx" // for std::strcmp
#include "exception.hxx"

#pragma warning(push)
#pragma warning(disable:4820) // '3' bytes padding added after data member 'type_info::mname'

/// 18.6.1 Class type_info [type.info]
class type_info
{
  public:
    /** type_info destructor */
    virtual ~type_info() { }

    /**
     *	@brief Compares the current object with \c rhs.
     *	@return true if the two values describe the same type.
     **/
    bool operator==(const type_info& rhs) const
    {
      return std::strcmp(mname+1, rhs.mname+1) == 0;
    }

    /**
     *	@brief Compares the current object with \c rhs.
     *	@return true if the two values describe the different types.
     **/
    bool operator!=(const type_info& rhs) const
    {
      return std::strcmp(mname+1, rhs.mname+1) != 0;
    }

    /**
     *	@brief Compares the current object with \c rhs.
     *	@return true if \c *this precedes \c rhs in the implementation's collation order.
     **/
    bool before(const type_info& rhs) const
    {
      return std::strcmp(mname+1, rhs.mname+1) <= 0;
    }

    /**
     *	@brief Returns an implementation-defined NTBS.
     *	@return a null-terminated string representing the human-readable name of the type.
     **/
    const char* name() const
#if !STLX__USE_RTTI
    {
      return mname;
    }
#else
    ;
#endif
  private:
    type_info(const type_info& rhs);
    type_info& operator=(const type_info& rhs);
    mutable void* data;
    char    mname[1];
};

#pragma warning(pop)

namespace std {

#if STLX__USE_RTTI

/// 18.6.1 Class type_info [type.info]
using ::type_info;

#endif//if STLX__USE_RTTI


/// 18.6.2 Class bad_cast [bad.cast]
class bad_cast: public exception
{
public:
  bad_cast() __ntl_nothrow
  {}
  bad_cast(const bad_cast&) __ntl_nothrow
  {}
  bad_cast& operator=(const bad_cast&) __ntl_nothrow
  { return *this; }

  virtual const char* what() const __ntl_nothrow
  {
    return "bad_cast";
  }
};

/// 18.6.3 Class bad_typeid [bad.typeid]
class bad_typeid: public exception {
public:
  bad_typeid() __ntl_nothrow
  {}
  bad_typeid(const bad_typeid&) __ntl_nothrow
  {}
  bad_typeid& operator=(const bad_typeid&) __ntl_nothrow
  { return *this; }

  virtual const char* what() const __ntl_nothrow
  {
    return "bad_typeid";
  }
};

}//namespace std

#endif//#ifndef NTL__STLX_TYPEINFO
