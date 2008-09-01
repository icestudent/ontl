/**\file*********************************************************************
 *                                                                     \brief
 *  Registry support
 *
 ****************************************************************************
 */

#ifndef NTL__NT_REGISTRY
#define NTL__NT_REGISTRY

#include "../iterator"
#include "../string"
#include "../device_traits.hxx"
#include "basedef.hxx"
#include "handle.hxx"
#include "object.hxx"


namespace ntl {
namespace nt {

/**\addtogroup  registry **************** Registry API *********************
 *@{*/

///\note  Because ntoskernel does not export Nt* variants,
///       Zw* exports are used here, and this header is to be used from KM;
///       howecer, DO remember nt::object_attributes have
///       no default kernel_handle flag set

///\name  Legacy API

NTL__EXTERNAPI
ntstatus __stdcall
  ZwCreateKey(
    handle *                      KeyHandle,
    uint32_t                      DesiredAccess,
    const object_attributes *     ObjectAttributes,
    uint32_t                      TitleIndex,
    const const_unicode_string *  Class             __optional,
    uint32_t                      CreateOptions,
    uint32_t *                    Disposition       __optional
    );

NTL__EXTERNAPI
ntstatus __stdcall
  ZwOpenKey(
    handle *                  KeyHandle,
    uint32_t                  DesiredAccess,
    const object_attributes * ObjectAttributes
    );

enum key_value_information_class
{
  KeyValueBasicInformation,
  KeyValueFullInformation,
  KeyValuePartialInformation
};

NTL__EXTERNAPI
ntstatus __stdcall
  ZwQueryValueKey(
    legacy_handle                 KeyHandle,
    const const_unicode_string *  ValueName,
    key_value_information_class   KeyValueInformationClass,
    void *                        KeyValueInformation,
    uint32_t                      Length,
    uint32_t *                    ResultLength
    );

NTL__EXTERNAPI
ntstatus __stdcall
  ZwEnumerateValueKey(
    legacy_handle                 KeyHandle,
    uint32_t                      Index,
    key_value_information_class   KeyValueInformationClass,
    void *                        KeyValueInformation,
    uint32_t                      Length,
    uint32_t *                    ResultLength
    );

NTL__EXTERNAPI
ntstatus __stdcall
  ZwSetValueKey(
    legacy_handle                 KeyHandle,
    const const_unicode_string *  ValueName,
    uint32_t                      TitleIndex  __optional,
    uint32_t                      Type,
    const void *                  Data,
    uint32_t                      DataSize
    );

NTL__EXTERNAPI
ntstatus __stdcall
  ZwDeleteKey(
    legacy_handle KeyHandle
    );

NTL__EXTERNAPI
ntstatus __stdcall
  ZwFlushKey(
    legacy_handle KeyHandle
    );

enum key_information_class
{
  KeyBasicInformation,
  KeyNodeInformation,
  KeyFullInformation
};

NTL__EXTERNAPI
ntstatus __stdcall
  ZwEnumerateKey(
    legacy_handle         KeyHandle,
    uint32_t              Index,
    key_information_class KeyInformationClass,
    void *                KeyInformation,
    uint32_t              Length,
    uint32_t *            ResultLength
    );

///@}

class key;

/**@} registry */

}//namespace nt


template<>
struct device_traits<nt::key> : public device_traits<>
{
  typedef uint32_t  size_type;

  enum access_mask
  {
    query_value         = 0x0001,
    set_value           = 0x0002,
    create_sub_key      = 0x0004,
    enumerate_sub_keys  = 0x0008,
    notify              = 0x0010,
    create_link         = 0x0020,
    wow64_64key         = 0x0100,
    wow64_32key         = 0x0200,
    wow64_res           = wow64_64key | wow64_32key,

    read  = standard_rights_read | query_value | enumerate_sub_keys | notify,
    write = standard_rights_write | set_value | create_sub_key,
    execute = read,
    all_access = standard_rights_all | query_value | set_value | create_sub_key
    | enumerate_sub_keys | notify | create_link,

  };
  static const access_mask access_mask_default = access_mask(read);

  friend access_mask operator | (access_mask m, access_mask m2)
  {
    return bitwise_or(m, m2);
  }

  friend access_mask operator | (access_mask m, nt::access_mask m2)
  {
    return m | static_cast<access_mask>(m2);
  }

  friend access_mask operator | (nt::access_mask m, access_mask m2)
  {
    return m2 | m;
  }


  enum creation_options
  {
    option_reserved       = 0x00000000,
    option_non_volatile   = 0x00000000,
    option_volatile       = 0x00000001,
    option_create_link    = 0x00000002,
    option_backup_restore = 0x00000004,
    option_open_link      = 0x00000008,
  };
  static const creation_options creation_options_default = creation_options(0x00000000);

  friend creation_options operator | (creation_options m, creation_options m2)
  {
    return bitwise_or(m, m2);
  }

  enum  disposition
  {
    created_new_key     = 0x00000001L,
    opened_existing_key = 0x00000002L
  };

};


namespace nt {

class key : public handle, public device_traits<key>
{
    static const uint32_t query_buf_default_size = 64; // 32 seems to be smallest pool block size

    const key & operator=(const key &);

  ////////////////////////////////////////////////////////////////////////////
  public:

      /// Root key constructor
      key() {/**/}

//      key(const key&);

    bool
    __forceinline
      create(
        const object_attributes &     oa,
        const access_mask             desired_access,
        const creation_options        co          = creation_options_default,
        disposition *                 cd          = 0,
        uint32_t                      title_index = 0,
        const const_unicode_string *  key_class   = 0
        ) __ntl_nothrow
    {
      this->handle::reset();
      return nt::success(ZwCreateKey(this, desired_access, &oa, title_index,
                                key_class, co, reinterpret_cast<uint32_t*>(cd)));
    }

    bool
    __forceinline
      open(
        const object_attributes & oa,
        const access_mask         desired_access  = access_mask_default
        ) __ntl_nothrow
    {
      this->handle::reset();
      return nt::success(ZwOpenKey(this, desired_access, &oa));
    }

    __forceinline
    explicit key(
        const object_attributes & oa,
        const access_mask         desired_access  = access_mask_default
        )
    {
      __assume(subkey_end_ == subkey_iterator());
      open(oa, desired_access);
    }

    __forceinline
    explicit key(
        const const_unicode_string &  name,
        const access_mask             desired_access  = access_mask_default
        )
    {
      __assume(subkey_end_ == subkey_iterator());
      open(name, desired_access);
    }

    __forceinline
    explicit key(
        const handle &        root,
        const std::wstring &  name,
        const access_mask     desired_access  = access_mask_default
        )
    {
      __assume(subkey_end_ == subkey_iterator());
      open(object_attributes(root.get(), const_unicode_string(name)),
            desired_access);
    }

    __forceinline
    ~key()
    {
      __assume(subkey_end_ == subkey_iterator());
    }

    operator const handle &() const
    {
      return *this;
    }

    operator const void*() { return get(); }

    bool erase()
    {
      const ntstatus res = ZwDeleteKey(this->get());
      this->handle::release();
      return nt::success(res);
    }

    bool flush()
    {
      return nt::success(ZwFlushKey(this->get()));
    }

    enum value_type
    {
      reg_none                = 0,
      reg_sz                  = 1,
      reg_expand_sz           = 2,
      reg_binary              = 3,
      reg_dword               = 4,
      reg_dword_little_endian = reg_dword,
      reg_dword_big_endian    = 5,
      reg_link                = 6,
      reg_multi_sz            = 7,
      reg_resource_list       = 8,
      reg_fill_resource_descriptor  = 9,
      reg_resource_requirements_list  = 10,
      reg_qword               = 11,
      reg_qword_little_endian = reg_qword,
    };

    struct value_basic_information
    {
      uint32_t    TitleIndex;
      value_type  Type;
      uint32_t    NameLength; // in bytes
      wchar_t     Name[1];    // variable size
    };

    struct value_partial_information
    {
      uint32_t    TitleIndex;
      value_type  Type;
      uint32_t    DataLength; // in bytes
      uint8_t     Data[1];    // variable size
    };

    struct value_full_information
    {
      uint32_t    TitleIndex;
      value_type  Type;
      uint32_t    DataOffset;
      uint32_t    DataLength;
      uint32_t    NameLength; // in bytes
      wchar_t     Name[1];    // variable size
    };

    ntstatus
      query(
        const const_unicode_string &  value_name,
        key_value_information_class   info_class,
        void *                        information,
        uint32_t                      length,
        uint32_t &                    result_length
        ) const __ntl_nothrow
    {
      return ZwQueryValueKey(this->get(), &value_name, info_class, information,
                              length, &result_length);
    }

    __forceinline
    bool
      query(
        const const_unicode_string &  value_name,
        std::wstring &                value_string
        ) const
    {
      uint32_t size = query_buf_default_size;
      STATIC_ASSERT(query_buf_default_size > sizeof(value_partial_information));
      for ( ; ; )
      {
        uint8_t * const buf = new uint8_t[size];
        const ntstatus s = query(value_name, KeyValuePartialInformation,
                                       buf, size, size);
        const value_partial_information * const p =
                              reinterpret_cast<value_partial_information*>(buf);
        switch ( s )
        {
          // case status::buffer_too_small: // may be only if size < sizeof(value_partial_information)
          case status::buffer_overflow:
            delete[] buf;
            continue;
          case status::success:
            if ( p->Type == reg_sz || p->Type == reg_expand_sz || p->Type == reg_multi_sz ) //reg_link
            {
              value_string.assign(reinterpret_cast<const wchar_t*>(&p->Data[0]),
                                  p->DataLength / sizeof(wchar_t));
              delete[] buf;
              return true;
            }
          default:
            delete[] buf;
            return false;
        }
      }
    }

    __forceinline
    bool
      query(
        const const_unicode_string &  value_name,
        std::vector<uint8_t> &        data
        ) const
    {
      uint32_t size = 0;
      uint8_t * buf = 0;
      for ( ; ; )
      {
        const ntstatus s = query(value_name, KeyValuePartialInformation,
                                       buf, size, size);
        const value_partial_information * const p =
                              reinterpret_cast<value_partial_information*>(buf);
        if ( s == status::success && p->Type == reg_binary )
        {
          data.assign(&p->Data[0], &p->Data[p->DataLength + 1]);
          delete[] buf;
          return true;
        }
        delete[] buf;
        if ( s != status::buffer_overflow && s != status::buffer_too_small )
          return false;
        buf = new uint8_t[size];
      }
    }

    __forceinline
    bool
      query(
        const const_unicode_string &  value_name,
        uint32_t &                    value
        ) const
    {
      uint32_t dummy_size;
      uint8_t buf[sizeof(value_partial_information)+sizeof(uint32_t)-sizeof(uint8_t)];
      const ntstatus s = query(value_name, KeyValuePartialInformation,
                                    buf, sizeof(buf), dummy_size);
      const value_partial_information * const p =
                        reinterpret_cast<value_partial_information*>(buf);
      if ( !nt::success(s) || p->Type != reg_dword_little_endian )
        return false;
      value = *reinterpret_cast<const uint32_t*>(&p->Data[0]);
      return true;
    }

    ntstatus
      set(
        const const_unicode_string &  value_name,
        value_type                    type,
        const void *                  information,
        uint32_t                      length,
        uint32_t                      title_index  = 0
        ) __ntl_nothrow
    {
      return ZwSetValueKey(this->get(), &value_name, title_index, type, information,
                              length);
    }

    bool
      set(
        const const_unicode_string &  value_name,
        uint32_t                      value)
    {
      return nt::success(set(value_name, reg_dword_little_endian, &value, sizeof(value)));
    }

    bool
      set(
        const const_unicode_string &  value_name,
        const const_unicode_string &  value)
    {
      return nt::success(set(value_name, reg_sz, value.begin(),
                        value.size() * sizeof(const_unicode_string::value_type)));
    }

    bool
      set(
        const const_unicode_string &  value_name,
        const std::wstring &          value,
        value_type                    type        = reg_sz)
    {
      return nt::success(set(value_name, type, value.begin(),
                              (uint32_t)(value.size() * sizeof(std::wstring::value_type))));
    }


    struct basic_information
    {
      int64_t   LastWriteTime;
      uint32_t  TitleIndex;
      uint32_t  NameLength;
      wchar_t   Name[1];  // variable-length string
    };

    struct key_node_information
    {
      int64_t   LastWriteTime;
      uint32_t  TitleIndex;
      uint32_t  ClassOffset;
      uint32_t  ClassLength;
      uint32_t  NameLength;
      wchar_t   Name[1];  //  Variable-length string
    };

    static
    ntstatus
      enumerate_key(
        legacy_handle         hkey,
        uint32_t              index,
        key_information_class info_class,
        void *                key_info,
        uint32_t              length,
        uint32_t &            result_length
        )
    {
      return ZwEnumerateKey(hkey, index, info_class, key_info, length, &result_length);
    }

    ntstatus
      enumerate_key(
        uint32_t              index,
        key_information_class info_class,
        void *                key_info,
        uint32_t              length,
        uint32_t &            result_length
        )
    {
      return enumerate_key(get(), index, info_class, key_info, length, result_length);
    }


    struct subkey_iterator
    : public std::iterator<std::input_iterator_tag, std::wstring>
    {
        __forceinline
        ~subkey_iterator() {}

        reference operator* ()  { return name; }
        pointer   operator->()  { return &(operator*()); }

        __forceinline
        subkey_iterator & operator++()
        {
          ///\note does not handle ++subkey_end()
          ++index;
          read();
          return *this;
        }

      friend
        bool operator==(const subkey_iterator & x, const subkey_iterator & y)
          { return /*x.key == y.key &&*/ x.index == y.index; }

      friend
        bool operator!=(const subkey_iterator & x, const subkey_iterator & y)
          { return !(x == y); }

      ///////////////////////////////////////////////////////////////////////////
      private:

        __forceinline
        subkey_iterator()
        : index(end_index)
        {/**/}

        friend class key;

        static const uint32_t begin_index = 0;
        static const uint32_t end_index = static_cast<uint32_t>(-1);

        __forceinline
        subkey_iterator(legacy_handle hkey, uint32_t index)
        : hkey(hkey), index(index)
        {
          read();
        }

        legacy_handle hkey;
        uint32_t      index;
        std::wstring  name;

        __forceinline
        void read()
        {
          uint32_t size = query_buf_default_size;
          STATIC_ASSERT(query_buf_default_size > sizeof(basic_information));
          for ( ; ; )
          {
            uint8_t * const buf = new uint8_t[size];
            const basic_information & p = *reinterpret_cast<basic_information*>(buf);
            const ntstatus s = enumerate_key(hkey, index, KeyBasicInformation,
                                              buf, size, size);
            switch ( s )
            {
              //case status::buffer_too_small:  // may be only if size < sizeof(basic_information)
              case status::buffer_overflow:
                delete[] buf;
                continue;
              case status::success:
                name.assign(&p.Name[0], p.NameLength / sizeof(wchar_t));
                delete[] buf;
                return; // true
              case status::no_more_entries:
                index = end_index;
              default:
                delete[] buf;
                return; // false
            }
          }
        }

    };

    __forceinline
    subkey_iterator subkey_begin() const
    {
      return subkey_iterator(get() , subkey_iterator::begin_index);
    }

    __forceinline // God bless RVO
    const subkey_iterator & subkey_end() const
    {
      return subkey_end_;
    }

    const subkey_iterator subkey_end_;

}; // class key


namespace rtl_registry {
  enum Relative {
    absolute,
    services,
    control,
    windows_nt,
    devicemap,
    user,
    maximum,
    handle    = 0x40000000,
    optional  = 0x80000000
  };

  enum Flags {
    query_subkey   = 0x00000001,
    query_topkey   = 0x00000002,
    query_required = 0x00000004,
    query_novalue  = 0x00000008,
    query_noexpand = 0x00000010,
    query_direct   = 0x00000020,
    query_delete   = 0x00000040
  };
}

typedef ntstatus __stdcall QueryRoutine_t(
    wchar_t* ValueName,
    uint32_t ValueType,
    void*    ValueData,
    uint32_t ValueLength,
    void*    Context,
    void*    EntryContext
    );

struct rtl_query_registry_table {
  QueryRoutine_t* QueryRoutine;
  rtl_registry::Flags Flags;
  wchar_t* Name;
  void*    EntryContext;
  uint32_t DefaultType;
  void*    DefaultData;
  uint32_t DefaultLength;
};

NTL__EXTERNAPI
ntstatus __stdcall
  RtlCreateRegistryKey(
    rtl_registry::Relative RelativeTo,
    const wchar_t* Path
    );

NTL__EXTERNAPI
ntstatus __stdcall
  RtlCheckRegistryKey(
    rtl_registry::Relative RelativeTo,
    const wchar_t* Path
    );

NTL__EXTERNAPI
ntstatus __stdcall
  RtlQueryRegistryValues(
    rtl_registry::Relative    RelativeTo,
    const wchar_t*            Path,
    rtl_query_registry_table* QueryTable,
    void*                     Context,
    void*                     Environment
    );

NTL__EXTERNAPI
ntstatus __stdcall
  RtlWriteRegistryValue(
    rtl_registry::Relative RelativeTo,
    const wchar_t*  Path,
    const wchar_t*  ValueName,
    key::value_type ValueType,
    const void*     ValueData,
    uint32_t        ValueLength
    );

NTL__EXTERNAPI
ntstatus __stdcall
  RtlDeleteRegistryValue(
    rtl_registry::Relative RelativeTo,
    const wchar_t* Path,
    const wchar_t* ValueName
    );

NTL__EXTERNAPI
ntstatus __stdcall
  RtlFormatCurrentUserKeyPath(
    unicode_string* CurrentUserKeyPath
    );


}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_REGISTRY
