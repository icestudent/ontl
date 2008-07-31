/**\file*********************************************************************
 *                                                                     \brief
 *
 *
 ****************************************************************************
 */

#ifndef NTL__NT_SHARED_DATA
#define NTL__NT_SHARED_DATA

#include "../cpu.hxx"
#include "../stdint.h"

namespace ntl {
namespace nt {

struct system_time
{
    typedef int64_t type;
    static const type resolution = (1000 * 1000 * 1000) / 100; // 100ns

    __forceinline type get() volatile const
    {
#ifdef _M_X64
      return *reinterpret_cast<int64_t volatile const*>(this);
#else
      uint32_t l; int32_t h;
      do { cpu::pause(); l = low; h = high; } while ( h != high2 );
      return (type)h << 32 | l;
#endif
    }

    __forceinline operator type() const
    {
      return *reinterpret_cast<type const*>(this);
    }

    uint32_t  low;
    int32_t   high, high2;
};

enum product_type 
{
  NtProductWinNt = 1,
  NtProductLanManNt,
  NtProductServer
};

enum alternative_architecture_type 
{
  StandardDesign,
  NEC98x86,
  EndAlternatives
};

#pragma pack(push)
#pragma pack(8)

/// The data shared between kernel and user mode.
///\note read-only for UM.
struct shared_data_32
{
  static const size_t processor_features_max    = 64;
  static const size_t max_wow64_shared_entries  = 16;

  static const uintptr_t km_base = sizeof(uintptr_t) > sizeof(int)
                                   ? 0xFFFFF78000000000UI64 : 0xffdf0000;

  static const uintptr_t um_base = 0x7ffe0000;

protected:
  static const shared_data_32& um_instance()
  {
    return *reinterpret_cast<const shared_data_32*>(um_base);
  }
  static shared_data_32& km_instance()
  {
    return *reinterpret_cast<shared_data_32*>(km_base);
  }
public:
#ifdef NTL_KM
  static shared_data_32& instance()
  {
    return km_instance();
  }
#else //#elif defined NTL_NT || defined NTL_WIN
  static const shared_data_32& instance()
  {
    return um_instance();
  }
#endif

            uint32_t      TickCountLowDeprecated;
            uint32_t      TickCountMultiplier;
  volatile  system_time   InterruptTime;
  volatile  system_time   SystemTime;
  volatile  system_time   TimeZoneBias;
            uint16_t      ImageNumberLow;
            uint16_t      ImageNumberHigh;
            wchar_t       NtSystemRoot[260];
            uint32_t      MaxStackTraceDepth;
            uint32_t      CryptoExponent;
            uint32_t      TimeZoneId;
            uint32_t      LargePageMinimum;
            uint32_t      Reserved2[7];
            product_type  NtProductType;
            bool          ProductTypeIsValid;
            uint32_t      NtMajorVersion;
            uint32_t      NtMinorVersion;
            bool          ProcessorFeatures[processor_features_max];
            uint32_t      Reserved1;
            uint32_t      Reserved3;
  volatile  uint32_t      TimeSlip;
            alternative_architecture_type AlternativeArchitecture;
            int64_t       SystemExpirationDate;
            uint32_t      SuiteMask;
            bool          KdDebuggerEnabled;
            uint8_t       NXSupportPolicy;
  volatile  uint32_t      ActiveConsoleId;
  volatile  uint32_t      DismountCount;
            uint32_t      ComPlusPackage;
            uint32_t      LastSystemRITEventTickCount;
            uint32_t      NumberOfPhysicalPages;
            bool          SafeBootMode;
            uint32_t      TraceLogging;
            uint64_t      TestRetInstruction;
            uint32_t      SystemCall;
            uint32_t      SystemCallReturn;
            uint64_t      SystemCallPad[3];
  union {
    volatile system_time  TickCount;
    volatile uint64_t     TickCountQuad;
  };
            uint32_t      Cookie;
};
STATIC_ASSERT(sizeof(shared_data_32) == 0x338 );

#ifdef _M_X64
struct shared_data_64 : public shared_data_32
{
#ifdef NTL_KM
  static shared_data_64& instance()
  {
    return static_cast<shared_data_64&>(km_instance());
  }
#else
  static const shared_data_64& instance()
  {
    return static_cast<const shared_data_64&>(um_instance());
  }
#endif

  enum wow64_shared_information 
  {
    SharedNtdll32LdrInitializeThunk,
    SharedNtdll32KiUserExceptionDispatcher,
    SharedNtdll32KiUserApcDispatcher,
    SharedNtdll32KiUserCallbackDispatcher,
    SharedNtdll32LdrHotPatchRoutine,
    SharedNtdll32ExpInterlockedPopEntrySListFault,
    SharedNtdll32ExpInterlockedPopEntrySListResume,
    SharedNtdll32ExpInterlockedPopEntrySListEnd,
    SharedNtdll32Reserved2,
    Wow64SharedPageEntriesCount
  };
            uint32_t      Wow64SharedInformation[max_wow64_shared_entries];
            // NT6.0+
            uint16_t      UserModeGlobalLogger[8];
            uint32_t      HeapTracingPid[2];
            uint32_t      CritSecTracingPid[2];
            uint32_t      ImageFileExecutionOptions;
  union {
    uint64_t              AffinityPad;
    km::kaffinity_t       ActiveProcessorAffinity;
  };
  volatile  uint64_t      InterruptTimeBias;
};
#endif // _M_X64

#pragma pack(pop)

}//namespace nt

#if defined(_M_IX86)
  typedef nt::shared_data_32  user_shared_data;
#elif defined(_M_X64)
  typedef nt::shared_data_64  user_shared_data;
#else
  #error define the shared_data address
#endif

typedef nt::product_type      nt_product_type;

}//namespace ntl

#endif//#ifndef NTL__KM_SHARED_DATA
