#include "../stlx/iostream.hxx"
#include "../win/console.hxx"
#include "../atomic.hxx"

#ifdef NTL__SUBSYSTEM_KM
# include "../km/new.hxx"
#else
# include "../nt/new.hxx"
#endif

static void _cdecl __init_iostream_objects(bool init);

namespace std
{
  ostream  cout(nullptr);
  ostream  cerr(nullptr);
  ostream  clog(nullptr);
  wostream wcout(nullptr);
  wostream wcerr(nullptr);
  wostream wclog(nullptr);

  ios_base::Init::Init()
  {
    __init_iostream_objects(true);
  }
  ios_base::Init::~Init()
  {
    __init_iostream_objects(false);
  }
} // std

#ifndef NTL__SUBSYSTEM_KM

namespace
{
  typedef ntl::win::console_buffer<char>    buffer_n;
  typedef ntl::win::console_buffer<wchar_t> buffer_w;

  buffer_n coutb(0), cerrb(0), clogb(0);
  buffer_w woutb(0), werrb(0), wlogb(0);

  void init_iostream_objects()
  {
    using namespace std;
    using namespace ntl::nt;
    using ntl::win::console;

    buffer_n* bn[3] = {&coutb,&cerrb,&clogb};
    buffer_w* bw[3] = {&woutb,&werrb,&wlogb};
    ostream*  noss[3] = {&cout,&clog,&cerr};
    wostream* woss[3] = {&wcout,&wclog,&wcerr};

    const console::type types[3] = {console::stdout, console::stderr, console::stderr};

    // recreate the objects
    for(unsigned i = 0; i < _countof(noss); i++){
      legacy_handle h = console::handle(types[i]);
      if(h){
        new(bn[i]) buffer_n(h);
        new(bw[i]) buffer_w(h);
        new(noss[i]) ostream(bn[i]);
        new(woss[i]) wostream(bw[i]);
      }
    }
    if(cerr.rdbuf()){
      cerr.setf(ostream::unitbuf);
      if(cout.rdbuf())
        cerr.tie(&cout);
    }
    if(wcerr.rdbuf()){
      wcerr.setf(wostream::unitbuf);
      if(wcout.rdbuf())
        wcerr.tie(&wcout);
    }
  }

}

static void destroy_iostream_objects()
{
  using namespace std;
  ostream*  noss[3] = {&cout,&clog,&cerr};
  wostream* woss[3] = {&wcout,&wclog,&wcerr};

  std::allocator<buffer_n> na;
  std::allocator<buffer_w> wa;
  for(unsigned i = 0; i < _countof(noss); i++){
    __ntl_try{
      noss[i]->flush();
    }
    __ntl_catch(...){}
    __ntl_try{
      woss[i]->flush();
    }
    __ntl_catch(...){}
    na.destroy( static_cast<buffer_n*>( noss[i]->rdbuf(nullptr) ) );
    wa.destroy( static_cast<buffer_w*>( woss[i]->rdbuf(nullptr) ) );
  }
}

static int32_t _iostreams_init_count = 0;

extern "C" void _cdecl __check_iostream_objects()
{
  if(_iostreams_init_count) // if iostreams were initialized early
    init_iostream_objects();
}

void _cdecl __init_iostream_objects(bool init)
{
  if(init){
    if(ntl::atomic::exchange_add(_iostreams_init_count, 1) == 0)
      init_iostream_objects();
  }else{
    if(ntl::atomic::exchange_add(_iostreams_init_count, -1) == 1)
      destroy_iostream_objects();
  }
}
#else // km

extern "C" void _cdecl __check_iostream_objects()
{
}

void _cdecl __init_iostream_objects(bool)
{
}

#endif
