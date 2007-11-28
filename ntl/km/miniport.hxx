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


enum dma_width {
	Width8Bits,
	Width16Bits,
	Width32Bits,
	MaximumDmaWidth
};

enum dma_speed {
	Compatible,
	TypeA,
	TypeB,
	TypeC,
	TypeF,
	MaximumDmaSpeed
};

enum bus_data_type {
	ConfigurationSpaceUndefined = -1,
	Cmos,
	EisaConfiguration,
	Pos,
	CbusConfiguration,
	PCIConfiguration,
	VMEConfiguration,
	NuBusConfiguration,
	PCMCIAConfiguration,
	MPIConfiguration,
	MPSAConfiguration,
	PNPISAConfiguration,
	SgiInternalConfiguration,
	MaximumBusDataType
};


}//namespace km
}//namespace ntl

#endif//#ifndef NTL__KM_MINIPORT
