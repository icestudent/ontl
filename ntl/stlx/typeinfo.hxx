/**\file*********************************************************************
 *                                                                     \brief
 *  18.6 Type identification [support.rtti]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_TYPEINFO
#define NTL__STLX_TYPEINFO

//#include "exception.hxx"

#ifndef STLX__USE_RTTI
  #ifdef _MSC_VER
    #ifdef _CPPRTTI
      #define STLX__USE_RTTI 1
    #else
      #define STLX__USE_RTTI 0
    #endif
  #else
    #error define STLX__USE_RTTI
  #endif
#endif


#pragma warning(push)
#pragma warning(disable:4820) // '3' bytes padding added after data member 'type_info::mname'

class type_info
{
  public:
    virtual ~type_info() { }
    bool operator==(const type_info& rhs) const;
    bool operator!=(const type_info& rhs) const;
    bool before(const type_info& rhs) const;
    
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

}//namespace std

#endif//#ifndef NTL__STLX_TYPEINFO
