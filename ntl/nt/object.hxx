/**\file*********************************************************************
 *                                                                     \brief
 *  NT Objects
 *
 ****************************************************************************
 */

#ifndef NTL__NT_OBJECT
#define NTL__NT_OBJECT

#include "basedef.hxx"
#include "string.hxx"
#include "handle.hxx"


namespace ntl {
namespace nt {


struct security_descriptor;
struct security_quality_of_service;

struct object_attributes
{
    enum attributes
    {
      none                = 0,
      inherit             = 0x00000002L,
      permanent           = 0x00000010L,
      exclusive           = 0x00000020L,
      case_insensitive    = 0x00000040L,
      openif              = 0x00000080L,
      openlink            = 0x00000100L,
      kernel_handle       = 0x00000200L,
      force_access_check  = 0x00000400L,
      valid_attributes    = 0x000007F2L,
    };

    friend attributes operator | (attributes m, attributes m2) 
    { 
      return bitwise_or(m, m2);
    }

    object_attributes(
      const const_unicode_string &  name,
      const attributes              attr      = case_insensitive,
      const security_descriptor *   security  = 0)
    : Length(sizeof(*this)), RootDirectory(0), ObjectName(&name),
      Attributes(attr), SecurityDescriptor(security), SecurityQualityOfService(0)
    {/**/}

    object_attributes(
      const legacy_handle &         root,
      const const_unicode_string &  name,
      const attributes              attr      = case_insensitive,
      const security_descriptor *   security  = 0)
    : Length(sizeof(*this)), RootDirectory(root), ObjectName(&name),
      Attributes(attr), SecurityDescriptor(security), SecurityQualityOfService(0)
    {/**/}

    uint32_t                            Length;
    legacy_handle                       RootDirectory;
    const const_unicode_string *        ObjectName;
    attributes                          Attributes;
    const security_descriptor *         SecurityDescriptor;
    const security_quality_of_service * SecurityQualityOfService;

  private:

    //object_attributes(const object_attributes &);
    const object_attributes & operator=(const object_attributes &);

};

enum object_information_class
{
	ObjectBasicInformation,
	ObjectNameInformation,
	ObjectTypeInformation,
	ObjectTypesInformation,
	ObjectHandleFlagInformation,
	ObjectSessionInformation,
	MaxObjectInfoClass
};

struct object_basic_information
{
	uint32_t Attributes;
	access_mask GrantedAccess;
	uint32_t HandleCount;
	uint32_t PointerCount;
	uint32_t PagedPoolCharge;
	uint32_t NonPagedPoolCharge;
	uint32_t Reserved[3];
	uint32_t NameInfoSize;
	uint32_t TypeInfoSize;
	uint32_t SecurityDescriptorSize;
	int64_t CreationTime;
};

struct object_name_information
{
	unicode_string Name;
};

struct object_type_information
{
	unicode_string TypeName;
	uint32_t TotalNumberOfObjects;
	uint32_t TotalNumberOfHandles;
	uint32_t TotalPagedPoolUsage;
	uint32_t TotalNonPagedPoolUsage;
	uint32_t TotalNamePoolUsage;
	uint32_t TotalHandleTableUsage;
	uint32_t HighWaterNumberOfObjects;
	uint32_t HighWaterNumberOfHandles;
	uint32_t HighWaterPagedPoolUsage;
	uint32_t HighWaterNonPagedPoolUsage;
	uint32_t HighWaterNamePoolUsage;
	uint32_t HighWaterHandleTableUsage;
	uint32_t InvalidAttributes;
	generic_mapping GenericMapping;
	uint32_t ValidAccessMask;
	bool SecurityRequired;
	bool MaintainHandleCount;
	uint32_t PoolType;
	uint32_t DefaultPagedPoolCharge;
	uint32_t DefaultNonPagedPoolCharge;
};

struct object_types_information
{
	uint32_t NumberOfObjectTypes;
	object_type_information ObjectTypeInformation[1];
};

struct object_handle_flag_information
{
	bool InheritHandle;
	bool ProtectFromClose;
};

struct object_session_information
{
	uint32_t SessionId;
};

NTL__EXTERNAPI
ntstatus __stdcall
NtQueryObject(
		legacy_handle ObjectHandle,
		object_information_class ObjectInformationClass,
		void* ObjectInformation,
		int32_t Length,
		int32_t* ResultLength
	  );

}//namespace nt
}//namespace ntl


#endif//#ifndef NTL__NT_OBJECT
