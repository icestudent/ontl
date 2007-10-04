/**\file*********************************************************************
 *                                                                     \brief
 *  Transport Driver Interface
 *
 ****************************************************************************
 */


#ifndef NTL__KM_TDI
#define NTL__KM_TDI

#include "basedef.hxx"
//#include "string.hxx"
#include "irp.hxx"
//include "device_object.hxx"


namespace ntl {
namespace km {


class tdi
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    enum address_type
    {
      address_type_unspec     = 0,
      address_type_unix       = 1,
      address_type_ip         = 2,
      address_type_ipx        = 6,
      address_type_netbios    = 17,
      address_type_8022       = 18,
      address_type_netbios_ex = 22,
      address_type_ip6        = 23,
      address_type_netbios_unicode_ex = 24
    };

#pragma pack(push)
#pragma pack(1)

    struct ta_address
    {
      uint16_t  AddressLength;
      uint16_t  AddressType;
      uint8_t   Address[1];
    };


    struct transport_address
    {
      int32_t     TAAddressCount;
      ta_address  Address[1];
    };


    struct address_ip
    {
      uint16_t  sin_port;
      uint32_t  in_addr;
      uint8_t   sin_zero[8];
    };

    struct ta_address_ip
    {
      int32_t   TAAddressCount;
      struct
      {
        uint16_t    AddressLength;
        uint16_t    AddressType;
        address_ip  Address[1];
      } Address [1];
    };

#pragma pack(pop)

    struct connection_information
    {
      long    UserDataLength;
      void *  UserData;
      long    OptionsLength;
      void *  Options;
      long    RemoteAddressLength;
      void *  RemoteAddress;
    };

    enum query_type
    {
      query_broadcast_address   = 0x00000001,
      query_provider_info       = 0x00000002,
      query_address_info        = 0x00000003,
      query_connection_info     = 0x00000004,
      query_provider_statistics = 0x00000005,
      query_datagram_info       = 0x00000006,
      query_data_link_address   = 0x00000007,
      query_network_address     = 0x00000008,
      query_max_datagram_info   = 0x00000009,
      query_routing_info        = 0x0000000a
    };


    struct address_info
    {
      uint32_t          ActivityCount;
      transport_address Address;
    };


    struct request_kernel
    {
      uintptr_t RequestFlags;
      connection_information *  RequestConnectionInformation;
      connection_information *  ReturnConnectionInformation;
      void * RequestSpecific;
    };

    enum request_code
    {
      associate_address     = 0x01,
      disassociate_address  = 0x02,
      connect               = 0x03,
      listen                = 0x04,
      accept                = 0x05,
      disconnect            = 0x06,
      send                  = 0x07,
      receive               = 0x08,
      send_datagram         = 0x09,
      receive_datagram      = 0x0A,
      set_event_handler     = 0x0B,
      query_information     = 0x0C,
      set_information       = 0x0D,
      action                = 0x0E,

      direct_send           = 0x27,
      direct_send_datagram  = 0x29,
      direct_accept         = 0x2A,
    };

    struct request_kernel_connect : request_kernel {};

    struct request_kernel_query_information
    {
      query_type                QueryType;
      connection_information *  RequestConnectionInformation;
    };

    static
    void build_query_information(
      irp *                     Irp,
      device_object *           DevObj,
      file_object *             FileObj,
      io_completion_routine_t * CompRoutine,
      void *                    Contxt,
      query_type                QType,
      mdl *                     MdlAddr,
      connection_information *  ConnInfo = 0)
    {
      Irp->set_completion_routine(CompRoutine, Contxt,
                sl_invoke_on_cancel|sl_invoke_on_error|sl_invoke_on_success);
      Irp->MdlAddress = MdlAddr;
      io_stack_location * stack = Irp->get_next_stack_location();
      stack->MajorFunction = irp::mj_internal_device_control;
      stack->MinorFunction = query_information;
      stack->DeviceObject = DevObj;
      stack->FileObject = FileObj;
      request_kernel_query_information & params = 
        *reinterpret_cast<request_kernel_query_information*>(&stack->Parameters);
      params.QueryType = QType;
      params.RequestConnectionInformation = ConnInfo;
    }

  ///////////////////////////////////////////////////////////////////////////
  private:

};//class tdi


}//namspace km
}//namespace ntl


#endif//#ifndef NTL__KM_TDI
