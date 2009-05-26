/**\file*********************************************************************
 *                                                                     \brief
 *  I/O Completion objects
 *
 ****************************************************************************
 */
#ifndef NTL__KM_IOCP
#define NTL__KM_IOCP

#include "../nt/iocp.hxx"

namespace ntl { namespace km {

  using nt::io_completion_port;
  using nt::io_completion_information_class;
  using nt::io_completion_basic_information;

  using nt::ZwCreateIoCompletion;
  using nt::ZwOpenIoCompletion;
  using nt::ZwQueryIoCompletion;
  using nt::ZwSetIoCompletion;
  using nt::ZwRemoveIoCompletion;

}}
#endif // NTL__KM_IOCP
