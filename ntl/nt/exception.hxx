/**\file*********************************************************************
 *                                                                     \brief
 *  NT Exceptions
 *
 ****************************************************************************
 */

#ifndef NTL__NT_EXCEPTION
#define NTL__NT_EXCEPTION

#include "basedef.hxx"

#undef exception_code
#undef exception_info
#undef abnormal_termination

#define exception_code              _exception_code()
#define exception_info()            reinterpret_cast<struct exception::pointers*>(_exception_info())
#define abnormal_termination        (_abnormal_termination() != 0)

extern "C" 
{
  unsigned long __cdecl _exception_code(void);
  void *        __cdecl _exception_info(void);
  int           __cdecl _abnormal_termination(void);
}


namespace ntl {
namespace nt {

struct context;

class exception
{
  ///////////////////////////////////////////////////////////////////////////
  public:


    /// Exception disposition return values.
    enum disposition
    {
      continue_execution,
      continue_search,
      nested_exception,
      collided_unwind
    };

    struct record 
    {
      static const int maximum_parameters = 15;
      uint32_t  ExceptionCode;
      uint32_t  ExceptionFlags;
      record *  ExceptionRecord;
      void *    ExceptionAddress;
      uint32_t  NumberParameters;
      uintptr_t ExceptionInformation[maximum_parameters];
    };
    
    /// Exception handler prototype
    typedef
      disposition __stdcall
      handler_t(
        const exception::record * exception_record,
        const void              * establisher_frame,
              context           * context_record,
              void              * dispatcher_context
        );

    /// SEH recod
    struct registration
    {
      registration *  next;
      handler_t *     handler;

      static registration * end_of_chain()
      { 
        return reinterpret_cast<registration*>(-1);
      }
    };

    /// exception_info result
    struct pointers
    {
      record*   ExceptionRecord;
      context*  ContextRecord;
    };

};//class exception


}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_EXCEPTION
