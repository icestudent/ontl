#ifdef NTL_SUBSYSTEM_KM
# include "../km/basedef.hxx"
# include "../km/new.hxx"
#else
# include "../nt/basedef.hxx"
# include "../nt/new.hxx"
#endif
//#include "../stlx/new.hxx" // for buffer allocation; otherwise it produces Compiler Error C2748 at console_buffer::init() o_0
#include "../stlx/iostream.hxx"
#include "../win/console.hxx"
#include "../atomic.hxx"

#include "wchar_mask_data.cpp"

extern "C" void __cdecl __init_iostream_objects(bool init);

#ifndef NTL_SUBSYSTEM_KM

namespace
{
  typedef ntl::win::console_buffer<char>    buffer_n;
  typedef ntl::win::console_buffer<wchar_t> buffer_w;

  buffer_n *coutb(0), *cerrb(0), *clogb(0), *cinb(0);
  buffer_w *woutb(0), *werrb(0), *wlogb(0), *winb(0);

  void init_iostream_objects()
  {
    using namespace std;
    using namespace ntl::nt;
    using ntl::win::console;

    buffer_n** bn[3] = {&coutb,&cerrb,&clogb};
    buffer_w** bw[3] = {&woutb,&werrb,&wlogb};
    ostream*  noss[3] = {&cout,&clog,&cerr};
    wostream* woss[3] = {&wcout,&wclog,&wcerr};

    const console::type types[3] = {console::stdout, console::stderr, console::stderr};

    // recreate the objects
    for(unsigned i = 0; i < _countof(noss); i++){
      legacy_handle h = console::handle(types[i]);
      //if(h){
        *bn[i] = new buffer_n(h);
        *bw[i] = new buffer_w(h);
        new(noss[i]) ostream(*bn[i]);
        new(woss[i]) wostream(*bw[i]);
      //}
    }

    cinb = new buffer_n(console::stdin, false);
    winb = new buffer_w(console::stdin, false);
    new(&cin) istream(cinb);
    new(&wcin) wistream(winb);
    cin.tie(&cout);
    wcin.tie(&wcout);

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

  std::allocator<ostream> na;
  std::allocator<wostream> wa;

  // just flush (with the tied streams)
  for(unsigned i = 0; i < _countof(noss); i++){
    __ntl_try{
      noss[i]->flush();
    }
    __ntl_catch(...){}
    __ntl_try{
      woss[i]->flush();
    }
    __ntl_catch(...){}
  }

  // destroy buffers & streams
  for(unsigned i = 0; i < _countof(noss); i++){
    // destroy buffers
    delete static_cast<buffer_n*>( noss[i]->rdbuf(nullptr) );
    delete static_cast<buffer_w*>( woss[i]->rdbuf(nullptr) );
    
    // actually does nothing
    na.destroy(noss[i]);
    wa.destroy(woss[i]);
  }
}

static volatile int32_t _iostreams_init_count = 0;

void __cdecl __init_iostream_objects(bool init)
{
  if(init){
    if(ntl::atomic::exchange_add(_iostreams_init_count, 1) == 0)
      init_iostream_objects();
  }else{
    if(ntl::atomic::exchange_add(_iostreams_init_count, -1) == 0)
      destroy_iostream_objects();
  }
}
#else // km

void __cdecl __init_iostream_objects(bool)
{
}

#endif//km

namespace std
{
  istream  cin ((ios_base::__noinittag()));
  ostream  cout((ios_base::__noinittag()));
  ostream  cerr((ios_base::__noinittag()));
  ostream  clog((ios_base::__noinittag()));

  wistream wcin ((ios_base::__noinittag()));
  wostream wcout((ios_base::__noinittag()));
  wostream wcerr((ios_base::__noinittag()));
  wostream wclog((ios_base::__noinittag()));
} // std
