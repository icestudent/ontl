/**\file*********************************************************************
 *                                                                     \brief
 *  27.8.1 File streams [lib.fstreams]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_FSTREAM
#define NTL__STLX_FSTREAM

#include "streambuf.hxx"
#include "ostream.hxx"

namespace std {

/// 27.8.1.1 Class template basic_filebuf [lib.filebuf]
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_filebuf
: public basic_streambuf<charT, traits>
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef charT                     char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits                    traits_type;

    // 27.8.1.2 basic_filebuf constructors [lib.filebuf.cons]

    basic_filebuf();

    virtual ~basic_filebuf() { close(); }

    // 27.8.1.3 Member functions [lib.filebuf.members]

    bool is_open() const;

    basic_filebuf<charT,traits>* open(const char* s, ios_base::openmode mode);

    basic_filebuf<charT,traits>* close();

  ///////////////////////////////////////////////////////////////////////////
  protected:

    // 27.8.1.4 Overridden virtual functions [lib.filebuf.virtuals]

    virtual streamsize showmanyc();
    virtual int_type underflow();
    virtual int_type uflow();
    virtual int_type pbackfail(int_type c = traits::eof());
    virtual int_type overflow (int_type c = traits::eof());
    virtual basic_streambuf<charT,traits>*
    setbuf(char_type* s, streamsize n);
    virtual pos_type seekoff(off_type off, ios_base::seekdir way, ios_base::openmode which = ios_base::in | ios_base::out);
    virtual pos_type seekpos(pos_type sp, ios_base::openmode which = ios_base::in | ios_base::out);

    virtual int sync();

    virtual void imbue(const locale& loc);

  ///////////////////////////////////////////////////////////////////////////
  private:

};


/// 27.8.1.5 Class template basic_ifstream [lib.ifstream]
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_ifstream
: public basic_istream<charT, traits>
{
  public:

    typedef charT                     char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits                    traits_type;

    // 27.8.1.6 Constructors:

    basic_ifstream();
    explicit basic_ifstream(const char* s, ios_base::openmode mode = ios_base::in);

    // 27.8.1.7 Members:

    basic_filebuf<charT,traits>* rdbuf() const;
    bool is_open();
    void open(const char* s, ios_base::openmode mode = ios_base::in);
    void close();

  private:
    // basic_filebuf<charT,traits> sb; exposition only
};


/// 27.8.1.8 Class template basic_ofstream [lib.ofstream]
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_ofstream
: public basic_ostream<charT, traits>
{
  public:

    typedef charT char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits traits_type;

    // 27.8.1.9 Constructors:

    basic_ofstream();
    explicit basic_ofstream(const char* s, ios_base::openmode mode = ios_base::out);

    // 27.8.1.10 Members:

    basic_filebuf<charT,traits>* rdbuf() const;
    bool is_open();
    void open(const char* s, ios_base::openmode mode = ios_base::out);
    void close();

  ///////////////////////////////////////////////////////////////////////////
  private:
    // basic_filebuf<charT,traits> sb; exposition only

  private:
    basic_ofstream(const basic_ofstream&); // not defined
    basic_ofstream& operator=(const basic_ofstream&); // not defined
};


/// 27.8.1.11 Class template basic_fstream [lib.fstream]
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_fstream
: public basic_iostream<charT, traits>
{
  public:

    typedef charT char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits traits_type;
    // constructors/destructor
    basic_fstream();
    explicit basic_fstream(
    const char* s,
    ios_base::openmode mode = ios_base::in|ios_base::out);
    // Members:
    basic_filebuf<charT,traits>* rdbuf() const;
    bool is_open();
    void open(
    const char* s,
    ios_base::openmode mode = ios_base::in|ios_base::out);
    void close();

  private:
    // basic_filebuf<charT,traits> sb; exposition only
};


}//namespace std

#endif//#ifndef NTL__STLX_FSTREAM
