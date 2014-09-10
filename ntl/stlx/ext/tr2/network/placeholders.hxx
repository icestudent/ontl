/**\file*********************************************************************
 *                                                                     \brief
 *  Network async placeholders
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_NETWORK_PLACEHOLDERS
#define NTL__STLX_TR2_NETWORK_PLACEHOLDERS
#pragma once

#include "../../../placeholders.hxx"

namespace std { namespace tr2 { namespace network {
  namespace placeholders
  {
    static const std::placeholders::__::placeholder_t<1> error;
    static const std::placeholders::__::placeholder_t<2> bytes_transferred;
    static const std::placeholders::__::placeholder_t<2> iterator;
  }
}}}
#endif // NTL__STLX_TR2_NETWORK_PLACEHOLDERS
