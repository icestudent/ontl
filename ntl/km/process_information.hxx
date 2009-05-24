/**\file*********************************************************************
 *                                                                     \brief
 *  Process and Thread Information
 *
 ****************************************************************************
 */
#ifndef NTL__KM_PROCESS_INFORMATION
#define NTL__KM_PROCESS_INFORMATION

#include "../nt/process_information.hxx"

namespace ntl { namespace km {
/**\addtogroup process_information Process Information
  *@{*/

  using nt::query_thread_information_t;
  using nt::set_thread_information_t;
  using nt::thread_information_base;

NTL__EXTERNAPI query_thread_information_t ZwQueryInformationThread;
NTL__EXTERNAPI set_thread_information_t   ZwSetInformationThread;


template<class ThreadInformationClass>
struct thread_information
  :public thread_information_base <ThreadInformationClass, ZwQueryInformationThread, ZwSetInformationThread>
{
  typedef thread_information_base <ThreadInformationClass, ZwQueryInformationThread, ZwSetInformationThread> base;

  thread_information(legacy_handle thread_handle = current_thread()) __ntl_nothrow
    : base(thread_handle)
  {/**/}

  thread_information(
    legacy_handle thread_handle,
    const ThreadInformationClass&  info) __ntl_nothrow
    : base(thread_handle, info)
  {/**/}
};

using nt::thread_basic_information;

/** @} */
  }
}
#endif // NTL__KM_PROCESS_INFORMATION
