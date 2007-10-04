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


}//namespace nt
}//namespace ntl


#endif//#ifndef NTL__NT_OBJECT
