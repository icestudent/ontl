/**\file*********************************************************************
 *                                                                     \brief
 *  Registry support
 *
 ****************************************************************************
 */

#ifndef NTL__KM_REGISTRY
#define NTL__KM_REGISTRY

#include "basedef.hxx"
#include "../nt/registry.hxx"


namespace ntl {
namespace km {


enum reg_notify_class
{
  RegNtPreDeleteKey,              ///< 0
  RegNtPreSetValueKey,            ///< 1
  RegNtPreDeleteValueKey,         ///< 2
  RegNtPreSetInformationKey,      ///< 3
  RegNtPreRenameKey,              ///< 4
  RegNtPreEnumerateKey,           ///< 5
  RegNtPreEnumerateValueKey,      ///< 6
  RegNtPreQueryKey,               ///< 7
  RegNtPreQueryValueKey,          ///< 8
  RegNtPreQueryMultipleValueKey,  ///< 9
  RegNtPreCreateKey,              ///< 10
  RegNtPostCreateKey,             ///< 11
  RegNtPreOpenKey,                ///< 12
  RegNtPostOpenKey,               ///< 13
  RegNtPreKeyHandleClose,         ///< 14

  // 2K3+
  RegNtPostDeleteKey,
  RegNtPostSetValueKey,
  RegNtPostDeleteValueKey,
  RegNtPostSetInformationKey,
  RegNtPostRenameKey,
  RegNtPostEnumerateKey,
  RegNtPostEnumerateValueKey,
  RegNtPostQueryKey,
  RegNtPostQueryValueKey,
  RegNtPostQueryMultipleValueKey,
  RegNtPostKeyHandleClose,
  RegNtPreCreateKeyEx,
  RegNtPostCreateKeyEx,
  RegNtPreOpenKeyEx,
  RegNtPostOpenKeyEx,

  // Vista +
  RegNtPreFlushKey,
  RegNtPostFlushKey,
  RegNtPreLoadKey,
  RegNtPostLoadKey,
  RegNtPreUnLoadKey,
  RegNtPostUnLoadKey,

  RegNtCallbackContextCleanup
};


typedef
ntstatus __stdcall
  registry_callback_t(
    void *            CallbackContext,
    reg_notify_class  Argument1,
    void *            Argument2 
    );

typedef
ntstatus __stdcall
  cm_register_callback_t(
    registry_callback_t * Function,
    void *                Context,
    int64_t *             Cookie 
    );

NTL__EXTERNAPI
cm_register_callback_t CmRegisterCallback;

typedef
ntstatus __stdcall
  cm_unregister_callback_t(
    int64_t *             Cookie 
    );

NTL__EXTERNAPI
cm_unregister_callback_t CmUnRegisterCallback;



struct cm_name_hash
{
  /* 0x00 */ uint32_t       ConvKey;
  /* 0x04 */ cm_name_hash * NextHash;
  /* 0x08 */ uint16_t       NameLength;
  /* 0x0a */ wchar_t        Name[1];
};
//STATIC_ASSERT(sizeof(cm_name_hash) == 0xc);

struct cm_name_control_block
{
  /* 0x00 */ bool           Compressed;
  /* 0x02 */ unsigned short RefCount;
  /* 0x04 */ cm_name_hash   NameHash;
};
//STATIC_ASSERT(sizeof(cm_name_control_block) == 0x10);


struct cm_key_hash
{
  /* 0x00 */ uint32_t       ConvKey;
  /* 0x04 */ cm_key_hash *  NextHash;
  /* 0x08 */ struct _HHIVE* KeyHive;
  /* 0x0c */ uint32_t       KeyCell;
};
//STATIC_ASSERT(sizeof(cm_key_hash) == 0x10);


struct cm_key_control_block;

struct cached_child_list 
{
  /* 0x00 */ uint32_t               Count;
  union {
  /* 0x04 */ uint32_t               ValueList;
  /* 0x04 */ cm_key_control_block * RealKcb;
  };
};
//STATIC_ASSERT(sizeof(cached_child_list) == 0x8);

///\warning XP SP2
struct cm_key_control_block
{
#ifndef _WIN64
  /* 0x00 */ uint16_t                 RefCount;
  /* 0x02 */ uint16_t                 Flags;
#else
			 uint32_t				  RefCount;
#endif
  /* 0x04 */ uint32_t                 ExtFlags          /*:0x00*/ :8;
  /* 0x04 */ uint32_t                 PrivateAlloc      /*:0x08*/ :1;
  /* 0x04 */ uint32_t                 Delete            /*:0x09*/ :1;
  /* 0x04 */ uint32_t                 DelayedCloseIndex /*:0x0a*/ :0xc;
  /* 0x04 */ uint32_t                 TotalLevels       /*:0x16*/ :0xa;
  union {
	  cm_key_hash           KeyHash;
	  struct {
		  uint32_t          ConvKey;
		  cm_key_hash		*NextHash;
		  struct hhive*     KeyHive;
		  uint32_t			KeyCell;
	  };
  };
  /* 0x18 */ cm_key_control_block *   ParentKcb;
  /* 0x1c */ cm_name_control_block *  NameBlock;
  /* 0x20 */ struct _CM_KEY_SECURITY_CACHE* CachedSecurity;
  /* 0x24 */ cached_child_list        ValueCache;
  union {
  /* 0x2c */ struct _CM_INDEX_HINT_BLOCK* IndexHint;
  /* 0x2c */ uint32_t                 HashKey;
  /* 0x2c */ uint32_t                 SubKeyCount;
  };
  union {
  /* 0x30 */ list_entry               KeyBodyListHead;
  /* 0x30 */ list_entry               FreeListEntry;
  };
#if NTDDI_VERSION >= NTDDI_WS03
	struct cm_key_body*				  KeyBodyArray[4];
	void*							  DelayCloseEntry;
#endif
  /* 0x38 */ int64_t                  KcbLastWriteTime;
  /* 0x40 */ uint16_t                 KcbMaxNameLen;
  /* 0x42 */ uint16_t                 KcbMaxValueNameLen;
  /* 0x44 */ uint32_t                 KcbMaxValueDataLen;
#if defined(_WIN64)
	uint32_t						  KcbUserFlags:4;
	uint32_t						  KcbVirtControlFlags:4;
	uint32_t						  KcbDebug:8;
	uint32_t						  Flags:10;
	char*							  RealKeyName;
#endif
};// struct cm_key_control_block
//STATIC_ASSERT(sizeof(cm_key_control_block) == 0x48);


struct cm_key_body
{
  /* 0x00 */ uint32_t               Type;
  /* 0x04 */ cm_key_control_block * KeyControlBlock;
  /* 0x08 */ struct _CM_NOTIFY_BLOCK* NotifyBlock;
};

///\note XP version
struct cm_key_body_51 : cm_key_body
{
  /* 0x0c */ legacy_handle  ProcessID;
  /* 0x10 */ uint32_t       Callers;
  /* 0x14 */ void *         CallerAddress[10];
  /* 0x3c */ list_entry     KeyBodyList;
};
//STATIC_ASSERT(sizeof(cm_key_body_51) == 0x44);

struct cm_key_body_52: cm_key_body
{
	/* 0x18 */ uint32_t				ProcessID;
	/* 0x20 */ list_entry			KeyBodyList;
};

using nt::key_information_class;
using nt::KeyBasicInformation;
using nt::KeyNodeInformation;
using nt::KeyFullInformation;


/// RegNtDeleteKey
struct reg_delete_key_information
{
  cm_key_body *  Object;
};

///\todo RegNtPreSetValueKey

/// RegNtDeleteValueKey
struct reg_delete_value_key_information
{
  cm_key_body *                 Object;
  const const_unicode_string *  ValueName;
};

///\todo RegNtPreSetInformationKey
///\todo RegNtPreRenameKey

/// RegNtPreEnumerateKey
struct reg_enumerate_key_information
{
  cm_key_body *         Object;
  uint32_t              Index;
  key_information_class KeyInformationClass;
  void *                KeyInformation;
  uint32_t              Length;
  uint32_t *            ResultLength;
};

///\todo RegNtPreEnumerateValueKey

/// RegNtPreCreateKey
struct reg_pre_create_key_information
{
  const const_unicode_string *  CompleteName;
};

/// RegNtPreQueryKey
struct reg_query_key_information
{
  cm_key_body *         Object;
  key_information_class KeyInformationClass;
  void *                KeyInformation;
  uint32_t              Length;
  uint32_t *            ResultLength;
};

///\todo RegNtPreQueryValueKey
///\todo RegNtPreQueryMultipleValueKey
///\todo RegNtPreCreateKey
///\todo RegNtPostCreateKey
///\todo RegNtPreOpenKey
///\todo RegNtPostOpenKey

/// RegNtPreKeyHandleClose
struct reg_key_handle_close_information
{
  cm_key_body *  Object;
};


}//namespace km
}//namespace ntl

#endif//#ifndef NTL__KM_REGISTRY
