/**\file*********************************************************************
 *                                                                     \brief
 *  Miniport drivers support
 *
 ****************************************************************************
 */


#ifndef NTL__KM_MINIPORT
#define NTL__KM_MINIPORT

#include "basedef.hxx"

namespace ntl {
namespace km {


enum interface_type
{
  InterfaceTypeUndefined = -1,
  Internal,
  Isa,
  Eisa,
  MicroChannel,
  TurboChannel,
  PCIBus,
  VMEBus,
  NuBus,
  PCMCIABus,
  CBus,
  MPIBus,
  MPSABus,
  ProcessorInternal,
  InternalPowerBus,
  PNPISABus,
  PNPBus,
  USB,
  Irda,
  I1394,
};



}//namespace km
}//namespace ntl

#endif//#ifndef NTL__KM_MINIPORT
