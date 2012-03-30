/**\file*********************************************************************
*                                                                     \brief
*  COM support
*
****************************************************************************
*/
#ifndef NTL__WIN_COM
#define NTL__WIN_COM
#pragma once

#include "../nt/basedef.hxx"

namespace ntl {

  namespace win {

    /**\addtogroup  winapi_types_support *** Win32 API support library **********
    *@{*/
    using ntl::nt::guid;
    using ntl::nt::systime_t;

    typedef long hresult;
    typedef const _GUID iid;
    typedef const _GUID clsid;

    /** COM support */
    namespace com 
    {
      inline bool failed(const hresult hr) { return hr < 0; }
      inline bool succeeded(const hresult hr) { return hr >= 0; }

      struct error
      {
        error()
          :hr(s_ok)
        {}
        error(const hresult hr)
          :hr(static_cast<Type>(hr))
        {}
        error& operator=(const hresult hr)
        {
          this->hr = static_cast<Type>(hr);
          return *this;
        }
        operator bool() const
        {
          return succeeded(hr);
        }
        bool operator!() const
        {
          return failed(hr);
        }

        //////////////////////////////////////////////////////////////////////////
        enum Type {
          s_ok                                                  = 0,
          s_false                                               = 1,
          e_unexpected                                          = 0x8000FFFF,
          e_notimpl                                             = 0x80004001,
          e_outofmemory                                         = 0x8007000E,
          e_invalidarg                                          = 0x80070057,
          e_nointerface                                         = 0x80004002,
          e_pointer                                             = 0x80004003,
          e_handle                                              = 0x80070006,
          e_abort                                               = 0x80004004,
          e_fail                                                = 0x80004005,
          e_accessdenied                                        = 0x80070005,
          e_pending                                             = 0x8000000A,
          co_e_init_tls                                         = 0x80004006,
          co_e_init_shared_allocator                            = 0x80004007,
          co_e_init_memory_allocator                            = 0x80004008,
          co_e_init_class_cache                                 = 0x80004009,
          co_e_init_rpc_channel                                 = 0x8000400A,
          co_e_init_tls_set_channel_control                     = 0x8000400B,
          co_e_init_tls_channel_control                         = 0x8000400C,
          co_e_init_unaccepted_user_allocator                   = 0x8000400D,
          co_e_init_scm_mutex_exists                            = 0x8000400E,
          co_e_init_scm_file_mapping_exists                     = 0x8000400F,
          co_e_init_scm_map_view_of_file                        = 0x80004010,
          co_e_init_scm_exec_failure                            = 0x80004011,
          co_e_init_only_single_threaded                        = 0x80004012,
          co_e_cant_remote                                      = 0x80004013,
          co_e_bad_server_name                                  = 0x80004014,
          co_e_wrong_server_identity                            = 0x80004015,
          co_e_ole1dde_disabled                                 = 0x80004016,
          co_e_runas_syntax                                     = 0x80004017,
          co_e_createprocess_failure                            = 0x80004018,
          co_e_runas_createprocess_failure                      = 0x80004019,
          co_e_runas_logon_failure                              = 0x8000401A,
          co_e_launch_permssion_denied                          = 0x8000401B,
          co_e_start_service_failure                            = 0x8000401C,
          co_e_remote_communication_failure                     = 0x8000401D,
          co_e_server_start_timeout                             = 0x8000401E,
          co_e_clsreg_inconsistent                              = 0x8000401F,
          co_e_iidreg_inconsistent                              = 0x80004020,
          co_e_not_supported                                    = 0x80004021,
          co_e_reload_dll                                       = 0x80004022,
          co_e_msi_error                                        = 0x80004023,
          co_e_attempt_to_create_outside_client_context         = 0x80004024,
          co_e_server_paused                                    = 0x80004025,
          co_e_server_not_paused                                = 0x80004026,
          co_e_class_disabled                                   = 0x80004027,
          co_e_clrnotavailable                                  = 0x80004028,
          co_e_async_work_rejected                              = 0x80004029,
          co_e_server_init_timeout                              = 0x8000402A,
          co_e_no_secctx_in_activate                            = 0x8000402B,
          co_e_tracker_config                                   = 0x80004030,
          co_e_threadpool_config                                = 0x80004031,
          co_e_sxs_config                                       = 0x80004032,
          co_e_malformed_spn                                    = 0x80004033,
          ole_e_oleverb                                         = 0x80040000,
          ole_e_advf                                            = 0x80040001,
          ole_e_enum_nomore                                     = 0x80040002,
          ole_e_advisenotsupported                              = 0x80040003,
          ole_e_noconnection                                    = 0x80040004,
          ole_e_notrunning                                      = 0x80040005,
          ole_e_nocache                                         = 0x80040006,
          ole_e_blank                                           = 0x80040007,
          ole_e_classdiff                                       = 0x80040008,
          ole_e_cant_getmoniker                                 = 0x80040009,
          ole_e_cant_bindtosource                               = 0x8004000A,
          ole_e_static                                          = 0x8004000B,
          ole_e_promptsavecancelled                             = 0x8004000C,
          ole_e_invalidrect                                     = 0x8004000D,
          ole_e_wrongcompobj                                    = 0x8004000E,
          ole_e_invalidhwnd                                     = 0x8004000F,
          ole_e_not_inplaceactive                               = 0x80040010,
          ole_e_cantconvert                                     = 0x80040011,
          ole_e_nostorage                                       = 0x80040012,
          dv_e_formatetc                                        = 0x80040064,
          dv_e_dvtargetdevice                                   = 0x80040065,
          dv_e_stgmedium                                        = 0x80040066,
          dv_e_statdata                                         = 0x80040067,
          dv_e_lindex                                           = 0x80040068,
          dv_e_tymed                                            = 0x80040069,
          dv_e_clipformat                                       = 0x8004006A,
          dv_e_dvaspect                                         = 0x8004006B,
          dv_e_dvtargetdevice_size                              = 0x8004006C,
          dv_e_noiviewobject                                    = 0x8004006D,
          dragdrop_e_notregistered                              = 0x80040100,
          dragdrop_e_alreadyregistered                          = 0x80040101,
          dragdrop_e_invalidhwnd                                = 0x80040102,
          class_e_noaggregation                                 = 0x80040110,
          class_e_classnotavailable                             = 0x80040111,
          class_e_notlicensed                                   = 0x80040112,
          view_e_draw                                           = 0x80040140,
          regdb_e_readregdb                                     = 0x80040150,
          regdb_e_writeregdb                                    = 0x80040151,
          regdb_e_keymissing                                    = 0x80040152,
          regdb_e_invalidvalue                                  = 0x80040153,
          regdb_e_classnotreg                                   = 0x80040154,
          regdb_e_iidnotreg                                     = 0x80040155,
          regdb_e_badthreadingmodel                             = 0x80040156,
          cat_e_catidnoexist                                    = 0x80040160,
          cat_e_nodescription                                   = 0x80040161,
          cs_e_package_notfound                                 = 0x80040164,
          cs_e_not_deletable                                    = 0x80040165,
          cs_e_class_notfound                                   = 0x80040166,
          cs_e_invalid_version                                  = 0x80040167,
          cs_e_no_classstore                                    = 0x80040168,
          cs_e_object_notfound                                  = 0x80040169,
          cs_e_object_already_exists                            = 0x8004016A,
          cs_e_invalid_path                                     = 0x8004016B,
          cs_e_network_error                                    = 0x8004016C,
          cs_e_admin_limit_exceeded                             = 0x8004016D,
          cs_e_schema_mismatch                                  = 0x8004016E,
          cs_e_internal_error                                   = 0x8004016F,
          cache_e_nocache_updated                               = 0x80040170,
          oleobj_e_noverbs                                      = 0x80040180,
          oleobj_e_invalidverb                                  = 0x80040181,
          inplace_e_notundoable                                 = 0x800401A0,
          inplace_e_notoolspace                                 = 0x800401A1,
          convert10_e_olestream_get                             = 0x800401C0,
          convert10_e_olestream_put                             = 0x800401C1,
          convert10_e_olestream_fmt                             = 0x800401C2,
          convert10_e_olestream_bitmap_to_dib                   = 0x800401C3,
          convert10_e_stg_fmt                                   = 0x800401C4,
          convert10_e_stg_no_std_stream                         = 0x800401C5,
          convert10_e_stg_dib_to_bitmap                         = 0x800401C6,
          clipbrd_e_cant_open                                   = 0x800401D0,
          clipbrd_e_cant_empty                                  = 0x800401D1,
          clipbrd_e_cant_set                                    = 0x800401D2,
          clipbrd_e_bad_data                                    = 0x800401D3,
          clipbrd_e_cant_close                                  = 0x800401D4,
          mk_e_connectmanually                                  = 0x800401E0,
          mk_e_exceededdeadline                                 = 0x800401E1,
          mk_e_needgeneric                                      = 0x800401E2,
          mk_e_unavailable                                      = 0x800401E3,
          mk_e_syntax                                           = 0x800401E4,
          mk_e_noobject                                         = 0x800401E5,
          mk_e_invalidextension                                 = 0x800401E6,
          mk_e_intermediateinterfacenotsupported                = 0x800401E7,
          mk_e_notbindable                                      = 0x800401E8,
          mk_e_notbound                                         = 0x800401E9,
          mk_e_cantopenfile                                     = 0x800401EA,
          mk_e_mustbotheruser                                   = 0x800401EB,
          mk_e_noinverse                                        = 0x800401EC,
          mk_e_nostorage                                        = 0x800401ED,
          mk_e_noprefix                                         = 0x800401EE,
          mk_e_enumeration_failed                               = 0x800401EF,
          co_e_notinitialized                                   = 0x800401F0,
          co_e_alreadyinitialized                               = 0x800401F1,
          co_e_cantdetermineclass                               = 0x800401F2,
          co_e_classstring                                      = 0x800401F3,
          co_e_iidstring                                        = 0x800401F4,
          co_e_appnotfound                                      = 0x800401F5,
          co_e_appsingleuse                                     = 0x800401F6,
          co_e_errorinapp                                       = 0x800401F7,
          co_e_dllnotfound                                      = 0x800401F8,
          co_e_errorindll                                       = 0x800401F9,
          co_e_wrongosforapp                                    = 0x800401FA,
          co_e_objnotreg                                        = 0x800401FB,
          co_e_objisreg                                         = 0x800401FC,
          co_e_objnotconnected                                  = 0x800401FD,
          co_e_appdidntreg                                      = 0x800401FE,
          co_e_released                                         = 0x800401FF,
          event_s_some_subscribers_failed                       = 0x00040200,
          event_e_all_subscribers_failed                        = 0x80040201,
          event_s_nosubscribers                                 = 0x00040202,
          event_e_querysyntax                                   = 0x80040203,
          event_e_queryfield                                    = 0x80040204,
          event_e_internalexception                             = 0x80040205,
          event_e_internalerror                                 = 0x80040206,
          event_e_invalid_per_user_sid                          = 0x80040207,
          event_e_user_exception                                = 0x80040208,
          event_e_too_many_methods                              = 0x80040209,
          event_e_missing_eventclass                            = 0x8004020A,
          event_e_not_all_removed                               = 0x8004020B,
          event_e_complus_not_installed                         = 0x8004020C,
          event_e_cant_modify_or_delete_unconfigured_object     = 0x8004020D,
          event_e_cant_modify_or_delete_configured_object       = 0x8004020E,
          event_e_invalid_event_class_partition                 = 0x8004020F,
          event_e_per_user_sid_not_logged_on                    = 0x80040210,
          xact_e_alreadyothersinglephase                        = 0x8004D000,
          xact_e_cantretain                                     = 0x8004D001,
          xact_e_commitfailed                                   = 0x8004D002,
          xact_e_commitprevented                                = 0x8004D003,
          xact_e_heuristicabort                                 = 0x8004D004,
          xact_e_heuristiccommit                                = 0x8004D005,
          xact_e_heuristicdamage                                = 0x8004D006,
          xact_e_heuristicdanger                                = 0x8004D007,
          xact_e_isolationlevel                                 = 0x8004D008,
          xact_e_noasync                                        = 0x8004D009,
          xact_e_noenlist                                       = 0x8004D00A,
          xact_e_noisoretain                                    = 0x8004D00B,
          xact_e_noresource                                     = 0x8004D00C,
          xact_e_notcurrent                                     = 0x8004D00D,
          xact_e_notransaction                                  = 0x8004D00E,
          xact_e_notsupported                                   = 0x8004D00F,
          xact_e_unknownrmgrid                                  = 0x8004D010,
          xact_e_wrongstate                                     = 0x8004D011,
          xact_e_wronguow                                       = 0x8004D012,
          xact_e_xtionexists                                    = 0x8004D013,
          xact_e_noimportobject                                 = 0x8004D014,
          xact_e_invalidcookie                                  = 0x8004D015,
          xact_e_indoubt                                        = 0x8004D016,
          xact_e_notimeout                                      = 0x8004D017,
          xact_e_alreadyinprogress                              = 0x8004D018,
          xact_e_aborted                                        = 0x8004D019,
          xact_e_logfull                                        = 0x8004D01A,
          xact_e_tmnotavailable                                 = 0x8004D01B,
          xact_e_connection_down                                = 0x8004D01C,
          xact_e_connection_denied                              = 0x8004D01D,
          xact_e_reenlisttimeout                                = 0x8004D01E,
          xact_e_tip_connect_failed                             = 0x8004D01F,
          xact_e_tip_protocol_error                             = 0x8004D020,
          xact_e_tip_pull_failed                                = 0x8004D021,
          xact_e_dest_tmnotavailable                            = 0x8004D022,
          xact_e_tip_disabled                                   = 0x8004D023,
          xact_e_network_tx_disabled                            = 0x8004D024,
          xact_e_partner_network_tx_disabled                    = 0x8004D025,
          xact_e_xa_tx_disabled                                 = 0x8004D026,
          xact_e_unable_to_read_dtc_config                      = 0x8004D027,
          xact_e_unable_to_load_dtc_proxy                       = 0x8004D028,
          xact_e_aborting                                       = 0x8004D029,
          xact_e_clerknotfound                                  = 0x8004D080,
          xact_e_clerkexists                                    = 0x8004D081,
          xact_e_recoveryinprogress                             = 0x8004D082,
          xact_e_transactionclosed                              = 0x8004D083,
          xact_e_invalidlsn                                     = 0x8004D084,
          xact_e_replayrequest                                  = 0x8004D085,
          xact_s_async                                          = 0x0004D000,
          xact_s_defect                                         = 0x0004D001,
          xact_s_readonly                                       = 0x0004D002,
          xact_s_somenoretain                                   = 0x0004D003,
          xact_s_okinform                                       = 0x0004D004,
          xact_s_madechangescontent                             = 0x0004D005,
          xact_s_madechangesinform                              = 0x0004D006,
          xact_s_allnoretain                                    = 0x0004D007,
          xact_s_aborting                                       = 0x0004D008,
          xact_s_singlephase                                    = 0x0004D009,
          xact_s_locally_ok                                     = 0x0004D00A,
          xact_s_lastresourcemanager                            = 0x0004D010,
          context_e_aborted                                     = 0x8004E002,
          context_e_aborting                                    = 0x8004E003,
          context_e_nocontext                                   = 0x8004E004,
          context_e_would_deadlock                              = 0x8004E005,
          context_e_synch_timeout                               = 0x8004E006,
          context_e_oldref                                      = 0x8004E007,
          context_e_rolenotfound                                = 0x8004E00C,
          context_e_tmnotavailable                              = 0x8004E00F,
          co_e_activationfailed                                 = 0x8004E021,
          co_e_activationfailed_eventlogged                     = 0x8004E022,
          co_e_activationfailed_catalogerror                    = 0x8004E023,
          co_e_activationfailed_timeout                         = 0x8004E024,
          co_e_initializationfailed                             = 0x8004E025,
          context_e_nojit                                       = 0x8004E026,
          context_e_notransaction                               = 0x8004E027,
          co_e_threadingmodel_changed                           = 0x8004E028,
          co_e_noiisintrinsics                                  = 0x8004E029,
          co_e_nocookies                                        = 0x8004E02A,
          co_e_dberror                                          = 0x8004E02B,
          co_e_notpooled                                        = 0x8004E02C,
          co_e_notconstructed                                   = 0x8004E02D,
          co_e_nosynchronization                                = 0x8004E02E,
          co_e_isolevelmismatch                                 = 0x8004E02F,
          ole_s_usereg                                          = 0x00040000,
          ole_s_static                                          = 0x00040001,
          ole_s_mac_clipformat                                  = 0x00040002,
          dragdrop_s_drop                                       = 0x00040100,
          dragdrop_s_cancel                                     = 0x00040101,
          dragdrop_s_usedefaultcursors                          = 0x00040102,
          data_s_sameformatetc                                  = 0x00040130,
          view_s_already_frozen                                 = 0x00040140,
          cache_s_formatetc_notsupported                        = 0x00040170,
          cache_s_samecache                                     = 0x00040171,
          cache_s_somecaches_notupdated                         = 0x00040172,
          oleobj_s_invalidverb                                  = 0x00040180,
          oleobj_s_cannot_doverb_now                            = 0x00040181,
          oleobj_s_invalidhwnd                                  = 0x00040182,
          inplace_s_truncated                                   = 0x000401A0,
          convert10_s_no_presentation                           = 0x000401C0,
          mk_s_reduced_to_self                                  = 0x000401E2,
          mk_s_me                                               = 0x000401E4,
          mk_s_him                                              = 0x000401E5,
          mk_s_us                                               = 0x000401E6,
          mk_s_monikeralreadyregistered                         = 0x000401E7,
          sched_s_task_ready                                    = 0x00041300,
          sched_s_task_running                                  = 0x00041301,
          sched_s_task_disabled                                 = 0x00041302,
          sched_s_task_has_not_run                              = 0x00041303,
          sched_s_task_no_more_runs                             = 0x00041304,
          sched_s_task_not_scheduled                            = 0x00041305,
          sched_s_task_terminated                               = 0x00041306,
          sched_s_task_no_valid_triggers                        = 0x00041307,
          sched_s_event_trigger                                 = 0x00041308,
          sched_e_trigger_not_found                             = 0x80041309,
          sched_e_task_not_ready                                = 0x8004130A,
          sched_e_task_not_running                              = 0x8004130B,
          sched_e_service_not_installed                         = 0x8004130C,
          sched_e_cannot_open_task                              = 0x8004130D,
          sched_e_invalid_task                                  = 0x8004130E,
          sched_e_account_information_not_set                   = 0x8004130F,
          sched_e_account_name_not_found                        = 0x80041310,
          sched_e_account_dbase_corrupt                         = 0x80041311,
          sched_e_no_security_services                          = 0x80041312,
          sched_e_unknown_object_version                        = 0x80041313,
          sched_e_unsupported_account_option                    = 0x80041314,
          sched_e_service_not_running                           = 0x80041315,
          co_e_class_create_failed                              = 0x80080001,
          co_e_scm_error                                        = 0x80080002,
          co_e_scm_rpc_failure                                  = 0x80080003,
          co_e_bad_path                                         = 0x80080004,
          co_e_server_exec_failure                              = 0x80080005,
          co_e_objsrv_rpc_failure                               = 0x80080006,
          mk_e_no_normalized                                    = 0x80080007,
          co_e_server_stopping                                  = 0x80080008,
          mem_e_invalid_root                                    = 0x80080009,
          mem_e_invalid_link                                    = 0x80080010,
          mem_e_invalid_size                                    = 0x80080011,
          co_s_notallinterfaces                                 = 0x00080012,
          co_s_machinenamenotfound                              = 0x00080013,
          disp_e_unknowninterface                               = 0x80020001,
          disp_e_membernotfound                                 = 0x80020003,
          disp_e_paramnotfound                                  = 0x80020004,
          disp_e_typemismatch                                   = 0x80020005,
          disp_e_unknownname                                    = 0x80020006,
          disp_e_nonamedargs                                    = 0x80020007,
          disp_e_badvartype                                     = 0x80020008,
          disp_e_exception                                      = 0x80020009,
          disp_e_overflow                                       = 0x8002000A,
          disp_e_badindex                                       = 0x8002000B,
          disp_e_unknownlcid                                    = 0x8002000C,
          disp_e_arrayislocked                                  = 0x8002000D,
          disp_e_badparamcount                                  = 0x8002000E,
          disp_e_paramnotoptional                               = 0x8002000F,
          disp_e_badcallee                                      = 0x80020010,
          disp_e_notacollection                                 = 0x80020011,
          disp_e_divbyzero                                      = 0x80020012,
          disp_e_buffertoosmall                                 = 0x80020013,
          type_e_buffertoosmall                                 = 0x80028016,
          type_e_fieldnotfound                                  = 0x80028017,
          type_e_invdataread                                    = 0x80028018,
          type_e_unsupformat                                    = 0x80028019,
          type_e_registryaccess                                 = 0x8002801C,
          type_e_libnotregistered                               = 0x8002801D,
          type_e_undefinedtype                                  = 0x80028027,
          type_e_qualifiednamedisallowed                        = 0x80028028,
          type_e_invalidstate                                   = 0x80028029,
          type_e_wrongtypekind                                  = 0x8002802A,
          type_e_elementnotfound                                = 0x8002802B,
          type_e_ambiguousname                                  = 0x8002802C,
          type_e_nameconflict                                   = 0x8002802D,
          type_e_unknownlcid                                    = 0x8002802E,
          type_e_dllfunctionnotfound                            = 0x8002802F,
          type_e_badmodulekind                                  = 0x800288BD,
          type_e_sizetoobig                                     = 0x800288C5,
          type_e_duplicateid                                    = 0x800288C6,
          type_e_invalidid                                      = 0x800288CF,
          type_e_typemismatch                                   = 0x80028CA0,
          type_e_outofbounds                                    = 0x80028CA1,
          type_e_ioerror                                        = 0x80028CA2,
          type_e_cantcreatetmpfile                              = 0x80028CA3,
          type_e_cantloadlibrary                                = 0x80029C4A,
          type_e_inconsistentpropfuncs                          = 0x80029C83,
          type_e_circulartype                                   = 0x80029C84,
          stg_e_invalidfunction                                 = 0x80030001,
          stg_e_filenotfound                                    = 0x80030002,
          stg_e_pathnotfound                                    = 0x80030003,
          stg_e_toomanyopenfiles                                = 0x80030004,
          stg_e_accessdenied                                    = 0x80030005,
          stg_e_invalidhandle                                   = 0x80030006,
          stg_e_insufficientmemory                              = 0x80030008,
          stg_e_invalidpointer                                  = 0x80030009,
          stg_e_nomorefiles                                     = 0x80030012,
          stg_e_diskiswriteprotected                            = 0x80030013,
          stg_e_seekerror                                       = 0x80030019,
          stg_e_writefault                                      = 0x8003001D,
          stg_e_readfault                                       = 0x8003001E,
          stg_e_shareviolation                                  = 0x80030020,
          stg_e_lockviolation                                   = 0x80030021,
          stg_e_filealreadyexists                               = 0x80030050,
          stg_e_invalidparameter                                = 0x80030057,
          stg_e_mediumfull                                      = 0x80030070,
          stg_e_propsetmismatched                               = 0x800300F0,
          stg_e_abnormalapiexit                                 = 0x800300FA,
          stg_e_invalidheader                                   = 0x800300FB,
          stg_e_invalidname                                     = 0x800300FC,
          stg_e_unknown                                         = 0x800300FD,
          stg_e_unimplementedfunction                           = 0x800300FE,
          stg_e_invalidflag                                     = 0x800300FF,
          stg_e_inuse                                           = 0x80030100,
          stg_e_notcurrent                                      = 0x80030101,
          stg_e_reverted                                        = 0x80030102,
          stg_e_cantsave                                        = 0x80030103,
          stg_e_oldformat                                       = 0x80030104,
          stg_e_olddll                                          = 0x80030105,
          stg_e_sharerequired                                   = 0x80030106,
          stg_e_notfilebasedstorage                             = 0x80030107,
          stg_e_extantmarshallings                              = 0x80030108,
          stg_e_docfilecorrupt                                  = 0x80030109,
          stg_e_badbaseaddress                                  = 0x80030110,
          stg_e_docfiletoolarge                                 = 0x80030111,
          stg_e_notsimpleformat                                 = 0x80030112,
          stg_e_incomplete                                      = 0x80030201,
          stg_e_terminated                                      = 0x80030202,
          stg_s_converted                                       = 0x00030200,
          stg_s_block                                           = 0x00030201,
          stg_s_retrynow                                        = 0x00030202,
          stg_s_monitoring                                      = 0x00030203,
          stg_s_multipleopens                                   = 0x00030204,
          stg_s_consolidationfailed                             = 0x00030205,
          stg_s_cannotconsolidate                               = 0x00030206,
          stg_e_status_copy_protection_failure                  = 0x80030305,
          stg_e_css_authentication_failure                      = 0x80030306,
          stg_e_css_key_not_present                             = 0x80030307,
          stg_e_css_key_not_established                         = 0x80030308,
          stg_e_css_scrambled_sector                            = 0x80030309,
          stg_e_css_region_mismatch                             = 0x8003030A,
          stg_e_resets_exhausted                                = 0x8003030B,
          rpc_e_call_rejected                                   = 0x80010001,
          rpc_e_call_canceled                                   = 0x80010002,
          rpc_e_cantpost_insendcall                             = 0x80010003,
          rpc_e_cantcallout_inasynccall                         = 0x80010004,
          rpc_e_cantcallout_inexternalcall                      = 0x80010005,
          rpc_e_connection_terminated                           = 0x80010006,
          rpc_e_server_died                                     = 0x80010007,
          rpc_e_client_died                                     = 0x80010008,
          rpc_e_invalid_datapacket                              = 0x80010009,
          rpc_e_canttransmit_call                               = 0x8001000A,
          rpc_e_client_cantmarshal_data                         = 0x8001000B,
          rpc_e_client_cantunmarshal_data                       = 0x8001000C,
          rpc_e_server_cantmarshal_data                         = 0x8001000D,
          rpc_e_server_cantunmarshal_data                       = 0x8001000E,
          rpc_e_invalid_data                                    = 0x8001000F,
          rpc_e_invalid_parameter                               = 0x80010010,
          rpc_e_cantcallout_again                               = 0x80010011,
          rpc_e_server_died_dne                                 = 0x80010012,
          rpc_e_sys_call_failed                                 = 0x80010100,
          rpc_e_out_of_resources                                = 0x80010101,
          rpc_e_attempted_multithread                           = 0x80010102,
          rpc_e_not_registered                                  = 0x80010103,
          rpc_e_fault                                           = 0x80010104,
          rpc_e_serverfault                                     = 0x80010105,
          rpc_e_changed_mode                                    = 0x80010106,
          rpc_e_invalidmethod                                   = 0x80010107,
          rpc_e_disconnected                                    = 0x80010108,
          rpc_e_retry                                           = 0x80010109,
          rpc_e_servercall_retrylater                           = 0x8001010A,
          rpc_e_servercall_rejected                             = 0x8001010B,
          rpc_e_invalid_calldata                                = 0x8001010C,
          rpc_e_cantcallout_ininputsynccall                     = 0x8001010D,
          rpc_e_wrong_thread                                    = 0x8001010E,
          rpc_e_thread_not_init                                 = 0x8001010F,
          rpc_e_version_mismatch                                = 0x80010110,
          rpc_e_invalid_header                                  = 0x80010111,
          rpc_e_invalid_extension                               = 0x80010112,
          rpc_e_invalid_ipid                                    = 0x80010113,
          rpc_e_invalid_object                                  = 0x80010114,
          rpc_s_callpending                                     = 0x80010115,
          rpc_s_waitontimer                                     = 0x80010116,
          rpc_e_call_complete                                   = 0x80010117,
          rpc_e_unsecure_call                                   = 0x80010118,
          rpc_e_too_late                                        = 0x80010119,
          rpc_e_no_good_security_packages                       = 0x8001011A,
          rpc_e_access_denied                                   = 0x8001011B,
          rpc_e_remote_disabled                                 = 0x8001011C,
          rpc_e_invalid_objref                                  = 0x8001011D,
          rpc_e_no_context                                      = 0x8001011E,
          rpc_e_timeout                                         = 0x8001011F,
          rpc_e_no_sync                                         = 0x80010120,
          rpc_e_fullsic_required                                = 0x80010121,
          rpc_e_invalid_std_name                                = 0x80010122,
          co_e_failedtoimpersonate                              = 0x80010123,
          co_e_failedtogetsecctx                                = 0x80010124,
          co_e_failedtoopenthreadtoken                          = 0x80010125,
          co_e_failedtogettokeninfo                             = 0x80010126,
          co_e_trusteedoesntmatchclient                         = 0x80010127,
          co_e_failedtoqueryclientblanket                       = 0x80010128,
          co_e_failedtosetdacl                                  = 0x80010129,
          co_e_accesscheckfailed                                = 0x8001012A,
          co_e_netaccessapifailed                               = 0x8001012B,
          co_e_wrongtrusteenamesyntax                           = 0x8001012C,
          co_e_invalidsid                                       = 0x8001012D,
          co_e_conversionfailed                                 = 0x8001012E,
          co_e_nomatchingsidfound                               = 0x8001012F,
          co_e_lookupaccsidfailed                               = 0x80010130,
          co_e_nomatchingnamefound                              = 0x80010131,
          co_e_lookupaccnamefailed                              = 0x80010132,
          co_e_setserlhndlfailed                                = 0x80010133,
          co_e_failedtogetwindir                                = 0x80010134,
          co_e_pathtoolong                                      = 0x80010135,
          co_e_failedtogenuuid                                  = 0x80010136,
          co_e_failedtocreatefile                               = 0x80010137,
          co_e_failedtoclosehandle                              = 0x80010138,
          co_e_exceedsysacllimit                                = 0x80010139,
          co_e_acesinwrongorder                                 = 0x8001013A,
          co_e_incompatiblestreamversion                        = 0x8001013B,
          co_e_failedtoopenprocesstoken                         = 0x8001013C,
          co_e_decodefailed                                     = 0x8001013D,
          co_e_acnotinitialized                                 = 0x8001013F,
          co_e_cancel_disabled                                  = 0x80010140,
          rpc_e_unexpected                                      = 0x8001FFFF,
          error_auditing_disabled                               = 0xC0090001,
          error_all_sids_filtered                               = 0xC0090002,
          nte_bad_uid                                           = 0x80090001,
          nte_bad_hash                                          = 0x80090002,
          nte_bad_key                                           = 0x80090003,
          nte_bad_len                                           = 0x80090004,
          nte_bad_data                                          = 0x80090005,
          nte_bad_signature                                     = 0x80090006,
          nte_bad_ver                                           = 0x80090007,
          nte_bad_algid                                         = 0x80090008,
          nte_bad_flags                                         = 0x80090009,
          nte_bad_type                                          = 0x8009000A,
          nte_bad_key_state                                     = 0x8009000B,
          nte_bad_hash_state                                    = 0x8009000C,
          nte_no_key                                            = 0x8009000D,
          nte_no_memory                                         = 0x8009000E,
          nte_exists                                            = 0x8009000F,
          nte_perm                                              = 0x80090010,
          nte_not_found                                         = 0x80090011,
          nte_double_encrypt                                    = 0x80090012,
          nte_bad_provider                                      = 0x80090013,
          nte_bad_prov_type                                     = 0x80090014,
          nte_bad_public_key                                    = 0x80090015,
          nte_bad_keyset                                        = 0x80090016,
          nte_prov_type_not_def                                 = 0x80090017,
          nte_prov_type_entry_bad                               = 0x80090018,
          nte_keyset_not_def                                    = 0x80090019,
          nte_keyset_entry_bad                                  = 0x8009001A,
          nte_prov_type_no_match                                = 0x8009001B,
          nte_signature_file_bad                                = 0x8009001C,
          nte_provider_dll_fail                                 = 0x8009001D,
          nte_prov_dll_not_found                                = 0x8009001E,
          nte_bad_keyset_param                                  = 0x8009001F,
          nte_fail                                              = 0x80090020,
          nte_sys_err                                           = 0x80090021,
          nte_silent_context                                    = 0x80090022,
          nte_token_keyset_storage_full                         = 0x80090023,
          nte_temporary_profile                                 = 0x80090024,
          nte_fixedparameter                                    = 0x80090025,
          sec_e_insufficient_memory                             = 0x80090300,
          sec_e_invalid_handle                                  = 0x80090301,
          sec_e_unsupported_function                            = 0x80090302,
          sec_e_target_unknown                                  = 0x80090303,
          sec_e_internal_error                                  = 0x80090304,
          sec_e_secpkg_not_found                                = 0x80090305,
          sec_e_not_owner                                       = 0x80090306,
          sec_e_cannot_install                                  = 0x80090307,
          sec_e_invalid_token                                   = 0x80090308,
          sec_e_cannot_pack                                     = 0x80090309,
          sec_e_qop_not_supported                               = 0x8009030A,
          sec_e_no_impersonation                                = 0x8009030B,
          sec_e_logon_denied                                    = 0x8009030C,
          sec_e_unknown_credentials                             = 0x8009030D,
          sec_e_no_credentials                                  = 0x8009030E,
          sec_e_message_altered                                 = 0x8009030F,
          sec_e_out_of_sequence                                 = 0x80090310,
          sec_e_no_authenticating_authority                     = 0x80090311,
          sec_i_continue_needed                                 = 0x00090312,
          sec_i_complete_needed                                 = 0x00090313,
          sec_i_complete_and_continue                           = 0x00090314,
          sec_i_local_logon                                     = 0x00090315,
          sec_e_bad_pkgid                                       = 0x80090316,
          sec_e_context_expired                                 = 0x80090317,
          sec_i_context_expired                                 = 0x00090317,
          sec_e_incomplete_message                              = 0x80090318,
          sec_e_incomplete_credentials                          = 0x80090320,
          sec_e_buffer_too_small                                = 0x80090321,
          sec_i_incomplete_credentials                          = 0x00090320,
          sec_i_renegotiate                                     = 0x00090321,
          sec_e_wrong_principal                                 = 0x80090322,
          sec_i_no_lsa_context                                  = 0x00090323,
          sec_e_time_skew                                       = 0x80090324,
          sec_e_untrusted_root                                  = 0x80090325,
          sec_e_illegal_message                                 = 0x80090326,
          sec_e_cert_unknown                                    = 0x80090327,
          sec_e_cert_expired                                    = 0x80090328,
          sec_e_encrypt_failure                                 = 0x80090329,
          sec_e_decrypt_failure                                 = 0x80090330,
          sec_e_algorithm_mismatch                              = 0x80090331,
          sec_e_security_qos_failed                             = 0x80090332,
          sec_e_unfinished_context_deleted                      = 0x80090333,
          sec_e_no_tgt_reply                                    = 0x80090334,
          sec_e_no_ip_addresses                                 = 0x80090335,
          sec_e_wrong_credential_handle                         = 0x80090336,
          sec_e_crypto_system_invalid                           = 0x80090337,
          sec_e_max_referrals_exceeded                          = 0x80090338,
          sec_e_must_be_kdc                                     = 0x80090339,
          sec_e_strong_crypto_not_supported                     = 0x8009033A,
          sec_e_too_many_principals                             = 0x8009033B,
          sec_e_no_pa_data                                      = 0x8009033C,
          sec_e_pkinit_name_mismatch                            = 0x8009033D,
          sec_e_smartcard_logon_required                        = 0x8009033E,
          sec_e_shutdown_in_progress                            = 0x8009033F,
          sec_e_kdc_invalid_request                             = 0x80090340,
          sec_e_kdc_unable_to_refer                             = 0x80090341,
          sec_e_kdc_unknown_etype                               = 0x80090342,
          sec_e_unsupported_preauth                             = 0x80090343,
          sec_e_delegation_required                             = 0x80090345,
          sec_e_bad_bindings                                    = 0x80090346,
          sec_e_multiple_accounts                               = 0x80090347,
          sec_e_no_kerb_key                                     = 0x80090348,
          sec_e_cert_wrong_usage                                = 0x80090349,
          sec_e_downgrade_detected                              = 0x80090350,
          sec_e_smartcard_cert_revoked                          = 0x80090351,
          sec_e_issuing_ca_untrusted                            = 0x80090352,
          sec_e_revocation_offline_c                            = 0x80090353,
          sec_e_pkinit_client_failure                           = 0x80090354,
          sec_e_smartcard_cert_expired                          = 0x80090355,
          sec_e_no_s4u_prot_support                             = 0x80090356,
          sec_e_crossrealm_delegation_failure                   = 0x80090357,
          sec_e_revocation_offline_kdc                          = 0x80090358,
          sec_e_issuing_ca_untrusted_kdc                        = 0x80090359,
          sec_e_kdc_cert_expired                                = 0x8009035A,
          sec_e_kdc_cert_revoked                                = 0x8009035B,
          crypt_e_msg_error                                     = 0x80091001,
          crypt_e_unknown_algo                                  = 0x80091002,
          crypt_e_oid_format                                    = 0x80091003,
          crypt_e_invalid_msg_type                              = 0x80091004,
          crypt_e_unexpected_encoding                           = 0x80091005,
          crypt_e_auth_attr_missing                             = 0x80091006,
          crypt_e_hash_value                                    = 0x80091007,
          crypt_e_invalid_index                                 = 0x80091008,
          crypt_e_already_decrypted                             = 0x80091009,
          crypt_e_not_decrypted                                 = 0x8009100A,
          crypt_e_recipient_not_found                           = 0x8009100B,
          crypt_e_control_type                                  = 0x8009100C,
          crypt_e_issuer_serialnumber                           = 0x8009100D,
          crypt_e_signer_not_found                              = 0x8009100E,
          crypt_e_attributes_missing                            = 0x8009100F,
          crypt_e_stream_msg_not_ready                          = 0x80091010,
          crypt_e_stream_insufficient_data                      = 0x80091011,
          crypt_i_new_protection_required                       = 0x00091012,
          crypt_e_bad_len                                       = 0x80092001,
          crypt_e_bad_encode                                    = 0x80092002,
          crypt_e_file_error                                    = 0x80092003,
          crypt_e_not_found                                     = 0x80092004,
          crypt_e_exists                                        = 0x80092005,
          crypt_e_no_provider                                   = 0x80092006,
          crypt_e_self_signed                                   = 0x80092007,
          crypt_e_deleted_prev                                  = 0x80092008,
          crypt_e_no_match                                      = 0x80092009,
          crypt_e_unexpected_msg_type                           = 0x8009200A,
          crypt_e_no_key_property                               = 0x8009200B,
          crypt_e_no_decrypt_cert                               = 0x8009200C,
          crypt_e_bad_msg                                       = 0x8009200D,
          crypt_e_no_signer                                     = 0x8009200E,
          crypt_e_pending_close                                 = 0x8009200F,
          crypt_e_revoked                                       = 0x80092010,
          crypt_e_no_revocation_dll                             = 0x80092011,
          crypt_e_no_revocation_check                           = 0x80092012,
          crypt_e_revocation_offline                            = 0x80092013,
          crypt_e_not_in_revocation_database                    = 0x80092014,
          crypt_e_invalid_numeric_string                        = 0x80092020,
          crypt_e_invalid_printable_string                      = 0x80092021,
          crypt_e_invalid_ia5_string                            = 0x80092022,
          crypt_e_invalid_x500_string                           = 0x80092023,
          crypt_e_not_char_string                               = 0x80092024,
          crypt_e_fileresized                                   = 0x80092025,
          crypt_e_security_settings                             = 0x80092026,
          crypt_e_no_verify_usage_dll                           = 0x80092027,
          crypt_e_no_verify_usage_check                         = 0x80092028,
          crypt_e_verify_usage_offline                          = 0x80092029,
          crypt_e_not_in_ctl                                    = 0x8009202A,
          crypt_e_no_trusted_signer                             = 0x8009202B,
          crypt_e_missing_pubkey_para                           = 0x8009202C,
          crypt_e_oss_error                                     = 0x80093000,
          oss_more_buf                                          = 0x80093001,
          oss_negative_uinteger                                 = 0x80093002,
          oss_pdu_range                                         = 0x80093003,
          oss_more_input                                        = 0x80093004,
          oss_data_error                                        = 0x80093005,
          oss_bad_arg                                           = 0x80093006,
          oss_bad_version                                       = 0x80093007,
          oss_out_memory                                        = 0x80093008,
          oss_pdu_mismatch                                      = 0x80093009,
          oss_limited                                           = 0x8009300A,
          oss_bad_ptr                                           = 0x8009300B,
          oss_bad_time                                          = 0x8009300C,
          oss_indefinite_not_supported                          = 0x8009300D,
          oss_mem_error                                         = 0x8009300E,
          oss_bad_table                                         = 0x8009300F,
          oss_too_long                                          = 0x80093010,
          oss_constraint_violated                               = 0x80093011,
          oss_fatal_error                                       = 0x80093012,
          oss_access_serialization_error                        = 0x80093013,
          oss_null_tbl                                          = 0x80093014,
          oss_null_fcn                                          = 0x80093015,
          oss_bad_encrules                                      = 0x80093016,
          oss_unavail_encrules                                  = 0x80093017,
          oss_cant_open_trace_window                            = 0x80093018,
          oss_unimplemented                                     = 0x80093019,
          oss_oid_dll_not_linked                                = 0x8009301A,
          oss_cant_open_trace_file                              = 0x8009301B,
          oss_trace_file_already_open                           = 0x8009301C,
          oss_table_mismatch                                    = 0x8009301D,
          oss_type_not_supported                                = 0x8009301E,
          oss_real_dll_not_linked                               = 0x8009301F,
          oss_real_code_not_linked                              = 0x80093020,
          oss_out_of_range                                      = 0x80093021,
          oss_copier_dll_not_linked                             = 0x80093022,
          oss_constraint_dll_not_linked                         = 0x80093023,
          oss_comparator_dll_not_linked                         = 0x80093024,
          oss_comparator_code_not_linked                        = 0x80093025,
          oss_mem_mgr_dll_not_linked                            = 0x80093026,
          oss_pdv_dll_not_linked                                = 0x80093027,
          oss_pdv_code_not_linked                               = 0x80093028,
          oss_api_dll_not_linked                                = 0x80093029,
          oss_berder_dll_not_linked                             = 0x8009302A,
          oss_per_dll_not_linked                                = 0x8009302B,
          oss_open_type_error                                   = 0x8009302C,
          oss_mutex_not_created                                 = 0x8009302D,
          oss_cant_close_trace_file                             = 0x8009302E,
          crypt_e_asn1_error                                    = 0x80093100,
          crypt_e_asn1_internal                                 = 0x80093101,
          crypt_e_asn1_eod                                      = 0x80093102,
          crypt_e_asn1_corrupt                                  = 0x80093103,
          crypt_e_asn1_large                                    = 0x80093104,
          crypt_e_asn1_constraint                               = 0x80093105,
          crypt_e_asn1_memory                                   = 0x80093106,
          crypt_e_asn1_overflow                                 = 0x80093107,
          crypt_e_asn1_badpdu                                   = 0x80093108,
          crypt_e_asn1_badargs                                  = 0x80093109,
          crypt_e_asn1_badreal                                  = 0x8009310A,
          crypt_e_asn1_badtag                                   = 0x8009310B,
          crypt_e_asn1_choice                                   = 0x8009310C,
          crypt_e_asn1_rule                                     = 0x8009310D,
          crypt_e_asn1_utf8                                     = 0x8009310E,
          crypt_e_asn1_pdu_type                                 = 0x80093133,
          crypt_e_asn1_nyi                                      = 0x80093134,
          crypt_e_asn1_extended                                 = 0x80093201,
          crypt_e_asn1_noeod                                    = 0x80093202,
          certsrv_e_bad_requestsubject                          = 0x80094001,
          certsrv_e_no_request                                  = 0x80094002,
          certsrv_e_bad_requeststatus                           = 0x80094003,
          certsrv_e_property_empty                              = 0x80094004,
          certsrv_e_invalid_ca_certificate                      = 0x80094005,
          certsrv_e_server_suspended                            = 0x80094006,
          certsrv_e_encoding_length                             = 0x80094007,
          certsrv_e_roleconflict                                = 0x80094008,
          certsrv_e_restrictedofficer                           = 0x80094009,
          certsrv_e_key_archival_not_configured                 = 0x8009400A,
          certsrv_e_no_valid_kra                                = 0x8009400B,
          certsrv_e_bad_request_key_archival                    = 0x8009400C,
          certsrv_e_no_caadmin_defined                          = 0x8009400D,
          certsrv_e_bad_renewal_cert_attribute                  = 0x8009400E,
          certsrv_e_no_db_sessions                              = 0x8009400F,
          certsrv_e_alignment_fault                             = 0x80094010,
          certsrv_e_enroll_denied                               = 0x80094011,
          certsrv_e_template_denied                             = 0x80094012,
          certsrv_e_downlevel_dc_ssl_or_upgrade                 = 0x80094013,
          certsrv_e_unsupported_cert_type                       = 0x80094800,
          certsrv_e_no_cert_type                                = 0x80094801,
          certsrv_e_template_conflict                           = 0x80094802,
          certsrv_e_subject_alt_name_required                   = 0x80094803,
          certsrv_e_archived_key_required                       = 0x80094804,
          certsrv_e_smime_required                              = 0x80094805,
          certsrv_e_bad_renewal_subject                         = 0x80094806,
          certsrv_e_bad_template_version                        = 0x80094807,
          certsrv_e_template_policy_required                    = 0x80094808,
          certsrv_e_signature_policy_required                   = 0x80094809,
          certsrv_e_signature_count                             = 0x8009480A,
          certsrv_e_signature_rejected                          = 0x8009480B,
          certsrv_e_issuance_policy_required                    = 0x8009480C,
          certsrv_e_subject_upn_required                        = 0x8009480D,
          certsrv_e_subject_directory_guid_required             = 0x8009480E,
          certsrv_e_subject_dns_required                        = 0x8009480F,
          certsrv_e_archived_key_unexpected                     = 0x80094810,
          certsrv_e_key_length                                  = 0x80094811,
          certsrv_e_subject_email_required                      = 0x80094812,
          certsrv_e_unknown_cert_type                           = 0x80094813,
          certsrv_e_cert_type_overlap                           = 0x80094814,
          xenroll_e_key_not_exportable                          = 0x80095000,
          xenroll_e_cannot_add_root_cert                        = 0x80095001,
          xenroll_e_response_ka_hash_not_found                  = 0x80095002,
          xenroll_e_response_unexpected_ka_hash                 = 0x80095003,
          xenroll_e_response_ka_hash_mismatch                   = 0x80095004,
          xenroll_e_keyspec_smime_mismatch                      = 0x80095005,
          trust_e_system_error                                  = 0x80096001,
          trust_e_no_signer_cert                                = 0x80096002,
          trust_e_counter_signer                                = 0x80096003,
          trust_e_cert_signature                                = 0x80096004,
          trust_e_time_stamp                                    = 0x80096005,
          trust_e_bad_digest                                    = 0x80096010,
          trust_e_basic_constraints                             = 0x80096019,
          trust_e_financial_criteria                            = 0x8009601E,
          mssipotf_e_outofmemrange                              = 0x80097001,
          mssipotf_e_cantgetobject                              = 0x80097002,
          mssipotf_e_noheadtable                                = 0x80097003,
          mssipotf_e_bad_magicnumber                            = 0x80097004,
          mssipotf_e_bad_offset_table                           = 0x80097005,
          mssipotf_e_table_tagorder                             = 0x80097006,
          mssipotf_e_table_longword                             = 0x80097007,
          mssipotf_e_bad_first_table_placement                  = 0x80097008,
          mssipotf_e_tables_overlap                             = 0x80097009,
          mssipotf_e_table_padbytes                             = 0x8009700A,
          mssipotf_e_filetoosmall                               = 0x8009700B,
          mssipotf_e_table_checksum                             = 0x8009700C,
          mssipotf_e_file_checksum                              = 0x8009700D,
          mssipotf_e_failed_policy                              = 0x80097010,
          mssipotf_e_failed_hints_check                         = 0x80097011,
          mssipotf_e_not_opentype                               = 0x80097012,
          mssipotf_e_file                                       = 0x80097013,
          mssipotf_e_crypt                                      = 0x80097014,
          mssipotf_e_badversion                                 = 0x80097015,
          mssipotf_e_dsig_structure                             = 0x80097016,
          mssipotf_e_pconst_check                               = 0x80097017,
          mssipotf_e_structure                                  = 0x80097018,
          trust_e_provider_unknown                              = 0x800B0001,
          trust_e_action_unknown                                = 0x800B0002,
          trust_e_subject_form_unknown                          = 0x800B0003,
          trust_e_subject_not_trusted                           = 0x800B0004,
          digsig_e_encode                                       = 0x800B0005,
          digsig_e_decode                                       = 0x800B0006,
          digsig_e_extensibility                                = 0x800B0007,
          digsig_e_crypto                                       = 0x800B0008,
          persist_e_sizedefinite                                = 0x800B0009,
          persist_e_sizeindefinite                              = 0x800B000A,
          persist_e_notselfsizing                               = 0x800B000B,
          trust_e_nosignature                                   = 0x800B0100,
          cert_e_expired                                        = 0x800B0101,
          cert_e_validityperiodnesting                          = 0x800B0102,
          cert_e_role                                           = 0x800B0103,
          cert_e_pathlenconst                                   = 0x800B0104,
          cert_e_critical                                       = 0x800B0105,
          cert_e_purpose                                        = 0x800B0106,
          cert_e_issuerchaining                                 = 0x800B0107,
          cert_e_malformed                                      = 0x800B0108,
          cert_e_untrustedroot                                  = 0x800B0109,
          cert_e_chaining                                       = 0x800B010A,
          trust_e_fail                                          = 0x800B010B,
          cert_e_revoked                                        = 0x800B010C,
          cert_e_untrustedtestroot                              = 0x800B010D,
          cert_e_revocation_failure                             = 0x800B010E,
          cert_e_cn_no_match                                    = 0x800B010F,
          cert_e_wrong_usage                                    = 0x800B0110,
          trust_e_explicit_distrust                             = 0x800B0111,
          cert_e_untrustedca                                    = 0x800B0112,
          cert_e_invalid_policy                                 = 0x800B0113,
          cert_e_invalid_name                                   = 0x800B0114,
          spapi_e_expected_section_name                         = 0x800F0000,
          spapi_e_bad_section_name_line                         = 0x800F0001,
          spapi_e_section_name_too_long                         = 0x800F0002,
          spapi_e_general_syntax                                = 0x800F0003,
          spapi_e_wrong_inf_style                               = 0x800F0100,
          spapi_e_section_not_found                             = 0x800F0101,
          spapi_e_line_not_found                                = 0x800F0102,
          spapi_e_no_backup                                     = 0x800F0103,
          spapi_e_no_associated_class                           = 0x800F0200,
          spapi_e_class_mismatch                                = 0x800F0201,
          spapi_e_duplicate_found                               = 0x800F0202,
          spapi_e_no_driver_selected                            = 0x800F0203,
          spapi_e_key_does_not_exist                            = 0x800F0204,
          spapi_e_invalid_devinst_name                          = 0x800F0205,
          spapi_e_invalid_class                                 = 0x800F0206,
          spapi_e_devinst_already_exists                        = 0x800F0207,
          spapi_e_devinfo_not_registered                        = 0x800F0208,
          spapi_e_invalid_reg_property                          = 0x800F0209,
          spapi_e_no_inf                                        = 0x800F020A,
          spapi_e_no_such_devinst                               = 0x800F020B,
          spapi_e_cant_load_class_icon                          = 0x800F020C,
          spapi_e_invalid_class_installer                       = 0x800F020D,
          spapi_e_di_do_default                                 = 0x800F020E,
          spapi_e_di_nofilecopy                                 = 0x800F020F,
          spapi_e_invalid_hwprofile                             = 0x800F0210,
          spapi_e_no_device_selected                            = 0x800F0211,
          spapi_e_devinfo_list_locked                           = 0x800F0212,
          spapi_e_devinfo_data_locked                           = 0x800F0213,
          spapi_e_di_bad_path                                   = 0x800F0214,
          spapi_e_no_classinstall_params                        = 0x800F0215,
          spapi_e_filequeue_locked                              = 0x800F0216,
          spapi_e_bad_service_installsect                       = 0x800F0217,
          spapi_e_no_class_driver_list                          = 0x800F0218,
          spapi_e_no_associated_service                         = 0x800F0219,
          spapi_e_no_default_device_interface                   = 0x800F021A,
          spapi_e_device_interface_active                       = 0x800F021B,
          spapi_e_device_interface_removed                      = 0x800F021C,
          spapi_e_bad_interface_installsect                     = 0x800F021D,
          spapi_e_no_such_interface_class                       = 0x800F021E,
          spapi_e_invalid_reference_string                      = 0x800F021F,
          spapi_e_invalid_machinename                           = 0x800F0220,
          spapi_e_remote_comm_failure                           = 0x800F0221,
          spapi_e_machine_unavailable                           = 0x800F0222,
          spapi_e_no_configmgr_services                         = 0x800F0223,
          spapi_e_invalid_proppage_provider                     = 0x800F0224,
          spapi_e_no_such_device_interface                      = 0x800F0225,
          spapi_e_di_postprocessing_required                    = 0x800F0226,
          spapi_e_invalid_coinstaller                           = 0x800F0227,
          spapi_e_no_compat_drivers                             = 0x800F0228,
          spapi_e_no_device_icon                                = 0x800F0229,
          spapi_e_invalid_inf_logconfig                         = 0x800F022A,
          spapi_e_di_dont_install                               = 0x800F022B,
          spapi_e_invalid_filter_driver                         = 0x800F022C,
          spapi_e_non_windows_nt_driver                         = 0x800F022D,
          spapi_e_non_windows_driver                            = 0x800F022E,
          spapi_e_no_catalog_for_oem_inf                        = 0x800F022F,
          spapi_e_devinstall_queue_nonnative                    = 0x800F0230,
          spapi_e_not_disableable                               = 0x800F0231,
          spapi_e_cant_remove_devinst                           = 0x800F0232,
          spapi_e_invalid_target                                = 0x800F0233,
          spapi_e_driver_nonnative                              = 0x800F0234,
          spapi_e_in_wow64                                      = 0x800F0235,
          spapi_e_set_system_restore_point                      = 0x800F0236,
          spapi_e_incorrectly_copied_inf                        = 0x800F0237,
          spapi_e_sce_disabled                                  = 0x800F0238,
          spapi_e_unknown_exception                             = 0x800F0239,
          spapi_e_pnp_registry_error                            = 0x800F023A,
          spapi_e_remote_request_unsupported                    = 0x800F023B,
          spapi_e_not_an_installed_oem_inf                      = 0x800F023C,
          spapi_e_inf_in_use_by_devices                         = 0x800F023D,
          spapi_e_di_function_obsolete                          = 0x800F023E,
          spapi_e_no_authenticode_catalog                       = 0x800F023F,
          spapi_e_authenticode_disallowed                       = 0x800F0240,
          spapi_e_authenticode_trusted_publisher                = 0x800F0241,
          spapi_e_authenticode_trust_not_established            = 0x800F0242,
          spapi_e_authenticode_publisher_not_trusted            = 0x800F0243,
          spapi_e_signature_osattribute_mismatch                = 0x800F0244,
          spapi_e_only_validate_via_authenticode                = 0x800F0245,
          spapi_e_unrecoverable_stack_overflow                  = 0x800F0300,
          spapi_e_error_not_installed                           = 0x800F1000,
          scard_f_internal_error                                = 0x80100001,
          scard_e_cancelled                                     = 0x80100002,
          scard_e_invalid_handle                                = 0x80100003,
          scard_e_invalid_parameter                             = 0x80100004,
          scard_e_invalid_target                                = 0x80100005,
          scard_e_no_memory                                     = 0x80100006,
          scard_f_waited_too_long                               = 0x80100007,
          scard_e_insufficient_buffer                           = 0x80100008,
          scard_e_unknown_reader                                = 0x80100009,
          scard_e_timeout                                       = 0x8010000A,
          scard_e_sharing_violation                             = 0x8010000B,
          scard_e_no_smartcard                                  = 0x8010000C,
          scard_e_unknown_card                                  = 0x8010000D,
          scard_e_cant_dispose                                  = 0x8010000E,
          scard_e_proto_mismatch                                = 0x8010000F,
          scard_e_not_ready                                     = 0x80100010,
          scard_e_invalid_value                                 = 0x80100011,
          scard_e_system_cancelled                              = 0x80100012,
          scard_f_comm_error                                    = 0x80100013,
          scard_f_unknown_error                                 = 0x80100014,
          scard_e_invalid_atr                                   = 0x80100015,
          scard_e_not_transacted                                = 0x80100016,
          scard_e_reader_unavailable                            = 0x80100017,
          scard_p_shutdown                                      = 0x80100018,
          scard_e_pci_too_small                                 = 0x80100019,
          scard_e_reader_unsupported                            = 0x8010001A,
          scard_e_duplicate_reader                              = 0x8010001B,
          scard_e_card_unsupported                              = 0x8010001C,
          scard_e_no_service                                    = 0x8010001D,
          scard_e_service_stopped                               = 0x8010001E,
          scard_e_unexpected                                    = 0x8010001F,
          scard_e_icc_installation                              = 0x80100020,
          scard_e_icc_createorder                               = 0x80100021,
          scard_e_unsupported_feature                           = 0x80100022,
          scard_e_dir_not_found                                 = 0x80100023,
          scard_e_file_not_found                                = 0x80100024,
          scard_e_no_dir                                        = 0x80100025,
          scard_e_no_file                                       = 0x80100026,
          scard_e_no_access                                     = 0x80100027,
          scard_e_write_too_many                                = 0x80100028,
          scard_e_bad_seek                                      = 0x80100029,
          scard_e_invalid_chv                                   = 0x8010002A,
          scard_e_unknown_res_mng                               = 0x8010002B,
          scard_e_no_such_certificate                           = 0x8010002C,
          scard_e_certificate_unavailable                       = 0x8010002D,
          scard_e_no_readers_available                          = 0x8010002E,
          scard_e_comm_data_lost                                = 0x8010002F,
          scard_e_no_key_container                              = 0x80100030,
          scard_e_server_too_busy                               = 0x80100031,
          scard_w_unsupported_card                              = 0x80100065,
          scard_w_unresponsive_card                             = 0x80100066,
          scard_w_unpowered_card                                = 0x80100067,
          scard_w_reset_card                                    = 0x80100068,
          scard_w_removed_card                                  = 0x80100069,
          scard_w_security_violation                            = 0x8010006A,
          scard_w_wrong_chv                                     = 0x8010006B,
          scard_w_chv_blocked                                   = 0x8010006C,
          scard_w_eof                                           = 0x8010006D,
          scard_w_cancelled_by_user                             = 0x8010006E,
          scard_w_card_not_authenticated                        = 0x8010006F,
          comadmin_e_objecterrors                               = 0x80110401,
          comadmin_e_objectinvalid                              = 0x80110402,
          comadmin_e_keymissing                                 = 0x80110403,
          comadmin_e_alreadyinstalled                           = 0x80110404,
          comadmin_e_app_file_writefail                         = 0x80110407,
          comadmin_e_app_file_readfail                          = 0x80110408,
          comadmin_e_app_file_version                           = 0x80110409,
          comadmin_e_badpath                                    = 0x8011040A,
          comadmin_e_applicationexists                          = 0x8011040B,
          comadmin_e_roleexists                                 = 0x8011040C,
          comadmin_e_cantcopyfile                               = 0x8011040D,
          comadmin_e_nouser                                     = 0x8011040F,
          comadmin_e_invaliduserids                             = 0x80110410,
          comadmin_e_noregistryclsid                            = 0x80110411,
          comadmin_e_badregistryprogid                          = 0x80110412,
          comadmin_e_authenticationlevel                        = 0x80110413,
          comadmin_e_userpasswdnotvalid                         = 0x80110414,
          comadmin_e_clsidoriidmismatch                         = 0x80110418,
          comadmin_e_remoteinterface                            = 0x80110419,
          comadmin_e_dllregisterserver                          = 0x8011041A,
          comadmin_e_noservershare                              = 0x8011041B,
          comadmin_e_dllloadfailed                              = 0x8011041D,
          comadmin_e_badregistrylibid                           = 0x8011041E,
          comadmin_e_appdirnotfound                             = 0x8011041F,
          comadmin_e_registrarfailed                            = 0x80110423,
          comadmin_e_compfile_doesnotexist                      = 0x80110424,
          comadmin_e_compfile_loaddllfail                       = 0x80110425,
          comadmin_e_compfile_getclassobj                       = 0x80110426,
          comadmin_e_compfile_classnotavail                     = 0x80110427,
          comadmin_e_compfile_badtlb                            = 0x80110428,
          comadmin_e_compfile_notinstallable                    = 0x80110429,
          comadmin_e_notchangeable                              = 0x8011042A,
          comadmin_e_notdeleteable                              = 0x8011042B,
          comadmin_e_session                                    = 0x8011042C,
          comadmin_e_comp_move_locked                           = 0x8011042D,
          comadmin_e_comp_move_bad_dest                         = 0x8011042E,
          comadmin_e_registertlb                                = 0x80110430,
          comadmin_e_systemapp                                  = 0x80110433,
          comadmin_e_compfile_noregistrar                       = 0x80110434,
          comadmin_e_coreqcompinstalled                         = 0x80110435,
          comadmin_e_servicenotinstalled                        = 0x80110436,
          comadmin_e_propertysavefailed                         = 0x80110437,
          comadmin_e_objectexists                               = 0x80110438,
          comadmin_e_componentexists                            = 0x80110439,
          comadmin_e_regfile_corrupt                            = 0x8011043B,
          comadmin_e_property_overflow                          = 0x8011043C,
          comadmin_e_notinregistry                              = 0x8011043E,
          comadmin_e_objectnotpoolable                          = 0x8011043F,
          comadmin_e_applid_matches_clsid                       = 0x80110446,
          comadmin_e_role_does_not_exist                        = 0x80110447,
          comadmin_e_start_app_needs_components                 = 0x80110448,
          comadmin_e_requires_different_platform                = 0x80110449,
          comadmin_e_can_not_export_app_proxy                   = 0x8011044A,
          comadmin_e_can_not_start_app                          = 0x8011044B,
          comadmin_e_can_not_export_sys_app                     = 0x8011044C,
          comadmin_e_cant_subscribe_to_component                = 0x8011044D,
          comadmin_e_eventclass_cant_be_subscriber              = 0x8011044E,
          comadmin_e_lib_app_proxy_incompatible                 = 0x8011044F,
          comadmin_e_base_partition_only                        = 0x80110450,
          comadmin_e_start_app_disabled                         = 0x80110451,
          comadmin_e_cat_duplicate_partition_name               = 0x80110457,
          comadmin_e_cat_invalid_partition_name                 = 0x80110458,
          comadmin_e_cat_partition_in_use                       = 0x80110459,
          comadmin_e_file_partition_duplicate_files             = 0x8011045A,
          comadmin_e_cat_imported_components_not_allowed        = 0x8011045B,
          comadmin_e_ambiguous_application_name                 = 0x8011045C,
          comadmin_e_ambiguous_partition_name                   = 0x8011045D,
          comadmin_e_regdb_notinitialized                       = 0x80110472,
          comadmin_e_regdb_notopen                              = 0x80110473,
          comadmin_e_regdb_systemerr                            = 0x80110474,
          comadmin_e_regdb_alreadyrunning                       = 0x80110475,
          comadmin_e_mig_versionnotsupported                    = 0x80110480,
          comadmin_e_mig_schemanotfound                         = 0x80110481,
          comadmin_e_cat_bitnessmismatch                        = 0x80110482,
          comadmin_e_cat_unacceptablebitness                    = 0x80110483,
          comadmin_e_cat_wrongappbitness                        = 0x80110484,
          comadmin_e_cat_pause_resume_not_supported             = 0x80110485,
          comadmin_e_cat_serverfault                            = 0x80110486,
          comqc_e_application_not_queued                        = 0x80110600,
          comqc_e_no_queueable_interfaces                       = 0x80110601,
          comqc_e_queuing_service_not_available                 = 0x80110602,
          comqc_e_no_ipersiststream                             = 0x80110603,
          comqc_e_bad_message                                   = 0x80110604,
          comqc_e_unauthenticated                               = 0x80110605,
          comqc_e_untrusted_enqueuer                            = 0x80110606,
          msdtc_e_duplicate_resource                            = 0x80110701,
          comadmin_e_object_parent_missing                      = 0x80110808,
          comadmin_e_object_does_not_exist                      = 0x80110809,
          comadmin_e_app_not_running                            = 0x8011080A,
          comadmin_e_invalid_partition                          = 0x8011080B,
          comadmin_e_svcapp_not_poolable_or_recyclable          = 0x8011080D,
          comadmin_e_user_in_set                                = 0x8011080E,
          comadmin_e_cantrecyclelibraryapps                     = 0x8011080F,
          comadmin_e_cantrecycleserviceapps                     = 0x80110811,
          comadmin_e_processalreadyrecycled                     = 0x80110812,
          comadmin_e_pausedprocessmaynotberecycled              = 0x80110813,
          comadmin_e_cantmakeinprocservice                      = 0x80110814,
          comadmin_e_progidinusebyclsid                         = 0x80110815,
          comadmin_e_default_partition_not_in_set               = 0x80110816,
          comadmin_e_recycledprocessmaynotbepaused              = 0x80110817,
          comadmin_e_partition_accessdenied                     = 0x80110818,
          comadmin_e_partition_msi_only                         = 0x80110819,
          comadmin_e_legacycomps_not_allowed_in_1_0_format      = 0x8011081A,
          comadmin_e_legacycomps_not_allowed_in_nonbase_partit  = 0x8011081B,
          comadmin_e_comp_move_source                           = 0x8011081C,
          comadmin_e_comp_move_dest                             = 0x8011081D,
          comadmin_e_comp_move_private                          = 0x8011081E,
          comadmin_e_basepartition_required_in_set              = 0x8011081F,
          comadmin_e_cannot_alias_eventclass                    = 0x80110820,
          comadmin_e_private_accessdenied                       = 0x80110821,
          comadmin_e_saferinvalid                               = 0x80110822,
          comadmin_e_registry_accessdenied                      = 0x80110823,
          comadmin_e_partitions_disabled                        = 0x80110824,

          inet_e_invalid_url                                    = 0x800C0002L,      
          inet_e_no_session                                     = 0x800C0003L,      
          inet_e_cannot_connect                                 = 0x800C0004L,      
          inet_e_resource_not_found                             = 0x800C0005L,      
          inet_e_object_not_found                               = 0x800C0006L,      
          inet_e_data_not_available                             = 0x800C0007L,      
          inet_e_download_failure                               = 0x800C0008L,      
          inet_e_authentication_required                        = 0x800C0009L,      
          inet_e_no_valid_media                                 = 0x800C000AL,      
          inet_e_connection_timeout                             = 0x800C000BL,      
          inet_e_invalid_request                                = 0x800C000CL,      
          inet_e_unknown_protocol                               = 0x800C000DL,      
          inet_e_security_problem                               = 0x800C000EL,      
          inet_e_cannot_load_data                               = 0x800C000FL,      
          inet_e_cannot_instantiate_object                      = 0x800C0010L,      
          inet_e_redirect_failed                                = 0x800C0014L,      
          inet_e_redirect_to_dir                                = 0x800C0015L,      
          inet_e_cannot_lock_request                            = 0x800C0016L,      
          inet_e_use_extend_binding                             = 0x800C0017L,      
          inet_e_terminated_bind                                = 0x800C0018L,      
          inet_e_error_first                                    = 0x800C0002L,      
          inet_e_code_download_declined                         = 0x800C0100L,      
          inet_e_result_dispatched                              = 0x800C0200L,      
          inet_e_cannot_replace_sfp_file                        = 0x800C0300L,      
          inet_e_code_install_suppressed                        = 0x800C0400L,      
          inet_e_error_last                                     = inet_e_cannot_replace_sfp_file,
        };


        //////////////////////////////////////////////////////////////////////////
        Type hr;

      };

      //////////////////////////////////////////////////////////////////////////

      #pragma comment(lib, "uuid.lib")
      extern "C" const iid IID_IUnknown;
      extern "C" const iid IID_IClassFactory;
      extern "C" const iid IID_IMalloc;

      //////////////////////////////////////////////////////////////////////////
      struct IUnknown;

      template<class T>
      class CComPtr
      {
      public:
        CComPtr()
          :p(NULL)
        {}
        CComPtr(T* lp)
          :p(lp)
        {
          if(p)
            p->AddRef();
        }
        CComPtr(const CComPtr<T>& lp)
          :p(lp.p)
        {
          if(p)
            p->AddRef();
        }
        ~CComPtr()
        {
          if(p)
            p->Release();
        }

        T* operator=(T* lp)
        {
          return static_cast<T*>(assign(&p, lp));
        }
#if 0
        template<typename Q>
        T* operator=(const CComPtr<Q>& lp)
        {
          return 0;//static_cast<T*>(assign(&p, lp, __uuidof(T)));
        }
        template<>
        T* operator=(const CComPtr<T>& lp)
        {
          return 0;//static_cast<T*>(assign(&p, lp));
        }
#endif
        // ops
        operator T*() const
        {
          return p;
        }
        T& operator*() const
        {
          return *p;
        }
        T** operator& ()
        {
          return &p;
        }
        T* operator->() const
        {
          return p;
        }
        bool operator! () const
        {
          return p == 0;
        }
        bool operator==(T* lp) const
        {
          return p == lp;
        }

        //
        void Release()
        {
          if(p){
            p->Release();
            p = NULL;
          }
        }

        void Attach(T* lp)
        {
          if(p)
            p->Release();
          p = lp;
        }
        T* Detach()
        {
          T* pt = NULL;
          swap(p, pt);
          return pt;
        }

        template<class Q>
        hresult QueryInterface(Q** pp) const
        {
          return p->QueryInterface(__uuidof(Q), (void**)pp);
        }

        static IUnknown* assign(IUnknown** pp, IUnknown* p)
        {
          if(!pp)
            return NULL;

          if(lp)
            lp->AddRef();
          if(*pp)
            (*pp)->Release();
          *pp = p;
          return p;
        }

      protected:
        static IUnknown* assign(IUnknown** pp, IUnknown* p, const iid& riid)
        {
          if(!pp)
            return NULL;

          IUnknown* tmp = NULL;
          std::swap(*pp, tmp);
          if(p)
            p->QueryInterface(riid, (void**)pp);
          if(tmp)
            (tmp)->Release();
          return *pp;
        }
      public:
        T* p;
      };

      template<class T, const iid* riid = (const iid*)&__uuidof(T)>
      class CComQIPtr:
        public CComPtr<T>
      {
      public:
        CComQIPtr()
        {}
        CComQIPtr(T* lp)
          :CComPtr<T>(lp)
        {}
        CComQIPtr(const CComQIPtr<T, riid>& lp)
          :CComPtr<T>(lp.p)
        {}
        CComQIPtr(IUnknown* lp)
        {
          if(lp)
            lp->QueryInterface(*riid, (void**)&p);
        }

        T* operator= (T* lp)
        {
          return static_cast<T*>(assign(&p, lp));
        }
        T* operator= (IUnknown* lp)
        {
          return static_cast<T*>(assign(&p, lp, *riid));
        }

      };

      //////////////////////////////////////////////////////////////////////////
      struct 
      __declspec(uuid("00000000-0000-0000-C000-000000000046")) 
      __declspec(novtable)
      IUnknown
      {
        virtual hresult  __stdcall QueryInterface(const iid& riid, void** ppvObject) = 0;
        virtual uint32_t __stdcall AddRef() = 0;
        virtual uint32_t __stdcall Release()= 0;
      };

      struct 
      __declspec(uuid("00000001-0000-0000-C000-000000000046")) 
      __declspec(novtable)
      IClassFactory: IUnknown
      {
        virtual hresult  __stdcall CreateInstance(IUnknown *pUnkOuter, const iid& riid, void **ppvObject) = 0;
        virtual hresult  __stdcall LockServer(int32_t fLock) = 0;
      };

      struct
      __declspec(uuid("00000002-0000-0000-C000-000000000046"))
      __declspec(novtable)
      IMalloc : public IUnknown
      {
      public:
        virtual void *__stdcall Alloc(size_t cb) = 0;
        virtual void *__stdcall Realloc(void *pv, size_t cb) = 0;
        virtual void __stdcall Free(void *pv) = 0;
        virtual size_t __stdcall GetSize(void *pv) = 0;
        virtual int __stdcall DidAlloc(void *pv) = 0;
        virtual void __stdcall HeapMinimize(void) = 0;
      };

      enum STREAM_SEEK
      {
        STREAM_SEEK_SET = 0,
        STREAM_SEEK_CUR = 1,
        STREAM_SEEK_END = 2
      };
      enum STGC
      {
        STGC_DEFAULT	= 0,
      STGC_OVERWRITE	= 1,
      STGC_ONLYIFCURRENT	= 2,
      STGC_DANGEROUSLYCOMMITMERELYTODISKCACHE	= 4,
      STGC_CONSOLIDATE	= 8
      };

      enum STATFLAG
      {
        STATFLAG_DEFAULT	= 0,
      STATFLAG_NONAME	= 1,
      STATFLAG_NOOPEN	= 2
      };

      enum LOCKTYPE
      {
        LOCK_WRITE	= 1,
      LOCK_EXCLUSIVE	= 2,
      LOCK_ONLYONCE	= 4
      };

      enum STGTY
      {
        STGTY_STORAGE	= 1,
      STGTY_STREAM	= 2,
      STGTY_LOCKBYTES	= 3,
      STGTY_PROPERTY	= 4
      };

      enum STGM
      {
        STGM_DIRECT             = 0x00000000L,
        STGM_TRANSACTED         = 0x00010000L,
        STGM_SIMPLE             = 0x08000000L,

        STGM_READ               = 0x00000000L,
        STGM_WRITE              = 0x00000001L,
        STGM_READWRITE          = 0x00000002L,

        STGM_SHARE_DENY_NONE    = 0x00000040L,
        STGM_SHARE_DENY_READ    = 0x00000030L,
        STGM_SHARE_DENY_WRITE   = 0x00000020L,
        STGM_SHARE_EXCLUSIVE    = 0x00000010L,

        STGM_PRIORITY           = 0x00040000L,
        STGM_DELETEONRELEASE    = 0x04000000L,
        STGM_NOSCRATCH          = 0x00100000L,

        STGM_CREATE             = 0x00001000L,
        STGM_CONVERT            = 0x00020000L,
        STGM_FAILIFTHERE        = 0x00000000L,

        STGM_NOSNAPSHOT         = 0x00200000L,
        STGM_DIRECT_SWMR        = 0x00400000L,
      };
      inline STGM operator|(STGM m1, STGM m2)
      {
        return bitwise_or(m1, m2);
      }

      struct STATSTG
      {
        wchar_t* pwcsName;
        STGTY type;
        uint64_t cbSize;
        systime_t mtime;
        systime_t ctime;
        systime_t atime;
        STGM grfMode;
        LOCKTYPE grfLocksSupported;
        guid clsid;
        uint32_t grfStateBits;
        uint32_t reserved;
      };

      struct __declspec(novtable) __declspec(uuid("0c733a30-2a1c-11ce-ade5-00aa0044773d"))
      ISequentialStream : public IUnknown
      {
      public:
        virtual /* [local] */ hresult __stdcall Read( 
          /* [length_is][size_is][out] */ void *pv,
          /* [in] */ uint32_t cb,
          /* [out] */ uint32_t *pcbRead) = 0;

        virtual /* [local] */ hresult __stdcall Write( 
          /* [size_is][in] */ const void *pv,
          /* [in] */ uint32_t cb,
          /* [out] */ uint32_t *pcbWritten) = 0;

      };

      struct __declspec(novtable) __declspec(uuid("0000000c-0000-0000-C000-000000000046"))
      IStream : public ISequentialStream
      {
      public:
        virtual /* [local] */ hresult __stdcall Seek( 
          /* [in] */ int64_t dlibMove,
          /* [in] */ STREAM_SEEK dwOrigin,
          /* [out] */ uint64_t *plibNewPosition) = 0;

        virtual hresult __stdcall SetSize( 
          /* [in] */ uint64_t libNewSize) = 0;

        virtual /* [local] */ hresult __stdcall CopyTo( 
          /* [unique][in] */ IStream *pstm,
          /* [in] */ uint64_t cb,
          /* [out] */ uint64_t *pcbRead,
          /* [out] */ uint64_t *pcbWritten) = 0;

        virtual hresult __stdcall Commit( 
          /* [in] */ STGC grfCommitFlags) = 0;

        virtual hresult __stdcall Revert( void) = 0;

        virtual hresult __stdcall LockRegion( 
          /* [in] */ uint64_t libOffset,
          /* [in] */ uint64_t cb,
          /* [in] */ LOCKTYPE dwLockType) = 0;

        virtual hresult __stdcall UnlockRegion( 
          /* [in] */ uint64_t libOffset,
          /* [in] */ uint64_t cb,
          /* [in] */ LOCKTYPE dwLockType) = 0;

        virtual hresult __stdcall Stat( 
          /* [out] */ STATSTG *pstatstg,
          /* [in] */ STATFLAG grfStatFlag) = 0;

        virtual hresult __stdcall Clone( 
          /* [out] */ IStream **ppstm) = 0;

      };
    
    } // com

    /**@} winapi_types_support */
  }//namespace win
}//namespace ntl

#endif//#ifndef NTL__WIN_COM
