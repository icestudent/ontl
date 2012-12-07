/**
 *	@file psinfo.cpp
 *	@brief psinfo - unix's time(1) clone for windows os
 *
 *  History:
 *  - v0.1  - initial release
 *  - v0.1a - small x86 fix
 *  - v0.2  - changed output handle to stderr
 *  - v0.3  - output changed to iostream
 *
 *  @version 0.3
 *	@author icestudent
 *	@date 2009/02/11
 **/
#include <consoleapp.hxx>
#include <nt/process_information.hxx>
#include <nt/resource.hxx>
#include <nt/thread.hxx>
#include <pe/image.hxx>

#include <cstdio>
#include <chrono>

#include <iostream>
#include <sstream>
#include <iomanip>

using namespace ntl;
using namespace ntl::nt;

using std::setw;
using std::endl;
using std::flush;

std::ostream& operator<<(std::ostream& os, const wchar_t* ws)
{
  using namespace std;
  wstring_convert<codecvt<wchar_t, char, mbstate_t> > wsc;
  return os << wsc.to_bytes(ws);
}

namespace fmt
{
  template<typename T>
  struct format_bytes
  {
    format_bytes(T value, int prec = -1)
      :value(value), prec(prec)
    {}
    operator T() const { return value; }
    int precision() const { return prec; }
  private:
    const T value;
    const int prec;

    format_bytes& operator=(const format_bytes&) __deleted;
  };
}

template<typename T>
inline fmt::format_bytes<T> prefix(T value)
{
  return fmt::format_bytes<T>(value);
}
template<typename T>
inline fmt::format_bytes<T> prefix(T value, int precision)
{
  return fmt::format_bytes<T>(value, precision);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const fmt::format_bytes<T>& fmt)
{
  T value = fmt;
  unsigned rem = 0, prefix = 0;
  while(value / 1024){
    rem = static_cast<unsigned>(value % 1024);
    value /= 1024;
    prefix++;
  }

  static const char* prefixes[] = {"B", "KiB", "MiB", "GiB", "TiB"};

  os << value;
  if(fmt.precision() > 0 || os.precision() > 0){
    using std::ios_base;
    const int precision = fmt.precision() > 0 ? fmt.precision() : os.precision();

    static const unsigned pow_table[6] = {100, 1000, 10000, 100000, 1000000, 10000000};
    const unsigned mod = pow_table[ std::min<std::ssize_t>(_countof(pow_table), precision) - 1 ];
    rem = (rem * mod) / 1024;
    rem = (rem+5) / 10; // round(rem)
    if(rem || os.flags() & ios_base::showpoint){
      ios_base::fmtflags old = os.setf(ios_base::internal, ios_base::adjustfield);
      os << '.' << std::setw(static_cast<int>(precision)) << rem;
      os.setf(old, ios_base::adjustfield);
    }
  }
  os << ' ' << prefixes[prefix];
  return os;
}

class app: ntl::consoleapp
{
  const char* header() { return "psinfo v0.3\n"; }
  void usage()
  {
    log << 
      "usage: psinfo [options] <command>\n"
      "options:\n"
      "-a\t"  "--all\t\t"   "print all information, in the following order:\n"
      "-t\t"  "--times\t\t" "print cpu times\n"
      "\t"    "--pool\t\t"  "print the pool memory usage\n"
      "\t"    "--vm\t\t"    "print the virtual memory usage\n"
      "\t"    "--io\t\t"    "print the io counters\n"
      "-h\t"  "--handles\t" "print the open handles count\n"
      "-v\t"  "--verbose" "explain what is being done\n"
      << flush;
  }

protected:
  std::ostream& log;
  std::ostringstream error_info;
  std::wstring command;
  legacy_handle process;

  enum options {
    opt_none,
    opt_verbose,
    opt_handles   = 0x02,
    opt_io        = 0x04,
    opt_pool      = 0x08,
    opt_times     = 0x10,
    opt_vm        = 0x20,
  } m_options;

  static const options opt_default = opt_times, opt_all = static_cast<options>(opt_vm|opt_times|opt_pool|opt_io|opt_handles);

  __ntl_bitmask_type(options, friend);

  struct cmd_t
  {
    command_line::const_iterator i;

    explicit cmd_t(command_line::const_iterator c)
      :i(c)
    {}
    cmd_t(){}

    inline bool operator==(const wchar_t* cmd) const { return std::wcscmp(*i, cmd) == 0; }
    inline bool operator!=(const wchar_t* cmd) const { return std::wcscmp(*i, cmd) != 0; }
    inline bool operator==(const char* cmd) const
    {
      const wchar_t* s = *i;
      while(*s == *cmd && *s)
        ++s, ++cmd;
      return !(*s || *cmd);
    }
    inline bool operator!=(const char* cmd) const
    {
      return !(*this == cmd);
    }
  };

  bool parse_args()
  {
    m_options = opt_none;

    std::wstring raw(ntl::win::GetCommandLineW());
    command_line cmdl;
    if(cmdl.size() < 2)
      return false;

    for(command_line::const_iterator cmdi = cmdl.cbegin()+1; cmdi != cmdl.cend(); ++cmdi){
      cmd_t cmd(cmdi);
      if(cmd == L"-?" || cmd == L"--help"){
        return false;
     }else if(cmd == L"-a" || cmd == L"--all")
        m_options = opt_all;
      else if(cmd == L"-h" || cmd == L"--handles")
        m_options |= opt_handles;
      else if(cmd == L"-t" || cmd == L"--times")
        m_options |= opt_times;
      else if(cmd == L"--io")
        m_options |= opt_io;
      else if(cmd == L"--pool")
        m_options |= opt_pool;
      else if(cmd == L"--vm")
        m_options |= opt_vm;
      else if(cmd == L"-v" || cmd == L"--verbose")
        m_options |= opt_verbose;
      else{
        if(**cmdi == L'-'){
          // invalid option
          error_info << "error: invalid option '" << *cmdi << '\'';
          return false;
        }
        size_t c = raw.find(*cmdi);
        if(c != std::wstring::npos && raw[c-1] != L'"')
          command = &raw[c];
        else
          command = *cmdi;
        command.c_str();
        if(m_options == opt_none)
          m_options = opt_default;
        return true;
      }
    }
    return false;
  }

protected:
  static const unsigned max_buf_count = 10;

  static const char* format_time(int64_t time, int buf_no, bool short_time = false)
  {
    static char buf[16*max_buf_count];
    time_fields tm;
    RtlTimeToTimeFields(&time, &tm);

    char* re = buf+16*buf_no, *p = re;
    size_t l;
    if(tm.Hour || !short_time){
      l = _snprintf(p, _countof(buf), "%02d:", tm.Hour); p += l;
    }
    if(tm.Minute || !short_time){
      l = _snprintf(p, _countof(buf), "%02d:", tm.Minute); p += l;
    }
    _snprintf(p, _countof(buf), short_time ? "%02d.%d ms" : "%02d.%03d", tm.Second, tm.Milliseconds);
    return re;
  }
public:
  app(std::ostream& output): 
      log(output), process()
  {}

  bool print_handles()
  {
    uint32_t count;
    ntstatus st = NtQueryInformationProcess(process, ProcessHandleCount, &count, sizeof(uint32_t), nullptr);
    if(!success(st))
      return false;
    log << "opened handles remained: " << count << '\n';
    return true;
  }

  bool print_io()
  {
    process_information<process_io_counters> info(process);
    if(!info)
      return false;
    log << "io counters:\n"
      << "  read:\t\t" << setw(3) << info->ReadOperationCount  << '\n'
      << "  write:\t"  << setw(3) << info->WriteOperationCount << '\n'
      << "  other:\t"  << setw(3) << info->OtherOperationCount << '\n'
      << "transfer:\n" << std::setprecision(2) << std::showpoint
      << "  read:\t\t" << setw(3) << prefix(info->ReadTransferCount) << '\n'
      << "  write:\t"  << setw(3) << prefix(info->WriteTransferCount)<< '\n'
      << "  other:\t"  << setw(3) << prefix(info->OtherTransferCount)<< "\n\n";
    return true;
  }

  bool print_times()
  {
    process_information<kernel_user_times> info(process);
    if(!info)
      return false;
    if(!info->ExitTime)
      info->ExitTime = query_system_time();

    log << "cpu time usage:\n"
          << "  user:\t\t" << format_time(info->UserTime, 0) << '\n'
          << "  kernel:\t" << format_time(info->KernelTime, 1) <<'\n'
          << "  total:\t"  << format_time(info->ExitTime-info->CreateTime, 2) << "\n\n";
    return true;
  }

  bool print_time()
  {
    process_information<kernel_user_times> info(process);
    if(!info)
      return false;
    if(!info->ExitTime)
      info->ExitTime = query_system_time();

    log << "cpu time usage:\n"
          << "  user:\t\t" << format_time(info->UserTime, 0, true) << '\n'
          << "  kernel:\t" << format_time(info->KernelTime, 1, true) <<'\n'
          << "  total:\t"  << format_time(info->ExitTime-info->CreateTime, 2, true) << "\n";
    return true;
  }

  bool print_pool()
  {
    process_information<pooled_usage_and_limits> info(process);
    if(!info)
      return false;
    log << "pooled memory usage:\n"
          << std::setprecision(0)
          << "  paged:\t"     << setw(3) << prefix(info->PagedPoolUsage) << '\n'
          << "  nonpaged:\t"  << setw(3) << prefix(info->NonPagedPoolUsage) << '\n'
          << "peak:\n"
          << "  paged:\t"     << setw(3) << prefix(info->PeakPagedPoolUsage) << '\n'
          << "  nonpaged:\t"  << setw(3) << prefix(info->PeakNonPagedPoolUsage) << "\n\n";
    return true;
  }

  bool print_vm()
  {
    process_information<process_vm_counters_ex> info(process);
    if(!info)
      return false;
    log << "virtual memory usage:\n"
        << std::setprecision(0)
          << "  virtual:\t" << setw(3) << prefix(info->VirtualSize) << '\n'
          << "  ws size:\t" << setw(3) << prefix(info->WorkingSetSize) << '\n'
          << "  pagefile:\t"<< setw(3) << prefix(info->PagefileUsage) << '\n'
          << "  private:\t" << setw(3) << prefix(info->PrivateUsage) << '\n'
          << "peak:\n"
          << "  virtual:\t" << setw(3) << prefix(info->PeakVirtualSize) << '\n'
          << "  ws size:\t" << setw(3) << prefix(info->PeakWorkingSetSize) << '\n'
          << "  pagefile:\t"<< setw(3) << prefix(info->PeakPagefileUsage) << "\n\n";
    return true;
  }

  legacy_handle exec_command()
  {
    typedef long _stdcall PCreateProcess(const wchar_t*, wchar_t*, const void*, const void*, long, uint32_t, void*, const wchar_t*, const void*, void*);

    struct process_information
    {
      legacy_handle hProcess;
      legacy_handle hThread;
      uint32_t      dwProcessId;
      uint32_t      dwThreadId;
    };

    peb::find_dll fm;
    const pe::image* kernel = fm("kernel32.dll");
    PCreateProcess* exec = kernel->find_export<PCreateProcess*>("CreateProcessW");
    application<wchar_t>::startupinfo si = {sizeof(si)};
    process_information pi;
    long ok = exec(nullptr, (command.c_str(), command.begin()), nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi);
    if(!ok){
      error_info << "error: failed to execute command with " << std::showbase << std::hex << teb::instance().LastErrorValue << ":\n\t'" << command.c_str() << '\'';
      return nullptr;
    }
    NtClose(pi.hThread);
    return pi.hProcess;
  }

  int main()
  {
    if(!parse_args()){
      log << header();
      if(!error_info.str().empty())
        log << error_info.str() << endl;
      usage();
      return 1;
    }

    process = exec_command();
    if(!process){
      log << header() << error_info.str() << endl;
      return 3;
    }
    handle h_process(process); // autoclose
    NtWaitForSingleObject(process, false, infinite_timeout());

    // collect information
    log << '\n' << header();
    process_information<process_basic_information> basicInfo(process);
    if(!basicInfo){
      log << "error: failed to get the basic info" << endl;
      return 2;
    }

    log << "process exit code: " << basicInfo->ExitStatus << '\n';

    // if only the time specified, print it
    if((m_options & ~opt_verbose) == opt_times)
      print_time();
    else
      print_times();
    if(m_options & opt_pool)
      print_pool();
    if(m_options & opt_vm)
      print_vm();
    if(m_options & opt_io)
      print_io();
    if(m_options & opt_handles)
      print_handles();

    log << flush;
    return 0;
  }
};

int consoleapp::main()
{
  app x(std::cout);
  return x.main();
}
