/**\file*********************************************************************
 *                                                                     \brief
 *  NDIS interfase
 *
 ****************************************************************************
 */
#ifndef NTL__KM_NDIS
#define NTL__KM_NDIS
#pragma once

#ifndef NTL__POOL_TAG
#define NTL__POOL_TAG 'LTN_'  // _NTL
#endif

#include "miniport.hxx"
#include "string.hxx"
#include "device_object.hxx"


namespace ntl {
namespace km {


enum net_pnp_event_code
{
  NetEventSetPower,
  NetEventQueryPower,
  NetEventQueryRemoveDevice,
  NetEventCancelRemoveDevice,
  NetEventReconfigure,
  NetEventBindList,
  NetEventBindsComplete,
  NetEventPnPCapabilities,
  NetEventPause,
  NetEventRestart,
  NetEventPortActivation,
  NetEventPortDeactivation,
  NetEventMaximum
};

struct net_pnp_event
{
  net_pnp_event_code  NetEvent;
  void *              Buffer;
  uint32_t            BufferLength;
  uintptr_t           NdisReserved[4];
  uintptr_t           TransportReserved[4];
  uintptr_t           TdiReserved[4];
  uintptr_t           TdiClientReserved[4];
};


class ndis
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    struct status
    {
      enum type
      {
        success             = 0,
        pending             = 0x00000103,
        closing				      = (int)0xC0010002,
        bad_version         = (int)0xC0010004,
        bad_characteristics = (int)0xC0010005,
        adapter_not_found   = (int)0xC0010006,
        open_failed			    = (int)0xC0010007,
        resources           = (int)0xC000009A
      };
    };

    static __forceinline
    void * alloc(size_t Length, uint32_t Tag = NTL__POOL_TAG);

    static __forceinline
    void ndis::free(void * va, size_t length = 0, uint32_t flags = 0);

    typedef const_unicode_string string;

    struct event { kevent Event; };

    struct address_family
    {
      enum type
      {
        q2931       = 0x1,
        psched      = 0x2,
        l2tp        = 0x3,
        irda        = 0x4,
        _1394       = 0x5,
        ppp         = 0x6,
        infiniband  = 0x7,
        tapi        = 0x800,
        tapi_proxy  = 0x801,
        proxy       = 0x80000000
      };
      type      AddressFamily;
      uint32_t  MajorVersion;
      uint32_t  MinorVersion;
    };

    enum medium
    {
      NdisMedium802_3,
      NdisMedium802_5,
      NdisMediumFddi,
      NdisMediumWan,
      NdisMediumLocalTalk,
      NdisMediumDix,        // not a real medium
      NdisMediumArcnetRaw,
      NdisMediumArcnet878_2,
      NdisMediumAtm,
      NdisMediumWirelessWan,
      NdisMediumIrda,
      NdisMediumBpc,
      NdisMediumCoWan,
      NdisMedium1394,
      NdisMediumInfiniBand,
    };


    struct reference
    {
      kspin_lock  SpinLock;
      uint16_t    ReferenceCount;
      bool        Closing;
    };
    //STATIC_ASSERT(sizeof(reference)==0x8);

    struct timer
    {
      ktimer  Timer;
      kdpc    Dpc;
    };

    typedef struct _opaque {} * handle;

    struct buffer;

    struct buffer_pool
    {
      explicit buffer_pool(uint32_t number_of_descriptors)
      {
        _allocate_pool(pool, number_of_descriptors);
      }

      ~buffer_pool()
      {
        _free_pool(pool);
      }

      static __forceinline
      status::type
        _allocate_pool(
          ndis::handle  & pool_handle,
          uint32_t        number_of_descriptors);

      static __forceinline
      void _free_pool(ndis::handle pool_handle);

      __forceinline
      ndis::buffer * alloc(void * virtual_address, size_t length);

      private:
        ndis::handle  pool;
    }; // class buffer_pool

    struct buffer : mdl
    {
      void * operator new(
        std::size_t,
        buffer_pool & bp,
        void *        virtual_address,
        size_t        length
        ) __ntl_nothrow
      {
        return bp.alloc(virtual_address, length);
      }

      buffer * next() const
      {
        return static_cast<buffer *>(mdl::next());
      }
    };

    typedef buffer * buffer_ptr;

    struct packet
    {
        __forceinline
        void ndis::packet::free();

        struct oob_data
        {
          union { uint64_t  TimeToSend, TimeSent; };
          uint64_t          TimeReceived;
          uint32_t          HeaderSize;
          uint32_t          SizeMediaSpecificInfo;
          void *            MediaSpecificInformation;
          status::type      Status;
        };
        //STATIC_ASSERT(sizeof(oob_data) == 0x20);

        typedef void  * pool;

      private:
        struct _private
        {
          unsigned      PhysicalCount;
          unsigned      TotalLength;
          buffer *      Head;
          buffer *      Tail;
          packet::pool  Pool;
          unsigned      Count;
          uint32_t      Flags;
          bool          ValidCounts;
          uint8_t       NdisPacketFlags;
          uint16_t      NdisPacketOobOffset;
        };
        _private      Private;

        uintptr_t     MacReserved[4];
        uintptr_t     _Win95Reserved[2];
        uint8_t       ProtocolReserved[1];

      public:

        bool valid_counts() const
        {
          return Private.ValidCounts;
        }

        void valid_counts(bool valid)
        {
          Private.ValidCounts = valid;
        }

        buffer * first_buffer() const
        {
          return Private.Head;
        }

        void first_buffer(buffer * buf)
        {
          Private.Head = buf;
        }

        uint32_t total_bytes() const
        {
          return first_buffer()->total_bytes();
        }

        unsigned count()
        {
          if ( !Private.ValidCounts )
          {
            Private.TotalLength = Private.PhysicalCount = Private.Count = 0;
            for ( buffer * tmp = Private.Head; tmp; tmp = tmp->next() )
            {
              Private.TotalLength += tmp->byte_count();
              Private.PhysicalCount += tmp->ByteCount ?
                ((tmp->virtual_address() & (4096-1)) + tmp->byte_count() + (4096-1)) >> 12 : 1;
              ++Private.Count;
            }
            Private.ValidCounts = true;
          }
          return Private.Count;
        }

        void chain_buffer_at_back(buffer * const buf)
        {
          buffer * tmp = buf;
          while ( tmp->next() ) tmp = tmp->next();
          if ( Private.Head ) Private.Tail->Next = buf;
          else Private.Head = buf;
          Private.Tail = tmp;
          Private.ValidCounts = false;
        }

        oob_data & oob()
        {
          return *(oob_data*)((char*)this + Private.NdisPacketOobOffset);
        }

        const oob_data & oob() const
        {
          return *(const oob_data*)((char*)this + Private.NdisPacketOobOffset);
        }

        struct info
        {
          void *  TcpIpChecksumPacketInfo;
          void *  IpSecPacketInfo;
          void *  TcpLargeSendPacketInfo;
          void *  ClassificationHandlePacketInfo;
          void *  NdisReserved;
          void *  ScatterGatherListPacketInfo;
          void *  Ieee8021QInfo;
          void *  OriginalPacketInfo;
          void *  PacketCancelId;
          void *  OriginalNetBufferList;
          void *  CachedNetBufferList;
        };

        info & extension()
        {
          return *reinterpret_cast<info*>(&oob() + 1);
        }

    };//struct packet

    struct wan_packet
    {
      list_entry  WanPacketQueue;
      uint8_t *   CurrentBuffer;
      uint32_t    CurrentLength;
      uint8_t *   StartBuffer;
      uint8_t *   EndBuffer;
      void *      ProtocolReserved1;
      void *      ProtocolReserved2;
      void *      ProtocolReserved3;
      void *      ProtocolReserved4;
      void *      MacReserved1;
      void *      MacReserved2;
      void *      MacReserved3;
      void *      MacReserved4;
    };

    typedef uint32_t  oid;

    struct request
    {
      enum type
      {
        NdisRequestQueryInformation,
        NdisRequestSetInformation,
        NdisRequestQueryStatistics,
        NdisRequestOpen,
        NdisRequestClose,
        NdisRequestSend,
        NdisRequestTransferData,
        NdisRequestReset,
        NdisRequestGeneric1,
        NdisRequestGeneric2,
        NdisRequestGeneric3,
        NdisRequestGeneric4,
        NdisRequestMethod,
      };
      uint8_t   MacReserved[4 * sizeof(void*)];
      type      RequestType;
      union data
      {
        struct query_information
        {
          oid         Oid;
          void *      InformationBuffer;
          unsigned    InformationBufferLength;
          unsigned    BytesWritten;
          unsigned    BytesNeeded;
        };
        struct set_information
        {
          oid         Oid;
          void *      InformationBuffer;
          unsigned    InformationBufferLength;
          unsigned    BytesRead;
          unsigned    BytesNeeded;
        };
      }         data;
      uint8_t   NdisReserved[9 * sizeof(void*)];
      union
      {
        uint8_t   CallMgrReserved[2 * sizeof(void*)];
        uint8_t   ProtocolReserved[2 * sizeof(void*)];
      };
      uint8_t   MiniportReserved[2 * sizeof(void*)];
    } ;//struct request

    struct vc_context;
    struct open_block;


    class protocol
    {
      ///////////////////////////////////////////////////////////////////////
      public:

        struct  block;
        typedef block * handle;

        struct characteristics
        {
            characteristics() {/**/}

          explicit
            characteristics(
              const string &  name,
              uint8_t         major = 0x04,
              uint8_t         minor = 0x00)
              : Name(name), MajorNdisVersion(major), MinorNdisVersion(minor)
              , Flags(),
                OpenAdapterCompleteHandler(),
                CloseAdapterCompleteHandler(),
                SendCompleteHandler(),
                TransferDataCompleteHandler(),
                ResetCompleteHandler(),
                RequestCompleteHandler(),
                ReceiveHandler(),
                ReceiveCompleteHandler(),
                StatusHandler(),
                StatusCompleteHandler(),
                ReceivePacketHandler(),
                BindAdapterHandler(_bind_adapter_handler_stub),
                UnbindAdapterHandler(_unbind_adapter_handler_stub),
                PnPEventHandler(),
                UnloadHandler(),
                CoSendCompleteHandler(),
                CoStatusHandler(),
                CoReceivePacketHandler(),
                CoAfRegisterNotifyHandler()
            {/**/}

          uint8_t   MajorNdisVersion;
          uint8_t   MinorNdisVersion;
          uint16_t  Filler;
          uint32_t  Flags;

          typedef
          void __stdcall
            open_adapter_complete_handler_t(
              ndis::protocol::handle  ProtocolBindingContext,
              ndis::status::type      Status,
              ndis::status::type      OpenErrorStatus
              );

          open_adapter_complete_handler_t * OpenAdapterCompleteHandler;

          typedef
          void __stdcall
            close_adapter_complete_handler_t(
                ndis::protocol::handle  ProtocolBindingContext,
                ndis::status::type      Status
                );

          close_adapter_complete_handler_t *  CloseAdapterCompleteHandler;

          typedef
          void __stdcall
            send_complete_handler_t(
              ndis::protocol::handle  ProtocolBindingContext,
              ndis::packet *          Packet,
              ndis::status::type      Status
              );

          typedef
          void __stdcall
            wan_send_complete_handler_t(
              ndis::protocol::handle  ProtocolBindingContext,
              ndis::wan_packet *      Packet,
              ndis::status::type      Status
              );

          union
          {
            send_complete_handler_t *     SendCompleteHandler;
            wan_send_complete_handler_t * WanSendCompleteHandler;
          };

          typedef
          void __stdcall
            transfer_data_complete_handler_t(
              ndis::protocol::handle  ProtocolBindingContext,
              ndis::packet *          Packet,
              ndis::status::type      Status,
              unsigned                BytesTransferred
              );

          typedef
          void __stdcall
            wan_transfer_data_complete_handler_t();

          union
          {
            transfer_data_complete_handler_t *      TransferDataCompleteHandler;
            wan_transfer_data_complete_handler_t *  WanTransferDataCompleteHandler;
          };

          typedef
          void __stdcall
            reset_complete_handler_t(
              ndis::protocol::handle  ProtocolBindingContext,
              ndis::status::type      Status
              );

          reset_complete_handler_t *  ResetCompleteHandler;

          typedef
          void __stdcall
            request_complete_handler_t(
              ndis::protocol::handle  ProtocolBindingContext,
              ndis::request           NdisRequest,
              ndis::status::type      Status
              );

          request_complete_handler_t *  RequestCompleteHandler;

          typedef
          int __stdcall
            receive_handler_t(
              ndis::protocol::handle  ProtocolBindingContext,
              ndis::handle            MacReceiveContext,
              void *                  HeaderBuffer,
              unsigned                HeaderBufferSize,
              void *                  LookAheadBuffer,
              unsigned                LookaheadBufferSize,
              unsigned                PacketSize
              );

          typedef
          int __stdcall
            wan_receive_handler_t(
              ndis::protocol::handle  NdisLinkHandle,
              uint8_t *               Packet,
              unsigned                PacketSize
              );

          union
          {
            receive_handler_t *     ReceiveHandler;
            wan_receive_handler_t * WanReceiveHandler;
          };

          typedef
          void __stdcall
            receive_complete_handler_t(
                ndis::protocol::handle  ProtocolBindingContext
                );

          receive_complete_handler_t *  ReceiveCompleteHandler;

          typedef
          void __stdcall
            status_handler_t(
                ndis::protocol::handle  ProtocolBindingContext,
                ndis::status::type      GeneralStatus,
                void *                  StatusBuffer,
                unsigned                StatusBufferSize
                );

          status_handler_t * StatusHandler;

          typedef
          void __stdcall
            status_complete_handler_t(
              ndis::protocol::handle  ProtocolBindingContext
              );

          status_complete_handler_t * StatusCompleteHandler;

          ndis::string  Name;

          typedef
          int __stdcall
            receive_packet_handler_t(
              ndis::protocol::handle  ProtocolBindingContext,
              ndis::packet *          Packet
              );

          receive_packet_handler_t *  ReceivePacketHandler;

          typedef
          void __stdcall
            bind_adapter_handler_t(
              ndis::status::type  *   Status,
              ndis::protocol::handle  ProtocolBindingContext,
              ndis::string *          DeviceName,
              ndis::handle            SystemSpecific1,
              ndis::handle            SystemSpecific2
              );

          bind_adapter_handler_t *  BindAdapterHandler;

          typedef
          void __stdcall
            unbind_adapter_handler_t(
              ndis::status::type    * Status,
              ndis::protocol::handle  ProtocolBindingContext,
              ndis::handle            UnbindContext
              );

          unbind_adapter_handler_t *  UnbindAdapterHandler;

          typedef
          int __stdcall
            pnp_event_handler_t(
              ndis::protocol::handle  ProtocolBindingContext,
              net_pnp_event *         NetPnPEvent
              );

          pnp_event_handler_t * PnPEventHandler;

          typedef
          void __stdcall
            unload_handler_t();

          unload_handler_t * UnloadHandler;

          void * ReservedHandlers[4];

          typedef
          void __stdcall
            co_send_complete_handler_t(
              ndis::status::type    Status,
              ndis::vc_context *    ProtocolVcContext,
              ndis::packet *        Packet
              );

          co_send_complete_handler_t *  CoSendCompleteHandler;

          typedef
          void __stdcall
            co_status_handler_t(
              ndis::protocol::handle  ProtocolBindingContext,
              ndis::vc_context *      ProtocolVcContext       __optional,
              ndis::status::type      GeneralStatus,
              void *                  StatusBuffer,
              unsigned                StatusBufferSize
              );

          co_status_handler_t * CoStatusHandler;

          typedef
          unsigned __stdcall
            co_receive_packet_handler_t(
              ndis::protocol::handle  ProtocolBindingContext,
              ndis::vc_context *      ProtocolVcContext,
              ndis::packet *          Packet
              );

          co_receive_packet_handler_t * CoReceivePacketHandler;

          typedef
          void __stdcall
            co_af_register_notify_handler_t(
              ndis::protocol::handle  ProtocolBindingContext,
              address_family *        AddressFamily
              );

          co_af_register_notify_handler_t * CoAfRegisterNotifyHandler;

        };//struct characteristics
        //STATIC_ASSERT(sizeof(characteristics) == 0x6c);

        struct block
        {
          open_block *              OpenQueue;
          reference                 Ref;
          kevent *                  DeregEvent;
          protocol::block *         NextProtocol;
          protocol::characteristics ProtocolCharacteristics;
          work_queue_item           WorkItem;
          kmutant                   Mutex;
          unsigned long             MutexOwner;
          ndis::string *            BindDeviceName;
          ndis::string *            RootDeviceName;
          struct m_driver_block *   AssociatedMiniDriver;
          struct miniport_block *   BindingAdapter;
        };// <size 0xc4>
        //STATIC_ASSERT(sizeof(block) == 0xc4);

        explicit
        protocol(const protocol::characteristics & characteristics)
        : handle_()
        {
          _register(handle_, characteristics, status_);
        }

        ~protocol() { _deregister(handle_, status_); }

        block * get_block() { return handle_; }
        const block * get_block() const { return handle_; }

        operator const void *() const { return handle_; }

        ndis::status::type status() const { return status_; }

        static __forceinline
          ndis::status::type
            _register(
              ndis::protocol::handle &                NdisProtocolHandle,
              const ndis::protocol::characteristics & ProtocolCharacteristics,
              ndis::status::type &                    status
              );

        static __forceinline
          ndis::status::type
            _register(
              ndis::protocol::handle &                NdisProtocolHandle,
              const ndis::protocol::characteristics & ProtocolCharacteristics
              )
        {
          ndis::status::type  status;
          return _register(NdisProtocolHandle, ProtocolCharacteristics, status);
        }

        static __forceinline
          ndis::status::type
            _deregister(
              ndis::protocol::handle  NdisProtocolHandle,
              ndis::status::type &    status
              );

        static __forceinline
          ndis::status::type
            _deregister(
              ndis::protocol::handle  NdisProtocolHandle
              )
        {
          ndis::status::type  status;
          return _deregister(NdisProtocolHandle, status);
        }

      ///////////////////////////////////////////////////////////////////////
      private:

        ndis::protocol::handle  handle_;
        ndis::status::type      status_;

        protocol(const protocol&);
        const protocol& operator=(const protocol&);

    };//class protocol


    typedef ndis::protocol::characteristics::open_adapter_complete_handler_t
      open_adapter_complete_handler_t;

    typedef ndis::protocol::characteristics::close_adapter_complete_handler_t
      close_adapter_complete_handler_t;

    typedef ndis::protocol::characteristics::send_complete_handler_t
      send_complete_handler_t;

    typedef ndis::protocol::characteristics::wan_send_complete_handler_t
      wan_send_complete_handler_t;

    typedef ndis::protocol::characteristics::transfer_data_complete_handler_t
      transfer_data_complete_handler_t;

    typedef ndis::protocol::characteristics::wan_transfer_data_complete_handler_t
      wan_transfer_data_complete_handler_t;

    typedef ndis::protocol::characteristics::reset_complete_handler_t
      reset_complete_handler_t;

    typedef ndis::protocol::characteristics::request_complete_handler_t
      request_complete_handler_t;

    typedef ndis::protocol::characteristics::receive_handler_t
      receive_handler_t;

    typedef ndis::protocol::characteristics::wan_receive_handler_t
      wan_receive_handler_t;

    typedef ndis::protocol::characteristics::receive_complete_handler_t
      receive_complete_handler_t;

    typedef ndis::protocol::characteristics::status_handler_t
      status_handler_t;

    typedef ndis::protocol::characteristics::status_complete_handler_t
      status_complete_handler_t;

    typedef ndis::protocol::characteristics::receive_packet_handler_t
      receive_packet_handler_t;

    typedef ndis::protocol::characteristics::bind_adapter_handler_t
      bind_adapter_handler_t;

    typedef ndis::protocol::characteristics::unbind_adapter_handler_t
      unbind_adapter_handler_t;

    typedef ndis::protocol::characteristics::pnp_event_handler_t
      pnp_event_handler_t;

    typedef ndis::protocol::characteristics::unload_handler_t
      unload_handler_t;

    typedef ndis::protocol::characteristics::co_send_complete_handler_t
      co_send_complete_handler_t;

    typedef ndis::protocol::characteristics::co_status_handler_t
      co_status_handler_t;

    typedef ndis::protocol::characteristics::co_receive_packet_handler_t
      co_receive_packet_handler_t;

    typedef ndis::protocol::characteristics::co_af_register_notify_handler_t
      co_af_register_notify_handler_t;

    typedef
    ndis::status::type __stdcall
      send_handler_t(
        ndis::handle    NdisBindingHandle,
        ndis::packet *  Packet
        );

    typedef
    ndis::status::type __stdcall
      wan_send_handler_t(
        ndis::handle    NdisBindingHandle,
        ndis::handle    LinkHandle,
        oid *           Packet
        );

    typedef
    ndis::status::type __stdcall
      transfer_data_handler_t(
        ndis::handle    NdisBindingHandle,
        ndis::handle    MacReceiveContext,
        unsigned        ByteOffset,
        unsigned        BytesToTransfer,
        ndis::packet *  Packet,
        unsigned *      BytesTransferred
        );

    typedef
    void __stdcall
      send_packets_handler_t(
        ndis::handle    MiniportAdapterContext,
        ndis::packet *  PacketArray[],
        unsigned        NumberOfPackets
        );

    typedef
    ndis::status::type __stdcall
      reset_handler_t (
        ndis::handle    NdisBindingHandle
        );

    typedef
    ndis::status::type __stdcall
      request_handler_t(
        ndis::handle    NdisBindingHandle,
        const request * NdisRequest
        );

    struct miniport_block;

    struct queued_close
    {
      int16_t         Status;
      work_queue_item WorkItem;
    };

    struct common_open_block
    {
      void *              MacHandle;
      handle              BindingHandle;
      miniport_block *    MiniportHandle;
      protocol::block *   ProtocolHandle;
      protocol::handle    ProtocolBindingContext;
      common_open_block * MiniportNextOpen;
      common_open_block * ProtocolNextOpen;
      void *              MiniportAdapterContext;
      uint8_t             Reserved1;
      uint8_t             Reserved2;
      uint8_t             Reserved3;
      uint8_t             Reserved4;
      string *            BindDeviceName;
      uint32_t            Reserved5;
      string *            RootDeviceName;
      union
      {
        send_handler_t *                  SendHandler;
        wan_send_handler_t *              WanSendHandler;
      };
      transfer_data_handler_t *           TransferDataHandler;

      send_complete_handler_t *           SendCompleteHandler;
      transfer_data_complete_handler_t *  TransferDataCompleteHandler;
      receive_handler_t *                 ReceiveHandler;
      receive_complete_handler_t *        ReceiveCompleteHandler;
      wan_receive_handler_t *             WanReceiveHandler;
      request_complete_handler_t *        RequestCompleteHandler;

      receive_packet_handler_t *          ReceivePacketHandler;
      send_packets_handler_t *            SendPacketsHandler;

      reset_handler_t *                   ResetHandler;
      request_handler_t *                 RequestHandler;
      reset_complete_handler_t *          ResetCompleteHandler;
      status_handler_t *                  StatusHandler;
      status_complete_handler_t *         StatusCompleteHandler;

      uint32_t            Flags;
      int32_t             References;
      kspin_lock          SpinLock;
      void *              FilterHandle;
      uint32_t            ProtocolOptions;
      uint16_t            CurrentLookahead;
      uint16_t            ConnectDampTicks;
      uint16_t            DisconnectDampTicks;
      int  (__stdcall *   WSendHandler)(void*, packet *, uint32_t);
      int  (__stdcall *   WTransferDataHandler)(packet *, uint32_t *, void *, void *, uint32_t, uint32_t);
      void  (__stdcall *  WSendPacketsHandler)(void *, packet **, uint32_t);
      void  (__stdcall *  CancelSendPacketsHandler)(void *, void *);
      uint32_t            WakeUpEnable;
      kevent *            CloseCompleteEvent;
      queued_close        QC;
      int32_t             AfReferences;
      common_open_block * NextGlobalOpen;
      struct _NDIS_CO_AF_BLOCK* NextAf;
      int  (__stdcall *   MiniportCoCreateVcHandler)(void *, void *, void **);
      int  (__stdcall *   MiniportCoRequestHandler)(void *, void *, request *);
      int  (__stdcall *   CoCreateVcHandler)(void *, void *, void **);
      int  (__stdcall *   CoDeleteVcHandler)(void *);
      void  (__stdcall *  CmActivateVcCompleteHandler)(int, void *, void/*struct _CO_CALL_PARAMETERS*/*);
      void  (__stdcall *  CmDeactivateVcCompleteHandler)(int, void *);
      void  (__stdcall *  CoRequestCompleteHandler)(int, void *, void *, void *, request *);
      list_entry          ActiveVcHead;
      list_entry          InactiveVcHead;
      int32_t             PendingAfNotifications;
      kevent *            AfNotifyCompleteEvent;

    }; // struct common_open_block
    //STATIC_ASSERT(sizeof(common_open_block) == 0xf4);

    struct open_block : public common_open_block { };

    // Legacy miniport

    struct stats
    {
      uint64_t  StartTicks;
      uint64_t  DirectedPacketsOut;
      uint64_t  DirectedPacketsIn;
    };

    struct miniport_work_item
    {
      enum type
      {
        NdisWorkItemRequest           = 0x0,
        NdisWorkItemSend              = 0x1,
        NdisWorkItemReturnPackets     = 0x2,
        NdisWorkItemResetRequested    = 0x3,
        NdisWorkItemResetInProgress   = 0x4,
        NdisWorkItemReserved          = 0x5,
        NdisWorkItemMiniportCallback  = 0x6,
        NdisMaxWorkItems              = 0x7,
      };
      single_list_entry   Link;
      type                WorkItemType;
      void*               WorkItemContext;
    };

    typedef
    void __stdcall
      filter_packet_indication_handler_t(
        const ndis::handle    Miniport,
        const ndis::packet *  PacketArray[],
        uint32_t              NumberOfPackets
        );

    struct x_filter;
    typedef x_filter eth_filter;

    typedef
    void __stdcall
      eth_rcv_indicate_handler_t(
        const eth_filter *  Filter,
        const ndis::handle  MacReceiveContext,
        const char *        Address,
        const void *        HeaderBuffer,
        uint32_t            HeaderBufferSize,
        const void *        LookaheadBuffer,
        uint32_t            LookaheadBufferSize,
        uint32_t            PacketSize
        );

    //typedef send_complete_handler_t m_send_complete_handler_t;
    typedef
    void __stdcall
      m_send_complete_handler_t(
        const ndis::handle        MiniportAdapterHandle,
        const ndis::packet *      Packet,
        const ndis::status::type  Status
        );

    //typedef transfer_data_complete_handler_t m_td_complete_handler_t;
    typedef
    void __stdcall
      m_td_complete_handler_t(
        const ndis::handle        MiniportAdapterHandle,
        const ndis::packet *      Packet,
        const ndis::status::type  Status,
        uint32_t                  BytesTransferred
        );

#if 0
    enum interface_type
    {
      NdisInterfaceInternal = Internal,
      NdisInterfaceIsa = Isa,
      NdisInterfaceEisa = Eisa,
      NdisInterfaceMca = MicroChannel,
      NdisInterfaceTurboChannel = TurboChannel,
      NdisInterfacePci = PCIBus,
      NdisInterfacePcMcia = PCMCIABus,
      NdisInterfaceCBus = CBus,
      NdisInterfaceMPIBus = MPIBus,
      NdisInterfaceMPSABus = MPSABus,
      NdisInterfaceProcessorInternal = ProcessorInternal,
      NdisInterfaceInternalPowerBus = InternalPowerBus,
      NdisInterfacePNPISABus = PNPISABus,
      NdisInterfacePNPBus = PNPBus,
      NdisInterfaceUSB,
      NdisInterfaceIrda,
      NdisInterface1394,
      NdisMaximumInterfaceType
    };
#endif

    enum pnp_device_state
    {
        NdisPnPDeviceAdded,
        NdisPnPDeviceStarted,
        NdisPnPDeviceQueryStopped,
        NdisPnPDeviceStopped,
        NdisPnPDeviceQueryRemoved,
        NdisPnPDeviceRemoved,
        NdisPnPDeviceSurpriseRemoved
    };

    struct miniport_block
    {
      /*<thisrel this+0x0>*/ /*|0x4|*/ void* Signature;
      /*<thisrel this+0x4>*/ /*|0x4|*/ miniport_block * NextMiniport;
      /*<thisrel this+0x8>*/ /*|0x4|*/ ndis::m_driver_block * DriverHandle;
      /*<thisrel this+0xc>*/ /*|0x4|*/ ndis::handle     MiniportAdapterContext;
      /*<thisrel this+0x10>*/ /*|0x8|*/ string MiniportName;
      /*<thisrel this+0x18>*/ /*|0x4|*/ struct _NDIS_BIND_PATHS* BindPaths;
      /*<thisrel this+0x1c>*/ /*|0x4|*/ void* OpenQueue;
      /*<thisrel this+0x20>*/ /*|0x8|*/ reference ShortRef;
      /*<thisrel this+0x28>*/ /*|0x4|*/ void* DeviceContext;
      /*<thisrel this+0x2c>*/ /*|0x1|*/ unsigned char Padding1;
      /*<thisrel this+0x2d>*/ /*|0x1|*/ unsigned char LockAcquired;
      /*<thisrel this+0x2e>*/ /*|0x1|*/ unsigned char PmodeOpens;
      /*<thisrel this+0x2f>*/ /*|0x1|*/ unsigned char AssignedProcessor;
      /*<thisrel this+0x30>*/ /*|0x4|*/ unsigned long Lock;
      /*<thisrel this+0x34>*/ /*|0x4|*/ request* MediaRequest;
      /*<thisrel this+0x38>*/ /*|0x4|*/ struct _NDIS_MINIPORT_INTERRUPT* Interrupt;
      /*<thisrel this+0x3c>*/ /*|0x4|*/ unsigned long Flags;
      /*<thisrel this+0x40>*/ /*|0x4|*/ unsigned long PnPFlags;
      /*<thisrel this+0x44>*/ /*|0x8|*/ list_entry PacketList;
      /*<thisrel this+0x4c>*/ /*|0x4|*/ packet* FirstPendingPacket;
      /*<thisrel this+0x50>*/ /*|0x4|*/ packet* ReturnPacketsQueue;
      /*<thisrel this+0x54>*/ /*|0x4|*/ unsigned long RequestBuffer;
      /*<thisrel this+0x58>*/ /*|0x4|*/ void* SetMCastBuffer;
      /*<thisrel this+0x5c>*/ /*|0x4|*/ miniport_block * PrimaryMiniport;
      /*<thisrel this+0x60>*/ /*|0x4|*/ void* WrapperContext;
      /*<thisrel this+0x64>*/ /*|0x4|*/ void* BusDataContext;
      /*<thisrel this+0x68>*/ /*|0x4|*/ unsigned long PnPCapabilities;
      /*<thisrel this+0x6c>*/ /*|0x4|*/ struct _CM_RESOURCE_LIST* Resources;
      /*<thisrel this+0x70>*/ /*|0x48|*/ timer  WakeUpDpcTimer;
      /*<thisrel this+0xb8>*/ /*|0x8|*/ string BaseName;
      /*<thisrel this+0xc0>*/ /*|0x8|*/ string SymbolicLinkName;
      /*<thisrel this+0xc8>*/ /*|0x4|*/ unsigned long CheckForHangSeconds;
      /*<thisrel this+0xcc>*/ /*|0x2|*/ unsigned short CFHangTicks;
      /*<thisrel this+0xce>*/ /*|0x2|*/ unsigned short CFHangCurrentTick;
      /*<thisrel this+0xd0>*/ /*|0x4|*/ int ResetStatus;
      /*<thisrel this+0xd4>*/ /*|0x4|*/ void* ResetOpen;
      union {
      /*<thisrel this+0xd8>*/ /*|0x4|*/ eth_filter * EthDB;
      /*<thisrel this+0xd8>*/ /*|0x4|*/ eth_filter * NullDB;
      };
      /*<thisrel this+0xdc>*/ /*|0x4|*/ eth_filter * TrDB;
      /*<thisrel this+0xe0>*/ /*|0x4|*/ eth_filter * FddiDB;
      /*<thisrel this+0xe4>*/ /*|0x4|*/ struct _ARC_FILTER* ArcDB;
      /*<thisrel this+0xe8>*/ /*|0x4|*/ filter_packet_indication_handler_t * PacketIndicateHandler;
      /*<thisrel this+0xec>*/ /*|0x4|*/ m_send_complete_handler_t * SendCompleteHandler;
      /*<thisrel this+0xf0>*/ /*|0x4|*/ void  (__stdcall*SendResourcesHandler)(void*);
      /*<thisrel this+0xf4>*/ /*|0x4|*/ void  (__stdcall*ResetCompleteHandler)(void*, int, unsigned char);
      /*<thisrel this+0xf8>*/ /*|0x4|*/ medium  MediaType;
      /*<thisrel this+0xfc>*/ /*|0x4|*/ unsigned long BusNumber;
      /*<thisrel this+0x100>*/ /*|0x4|*/ interface_type   BusType;
      /*<thisrel this+0x104>*/ /*|0x4|*/ interface_type   AdapterType;
      /*<thisrel this+0x108>*/ /*|0x4|*/ device_object* DeviceObject;
      /*<thisrel this+0x10c>*/ /*|0x4|*/ device_object* PhysicalDeviceObject;
      /*<thisrel this+0x110>*/ /*|0x4|*/ device_object* NextDeviceObject;
      /*<thisrel this+0x114>*/ /*|0x4|*/ struct _MAP_REGISTER_ENTRY* MapRegisters;
      /*<thisrel this+0x118>*/ /*|0x4|*/ struct _NDIS_AF_LIST* CallMgrAfList;
      /*<thisrel this+0x11c>*/ /*|0x4|*/ void* MiniportThread;
      /*<thisrel this+0x120>*/ /*|0x4|*/ void* SetInfoBuf;
      /*<thisrel this+0x124>*/ /*|0x2|*/ unsigned short SetInfoBufLen;
      /*<thisrel this+0x126>*/ /*|0x2|*/ unsigned short MaxSendPackets;
      /*<thisrel this+0x128>*/ /*|0x4|*/ int FakeStatus;
      /*<thisrel this+0x12c>*/ /*|0x4|*/ void* LockHandler;
      /*<thisrel this+0x130>*/ /*|0x4|*/ string* pAdapterInstanceName;
      /*<thisrel this+0x134>*/ /*|0x4|*/ struct _NDIS_MINIPORT_TIMER* TimerQueue;
      /*<thisrel this+0x138>*/ /*|0x4|*/ unsigned int MacOptions;
      /*<thisrel this+0x13c>*/ /*|0x4|*/ request * PendingRequest;
      /*<thisrel this+0x140>*/ /*|0x4|*/ unsigned int MaximumLongAddresses;
      /*<thisrel this+0x144>*/ /*|0x4|*/ unsigned int MaximumShortAddresses;
      /*<thisrel this+0x148>*/ /*|0x4|*/ unsigned int CurrentLookahead;
      /*<thisrel this+0x14c>*/ /*|0x4|*/ unsigned int MaximumLookahead;
      /*<thisrel this+0x150>*/ /*|0x4|*/ void  (__stdcall*HandleInterruptHandler)(void*);
      /*<thisrel this+0x154>*/ /*|0x4|*/ void  (__stdcall*DisableInterruptHandler)(void*);
      /*<thisrel this+0x158>*/ /*|0x4|*/ void  (__stdcall*EnableInterruptHandler)(void*);
      /*<thisrel this+0x15c>*/ /*|0x4|*/ void  (__stdcall*SendPacketsHandler)(void*, packet**, unsigned int);
      /*<thisrel this+0x160>*/ /*|0x4|*/ unsigned char  (__stdcall*DeferredSendHandler)(miniport_block*);
      /*<thisrel this+0x164>*/ /*|0x4|*/ eth_rcv_indicate_handler_t * EthRxIndicateHandler;
      /*<thisrel this+0x168>*/ /*|0x4|*/ void  (__stdcall*TrRxIndicateHandler)(struct _X_FILTER*, void*, void*, unsigned int, void*, unsigned int, unsigned int);
      /*<thisrel this+0x16c>*/ /*|0x4|*/ void  (__stdcall*FddiRxIndicateHandler)(struct _X_FILTER*, void*, char*, unsigned int, void*, unsigned int, void*, unsigned int, unsigned int);
      /*<thisrel this+0x170>*/ /*|0x4|*/ void  (__stdcall*EthRxCompleteHandler)(struct _X_FILTER*);
      /*<thisrel this+0x174>*/ /*|0x4|*/ void  (__stdcall*TrRxCompleteHandler)(struct _X_FILTER*);
      /*<thisrel this+0x178>*/ /*|0x4|*/ void  (__stdcall*FddiRxCompleteHandler)(struct _X_FILTER*);
      /*<thisrel this+0x17c>*/ /*|0x4|*/ void  (__stdcall*StatusHandler)(void*, int, void*, unsigned int);
      /*<thisrel this+0x180>*/ /*|0x4|*/ void  (__stdcall*StatusCompleteHandler)(void*);
      /*<thisrel this+0x184>*/ /*|0x4|*/ m_td_complete_handler_t * TDCompleteHandler;
      /*<thisrel this+0x188>*/ /*|0x4|*/ void  (__stdcall*QueryCompleteHandler)(void*, int);
      /*<thisrel this+0x18c>*/ /*|0x4|*/ void  (__stdcall*SetCompleteHandler)(void*, int);
      /*<thisrel this+0x190>*/ /*|0x4|*/ void  (__stdcall*WanSendCompleteHandler)(void*, void*, int);
      /*<thisrel this+0x194>*/ /*|0x4|*/ void  (__stdcall*WanRcvHandler)(int*, void*, void*, unsigned char*, unsigned long);
      /*<thisrel this+0x198>*/ /*|0x4|*/ void  (__stdcall*WanRcvCompleteHandler)(void*, void*);
      /*<thisrel this+0x19c>*/ /*|0x4|*/ miniport_block * NextGlobalMiniport;
      /*<thisrel this+0x1a0>*/ /*|0x1c|*/ single_list_entry WorkQueue[7];
      /*<thisrel this+0x1bc>*/ /*|0x18|*/ single_list_entry SingleWorkItems[6];
      /*<thisrel this+0x1d4>*/ /*|0x1|*/ unsigned char SendFlags;
      /*<thisrel this+0x1d5>*/ /*|0x1|*/ unsigned char TrResetRing;
      /*<thisrel this+0x1d6>*/ /*|0x1|*/ unsigned char ArcnetAddress;
      /*<thisrel this+0x1d7>*/ /*|0x1|*/ unsigned char XState;
      union {
      /*<thisrel this+0x1d8>*/ /*|0x4|*/ struct _NDIS_ARC_BUF* ArcBuf;
      /*<thisrel this+0x1d8>*/ /*|0x4|*/ void* BusInterface;
      };
      /*<thisrel this+0x1dc>*/ /*|0x4|*/ struct _NDIS_LOG* Log;
      /*<thisrel this+0x1e0>*/ /*|0x4|*/ unsigned long SlotNumber;
      /*<thisrel this+0x1e4>*/ /*|0x4|*/ struct _CM_RESOURCE_LIST* AllocatedResources;
      /*<thisrel this+0x1e8>*/ /*|0x4|*/ struct _CM_RESOURCE_LIST* AllocatedResourcesTranslated;
      /*<thisrel this+0x1ec>*/ /*|0x4|*/ single_list_entry PatternList;
      /*<thisrel this+0x1f0>*/ /*|0x10|*/ char __PMCapabilities[0x10];//struct _NDIS_PNP_CAPABILITIES PMCapabilities;
      /*<thisrel this+0x200>*/ /*|0x40|*/ char __DeviceCaps[0x40];//struct _DEVICE_CAPABILITIES DeviceCaps;
      /*<thisrel this+0x240>*/ /*|0x4|*/ unsigned long WakeUpEnable;
      /*<thisrel this+0x244>*/ /*|0x4|*/ device_power_state CurrentDevicePowerState;
      /*<thisrel this+0x248>*/ /*|0x4|*/ irp* pIrpWaitWake;
      /*<thisrel this+0x24c>*/ /*|0x4|*/ system_power_state WaitWakeSystemState;
      /*<thisrel this+0x250>*/ /*|0x8|*/ int64_t  VcIndex;
      /*<thisrel this+0x258>*/ /*|0x4|*/ unsigned long VcCountLock;
      /*<thisrel this+0x25c>*/ /*|0x8|*/ list_entry WmiEnabledVcs;
      /*<thisrel this+0x264>*/ /*|0x4|*/ struct _NDIS_GUID* pNdisGuidMap;
      /*<thisrel this+0x268>*/ /*|0x4|*/ struct _NDIS_GUID* pCustomGuidMap;
      /*<thisrel this+0x26c>*/ /*|0x2|*/ unsigned short VcCount;
      /*<thisrel this+0x26e>*/ /*|0x2|*/ unsigned short cNdisGuidMap;
      /*<thisrel this+0x270>*/ /*|0x2|*/ unsigned short cCustomGuidMap;
      /*<thisrel this+0x272>*/ /*|0x2|*/ unsigned short CurrentMapRegister;
      /*<thisrel this+0x274>*/ /*|0x4|*/ kevent* AllocationEvent;
      /*<thisrel this+0x278>*/ /*|0x2|*/ unsigned short BaseMapRegistersNeeded;
      /*<thisrel this+0x27a>*/ /*|0x2|*/ unsigned short SGMapRegistersNeeded;
      /*<thisrel this+0x27c>*/ /*|0x4|*/ unsigned long MaximumPhysicalMapping;
      /*<thisrel this+0x280>*/ /*|0x48|*/ timer MediaDisconnectTimer;
      /*<thisrel this+0x2c8>*/ /*|0x2|*/ unsigned short MediaDisconnectTimeOut;
      /*<thisrel this+0x2ca>*/ /*|0x2|*/ unsigned short InstanceNumber;
      /*<thisrel this+0x2cc>*/ /*|0x10|*/ ndis::event   OpenReadyEvent;
      /*<thisrel this+0x2dc>*/ /*|0x4|*/ pnp_device_state PnPDeviceState;
      /*<thisrel this+0x2e0>*/ /*|0x4|*/ pnp_device_state OldPnPDeviceState;
      /*<thisrel this+0x2e4>*/ /*|0x4|*/ unsigned long  (__stdcall*SetBusData)(void*, unsigned long, void*, unsigned long, unsigned long);
      /*<thisrel this+0x2e8>*/ /*|0x4|*/ unsigned long  (__stdcall*GetBusData)(void*, unsigned long, void*, unsigned long, unsigned long);
      /*<thisrel this+0x2ec>*/ /*|0x20|*/ kdpc  DeferredDpc;
      /*<thisrel this+0x310>*/ /*|0x18|*/ stats NdisStats;
      /*<thisrel this+0x328>*/ /*|0x80|*/ packet* IndicatedPacket[32];
      /*<thisrel this+0x3a8>*/ /*|0x4|*/ kevent* RemoveReadyEvent;
      /*<thisrel this+0x3ac>*/ /*|0x4|*/ kevent* AllOpensClosedEvent;
      /*<thisrel this+0x3b0>*/ /*|0x4|*/ kevent* AllRequestsCompletedEvent;
      /*<thisrel this+0x3b4>*/ /*|0x4|*/ unsigned long InitTimeMs;
      /*<thisrel this+0x3b8>*/ /*|0x48|*/ miniport_work_item  WorkItemBuffer[6];
      /*<thisrel this+0x400>*/ /*|0x4|*/ struct _DMA_ADAPTER* SystemAdapterObject;
      /*<thisrel this+0x404>*/ /*|0x4|*/ unsigned long DriverVerifyFlags;
      /*<thisrel this+0x408>*/ /*|0x4|*/ struct _OID_LIST* OidList;
      /*<thisrel this+0x40c>*/ /*|0x2|*/ unsigned short InternalResetCount;
      /*<thisrel this+0x40e>*/ /*|0x2|*/ unsigned short MiniportResetCount;
      /*<thisrel this+0x410>*/ /*|0x2|*/ unsigned short MediaSenseConnectCount;
      /*<thisrel this+0x412>*/ /*|0x2|*/ unsigned short MediaSenseDisconnectCount;
      /*<thisrel this+0x414>*/ /*|0x4|*/ packet** xPackets;
      /*<thisrel this+0x418>*/ /*|0x4|*/ unsigned long UserModeOpenReferences;
      /*<thisrel this+0x41c>*/ /*|0x4|*/ void* SavedSendHandler;
      /*<thisrel this+0x41c>*/ /*|0x4|*/ void* SavedWanSendHandler;
      /*<thisrel this+0x420>*/ /*|0x4|*/ void  (__stdcall*SavedSendPacketsHandler)(void*, packet**, unsigned int);
      /*<thisrel this+0x424>*/ /*|0x4|*/ void  (__stdcall*SavedCancelSendPacketsHandler)(void*, void*);
      /*<thisrel this+0x428>*/ /*|0x4|*/ void  (__stdcall*WSendPacketsHandler)(void*, packet**, unsigned int);
      /*<thisrel this+0x42c>*/ /*|0x4|*/ unsigned long MiniportAttributes;
      /*<thisrel this+0x430>*/ /*|0x4|*/ struct _DMA_ADAPTER* SavedSystemAdapterObject;
      /*<thisrel this+0x434>*/ /*|0x2|*/ unsigned short NumOpens;
      /*<thisrel this+0x436>*/ /*|0x2|*/ unsigned short CFHangXTicks;
      /*<thisrel this+0x438>*/ /*|0x4|*/ unsigned long RequestCount;
      /*<thisrel this+0x43c>*/ /*|0x4|*/ unsigned long IndicatedPacketsCount;
      /*<thisrel this+0x440>*/ /*|0x4|*/ unsigned long PhysicalMediumType;
      /*<thisrel this+0x444>*/ /*|0x4|*/ request* LastRequest;
      /*<thisrel this+0x448>*/ /*|0x4|*/ long DmaAdapterRefCount;
      /*<thisrel this+0x44c>*/ /*|0x4|*/ void* FakeMac;
      /*<thisrel this+0x450>*/ /*|0x4|*/ unsigned long LockDbg;
      /*<thisrel this+0x454>*/ /*|0x4|*/ unsigned long LockDbgX;
      /*<thisrel this+0x458>*/ /*|0x4|*/ void* LockThread;
      /*<thisrel this+0x45c>*/ /*|0x4|*/ unsigned long InfoFlags;
      /*<thisrel this+0x460>*/ /*|0x4|*/ unsigned long TimerQueueLock;
      /*<thisrel this+0x464>*/ /*|0x4|*/ kevent* ResetCompletedEvent;
      /*<thisrel this+0x468>*/ /*|0x4|*/ kevent* QueuedBindingCompletedEvent;
      /*<thisrel this+0x46c>*/ /*|0x4|*/ kevent* DmaResourcesReleasedEvent;
      /*<thisrel this+0x470>*/ /*|0x4|*/ void  (__stdcall*SavedPacketIndicateHandler)(void*, packet**, unsigned int);
      /*<thisrel this+0x474>*/ /*|0x4|*/ unsigned long RegisteredInterrupts;
      /*<thisrel this+0x478>*/ /*|0x4|*/ struct _NPAGED_LOOKASIDE_LIST* SGListLookasideList;
      /*<thisrel this+0x47c>*/ /*|0x4|*/ unsigned long ScatterGatherListSize;
      /*<thisrel this+0x480>*/ /*|0x4|*/ kevent* WakeUpTimerEvent;
      /*<thisrel this+0x484>*/ /*|0x4|*/ void* SecurityDescriptor;
      /*<thisrel this+0x488>*/ /*|0x4|*/ unsigned long NumUserOpens;
      /*<thisrel this+0x48c>*/ /*|0x4|*/ unsigned long NumAdminOpens;
      /*<thisrel this+0x490>*/ /*|0xc|*/ reference  Ref;
    }; // struct miniport_block
    // <size 0x4a0>
    //STATIC_ASSERT(sizeof(miniport_block)==0x4a0);

    typedef
    status::type __stdcall
      w_send_handler_t(
        const ndis::handle    MiniportAdapterContext,
        const ndis::packet *  Packet,
        const uint32_t        Flags
        );

    typedef
    status::type __stdcall
      wm_send_handler_t(
        const ndis::handle        MiniportAdapterContext,
        const ndis::handle        NdisLinkHandle,
        const ndis::wan_packet *  Packet
        );

    typedef
    status::type __stdcall
      w_transfer_data_handler_t(
        ndis::packet *  Packet,
        size_t *        BytesTransferred,
        ndis::handle    MiniportAdapterContext,
        ndis::handle    MiniportReceiveContext,
        uint32_t        ByteOffset,
        size_t          BytesToTransfer
        );

    typedef
    status::type __stdcall
      wm_transfer_data_handler_t();

    typedef
    void __stdcall
      w_send_packet_handler_t(
        ndis::handle    MiniportAdapterContext,
        ndis::packet *  PacketArray[],
        uint32_t        NumberOfPackets
        );


    enum device_pnp_event
    {
      NdisDevicePnPEventQueryRemoved,
      NdisDevicePnPEventRemoved,
      NdisDevicePnPEventSurpriseRemoved,
      NdisDevicePnPEventQueryStopped,
      NdisDevicePnPEventStopped,
      NdisDevicePnPEventPowerProfileChanged,
      NdisDevicePnPEventFilterListChanged,
    };

    struct miniport_characteristics_50
    {
      /*<thisrel this+0x0>*/ /*|0x1|*/ unsigned char MajorNdisVersion;
      /*<thisrel this+0x1>*/ /*|0x1|*/ unsigned char MinorNdisVersion;
      /*<thisrel this+0x2>*/ /*|0x2|*/ unsigned short Filler;
      /*<thisrel this+0x4>*/ /*|0x4|*/ unsigned int Reserved;
      /*<thisrel this+0x8>*/ /*|0x4|*/ unsigned char  (__stdcall*CheckForHangHandler)(void*);
      /*<thisrel this+0xc>*/ /*|0x4|*/ void  (__stdcall*DisableInterruptHandler)(void*);
      /*<thisrel this+0x10>*/ /*|0x4|*/ void  (__stdcall*EnableInterruptHandler)(void*);
      /*<thisrel this+0x14>*/ /*|0x4|*/ void  (__stdcall*HaltHandler)(void*);
      /*<thisrel this+0x18>*/ /*|0x4|*/ void  (__stdcall*HandleInterruptHandler)(void*);
      /*<thisrel this+0x1c>*/ /*|0x4|*/ int  (__stdcall*InitializeHandler)(int*, unsigned int*, medium*, unsigned int, void*, void*);
      /*<thisrel this+0x20>*/ /*|0x4|*/ void  (__stdcall*ISRHandler)(unsigned char*, unsigned char*, void*);
      /*<thisrel this+0x24>*/ /*|0x4|*/ int  (__stdcall*QueryInformationHandler)(void*, unsigned long, void*, unsigned long, unsigned long*, unsigned long*);
      /*<thisrel this+0x28>*/ /*|0x4|*/ int  (__stdcall*ReconfigureHandler)(int*, void*, void*);
      /*<thisrel this+0x2c>*/ /*|0x4|*/ int  (__stdcall*ResetHandler)(unsigned char*, void*);
      union {
      /*<thisrel this+0x30>*/ /*|0x4|*/ w_send_handler_t *  SendHandler;
      /*<thisrel this+0x30>*/ /*|0x4|*/ wm_send_handler_t * WanSendHandler;
      };
      /*<thisrel this+0x34>*/ /*|0x4|*/ int  (__stdcall*SetInformationHandler)(void*, unsigned long, void*, unsigned long, unsigned long*, unsigned long*);
      union {
      /*<thisrel this+0x38>*/ /*|0x4|*/ w_transfer_data_handler_t * TransferDataHandler;
      /*<thisrel this+0x38>*/ /*|0x4|*/ wm_transfer_data_handler_t * WanTransferDataHandler;
      };
      /*<thisrel this+0x3c>*/ /*|0x4|*/ void  (__stdcall*ReturnPacketHandler)(void*, packet*);
      /*<thisrel this+0x40>*/ /*|0x4|*/ w_send_packet_handler_t * SendPacketsHandler;
      /*<thisrel this+0x44>*/ /*|0x4|*/ void  (__stdcall*AllocateCompleteHandler)(void*, void*, int64_t*, unsigned long, void*);
      /*<thisrel this+0x48>*/ /*|0x4|*/ int  (__stdcall*CoCreateVcHandler)(void*, void*, void**);
      /*<thisrel this+0x4c>*/ /*|0x4|*/ int  (__stdcall*CoDeleteVcHandler)(void*);
      /*<thisrel this+0x50>*/ /*|0x4|*/ int  (__stdcall*CoActivateVcHandler)(void*, void/*struct _CO_CALL_PARAMETERS*/*);
      /*<thisrel this+0x54>*/ /*|0x4|*/ int  (__stdcall*CoDeactivateVcHandler)(void*);
      /*<thisrel this+0x58>*/ /*|0x4|*/ void  (__stdcall*CoSendPacketsHandler)(void*, packet**, unsigned int);
      /*<thisrel this+0x5c>*/ /*|0x4|*/ int  (__stdcall*CoRequestHandler)(void*, void*, request*);
    };
    //STATIC_ASSERT(sizeof(miniport_characteristics_50)==0x60);

    struct miniport_characteristics_51 : public miniport_characteristics_50
    {
      /*<thisrel this+0x60>*/ /*|0x4|*/ void  (__stdcall*CancelSendPacketsHandler)(void*, void*);
      /*<thisrel this+0x64>*/ /*|0x4|*/ void  (__stdcall*PnPEventNotifyHandler)(void*, device_pnp_event, void*, unsigned long);
      /*<thisrel this+0x68>*/ /*|0x4|*/ void  (__stdcall*AdapterShutdownHandler)(void*);
      /*<thisrel this+0x6c>*/ /*|0x4|*/ void* Reserved1;
      /*<thisrel this+0x70>*/ /*|0x4|*/ void* Reserved2;
      /*<thisrel this+0x74>*/ /*|0x4|*/ void* Reserved3;
      /*<thisrel this+0x78>*/ /*|0x4|*/ void* Reserved4;
    };
    //STATIC_ASSERT(sizeof(miniport_characteristics_51)==0x7c);

    struct m_driver_block
    {
      /*<thisrel this+0x0>*/ /*|0x4|*/ m_driver_block* NextDriver;
      /*<thisrel this+0x4>*/ /*|0x4|*/ miniport_block* MiniportQueue;
      /*<thisrel this+0x8>*/ /*|0x4|*/ struct _NDIS_WRAPPER_HANDLE* NdisDriverInfo;
      /*<thisrel this+0xc>*/ /*|0x4|*/ struct _NDIS_PROTOCOL_BLOCK* AssociatedProtocol;
      /*<thisrel this+0x10>*/ /*|0x8|*/ list_entry DeviceList;
      /*<thisrel this+0x18>*/ /*|0x4|*/ struct _NDIS_PENDING_IM_INSTANCE* PendingDeviceList;
      /*<thisrel this+0x1c>*/ /*|0x4|*/ void  (__stdcall*UnloadHandler)(struct _DRIVER_OBJECT*);
    };
    //STATIC_ASSERT(sizeof(m_driver_block)==0x20);

    struct m_driver_block_50 : public m_driver_block
    {
      /*<thisrel this+0x20>*/ /*|0x7c|*/ miniport_characteristics_50 MiniportCharacteristics;
      /*<thisrel this+0x9c>*/ /*|0x10|*/ kevent MiniportsRemovedEvent;
      /*<thisrel this+0xac>*/ /*|0x8|*/ reference   Ref; //!
      /*<thisrel this+0xb4>*/ /*|0x2|*/ unsigned short Flags;
      /*<thisrel this+0xb8>*/ /*|0x20|*/ kmutant IMStartRemoveMutex;
      /*<thisrel this+0xd8>*/ /*|0x4|*/ unsigned long DriverVersion;
    };
    //STATIC_ASSERT(sizeof(m_driver_block_50)==0xc0);

    struct m_driver_block_51 : public m_driver_block
    {
      /*<thisrel this+0x20>*/ /*|0x7c|*/ miniport_characteristics_51 MiniportCharacteristics;
      /*<thisrel this+0x9c>*/ /*|0x10|*/ kevent MiniportsRemovedEvent;
      /*<thisrel this+0xac>*/ /*|0x8|*/ reference   Ref; //!
      /*<thisrel this+0xb4>*/ /*|0x2|*/ unsigned short Flags;
      /*<thisrel this+0xb8>*/ /*|0x20|*/ kmutant IMStartRemoveMutex;
      /*<thisrel this+0xd8>*/ /*|0x4|*/ unsigned long DriverVersion;
    };
    //STATIC_ASSERT(sizeof(m_driver_block_51)==0xdc);

    struct packet_pool
    {
      explicit packet_pool(uint32_t number_of_descriptors)
      {
        _allocate_pool(pool, number_of_descriptors);
      }

      ~packet_pool()
      {
        _free_pool(pool);
      }

      __forceinline
      ndis::packet * alloc();

      static __forceinline
      void free(ndis::packet * packet);

      static __forceinline
      status::type
        _allocate_pool(
          ndis::handle  & pool_handle,
          uint32_t        number_of_descriptors);

      static __forceinline
      void _free_pool(ndis::handle pool_handle);

      private:
        ndis::handle  pool;
    }; // class packet_pool

    // weird `warning C4100: '__flags' : unreferenced formal parameter'
    struct __suppress_warning_C4100 {  };

  private:

    static bind_adapter_handler_t   _bind_adapter_handler_stub {/**/}
    static unbind_adapter_handler_t _unbind_adapter_handler_stub {/**/}

};//class ndis


NTL__EXTERNAPI
ndis::status::type __stdcall
  NdisAllocateMemoryWithTag(
    void **   VirtualAddress,
    size_t    Length,
    uint32_t  Tag
    );

NTL__EXTERNAPI
void __stdcall
  NdisFreeMemory(
    void *    VirtualAddress,
    size_t    Length,
    uint32_t  MemoryFlags
    );

void * ndis::alloc(size_t length, uint32_t tag)
{
  void *  va;
  return ndis::status::success
          == NdisAllocateMemoryWithTag(&va, length, tag) ? va : 0;
}

void ndis::free(void * va, size_t length, uint32_t flags)
{
  NdisFreeMemory(va, length, flags);
}


NTL__EXTERNAPI
void __stdcall
  NdisRegisterProtocol(
    ndis::status::type                    * Status,
    ndis::protocol::handle                * NdisProtocolHandle,
    const ndis::protocol::characteristics * ProtocolCharacteristics,
    unsigned                                CharacteristicsLength
    );

NTL__EXTERNAPI
void __stdcall
  NdisDeregisterProtocol(
    ndis::status::type    * Status,
    ndis::protocol::handle  NdisProtocolHandle
    );


ndis::status::type
  ndis::protocol::_register(
    ndis::protocol::handle &                NdisProtocolHandle,
    const ndis::protocol::characteristics & ProtocolCharacteristics,
    ndis::status::type &                    status
    )
{
  NdisRegisterProtocol(&status, &NdisProtocolHandle,
                    &ProtocolCharacteristics, sizeof(ProtocolCharacteristics));
  return status;
}

ndis::status::type
  ndis::protocol::_deregister(
    ndis::protocol::handle  NdisProtocolHandle,
    ndis::status::type &    status
    )
{
  NdisDeregisterProtocol(&status, NdisProtocolHandle);
  return status;
}


NTL__EXTERNAPI
void __stdcall
  NdisAllocatePacketPool(
    ndis::status::type *  Status,
    ndis::handle *        PoolHandle,
    uint32_t              NumberOfDescriptors,
    uint32_t              ProtocolReservedLength
    );

NTL__EXTERNAPI
void __stdcall
  NdisFreePacketPool(ndis::handle PoolHandle);

NTL__EXTERNAPI
void __stdcall
  NdisAllocatePacket(
    ndis::status::type *  Status,
    ndis::packet * *      Packet,
    ndis::handle          PoolHandle
    );

NTL__EXTERNAPI
void __stdcall NdisFreePacket(ndis::packet *  Packet);

ndis::status::type
  ndis::packet_pool::_allocate_pool(
    ndis::handle  & pool_handle,
    uint32_t        number_of_descriptors)
{
  ndis::status::type  status;
  NdisAllocatePacketPool(&status, &pool_handle, number_of_descriptors,
                          sizeof(uintptr_t) * 4);
  return status;
}

void
  ndis::packet_pool::_free_pool(ndis::handle pool_handle)
{
  NdisFreePacketPool(pool_handle);
}

ndis::packet * ndis::packet_pool::alloc()
{
  ndis::status::type  status;
  ndis::packet *      packet;
  NdisAllocatePacket(&status, &packet, pool);
  return ndis::status::success == status ? packet : 0;
}

void ndis::packet::free()
{
  NdisFreePacket(this);
}


NTL__EXTERNAPI
void __stdcall
  NdisAllocateBufferPool(
    ndis::status::type *  Status,
    ndis::handle *        PoolHandle,
    uint32_t              NumberOfDescriptors
    );

NTL__EXTERNAPI
void __stdcall
  NdisFreeBufferPool(ndis::handle PoolHandle);


NTL__EXTERNAPI
void __stdcall
  NdisAllocateBuffer(
    ndis::status::type *  Status,
    ndis::buffer_ptr *    Buffer,
    ndis::handle          PoolHandle,
    void *                VirtualAddress,
    size_t                Length
    );

ndis::status::type
  ndis::buffer_pool::_allocate_pool(
    ndis::handle  & pool_handle,
    uint32_t        number_of_descriptors)
{
  ndis::status::type  status;
  NdisAllocateBufferPool(&status, &pool_handle, number_of_descriptors);
  return status;
}

void
  ndis::buffer_pool::_free_pool(ndis::handle pool_handle)
{
  NdisFreeBufferPool(pool_handle);
}

ndis::buffer *
  ndis::buffer_pool::alloc(
    void *                VirtualAddress,
    size_t                Length)
{
  ndis::status::type  status;
  ndis::buffer_ptr    buffer;
  NdisAllocateBuffer(&status, &buffer, pool, VirtualAddress, Length);
  return ndis::status::success == status ? buffer : 0;
}


NTL__EXTERNAPI
void __stdcall
  NdisOpenAdapter(
    ndis::status::type *  Status,
    ndis::status::type *  OpenErrorStatus,
    ndis::open_block * *  NdisBindingHandle,
    uint32_t *            SelectedMediumIndex,
    const ndis::medium    MediumArray[],
    uint32_t              MediumArraySize,
    ndis::handle          NdisProtocolHandle,
    ndis::handle          ProtocolBindingContext,
    ndis::string *        AdapterName,
    uint32_t              OpenOptions,
    const_ansi_string *   AddressingInformation  __optional
    );


NTL__EXTERNAPI
void __stdcall
  NdisCloseAdapter(
    ndis::status::type *  Status,
    ndis::open_block *    NdisBindingHandle
    );

}//namspace km
}//namespace ntl

#ifndef NTL__NO_AUTOLINK
# pragma comment(lib,    "ndis.lib")
#endif

#endif//#ifndef NTL__KM_NDIS
