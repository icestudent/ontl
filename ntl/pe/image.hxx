/**\file*********************************************************************
*                                                                     \brief
*  Portable Executable images support
*
****************************************************************************
*/

#ifndef NTL__PE_IMAGE
#define NTL__PE_IMAGE

#include "../stdlib.hxx"
#include "../basedef.hxx"
#include "../cstring"

namespace ntl {
  namespace pe {


    /**\addtogroup  pe_images_support *** Portable Executable images support*****
    *@{*/

    class image
    {
      ///////////////////////////////////////////////////////////////////////////
    public:

      static __forceinline image * this_module();

      static image * bind(uintptr_t image_base)
      {
        return reinterpret_cast<image*>(image_base);
      }

      static image * bind(void * image_base)
      {
        return reinterpret_cast<image*>(image_base);
      }

      template<typename T>
      static __forceinline
        bool in_range(uintptr_t first, uintptr_t last, T p)
      {
        return first <= brute_cast<uintptr_t>(p) && brute_cast<uintptr_t>(p) < last;
      }

      ///\ name   Headers

      struct dos_header
      {
        bool is_valid() const { return e_magic == signature; }

        static const uint16_t signature = 0x5A4D; //'MZ'
        uint16_t  e_magic;
        uint16_t  e_cblp;
        uint16_t  e_cp;
        uint16_t  e_crlc;
        uint16_t  e_cparhdr;
        uint16_t  e_minalloc;
        uint16_t  e_maxalloc;
        uint16_t  e_ss;
        uint16_t  e_sp;
        uint16_t  e_csum;
        uint16_t  e_ip;
        uint16_t  e_cs;
        uint16_t  e_lfarlc;
        uint16_t  e_ovno;
        uint16_t  e_res[4];
        uint16_t  e_oemid;
        uint16_t  e_oeminfo;
        uint16_t  e_res2[10];
        long      e_lfanew;
      }; // struct dos_header
      STATIC_ASSERT(sizeof(dos_header)==0x40);


      struct file_header
      {
        enum Machines {
          unknown,
          i386 = 0x014C,
          amd64= 0x8664
        };
        uint16_t  Machine;
        uint16_t  NumberOfSections;
        uint32_t  TimeDateStamp;
        uint32_t  PointerToSymbolTable;
        uint32_t  NumberOfSymbols;
        uint16_t  SizeOfOptionalHeader;
        uint16_t  Characteristics;
      };


      struct data_directory
      {
        uint32_t  VirtualAddress;
        uint32_t  Size;

        enum entry
        {
          export_table        = 0,
          import_table        = 1,
          resource_table      = 2,
          exception_table     = 3,
          security_table      = 4,
          basereloc_table     = 5,
          debug_directory     = 6,
          architecture        = 7,
          globalptr           = 8,
          tls_table           = 9,
          load_config_table   = 10,
          bound_import_table  = 11,
          iat_table           = 12,
          delay_import_table  = 13,
          com_descriptor      = 14,

          number_of_directory_entries = 16
        };

      };


      struct optional_header32
      {
        static const uint16_t signature = 0x010B;
        bool is_valid() const { return Magic == signature; }

        uint16_t        Magic;
        uint8_t         MajorLinkerVersion;
        uint8_t         MinorLinkerVersion;
        uint32_t        SizeOfCode;
        uint32_t        SizeOfInitializedData;
        uint32_t        SizeOfUninitializedData;
        uint32_t        AddressOfEntryPoint;
        uint32_t        BaseOfCode;
        uint32_t        BaseOfData;
        uint32_t        ImageBase;
        uint32_t        SectionAlignment;
        uint32_t        FileAlignment;
        uint16_t        MajorOperatingSystemVersion;
        uint16_t        MinorOperatingSystemVersion;
        uint16_t        MajorImageVersion;
        uint16_t        MinorImageVersion;
        uint16_t        MajorSubsystemVersion;
        uint16_t        MinorSubsystemVersion;
        uint32_t        Win32VersionValue;
        uint32_t        SizeOfImage;
        uint32_t        SizeOfHeaders;
        uint32_t        CheckSum;
        uint16_t        Subsystem;
        uint16_t        DllCharacteristics;
        uint32_t        SizeOfStackReserve;
        uint32_t        SizeOfStackCommit;
        uint32_t        SizeOfHeapReserve;
        uint32_t        SizeOfHeapCommit;
        uint32_t        LoaderFlags;
        uint32_t        NumberOfRvaAndSizes;
        data_directory  DataDirectory[data_directory::number_of_directory_entries];
      }; // struct optional_header32

      STATIC_ASSERT(sizeof(optional_header32) == 224);

      struct optional_header64
      {
        static const uint16_t signature = 0x020B;
        bool is_valid() const { return Magic == signature; }

        uint16_t        Magic;
        uint8_t         MajorLinkerVersion;
        uint8_t         MinorLinkerVersion;
        uint32_t        SizeOfCode;
        uint32_t        SizeOfInitializedData;
        uint32_t        SizeOfUninitializedData;
        uint32_t        AddressOfEntryPoint;
        uint32_t        BaseOfCode;
        uint64_t        ImageBase;
        uint32_t        SectionAlignment;
        uint32_t        FileAlignment;
        uint16_t        MajorOperatingSystemVersion;
        uint16_t        MinorOperatingSystemVersion;
        uint16_t        MajorImageVersion;
        uint16_t        MinorImageVersion;
        uint16_t        MajorSubsystemVersion;
        uint16_t        MinorSubsystemVersion;
        uint32_t        Win32VersionValue;
        uint32_t        SizeOfImage;
        uint32_t        SizeOfHeaders;
        uint32_t        CheckSum;
        uint16_t        Subsystem;
        uint16_t        DllCharacteristics;
        uint64_t        SizeOfStackReserve;
        uint64_t        SizeOfStackCommit;
        uint64_t        SizeOfHeapReserve;
        uint64_t        SizeOfHeapCommit;
        uint32_t        LoaderFlags;
        uint32_t        NumberOfRvaAndSizes;
        data_directory  DataDirectory[data_directory::number_of_directory_entries];
      }; // struct optional_header64

      STATIC_ASSERT(sizeof(optional_header64) == 240);

      struct nt_headers
      {
        static const uint32_t signature = 0x00004550; // PE\x0\x0
        bool is_valid() const { return Signature == signature; }

        uint32_t            Signature;
        file_header         FileHeader;
        union 
        {
          optional_header32 OptionalHeader32;
          optional_header64 OptionalHeader64;
        };

        image::optional_header32 * optional_header32()
        {
          return OptionalHeader32.is_valid() ? &OptionalHeader32 : 0;
        }

        const image::optional_header32 * optional_header32() const
        {
          return OptionalHeader32.is_valid() ? &OptionalHeader32 : 0;
        }

        image::optional_header64 * optional_header64()
        {
          return OptionalHeader64.is_valid() ? &OptionalHeader64 : 0;
        }

        const image::optional_header64 * optional_header64() const
        {
          return OptionalHeader64.is_valid() ? &OptionalHeader64 : 0;
        }

        const image::data_directory * data_directory(data_directory::entry entry) const
        {
          if ( optional_header32()
            && optional_header32()->NumberOfRvaAndSizes > uint32_t(entry) )
            return &optional_header32()->DataDirectory[entry];
          else if ( optional_header64()
            && optional_header64()->NumberOfRvaAndSizes > uint32_t(entry) )
            return &optional_header64()->DataDirectory[entry];
          return 0;
        }

        image::data_directory * data_directory(data_directory::entry entry)
        {
          return const_cast<image::data_directory*>(
            const_cast<const nt_headers*>(this)->data_directory(entry));
        }

      }; // struct nt_headers

      image::data_directory * get_data_directory(data_directory::entry entry)
      {
        nt_headers * const nth = get_nt_headers();
        return nth->is_valid() ? nth->data_directory(entry) : 0;
      }

      const image::data_directory * 
        get_data_directory(data_directory::entry entry) const
      {
        const nt_headers * const nth = get_nt_headers();
        return nth->is_valid() ? nth->data_directory(entry) : 0;
      }

      uintptr_t base() const
      {
        return reinterpret_cast<uintptr_t>(this); 
      }

      /// converts RVA to VA
      template<typename T>
      T va(uintptr_t rva) const
      {
        return reinterpret_cast<T>(base() + rva);
      }

      uintptr_t va(uintptr_t rva) const
      {
        return base() + rva;
      }

      /// converts VA to RVA
      uintptr_t rva(uintptr_t va) const
      {
        return va - base();
      }

      dos_header * get_dos_header()
      { 
        return va<dos_header*>(0); 
      }

      const dos_header * get_dos_header() const
      { 
        return va<const dos_header*>(0); 
      }

      nt_headers * get_nt_headers() 
      { 
        return va<nt_headers*>(get_dos_header()->e_lfanew); 
      }

      const nt_headers * get_nt_headers() const
      { 
        return va<const nt_headers*>(get_dos_header()->e_lfanew); 
      }

      uint32_t checksum() const
      {
        const dos_header * const dh = get_dos_header();
        if ( !dh->is_valid() ) return 0;
        const nt_headers * const nth = get_nt_headers();
        uint32_t sum = 0;
        for ( const uint16_t * p = va<uint16_t*>(0);
          p < va<uint16_t*>(nth->OptionalHeader32.SizeOfImage); // care about odd SizeOfImage
          ++p )
        {
          // Can not use C flag in HLL :-(
          sum += *p;
          sum += sum >> 16;
          sum &= 0xFFFF;
        }
        sum -= ( sum < static_cast<uint16_t>(nth->OptionalHeader32.CheckSum) );
        sum -= static_cast<uint16_t>(nth->OptionalHeader32.CheckSum);
        sum -= ( sum < static_cast<uint16_t>(nth->OptionalHeader32.CheckSum >> 16) );
        sum -= static_cast<uint16_t>(nth->OptionalHeader32.CheckSum >> 16);
        return nth->OptionalHeader32.SizeOfImage + sum;
      }

      uint32_t checksum(bool update)
      {
        const uint32_t sum = checksum();
        if ( update ) get_nt_headers()->OptionalHeader32.CheckSum = sum;
        return sum;
      }


      struct section_header  
      {
        static const size_t sizeof_short_name = 8;
        char        Name[sizeof_short_name];
        union
        { 
          uint32_t  PhysicalAddress;
          uint32_t  VirtualSize;
        };
        uint32_t    VirtualAddress;
        uint32_t    SizeOfRawData;
        uint32_t    PointerToRawData;
        uint32_t    PointerToRelocations;
        uint32_t    PointerToLinenumbers;
        uint16_t    NumberOfRelocations;
        uint16_t    NumberOfLinenumbers;
        uint32_t    Characteristics;
      };

      STATIC_ASSERT(sizeof(section_header) == 40);

      section_header * get_section_header(size_t n = 0)
      {
        nt_headers * const nth = get_nt_headers();
        section_header * sh = (section_header*)(uintptr_t(&nth->OptionalHeader32)
          + nth->FileHeader.SizeOfOptionalHeader);
        return n < nth->FileHeader.NumberOfSections ? &sh[n] : 0;
      }

      section_header * get_section_header(const char name[])
      {
        nt_headers * const nth = get_nt_headers();
        section_header * sh = (section_header*)(uintptr_t(&nth->OptionalHeader32)
          + nth->FileHeader.SizeOfOptionalHeader);
        size_t n = nth->FileHeader.NumberOfSections;
        while ( n )
          if ( !std::strncmp(name, &sh[--n].Name[0], sizeof(sh->Name)) )
            return &sh[n];
        return 0;
      }


      struct import_by_name
      {
        uint16_t  Hint;
        char      Name[1];
      };

      union thunk_data
      {
        uint32_t  ForwarderString;  // char*
        uint32_t  Function;         // uint32_t*
        uint32_t  Ordinal;
        uint32_t  AddressOfData;    // import_by_name*
      };

      ///\name  Export support

      struct export_directory 
      {
        uint32_t  Characteristics;
        uint32_t  TimeDateStamp;
        uint16_t  MajorVersion;
        uint16_t  MinorVersion;
        uint32_t  Name;
        uint32_t  Base;                   // Ordinal Base
        uint32_t  NumberOfFunctions;      // Address Table Entries
        uint32_t  NumberOfNames;          // Number of Name Pointers
        uint32_t  AddressOfFunctions;     // Export Address Table RVA
        uint32_t  AddressOfNames;         // Name Pointer RVA
        uint32_t  AddressOfNameOrdinals;  // Ordinal Table RVA

        ///\note  binary search is not for speed here -
        ///       sequential one could give different results on tricky PEs
        ///       and probably is exposed for some exploitation 
        __forceinline
          uint32_t ordinal(const image * pe, const char name[]) const
        {
          const uint32_t * const name_table = pe->va<uint32_t*>(AddressOfNames);
          uint32_t l = 0, h = NumberOfNames - 1;
          while ( h >= l )
          {
            const uint32_t m = (l + h) / 2;
            const int r = std::strcmp(pe->va<char*>(name_table[m]), name);
            if ( ! r ) return (pe->va<const uint16_t*>(AddressOfNameOrdinals))[m];
            else if ( r > 0) h = m - 1;
            else if ( r < 0) l = m + 1;
          }
          return 0xffffffff;
        }

        uint32_t ordinal(const image * /*pe*/, uint16_t ordinal) const
        {
          return ordinal - Base;
        }

        void * function(const image * pe, uint32_t ordinal) const 
        {
          return ordinal < NumberOfFunctions
            ? pe->va<void*>(pe->va<uint32_t*>(AddressOfFunctions)[ordinal]) : 0;
        }

      }; // struct export_directory

      typedef
        const image * find_dll_t(const char * dll_name);

      template<typename PtrType, typename ExportType>
      PtrType * find_export(ExportType exp) const
      {
        const data_directory * const export_table = 
          get_data_directory(data_directory::export_table);
        if ( ! export_table || ! export_table->VirtualAddress ) return 0;
        export_directory * exports = va<export_directory*>(export_table->VirtualAddress);
        void * const f = exports->function(this, exports->ordinal(this, exp));
        const uintptr_t ex = reinterpret_cast<uintptr_t>(exports);
        return in_range(ex, ex + export_table->Size, f) ? 0 : brute_cast<PtrType*>(f);
      }

      template<typename PtrType, typename DllFinder>
      PtrType * find_export(const char * exp, DllFinder find_dll) const
      {
        const data_directory * const export_table = 
          get_data_directory(data_directory::export_table);
        if ( ! export_table || ! export_table->VirtualAddress ) return 0;
        export_directory * exports = va<export_directory*>(export_table->VirtualAddress);
        void * const f = exports->function(this, exports->ordinal(this, exp));
        const uintptr_t ex = reinterpret_cast<uintptr_t>(exports);
        if ( !in_range(ex, ex + export_table->Size, f) )
          return f;

        // forward export
        static const size_t dll_name_max = 16;
        char dll_name[dll_name_max + sizeof("dll")];
        char * forward = reinterpret_cast<char*>(f);
        size_t i = 0;
        for ( ; ; )
        {
          if ( i == dll_name_max ) return 0;
          const char c = *forward++;
          dll_name[i++] = c;
          if ( c == '.' ) break;
        }
        dll_name[i++] = 'd';
        dll_name[i++] = 'l';
        dll_name[i++] = 'l';
        dll_name[i] = '\0';
        const image * forwarded_dll = find_dll(dll_name);
        return forwarded_dll ? forwarded_dll->find_export<PtrType>(forward, find_dll) : 0;
      }

      ///\name  Imports

      struct import_descriptor
      {
        union
        {
          uint32_t  Characteristics;
          uint32_t  OriginalFirstThunk;
        };
        uint32_t  TimeDateStamp;
        uint32_t  ForwarderChain;
        uint32_t  Name;
        uint32_t  FirstThunk; // RVA to IAT

        bool is_terminating() const { return Characteristics == 0; }
        bool bound() const { return TimeDateStamp != 0; }
        bool no_forwarders() const { return ForwarderChain == -1; }

        uintptr_t * find(const image * const img, const char * const import_name)
        {
          uintptr_t * iat = img->va<uintptr_t*>(FirstThunk);
          for ( uintptr_t * hint_name = img->va<uintptr_t*>(OriginalFirstThunk);
            *hint_name;
            ++hint_name, ++iat )
            if ( !std::strcmp(img->va<const char*>(*hint_name) + 2, // skip Hint
              import_name) )
              return iat;
          return 0;
        }

      };

      static inline uintptr_t & null_import()
      {
        static uintptr_t _0;
        return _0;
      }

      import_descriptor *
        get_first_import_entry() const
      {
        const data_directory * const import_table = 
          get_data_directory(data_directory::import_table);
        return import_table && import_table->VirtualAddress 
          ? va<import_descriptor*>(import_table->VirtualAddress)
          : 0;
      }

      import_descriptor *
        find_import_entry(const char * const module_name) const
      {
        if ( !module_name ) return 0;
        for ( import_descriptor * import_entry = get_first_import_entry();
          import_entry && !import_entry->is_terminating();
          ++import_entry )
        {
          if ( !import_entry->Name ) continue;
          // compare names case-insensitive (simpified)
          const char * const name = va<const char*>(import_entry->Name);
          for ( unsigned i = 0; module_name[i]; ++i )
            if ( (module_name[i] ^ name[i]) & 0x5F) continue;
          return import_entry;
        }
        return 0;
      }

      uintptr_t &
        find_bound_import(
        const char *  const import_name,
        const char *  const module = 0) const
      {
        import_descriptor * import_entry;
        uintptr_t * iat = 0;
        if ( module && (import_entry = find_import_entry(module)) )
          iat = import_entry->find(this, import_name);
        else
          for ( import_entry = get_first_import_entry();
            import_entry && !import_entry->is_terminating();
            ++import_entry )
            if ( (iat = import_entry->find(this, import_name)) )
              break;
        return iat ? *iat : null_import();
      }


      template<typename DllFinder>
      bool bind_import(const DllFinder & find_dll)
      {
        for ( import_descriptor * import_entry = get_first_import_entry();
          import_entry && !import_entry->is_terminating();
          ++import_entry )
        {
          if ( ! import_entry->Name ) return false;
          const image * const dll =
            find_dll(va<const char*>(import_entry->Name));
          if ( ! dll ) return false;
          void ** iat = va<void**>(import_entry->FirstThunk);
          for ( int32_t * hint_name = va<int32_t*>(import_entry->OriginalFirstThunk);
            *hint_name;
            ++hint_name, ++iat )
          {
            *iat = *hint_name < 0
              ? dll->find_export(static_cast<uint16_t>(*hint_name))
              : dll->find_export(va<const char*>(*hint_name) + 2, find_dll);
            if ( !*iat ) return false;
          }
        }
        return true;
      }


      ///\name  Relocations

      struct base_relocation 
      {
        uint32_t  VirtualAddress;
        uint32_t  SizeOfBlock;
        struct entry_t
        {
          uint16_t  Offset : 12;
          uint16_t  Type   : 4;
        } entry[1];

        enum type
        {
          absolute  = 0,
          high      = 1,
          low       = 2,
          highlow   = 3,
          dir64     = 10,
        };
      };

      bool relocate(ptrdiff_t delta)
      {
        const data_directory * const reloc_dir = 
          get_data_directory(data_directory::basereloc_table);
        if ( ! reloc_dir || ! reloc_dir->VirtualAddress ) return false;
        const base_relocation * fixups = va<base_relocation*>(reloc_dir->VirtualAddress);
        const uintptr_t end = va(reloc_dir->VirtualAddress + reloc_dir->Size);
        while ( reinterpret_cast<uintptr_t>(fixups) < end )
        {
          const uintptr_t addr = va(fixups->VirtualAddress);
          const uintptr_t end = fixups->SizeOfBlock
            + reinterpret_cast<uintptr_t>(fixups);
          const base_relocation::entry_t * entry = &fixups->entry[0];
          for ( ; reinterpret_cast<uintptr_t>(entry) < end; ++entry )
            switch ( entry->Type )
          {
            case base_relocation::highlow:
              *reinterpret_cast<uint32_t*>(addr + entry->Offset) += static_cast<uint32_t>(delta);
              break;
            default:
              break;
          }
          fixups = reinterpret_cast<const base_relocation*>(entry);
        }
        return true;
      }

      ///\name Resources

      struct resource_directory_entry 
      {
        union 
        {
          struct
          {
            uint32_t  Offset:31;
            uint32_t  IsString:1;
          } Name;
          uint16_t  Id;
        };
        uint32_t  OffsetToDirectory:31;
        uint32_t  DataIsDirectory:1;
      };

      struct resource_directory
      {
        uint32_t  Characteristics;
        uint32_t  TimeDateStamp;
        uint16_t  MajorVersion;
        uint16_t  MinorVersion;
        uint16_t  NumberOfNamedEntries;
        uint16_t  NumberOfIdEntries;
      };

      resource_directory_entry* get_res_named_entry(size_t n = 0)
      {
        const data_directory* const resd = get_data_directory(data_directory::resource_table);
        if(!resd || !resd->VirtualAddress)
          return 0;
        resource_directory* rsrc = va<resource_directory*>(resd->VirtualAddress);
        return n < rsrc->NumberOfNamedEntries
          ? &reinterpret_cast<resource_directory_entry*>(uintptr_t(rsrc) + sizeof(resource_directory)) [n]
          : 0;
      }

      resource_directory_entry* get_res_entry(size_t n = 0)
      {
        const data_directory* const resd = get_data_directory(data_directory::resource_table);
        if(!resd || !resd->VirtualAddress)
          return 0;
        resource_directory* rsrc = va<resource_directory*>(resd->VirtualAddress);
        if(!(n < rsrc->NumberOfIdEntries))
          return 0;
        resource_directory_entry* ride = reinterpret_cast<resource_directory_entry*>(
          uintptr_t(rsrc) + sizeof(resource_directory) + 
          sizeof(resource_directory_entry)*rsrc->NumberOfNamedEntries);
        return &ride[n];
        //return & reinterpret_cast<resource_directory_entry*>
        //  (uintptr_t(rsrc) + sizeof(resource_directory) + sizeof(resource_directory_entry)*rsrc->NumberOfNamedEntries)
        //  [n];
      }

      struct resource_dir_string
      {
        uint16_t  Length;
        wchar_t   NameString[1];
      };

      struct resource_data_entry 
      {
        uint32_t  OffsetToData;
        uint32_t  Size;
        uint32_t  CodePage;
        uint32_t  Reserved;
      };

      ///\name Security table
      struct security_table
      {
        uint32_t  OffsetToData;
        uint32_t  Size;
      };

      ///}

      ///////////////////////////////////////////////////////////////////////////
    private:

    };// class image

    extern "C" image  __ImageBase;

    __forceinline image * image::this_module()
    {
      return &__ImageBase;
    }


    /**@} pe_images_support */


  }//namespace pe
}//namespace ntl

#endif//#ifndef NTL__PE_IMAGE
