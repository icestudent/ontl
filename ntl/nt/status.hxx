/**\file*********************************************************************
 *                                                                     \brief
 *  NT Status codes
 *
 ****************************************************************************
 */

#ifndef NTL__NT_STATUS
#define NTL__NT_STATUS


namespace ntl {
namespace nt {

/**\addtogroup  native_types_support *** NT Types support library ***********
 *@{*/

struct status
{
  enum type
  {
    success               = 0,
    pending               = 0x00000103,

    buffer_overflow           = (int)0x80000005,

    no_more_entries           = (int)0x8000001A,

    unsuccessful              = (int)0xC0000001,
    not_implemented           = (int)0xC0000002,

    info_length_mismatch      = (int)0xC0000004,

    invalid_device_request    = (int)0xC0000010,

    more_processing_required  = (int)0xC0000016,

    buffer_too_small          = (int)0xC0000023,

    insufficient_resources    = (int)0xC000009A,

    invalid_image_not_mz      = (int)0xC000012F,
    invalid_image_win_32      = (int)0xC0000359,
    entrypoint_not_found      = (int)0xC0000139,

    driver_entrypoint_not_found = (int)0xC0000263,

    device_configuraton_error = (int)0xC0000182,
  };
};

static __forceinline
bool success(const status::type & s)
{
  return s >= 0;
}

/**@} native_types_support */

typedef nt::status::type ntstatus;

}//namespace nt

using nt::ntstatus;

}//namespace ntl

#endif//#ifndef NTL__NT_BASEDEF
