/**\file*********************************************************************
 *                                                                     \brief
 *  Filter Manager
 *
 ****************************************************************************
 **/
#ifndef NTL__KM_FLTMGR
#define NTL__KM_FLTMGR

#include "handle.hxx"
#include "driver_object.hxx"
#include "irp.hxx"
#include "pool.hxx"

#ifdef SYSTEM_ERROR_SUPPORT
# include "../nt/system_error.hxx"
#endif

#include "../memory"

namespace ntl { namespace km { 

#ifdef SYSTEM_ERROR_SUPPORT
#pragma message("fltmgr.hxx(24) : error_code::message may not work in the kernel mode, i'm fix it later")
  using nt::make_error_code;
  using nt::make_error_condition;
#endif

  /// Minifilters
  namespace flt
  {
    struct object
    {
      enum flags {
        Draining = 0x1,
        Zombied = 0x2,
        Type_instance = 0x1000000,
        Type_filter = 0x2000000,
        Type_volume = 0x4000000,
      } Flags;
      __ntl_bitmask_type(flags, friend);

      uint32_t PointerCount;
      ex_rundown_ref RundownRef;
      list_entry PrimaryLink;
    };


    NTL__EXTERNAPI void __stdcall FltObjectDereference(object*);
    NTL__EXTERNAPI void __stdcall FltObjectReference(object*);

    namespace aux 
    {
      __forceinline void close(object* FltObject)
      {
        FltObjectDereference(FltObject);
      }

      __forceinline object* duplicate(object* FltObject)
      {
        FltObjectReference(FltObject);
        return FltObject;
      }
    }

    typedef ntl::basic_handle<object*, aux::close, aux::duplicate> filter_handle;

    struct instance;
    struct filter;
    struct volumn;

    // parameters
    struct io_security_context;
#ifdef _M_X64
#pragma pack(push,4)
#endif
    union parameters
    {
      struct {
        io_security_context* SecurityContext;
        uint32_t Options;
        uint16_t __ptr_align FileAttributes;
        uint16_t ShareAccess;
        uint32_t __ptr_align EaLength;

        void* EaBuffer;                 
        int64_t AllocationSize;   
      } Create;

      struct {
        io_security_context* SecurityContext;
        uint32_t Options;
        uint16_t __ptr_align Reserved;
        uint16_t ShareAccess;
        void* Parameters; 
      } CreatePipe;

      struct {
        io_security_context* SecurityContext;
        uint32_t Options;
        uint16_t __ptr_align Reserved;
        uint16_t ShareAccess;
        void* Parameters; 
      } CreateMailslot;

      struct {
        uint32_t Length;                   
        uint32_t __ptr_align Key;
        int64_t ByteOffset;       

        void* ReadBuffer;       
        mdl* MdlAddress;        
      } Read;

      struct {
        uint32_t Length;                   
        uint32_t __ptr_align Key;
        int64_t ByteOffset;       

        void* WriteBuffer;      
        mdl* MdlAddress;        
      } Write;

      struct {
        uint32_t Length;           
        file_information_class __ptr_align FileInformationClass; 
        void* InfoBuffer;       
      } QueryFileInformation;

      struct {
        uint32_t Length;
        file_information_class __ptr_align FileInformationClass;
        file_object* ParentOfTarget;
        union {
          struct {
            bool ReplaceIfExists;
            bool AdvanceOnly;
          };
          uint32_t ClusterCount;
          legacy_handle DeleteHandle;
        };
        void* InfoBuffer;       
      } SetFileInformation;

      struct {
        uint32_t Length;
        void* EaList;
        uint32_t EaListLength;
        uint32_t __ptr_align EaIndex;

        void* EaBuffer;         
        mdl* MdlAddress;        
      } QueryEa;

      struct {
        uint32_t Length;
        void* EaBuffer;         
        mdl* MdlAddress;        
      } SetEa;

      struct {
        uint32_t Length;
        fs_information_class __ptr_align FsInformationClass;
        void* VolumeBuffer;     
      } QueryVolumeInformation;

      struct {
        uint32_t Length;
        fs_information_class __ptr_align FsInformationClass;
        void* VolumeBuffer;     
      } SetVolumeInformation;

      union {
        struct {
          uint32_t Length;
          unicode_string* FileName;
          file_information_class FileInformationClass;
          uint32_t __ptr_align FileIndex;
          void* DirectoryBuffer;  
          mdl* MdlAddress;        
        } QueryDirectory;

        struct {
          uint32_t Length;
          uint32_t __ptr_align CompletionFilter;
          uint32_t __ptr_align Spare1;
          uint32_t __ptr_align Spare2;
          void* DirectoryBuffer;  
          mdl* MdlAddress;        
        } NotifyDirectory;

      } DirectoryControl;

      union {
        struct {
          vpb* Vpb;
          device_object* DeviceObject;
        } VerifyVolume;

        struct {
          uint32_t OutputBufferLength;
          uint32_t __ptr_align InputBufferLength;
          uint32_t __ptr_align FsControlCode;
        } Common;

        struct {
          uint32_t OutputBufferLength;
          uint32_t __ptr_align InputBufferLength;
          uint32_t __ptr_align FsControlCode;
          void* InputBuffer;
          void* OutputBuffer;
          mdl* OutputMdlAddress;
        } Neither;

        struct {
          uint32_t OutputBufferLength;
          uint32_t __ptr_align InputBufferLength;
          uint32_t __ptr_align FsControlCode;
          void* SystemBuffer;
        } Buffered;

        struct {
          uint32_t OutputBufferLength;
          uint32_t __ptr_align InputBufferLength;
          uint32_t __ptr_align FsControlCode;
          void* InputSystemBuffer;
          void* OutputBuffer;
          mdl* OutputMdlAddress;
        } Direct;

      } FileSystemControl;

      union {
        struct {
          uint32_t OutputBufferLength;
          uint32_t __ptr_align InputBufferLength;
          uint32_t __ptr_align IoControlCode;
        } Common;

        struct {
          uint32_t OutputBufferLength;
          uint32_t __ptr_align InputBufferLength;
          uint32_t __ptr_align IoControlCode;
          void* InputBuffer;
          void* OutputBuffer;
          mdl* OutputMdlAddress;
        } Neither;

        struct {
          uint32_t OutputBufferLength;
          uint32_t __ptr_align InputBufferLength;
          uint32_t __ptr_align IoControlCode;
          void* SystemBuffer;
        } Buffered;

        struct {
          uint32_t OutputBufferLength;
          uint32_t __ptr_align InputBufferLength;
          uint32_t __ptr_align IoControlCode;
          void* InputSystemBuffer;
          void* OutputBuffer;
          mdl* OutputMdlAddress;
        } Direct;

        struct {
          uint32_t OutputBufferLength;
          uint32_t __ptr_align InputBufferLength;
          uint32_t __ptr_align IoControlCode;
          void* InputBuffer;
          void* OutputBuffer;
        } FastIo;

      } DeviceIoControl;

      struct {
        int64_t* Length;
        uint32_t __ptr_align Key;
        int64_t ByteOffset;
        struct eprocess* ProcessId;        
        bool FailImmediately;    
        bool ExclusiveLock;      
      } LockControl;

      struct {
        uint32_t SecurityInformation;
        uint32_t __ptr_align Length;
        void* SecurityBuffer;   
        mdl* MdlAddress;        
      } QuerySecurity;

      struct {
        uint32_t SecurityInformation;
        security_descriptor* SecurityDescriptor;
      } SetSecurity;

      struct {
        uintptr_t ProviderId;
        void* DataPath;
        uint32_t BufferSize;
        void* Buffer;
      } WMI;

      struct {
        uint32_t Length;
        struct sid* StartSid;
        struct file_get_quota_information* SidList;
        uint32_t SidListLength;
        void* QuotaBuffer;      
        mdl* MdlAddress;        
      } QueryQuota;

      struct {
        uint32_t Length;
        void* QuotaBuffer;      
        mdl* MdlAddress;        
      } SetQuota;

      union {
        struct {
          struct cm_resource_list* AllocatedResources;
          struct cm_resource_list* AllocatedResourcesTranslated;
        } StartDevice;

        struct {
          uint32_t Type; 
        } QueryDeviceRelations;

        struct {
          const struct GUID *InterfaceType;
          uint16_t Size;
          uint16_t Version;
          struct interface_t* Interface;
          void* InterfaceSpecificData;
        } QueryInterface;

        struct {
          struct device_capabilities* Capabilities;
        } DeviceCapabilities;

        struct {
          struct io_resource_requirements_list* IoResourceRequirementList;
        } FilterResourceRequirements;

        struct {
          uint32_t WhichSpace;
          void* Buffer;
          uint32_t Offset;
          uint32_t __ptr_align Length;
        } ReadWriteConfig;

        struct {
          bool Lock;
        } SetLock;

        struct {
          uint32_t IdType; 
        } QueryId;

        struct {
          uint32_t DeviceTextType; 
          uint32_t __ptr_align LocaleId;
        } QueryDeviceText;

        struct {
          bool InPath;
          bool Reserved[3];
          uint32_t __ptr_align Type; 
        } UsageNotification;

      } Pnp;

      struct {
        uint32_t SyncType; 
        uint32_t PageProtection;
      } AcquireForSectionSynchronization;

      struct {
        int64_t* EndingOffset;
        eresource** ResourceToRelease;
      } AcquireForModifiedPageWriter;

      struct {
        eresource* ResourceToRelease;
      } ReleaseForModifiedPageWriter;

      struct {
        int64_t FileOffset;
        uint32_t Length;
        uint32_t __ptr_align LockKey;
        bool __ptr_align CheckForReadOperation;
      } FastIoCheckIfPossible;

      struct {
        irp* Irp;
        struct file_network_open_information* NetworkInformation;
      } NetworkQueryOpen;

      struct {
        int64_t FileOffset;
        uint32_t __ptr_align Length;
        uint32_t __ptr_align Key;
        mdl* *MdlChain;
      } MdlRead;

      struct {
        mdl* MdlChain;
      } MdlReadComplete;

      struct {
        int64_t FileOffset;
        uint32_t __ptr_align Length;
        uint32_t __ptr_align Key;
        mdl* *MdlChain;
      } PrepareMdlWrite;

      struct {
        int64_t FileOffset;
        mdl* MdlChain;
      } MdlWriteComplete;

      struct {
        uint32_t DeviceType;
      } MountVolume;

      struct {
        void* Argument1;
        void* Argument2;
        void* Argument3;
        void* Argument4;
        void* Argument5;
        int64_t Argument6;
      } Others;
    }; // parameters

#ifdef _M_X64
# pragma pack(pop)
#endif

    struct io_parameter_block
    {
      irp_flags           IrpFlags;
      uint8_t             MajorFunction;
      uint8_t 			      MinorFunction;
      uint8_t 			      OperationFlags;
      uint8_t 			      Reserved;
      file_object* 		    TargetFileObject;
      instance* 			    TargetInstance;
      parameters  	      Parameters;
    };

    struct callback_data
    {
      enum flags {
        reissue_mask           = 0x0000ffff,
        irp_operation           = 0x00000001,
        fast_io_operation       = 0x00000002,
        fs_filter_operation     = 0x00000004,
        system_buffer           = 0x00000008,

        generated_io            = 0x00010000,
        reissued_io             = 0x00020000,

        draining_io             = 0x00040000,
        post_operation          = 0x00080000,
        dirty                   = 0x80000000
      };
      __ntl_bitmask_type(flags, friend);

      uint32_t 					        Flags;
      struct ethread* const     Thread;
      io_parameter_block* const Iopb;
      io_status_block 			    IoStatus;
      struct tag_data_buffer*   TagData;

      union {
        struct {
          list_entry QueueLinks;
          void* 		QueueContext[2];
        };
        void* FilterContext[4];
      };
      kprocessor_mode 			RequestorMode;
    };

    struct file_name_info
    {
      enum options
      {
        normalized = 0x01,
        opened = 0x02,
        shortt = 0x03,

        query_default = 0x0100,
        query_cache_only = 0x0200,
        query_filesystem_only = 0x0300,
        query_always_allow_cache_lookup = 0x0400
      };

      enum parsed: uint16_t
      {
        final_component = 0x0001,
        extension  = 0x0002,
        stream = 0x0004,
        parent_dir = 0x0008
      };

      uint16_t  		  Size;
      parsed  		    NamesParsed;
      options  		    Format;
      unicode_string  Name;
      unicode_string  Volume;
      unicode_string  Share;
      unicode_string  Extension;
      unicode_string  Stream;
      unicode_string  FinalComponent;
      unicode_string  ParentDir;
    };

    NTL__EXTERNAPI ntstatus __stdcall FltGetFileNameInformation(callback_data*, file_name_info::options, file_name_info**);
    NTL__EXTERNAPI void __stdcall FltReleaseFileNameInformation(file_name_info*);

    struct related_objects
    {
      const uint16_t 			Size;
      const uint16_t 			TransactionContext; 
      filter*const  			Filter;
      volumn*const 		 	  Volume;
      filter_handle*const Instance;
      file_object*const   FileObject;
      struct ktransaction*const Transaction;
    };

    const uint32_t context_end = 0xffff;
    const uint8_t mj_operation_end = 0x80;

    struct context_registration
    {
      enum type:uint16_t {
        volume_context          = 0x0001,
        instance_context        = 0x0002,
        file_context            = 0x0004,
        stream_context          = 0x0008,
        streamhandle_context    = 0x0010,
        transaction_context     = 0x0020,
      };

      enum flags:uint16_t {
        none,
        no_exact_size_match
      };

      typedef void  __stdcall cleanup_t(void*, type);
      typedef void* __stdcall allocate_t(pool_type, size_t, type);
      typedef void  __stdcall free_t(void*, type);

      type        ContextType;
      flags       Flags;
      cleanup_t*  ContextCleanupCallback;
      size_t      Size;
      uint32_t    PoolTag;
      allocate_t* ContextAllocateCallback;
      free_t*     ContextFreeCallback;
      void*       Reserved;
    };	

    namespace preop
    {
      enum callback_status {
        success_with_callback,
        success_no_callback,
        pending,
        disallow_fastio,
        complete,
        synchronize
      };

      typedef callback_status __stdcall callback(callback_data*, related_objects*, void**);
    }

    namespace postop
    {
      enum callback_status {
        finished_processing,
        more_processing_required
      };

      typedef callback_status __stdcall callback(callback_data*, related_objects*, void*, uint32_t flags);
    }

    struct operation_registration
    {
      enum flags {
        skip_paging_io = 0x01,
        skip_cached_io = 0x02
      };
      uint8_t 			    MajorFunction;
      flags   			    Flags;

      preop::callback* 	PreOperation;
      postop::callback* PostOperation;

      void* 				    Reserved1;
    };

    struct filesystem_type
    {
      enum type {
        unknown,
        raw,
        ntfs,
        fat,
        cdfs,
        udfs,
        lanman,
        webdav,
        rdpdr,
        nfs,
        ms_netware,
        netware,
        bsudf,
        mup,
        rsfx,
        roxio_udf1,
        roxio_udf2,
        roxio_udf3,
        tacit,
        fs_rec,
        incd,
        incd_fat,
        exfat,
        psfs
      };
    };

    const uint16_t registration_version = 0x0200; 

    struct registration
    {
      uint16_t 			Size;
      uint16_t 			Version;
      uint32_t 			Flags;
      const context_registration* 	ContextRegistration;
      const operation_registration* OperationRegistration;

      typedef ntstatus __stdcall unload_callback(uint32_t flags);

      unload_callback* 	FilterUnloadCallback;

      typedef ntstatus __stdcall setup_callback(related_objects*, uint32_t flags, device_type, filesystem_type::type);

      setup_callback* 	InstanceSetupCallback;

      void*  				InstanceQueryTeardownCallback; /*PFLT_INSTANCE_QUERY_TEARDOWN_CALLBACK*/
      void*  				InstanceTeardownStartCallback; /*PFLT_INSTANCE_TEARDOWN_CALLBACK*/
      void*  				InstanceTeardownCompleteCallback; /*PFLT_INSTANCE_TEARDOWN_CALLBACK*/
      void*  				GenerateFileNameCallback; /*PFLT_GENERATE_FILE_NAME*/
      void*  				NormalizeNameComponentCallback; /*PFLT_NORMALIZE_NAME_COMPONENT*/
      void*  				NormalizeContextCleanupCallback; /*PFLT_NORMALIZE_CONTEXT_CLEANUP*/
#if FLT_MGR_LONGHORN
      void* 				TransactionNotificationCallback; /*PFLT_TRANSACTION_NOTIFICATION_CALLBACK*/
      void*  				NormalizeNameComponentExCallback; /*PFLT_NORMALIZE_NAME_COMPONENT_EX*/
#endif // FLT_MGR_LONGHORN
    };

    NTL__EXTERNAPI ntstatus __stdcall FltGetFilterFromName(const const_unicode_string&, filter_handle*);
    NTL__EXTERNAPI ntstatus __stdcall FltRegisterFilter(driver_object*, const registration*, filter_handle*);
    NTL__EXTERNAPI ntstatus __stdcall FltStartFiltering(object*);
    NTL__EXTERNAPI ntstatus __stdcall FltUnregisterFilter(object*);

    NTL__EXTERNAPI ntstatus __stdcall FltGetVolumeFromName(object* Filter, const const_unicode_string&, filter_handle* Volume);
    NTL__EXTERNAPI ntstatus __stdcall FltGetVolumeName(const object* Volume, unicode_string*, uint32_t*);


    //////////////////////////////////////////////////////////////////////////
    /// file_name_info pointer wrapper
    typedef std::unique_ptr<file_name_info, void (__stdcall *)(file_name_info*)> file_name_info_ptr;

    inline file_name_info_ptr get_file_name_info(callback_data* callback, file_name_info::options options, ntstatus* st = 0)
    {
      file_name_info* info = nullptr;
      ntstatus status = FltGetFileNameInformation(callback, options, &info);
      if(st)
        *st = status;
      return file_name_info_ptr(info, FltReleaseFileNameInformation);
    }

    //////////////////////////////////////////////////////////////////////////

    struct filter: filter_handle
    {
      /**
       *	@brief get filter object from its name (variant #1 of the object construction)
       *
       *	@param[out] Filter filter instance
       *	@param[in] FilterName %filter name
       *	@return status of operation
       **/
      static inline ntstatus from_name(filter& Filter, const const_unicode_string& FilterName)
      {
        return FltGetFilterFromName(FilterName, &Filter);
      }

      
      /**
       *	@brief register the new filter object
       *
       *	@param[out] Filter filter instance
       *	@param[in] Driver driver object for the minifilter driver
       *	@param[in] Registration minifilter driver registration structure
       *	@return status of operation
       **/
      static inline ntstatus register_filter(filter& Filter, driver_object& Driver, const registration& Registration)
      {
        return FltRegisterFilter(&Driver, &Registration, &Filter);
      }

      ntstatus start()
      {
        return FltStartFiltering(get());
      }

      void stop()
      {
        FltUnregisterFilter(get());
      }
    };


    struct volume: filter_handle
    {
      /**
       *	@brief get volume object by its name (variant #2 of the object construction)
       *
       *	@param[in] Filter filter instance
       *	@param[in] VolumeName volume name
       *	@param[out] st description
       *	@return  value
       **/
      explicit volume(filter& Filter, const const_unicode_string& VolumeName, ntstatus& st)
      {
        st = FltGetVolumeFromName(Filter.get(), VolumeName, this);
      }
#ifdef SYSTEM_ERROR_SUPPORT
      /// get volume object by its name (variant #3 of the object construction)
      __forceinline 
        explicit volume(filter& Filter, const const_unicode_string& VolumeName, std::error_code& ec = std::throws())
      {
        ntstatus st = FltGetVolumeFromName(&Filter, VolumeName, this);
        if(!success(st)){
          ec = make_error_code(st);
          if(ec == std::throws())
            __ntl_throw(std::system_error(ec));
        }
      }
#endif

      /// gets the volume name
      std::wstring name() const
      {
        uint32_t size; 
        std::wstring volume_name; 
        ntstatus st = FltGetVolumeName(get(), nullptr, &size); 
        if(st == status::buffer_too_small){
          volume_name.resize(size/sizeof(wchar_t));
          unicode_string us(volume_name); // wstring to unicode_string wrapper 
          st = FltGetVolumeName(get(), &us, nullptr); 
          if(!success(st))
            volume_name.clear();
        }
        return volume_name;
      }
    };


    struct instance: filter_handle
    {

    };



  } // flt

#ifdef NTL_TEST

  void inline test__implementation(driver_object& Driver)
  {
    flt::filter myfilter;
    flt::registration reg = {};
    ntstatus st = flt::filter::register_filter(myfilter, Driver, reg);
    if(success(st)){
      flt::volume volume(myfilter, L"C:", st);
      if(success(st)) {
        std::wstring volume_name = volume.name();
        assert(volume_name == L"\\Device\\HarddiskVolume1");
      }
    }
#ifdef SYSTEM_ERROR_SUPPORT
# if STLX__USE_EXCEPTIONS
    __ntl_try {
      flt::volume vol2(myfilter, L"Z:"); // if 'Z:' volume doesn't exists, 
      std::wstring name = vol2.name();
      assert(!name.empty());
    }
    __ntl_catch(std::system_error& e){ // exception handling (/EHsc) must me enabled
      dbg::error.printf("can't work with volume Z: '%s'\n", e.what());
    }
#else
    std::error_code ec; // ntstatus or other error code value
    flt::volume vol3(myfilter, L"Z:", ec); // if 'Z:' volume doesn't exists, 
    if(ec){
      // get volume from name failed
      dbg::error.printf("failed to get volume Z: 0x%X\n", ec.code()); // error_code::message may not work in the kernel mode, i'm fix it later
    }
    std::wstring name3 = vol3.name();
    assert(!name.empty());

#endif  // STLX__USE_EXCEPTIONS
#endif // SYSTEM_ERROR_SUPPORT
  }
#endif // NTL_TEST

}} // ntl::km

#endif//#ifndef NTL__KM_FLTMGR
