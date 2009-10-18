/**\file*********************************************************************
 *                                                                     \brief
 *  27.9 File streams [file.streams] (N2960)
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_FSTREAM
#define NTL__STLX_FSTREAM

#include "streambuf.hxx"
#include "istream.hxx"

#ifdef NTL__SUBSYSTEM_KM
# include "../km/file.hxx"
#else
# include "../nt/file.hxx"
#endif

#include "ext/tr2/filesystem.hxx"

#ifndef STLX__CONFORMING_FSTREAM
// turn off UCS-MBCS character conversion in file streams
#define STLX__CONFORMING_FSTREAM 0
#endif

namespace std {

  /**\addtogroup  lib_input_output ******* 27 Input/output library [input.output]
   *@{*/
  /**\addtogroup  lib_file_streams ******* 27.9 File-based streams [file.streams]
   *@{*/

/**
 *	@brief 27.9.1.1 Class template basic_filebuf [filebuf]
 *  @details The class basic_filebuf<charT,traits> associates both the input sequence and the output sequence with a file.
 **/
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_filebuf:
  public basic_streambuf<charT, traits>
{
  /** 16Kb default buffer size */
  static const streamsize default_file_buffer_size = 1024 * 16;

  public:
    typedef charT                     char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits                    traits_type;

    ///\name 27.9.1.2 basic_filebuf constructors [filebuf.cons]

    basic_filebuf()
      :mode(), our_buffer(true)
    {}

  #ifdef NTL__CXX_RV
    basic_filebuf(basic_filebuf&& rhs);
  #endif

    virtual ~basic_filebuf()
    {
      __ntl_try { 
        close();
      }
      __ntl_catch(...){}
      if(buf.second && our_buffer) // prevent to call the child virtual function
        reallocate_buffer(0);
    }

    ///\name 27.9.1.3 Assign and swap [filebuf.assign]
  #ifdef NTL__CXX_RV
    basic_filebuf& operator=(basic_filebuf&& rhs)
    {
      swap(rhs); return *this;
    }
  #endif
    
    void swap(basic_filebuf& rhs)
    {
      basic_streambuf::swap(rhs);
    }

    ///\name 27.9.1.4 Member functions [filebuf.members]
    bool is_open() const { return !!f; }

    basic_filebuf<charT,traits>* open(const string& s, ios_base::openmode mode)
    {
      return open(tr2::sys::filesystem::path(s), mode) ? this : nullptr;
    }
    basic_filebuf<charT,traits>* open(const char* s, ios_base::openmode mode)
    {
      return open(tr2::sys::filesystem::path(s), mode) ? this : nullptr;
    }
    // NTL extension
    basic_filebuf<charT,traits>* open(const wchar_t* s, ios_base::openmode mode)
    {
      return open(tr2::sys::filesystem::wpath(s), mode) ? this : nullptr;
    }


    basic_filebuf<charT,traits>* close()
    {
      if(!f)
        return nullptr;

      // flush
      bool ok = true;
      if(pbase()){
        const int_type eof = traits_type::eof();
        ok = !traits_type::eq_int_type(overflow(eof), eof);
      }

      /* ???
      If the last virtual member function called on *this (between underflow, overflow,
      seekoff, and seekpos) was overflow then calls a_codecvt.unshift (possibly several times) to de-
      termine a termination sequence, inserts those characters and calls overflow(traits::eof()) again.
      */

      // at last, close file
      f.close();
      return ok ? this : nullptr;
    }

  ///////////////////////////////////////////////////////////////////////////
  protected:

    ///\name 27.9.1.5 Overridden virtual functions [filebuf.virtuals]
    //virtual streamsize showmanyc(); // default
    virtual int_type underflow()
    {
      assert(false && "not implemented yet");
      return traits_type::eof();
    }

    //virtual int_type uflow(); // default

    virtual int_type pbackfail(int_type c = traits::eof())
    {
      const int_type eof = traits_type::eof();
      if(!f)
        return eof;
      assert(false && "not implemented yet");
      return c;
    }

    virtual int_type overflow (int_type c = traits::eof())
    {
      const int_type eof = traits_type::eof();
      const bool eofc = traits_type::eq_int_type(c, eof);
      if(!f || (!pbase() && eofc))
        return eof;

      bool ok = true;
      if(const streamsize pending = pptr()-pbase()) {
        // write buffer
        if(ok = write(pbase(), pending), ok)
          reset();
      }

      if(ok && !eofc){
        // write c
        const char_type cc = traits_type::to_char_type(c);
        ok = write(&cc, 1);
      }
      ok &= flush();
      return eofc ? traits_type::not_eof(c) : c;
    }

    virtual basic_streambuf<charT,traits>* setbuf(char_type* s, streamsize n)
    {
      if(buf.second) reallocate_buffer(0);
      buf.first = s,
        buf.second = n;
      our_buffer = false;
      reset();
      return this;
    }

    virtual pos_type seekoff(off_type off, ios_base::seekdir way, ios_base::openmode = ios_base::in | ios_base::out)
    {
      pos_type re = pos_type(off_type(-1));
      if(!f)
        return re;
      
      const int width = 
#if !STLX__CONFORMING_FSTREAM
        1;
#else
        use_facet<codecvt<charT,char,typename traits::state_type> >(getloc()).encoding();
#endif
      if(off != 0 && width <= 0)
        return re;

      if(way != ios_base::cur || off != 0 && pptr()-pbase()){
        if(sync() == -1)
          return re;
      }

      static const native_file::Origin origins[] = {native_file::file_begin, native_file::file_end, native_file::file_current};
      if(NTL__SUBSYSTEM_NS::success(f.seek(off*width, origins[way])))
        re = pos_type(off);
      return re;
    }

    virtual pos_type seekpos(pos_type sp, ios_base::openmode = ios_base::in | ios_base::out)
    {
      pos_type re = pos_type(off_type(-1));
      if(!f)
        return re;
      assert(false && "not implemented yet");
      (void)sp;
      return re;
    }

    virtual int sync()
    {
      if(!f)
        return -1;

      const streamsize pending = pptr()-pbase();
      if(pending){
        if(!write(pbase(), pending) || !flush())
          return -1;
        reset();
      }
      return static_cast<int>(pending);
    }

    //virtual void imbue(const locale& loc); // default
    ///\}

  ///////////////////////////////////////////////////////////////////////////
protected:
  // manage buffer memory
  virtual bool reallocate_buffer(streamsize n)
  {
    if(!our_buffer) // dont manage foreign memory
      return true;

    if(n == 0) {
      // free buffer
      buf.second = n;
      delete[] buf.first;
      return true;

    } else if(n > buf.second) {

      // [re]allocate buffer
      if(buf.second)
        delete[] buf.first;

      buf.first = new char_type[n];
      buf.second = buf.first ? n : 0;
    }
    return buf.second != 0;
  }

private:
  template<class Path>
  bool open(const Path& name, ios_base::openmode mode)
  {
    if(f)
      return false;

    // open file
    native_file::creation_disposition cd;
    native_file::access_mask am;

    if(mode & ios_base::out){
      // write
      cd = mode & ios_base::trunc ? native_file::overwrite_if : native_file::create_if;
      am = native_file::generic_write;
    }else{
      // read
      cd =  native_file::open_existing;
      am = native_file::generic_read;
    }
    using namespace NTL__SUBSYSTEM_NS;
    bool ok = success(f.create(name.external_file_string(), cd, am));
    if(!ok)
      return false;

    if(mode & ios_base::ate){
      ok = success(f.seek(0, native_file::file_end));
      if(!ok){
        f.close();
        return false;
      }
    }

    // setup buffer
    if(!buf.second){
      reallocate_buffer(default_file_buffer_size);
      reset();
    }

    this->mode = mode;
    return ok;
  }

  bool write(const char_type* from, const char_type* to)
  {
    return write(from, to-from);
  }

  bool write(const char_type* s, streamsize n)
  {
    if(!n)
      return true;

    const char_type *inpos = s, *frompos = s, *endpos = s + n;
    const void* p;

#if STLX__CONFORMING_FSTREAM
    codecvt_base::result re = codecvt_base::noconv;
    typedef typename traits::state_type state_type;
    typedef codecvt<charT,char,typename traits::state_type> codecvt;
    const codecvt& a_codecvt = use_facet<codecvt>(getloc());
    char buf[1024], *outpos = buf
    const size_t width = (mode & ios_base::binary) ? sizeof(char_type) : sizeof(char);
#else
    static const size_t width = sizeof(char_type);
    if(!(mode & ios_base::binary) && f.tell() == 0){
      // we are in the unicode text mode, write BOM
      //(UTF-16 LE)
      static const uint32_t bom_le = 0xFEFF;
      uint32_t bom_size = 0;
      if(is_same<char_type, wchar_t>::value || is_same<char_type,char16_t>::value)
        bom_size = 2;
      else if(is_same<char_type, char32_t>::value)
        bom_size = 4;
      if(bom_size)
        f.write(&bom_le, bom_size);
    }
#endif

    do {
      streamsize chunk_size;
#if STLX__CONFORMING_FSTREAM
      state_type state;
      if(!(mode & ios_base::binary))
        re = a_codecvt.out(state, frompos, endpos, inpos, buf, buf+_countof(buf), outpos);
      if(re == codecvt_base::error)
        break;
      else if(re == codecvt_base::noconv)
        p = frompos, chunk_size = endpos-frompos, inpos += chunk_size;
      else // ok || partial
        p = buf, chunk_size = outpos-buf;
#else
      p = frompos;
      chunk_size = endpos-frompos;
      inpos += chunk_size;
#endif
      const uint32_t write_size = chunk_size*width;

      static_assert(numeric_limits<uint32_t>::__max >= numeric_limits<streamsize>::__max, "possible incompatible types");
      if(!NTL__SUBSYSTEM_NS::success(f.write(p,  write_size)))
        break;
      if(static_cast<streamsize>(f.get_io_status_block().Information) != write_size)
        break;

      n -= inpos-frompos;
      frompos = inpos;
    } while(n > 0);

    return n == 0;
  }

  bool flush()
  {
    return NTL__SUBSYSTEM_NS::success(f.flush());
  }

  // reset buffer positions
  void reset()
  {
    setp(buf.first, buf.first+buf.second);
  }

  private:
    //typedef ntl::nt::file_handler native_file;
    typedef NTL__SUBSYSTEM_NS::file_handler native_file;

    native_file f;
    pair<char_type*, streamsize> buf;
    ios_base::openmode mode;
    bool our_buffer;
};


/**
 *	@brief 27.9.1.6 Class template basic_ifstream [ifstream]
 *  @details The class basic_ifstream<charT, traits> supports reading from named files.
 *  It uses a basic_filebuf<charT, traits> object to control the associated sequence.
 **/
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_ifstream:
  public basic_istream<charT, traits>
{
  public:
    typedef charT                     char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits                    traits_type;

    ///\name 27.9.1.7 basic_ifstream constructors [ifstream.cons]

    basic_ifstream()
      :basic_istream(&sb)
    {}

    explicit basic_ifstream(const char* s, ios_base::openmode mode = ios_base::in)
      :basic_istream(&sb)
    {
      open(s, mode);
    }
    explicit basic_ifstream(const string& s, ios_base::openmode mode = ios_base::in)
      :basic_istream(&sb)
    {
      open(s, mode);
    }
#ifdef NTL__CXX_RV
    basic_ifstream(basic_ifstream&& rhs);
#endif

    // NTL extension
    explicit basic_ifstream(const wchar_t* s, ios_base::openmode mode = ios_base::in)
      :basic_istream(&sb)
    {
      open(s, mode);
    }
    explicit basic_ifstream(const wstring& s, ios_base::openmode mode = ios_base::in)
      :basic_istream(&sb)
    {
      open(s, mode);
    }


    ///\name 27.9.1.8 Assign and swap [ifstream.assign]
#ifdef NTL__CXX_RV
    basic_ifstream& operator=(basic_ifstream&& rhs)
    {
      swap(rhs); return *this;
    }
#endif
    void swap(basic_ifstream& rhs)
    {
      basic_istream::swap(rhs);
      sb.swap(rhs.sb);
    }

    ///\name 27.9.1.9 Member functions [ifstream.members]
    basic_filebuf<charT,traits>* rdbuf() const { return const_cast<basic_filebuf<charT,traits>*>(&sb); }

    bool is_open() const { return sb.is_open(); }

    void open(const char* s, ios_base::openmode mode = ios_base::in)
    {
      if(!sb.open(s, mode|ios_base::in))
        setstate(ios_base::failbit);
    }
    void open(const string& s, ios_base::openmode mode = ios_base::in)
    {
      if(!sb.open(s, mode|ios_base::in))
        setstate(ios_base::failbit);
    }
    // NTL extension
    void open(const wchar_t* s, ios_base::openmode mode = ios_base::in)
    {
      if(!sb.open(s, mode|ios_base::in))
        setstate(ios_base::failbit);
    }
    void open(const wstring& s, ios_base::openmode mode = ios_base::in)
    {
      if(!sb.open(s.c_str(), mode|ios_base::in))
        setstate(ios_base::failbit);
    }

    void close()
    {
      if(!sb.close())
        setstate(ios_base::failbit);
    }

  private:
    basic_filebuf<charT,traits> sb;

    basic_ifstream(const basic_ifstream&) __deleted;
    basic_ifstream& operator=(const basic_ifstream&) __deleted;
};


/**
 *	@brief 27.9.1.10 Class template basic_ofstream [ofstream]
 *  @details The class basic_ofstream<charT, traits> supports writing to named files.
 *  It uses a basic_filebuf<charT, traits> object to control the associated sequence.
 **/
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_ofstream:
  public basic_ostream<charT, traits>
{
  public:
    typedef charT char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits traits_type;

    ///\name 27.9.1.11 basic_ofstream constructors [ofstream.cons]
    basic_ofstream()
      :basic_ostream(&sb)
    {}
    explicit basic_ofstream(const char* s, ios_base::openmode mode = ios_base::out)
      :basic_ostream(&sb)
    {
      open(s, mode);
    }
    explicit basic_ofstream(const string& s, ios_base::openmode mode = ios_base::out)
      :basic_ostream(&sb)
    {
      open(s, mode);
    }
#ifdef NTL__CXX_RV
    basic_ofstream(basic_ofstream&& rhs);
#endif

    // NTL extension
    explicit basic_ofstream(const wchar_t* s, ios_base::openmode mode = ios_base::out)
      :basic_ostream(&sb)
    {
      open(s, mode);
    }
    explicit basic_ofstream(const wstring& s, ios_base::openmode mode = ios_base::out)
      :basic_ostream(&sb)
    {
      open(s, mode);
    }

    ///\name 27.9.1.12 Assign and swap [ofstream.assign]
#ifdef NTL__CXX_RV
    basic_ofstream& operator=(basic_ofstream&& rhs)
    {
      return swap(rhs), *this;
    }
#endif

    void swap(basic_ofstream& rhs)
    {
      basic_ostream::swap(rhs);
      sb.swap(rhs.sb);
    }

    ///\name 27.9.1.13 Member functions [ofstream.members]
    basic_filebuf<charT,traits>* rdbuf() const { return const_cast<basic_filebuf<charT,traits>*>(&sb); }
    
    bool is_open() const { return sb.is_open(); }

    void open(const char* s, ios_base::openmode mode = ios_base::out)
    {
      if(!sb.open(s, mode|ios_base::out))
        setstate(ios_base::failbit);
    }
    void open(const string& s, ios_base::openmode mode = ios_base::out)
    {
      if(!sb.open(s, mode|ios_base::out))
        setstate(ios_base::failbit);
    }
    // NTL extension
    void open(const wchar_t* s, ios_base::openmode mode = ios_base::out)
    {
      if(!sb.open(s, mode|ios_base::in))
        setstate(ios_base::failbit);
    }
    void open(const wstring& s, ios_base::openmode mode = ios_base::out)
    {
      if(!sb.open(s.c_str(), mode|ios_base::in))
        setstate(ios_base::failbit);
    }

    void close()
    {
      if(!sb.close())
        setstate(ios_base::failbit);
    }

  ///////////////////////////////////////////////////////////////////////////
  private:
    basic_filebuf<charT,traits> sb;

    basic_ofstream(const basic_ofstream&) __deleted;
    basic_ofstream& operator=(const basic_ofstream&) __deleted;
};


/**
 *	@brief 27.9.1.14 Class template basic_fstream [fstream]
 *  @details The class template basic_fstream<charT,traits> supports reading and writing from named files.
 *  It uses a basic_filebuf<charT,traits> object to control the associated sequences. 
 **/
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_fstream:
  public basic_iostream<charT, traits>
{
  public:
    typedef charT char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits traits_type;

    ///\name 27.9.1.15 basic_fstream constructors [fstream.cons]
    basic_fstream()
      :basic_iostream(&sb)
    {}
    explicit basic_fstream(const char* s, ios_base::openmode mode = ios_base::in|ios_base::out)
      :basic_iostream(&sb)
    {
      open(s, mode);
    }
    explicit basic_fstream(const string& s, ios_base::openmode mode = ios_base::in|ios_base::out)
      :basic_iostream(&sb)
    {
      open(s, mode);
    }
#ifdef NTL__CXX_RV
    basic_fstream(basic_fstream&& rhs);
#endif

    // NTL extension
    explicit basic_fstream(const wchar_t* s, ios_base::openmode mode = ios_base::in|ios_base::out)
      :basic_iostream(&sb)
    {
      open(s, mode);
    }
    explicit basic_fstream(const wstring& s, ios_base::openmode mode = ios_base::in|ios_base::out)
      :basic_iostream(&sb)
    {
      open(s, mode);
    }


    ///\name 27.9.1.16 Assign and swap [fstream.assign]
#ifdef NTL__CXX_RV
    basic_fstream& operator=(basic_fstream&& rhs)
    {
      return swap(rhs), *this;
    }
#endif

    void swap(basic_fstream& rhs)
    {
      basic_iostream::swap(rhs);
      sb.swap(rhs.sb);
    }

    ///\name  27.9.1.17 Member functions [fstream.members]
    basic_filebuf<charT,traits>* rdbuf() const { return const_cast<basic_filebuf<charT,traits>*>(&sb); }

    bool is_open() const { return sb.is_open(); }

    void open(const char* s, ios_base::openmode mode = ios_base::in|ios_base::out)
    {
      if(!sb.open(s, mode))
        setstate(ios_base::failbit);
    }
    void open(const string& s, ios_base::openmode mode = ios_base::in|ios_base::out)
    {
      if(!sb.open(s, mode))
        setstate(ios_base::failbit);
    }
    // NTL extension
    void open(const wchar_t* s, ios_base::openmode mode = ios_base::in|ios_base::out)
    {
      if(!sb.open(s, mode|ios_base::in))
        setstate(ios_base::failbit);
    }
    void open(const wstring& s, ios_base::openmode mode = ios_base::in|ios_base::out)
    {
      if(!sb.open(s.c_str(), mode|ios_base::in))
        setstate(ios_base::failbit);
    }


    void close()
    {
      if(!sb.close())
        setstate(ios_base::failbit);
    }
  private:
    basic_filebuf<charT,traits> sb;

    basic_fstream(const basic_fstream&) __deleted;
    basic_fstream& operator=(const basic_fstream&) __deleted;
};

//////////////////////////////////////////////////////////////////////////

template <class charT, class traits>
inline void swap(basic_filebuf<charT, traits>& x, basic_filebuf<charT, traits>& y)   { x.swap(y); }

template <class charT, class traits>
inline void swap(basic_ifstream<charT, traits>& x, basic_ifstream<charT, traits>& y) { x.swap(y); }

template <class charT, class traits>
inline void swap(basic_ofstream<charT, traits>& x, basic_ofstream<charT, traits>& y) { x.swap(y); }

//////////////////////////////////////////////////////////////////////////
/**@} lib_file_streams */
/**@} lib_string_streams */
}//namespace std
#endif//#ifndef NTL__STLX_FSTREAM
