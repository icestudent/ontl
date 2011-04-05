#pragma once

#include "../system_network.hxx"
#include "../../../../nt/win32_error.hxx"

namespace ntl { namespace network {

  namespace win32_error = ntl::nt::win32_error;

  namespace winsock
  {

    /************************************************************************/
    /* Types                                                                */
    /************************************************************************/
    typedef ntl::nt::legacy_handle legacy_handle;
    typedef uintptr_t socket;

#pragma warning(push, 4)
#pragma warning(disable: 4201)

    struct sockerror
    {
      enum type;

      static void set(int32_t code)
      {
        win32_error::set(static_cast<win32_error::error_type>(code));
      }

      static type get()
      {
        return static_cast<type>(win32_error::get());
      }

      sockerror()
        :err_(get())
      {}

      sockerror(int32_t error)
        :err_(static_cast<type>(error))
      {
        set(error);
      }

      sockerror& operator=(const int32_t error)
      {
        err_ = static_cast<type>(error);
        set(error);
      }

      operator type() const { return err_; }
      operator bool() const { return err_ == success; }
      bool operator!() const { return err_ != success; }

      type err_;

      enum type {
        success                    = 0,

        io_pending                 = win32_error::io_pending,
        io_incomplete              = win32_error::io_incomplete,
        invalid_handle             = win32_error::invalid_handle,
        invalid_parameter          = win32_error::invalid_parameter,
        not_enough_memory          = win32_error::not_enough_memory,
        operation_aborted          = win32_error::operation_aborted,

        base_error_code            = 10000,
        intr                       = base_error_code + 4,
        badf                       = base_error_code + 9,
        acces                      = base_error_code + 13,
        fault                      = base_error_code + 14,
        inval                      = base_error_code + 22,
        mfile                      = base_error_code + 24,
        wouldblock                 = base_error_code + 35,
        inprogress                 = base_error_code + 36,
        already                    = base_error_code + 37,
        notsock                    = base_error_code + 38,
        destaddrreq                = base_error_code + 39,
        msgsize                    = base_error_code + 40,
        prototype                  = base_error_code + 41,
        noprotoopt                 = base_error_code + 42,
        protonosupport             = base_error_code + 43,
        socktnosupport             = base_error_code + 44,
        opnotsupp                  = base_error_code + 45,
        pfnosupport                = base_error_code + 46,
        afnosupport                = base_error_code + 47,
        addrinuse                  = base_error_code + 48,
        addrnotavail               = base_error_code + 49,
        netdown                    = base_error_code + 50,
        netunreach                 = base_error_code + 51,
        netreset                   = base_error_code + 52,
        connaborted                = base_error_code + 53,
        connreset                  = base_error_code + 54,
        nobufs                     = base_error_code + 55,
        isconn                     = base_error_code + 56,
        notconn                    = base_error_code + 57,
        shutdown                   = base_error_code + 58,
        toomanyrefs                = base_error_code + 59,
        timedout                   = base_error_code + 60,
        connrefused                = base_error_code + 61,
        loop                       = base_error_code + 62,
        nametoolong                = base_error_code + 63,
        hostdown                   = base_error_code + 64,
        hostunreach                = base_error_code + 65,
        notempty                   = base_error_code + 66,
        proclim                    = base_error_code + 67,
        users                      = base_error_code + 68,
        dquot                      = base_error_code + 69,
        stale                      = base_error_code + 70,
        remote                     = base_error_code + 71,
        sysnotready                = base_error_code + 91,
        vernotsupported            = base_error_code + 92,
        notinitialised             = base_error_code + 93,
        discon                     = base_error_code + 101,
        nomore                     = base_error_code + 102,
        cancelled                  = base_error_code + 103,
        invalidproctable           = base_error_code + 104,
        invalidprovider            = base_error_code + 105,
        providerfailedinit         = base_error_code + 106,
        syscallfailure             = base_error_code + 107,
        service_not_found          = base_error_code + 108,
        type_not_found             = base_error_code + 109,
        no_more                    = base_error_code + 110,
        e_cancelled                = base_error_code + 111,
        refused                    = base_error_code + 112,
        host_not_found             = base_error_code + 1001,
        try_again                  = base_error_code + 1002,
        no_recovery                = base_error_code + 1003,
        no_data                    = base_error_code + 1004,
        qos_receivers              = base_error_code + 1005,
        qos_senders                = base_error_code + 1006,
        qos_no_senders             = base_error_code + 1007,
        qos_no_receivers           = base_error_code + 1008,
        qos_request_confirmed      = base_error_code + 1009,
        qos_admission_failure      = base_error_code + 1010,
        qos_policy_failure         = base_error_code + 1011,
        qos_bad_style              = base_error_code + 1012,
        qos_bad_object             = base_error_code + 1013,
        qos_traffic_ctrl_error     = base_error_code + 1014,
        qos_generic_error          = base_error_code + 1015,
        qos_eservicetype           = base_error_code + 1016,
        qos_eflowspec              = base_error_code + 1017,
        qos_eprovspecbuf           = base_error_code + 1018,
        qos_efilterstyle           = base_error_code + 1019,
        qos_efiltertype            = base_error_code + 1020,
        qos_efiltercount           = base_error_code + 1021,
        qos_eobjlength             = base_error_code + 1022,
        qos_eflowcount             = base_error_code + 1023,
        qos_eunkownpsobj           = base_error_code + 1024,
        qos_epolicyobj             = base_error_code + 1025,
        qos_eflowdesc              = base_error_code + 1026,
        qos_epsflowspec            = base_error_code + 1027,
        qos_epsfilterspec          = base_error_code + 1028,
        qos_esdmodeobj             = base_error_code + 1029,
        qos_eshaperateobj          = base_error_code + 1030,
        qos_reserved_petype        = base_error_code + 1031,
      };
    };

    namespace wsa
    {
      struct wsabuf_t
      {
        uint32_t  len;
        char*     buf;
      };

      struct overlapped
      {
        uintptr_t     Internal, InternalHigh;
        union {
          struct { 
            uint32_t  Offset, OffsetHigh;
          };
          void*       Pointer; //-V117
        };
        legacy_handle hEvent;

        typedef void __stdcall completion_routine_t(
          uint32_t    ErrorCode,
          uint32_t    Transferred,
          overlapped* Overlapped,
          uint32_t    Flags
          );
      };

      struct protocol_chain
      {
        int32_t       length;
        uint32_t      entries[7];
      };

      struct protocol_info
      {
        uint32_t      ServiceFlags[4], ProviderFlags;
        nt::guid          ProviderId;
        uint32_t      CatalogEntryId;
        protocol_chain* ProtocolChain;
        int32_t       Version, AddressFamily, MaxSockAddr, MinSockAddr;
        int32_t       SocketType;
        int32_t       Protocol, ProtocolMaxOffset, NetworkByteOrder;
        int32_t       SecurityScheme;
        uint32_t      MessageSize;
        uint32_t      ProviderReserved;
        wchar_t*      szProtocol[256];
      };

      struct wsadata_t {
        static const uint16_t CurrentVersion = 0x0202; // 2.2
        uint16_t  Version;
        uint16_t  HighVersion;
#ifdef _M_X64
        uint16_t  MaxSockets;
        uint16_t  MaxUdpDg;
        char*     VendorInfo;
        char      Description[256+1];
        char      SystemStatus[128+1];
#else
        char      Description[256+1];
        char      SystemStatus[128+1];
        uint16_t  MaxSockets;
        uint16_t  MaxUdpDg;
        char*     VendorInfo;
#endif
      };

    } // namespace wsa
#pragma warning(pop)

    /************************************************************************/
    /* Functions                                                            */
    /************************************************************************/
    typedef hostent* __stdcall gethostbyname_t(const char* name);
    typedef int __stdcall  getnameinfo_t(const sockaddr* sa, int salen, char* host, uint32_t hostlen, char* serv, uint32_t servlen, int flags);
    typedef int __stdcall  getaddrinfo_t(const char* nodename, const char* servname, const addrinfo* hints, addrinfo** result);
    typedef void __stdcall freeaddrinfo_t(addrinfo* ai);
    typedef socket __stdcall socket_t(int af, int type, int protocol);
    typedef int __stdcall shutdown_t(socket s, int how);
    typedef int __stdcall closesocket_t(socket s);
    typedef int __stdcall connect_t(socket s, const sockaddr* name, int namelen);
    typedef int __stdcall bind_t(socket s, const sockaddr* name, int namelen);
    typedef int __stdcall listen_t(socket s, int backlog);
    typedef int __stdcall send_t(socket s, const char* buf, int len, int flags);
    typedef int __stdcall recv_t(socket s, char* buf, int len, int flags);
    typedef int __stdcall sendto_t(socket s, const char* buf, int len, int flags, const sockaddr* name, int namelen);
    typedef int __stdcall recvfrom_t(socket s, char* buf, int len, int flags, sockaddr* from, int* namelen);
    typedef int __stdcall ioctlsocket_t(socket s, long cmd, unsigned long* argp);
    typedef int __stdcall getsockopt_t(socket s, int level, int optname, byte* optval, int* optlen);
    typedef int __stdcall setsockopt_t(socket s, int level, int optname, const byte* optval, int optlen);
    typedef const char* __stdcall inet_ntoa_t(const in_addr in);
    typedef uint32_t __stdcall inet_addr_t(const char* addr);
    typedef int gethostname_t(char* name, int namelen);

    typedef int __stdcall getsockname_t(socket s, sockaddr* addr, int* addrlen);
    typedef int __stdcall getpeername_t(socket s, sockaddr* addr, int* addrlen);

    namespace wsa
    {
      typedef int __stdcall  Startup_t(uint16_t version, wsadata_t& wsainfo);
      typedef int __stdcall  Cleanup_t();
      typedef int  __stdcall GetLastError_t();
      typedef void __stdcall SetLastError_t(int Error);
      typedef socket __stdcall SocketW_t(int af, int type, int protocol, const protocol_info* ProtocolInfo, uint32_t, 
        uint32_t Flags);
      typedef int __stdcall Connect_t(socket s, const sockaddr* name, int namelen, const wsabuf_t* CallerData, wsabuf_t* CalleeData, void* sQos, void* gQos);
      typedef int __stdcall Recv_t(socket s, wsabuf_t Buffers[], uint32_t BuffersCount, uint32_t* Received, 
        uint32_t* Flags, overlapped* Overlapped, overlapped::completion_routine_t* CompletionRoutine);

      typedef int __stdcall StringToAddressW_t(wchar_t* name, int family, const protocol_info* ProtocolInfo, sockaddr* address, int* addrlen);

    }

    struct functions_t
    {
      wsa::Startup_t* startup;
      wsa::Cleanup_t* cleanup;
      
      socket_t* socket;
      closesocket_t* closesocket;
      
      connect_t* connect;
      shutdown_t* shutdown;
      bind_t* bind;
      listen_t* listen;

      getsockname_t* getsockname;
      getpeername_t* getpeername;

      gethostname_t* gethostname;
      getnameinfo_t* getnameinfo;
      getaddrinfo_t* getaddrinfo;
      freeaddrinfo_t* freeaddrinfo;

      ioctlsocket_t* ioctlsocket;
      getsockopt_t* getsockopt;
      setsockopt_t* setsockopt;
      
      send_t* send;
      recv_t* recv;
      sendto_t* sendto;
      recvfrom_t* recvfrom;
      
      bool initialized;
    };


    class winsock_runtime
    {
    public:
      static functions_t* winsock_runtime::instance()
      {
        functions_t* funcs = std::__::static_storage<functions_t>::get_buffer();
        assert(funcs != nullptr);
        return funcs;
      }

      static void setup()
      {
        check_wsa(true);
      }
      static void cleanup()
      {
        check_wsa(false);
      }

    protected:
      static void bind_import(functions_t& funcs)
      {
        using namespace ntl::nt;
        peb::find_dll f(&peb::instance());
        const ntl::pe::image* ws = f("ws2_32.dll");
        if(!ws){
          typedef ntl::pe::image* __stdcall load_library_t(const char* dll);
          const ntl::pe::image* kernel = f("kernel32.dll");
          load_library_t* dlopen = kernel->find_export<load_library_t*>("LoadLibraryA");
          ws = dlopen("ws2_32.dll");
        }
        if(ws){
          std::memset(&funcs, 0, sizeof(functions_t));
          funcs.startup = ws->find_export<wsa::Startup_t*>("WSAStartup");
          funcs.cleanup = ws->find_export<wsa::Cleanup_t*>("WSACleanup");
          // funcs.inet_addr = ws->find_export<inet_addr_t*>("inet_addr");
          #define NTL_IMP(func) funcs. func = ws->find_export< func ## _t*>(#func);
          NTL_IMP(socket);
          NTL_IMP(closesocket);
          NTL_IMP(connect);
          NTL_IMP(shutdown);
          NTL_IMP(ioctlsocket);
          NTL_IMP(getsockopt);
          NTL_IMP(setsockopt);
          NTL_IMP(send);
          NTL_IMP(recv);
          NTL_IMP(sendto);
          NTL_IMP(recvfrom);
          NTL_IMP(getnameinfo);
          NTL_IMP(getaddrinfo);
          NTL_IMP(freeaddrinfo);
          NTL_IMP(gethostname);
          NTL_IMP(bind);
          NTL_IMP(listen);
          NTL_IMP(getsockname);
          NTL_IMP(getpeername);
          #undef NTL_IMP

          // check import
          const void **first = (const void**)&funcs, **last = (const void**)&funcs.initialized;
          bool ok = true;
          do{
            if(!*first){
              assert(("no function at",*first));
              ok = false;
            }
          }while(++first != last);
          funcs.initialized = true;
        }
      }

      static functions_t* check_wsa(bool init)
      {
        static int refcount = 0;
        functions_t* funcs = std::__::static_storage<functions_t>::get_buffer();
        if(init){
          if(refcount++ == 0){
            if(!funcs->initialized)
              bind_import(*funcs);
            wsa::wsadata_t info;
            if(funcs->startup)
              funcs->startup(wsa::wsadata_t::CurrentVersion, info);
          }
        }else{
          if(--refcount == 0){
            if(funcs->cleanup)
              funcs->cleanup();
          }
        }
        return funcs;
      }
    };


    class winsock_service_base
    {
    protected:
      static const int socket_error = -1;

      // false if error
      static bool check_error(std::error_code& ec, int re)
      {
        if(re != socket_error)
          ec.clear();
        else
          sockerror(ec);
        return re != socket_error;
      }
      static std::error_code success(std::error_code& ec)
      {
        return ec.clear(), ec;
      }
      static std::error_code sockerror(std::error_code& ec)
      {
        // TODO: map win32 errors to network
        const winsock::sockerror::type errc = winsock::sockerror::get();
        return ec = std::make_error_code(static_cast<std::tr2::network::error::error_type>(errc));
      }
    public:
      winsock_service_base()
      {
        winsock_runtime::setup();
      }
      void shutdown_service()
      {
        winsock_runtime::cleanup();
      }
    };

  } // winsock
}}
