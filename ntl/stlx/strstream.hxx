/**\file*********************************************************************
 *                                                                     \brief
 *  N2315 D.7 char* streams [depr.str.strstreams]
 *
 *  ///\note deprecated by Standard
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_STRSTREAM
#define NTL__STLX_STRSTREAM

#include "string.hxx"
#include "iosfwd.hxx"
#include "streambuf.hxx"


namespace std {


/**\defgroup  str_strstreams ********** Strings streams [D.7] ***************
 *@{*/

///D.7.1 Class strstreambuf [depr.strstreambuf]
class strstreambuf : public basic_streambuf<char>
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    ///\name D.7.1.1 strstreambuf constructors [depr.strstreambuf.cons]
 
    explicit strstreambuf(streamsize alsize_arg = 0)
    : basic_streambuf(),
      strmode(dynamic), alsize(alsize_arg), palloc(0), pfree(0)
    {/**/}
    
    strstreambuf(void* (*palloc_arg)(size_t), void (*pfree_arg)(void*))
    : basic_streambuf(),/*alsize(alsize_arg), an unspecified value*/
      strmode(dynamic), palloc(palloc_arg), pfree(pfree_arg)
    {/**/}

    strstreambuf(char* gnext_arg, streamsize n, char* pbeg_arg = 0)
    : basic_streambuf(),/*alsize(alsize_arg), an unspecified value*/
      strmode(), palloc(0), pfree(0)
    {
      _construct(gnext_arg, n, pbeg_arg);
    }
    
    strstreambuf(const char* gnext_arg, streamsize n)
    : basic_streambuf(),/*alsize(alsize_arg), an unspecified value*/
      strmode(constant), palloc(0), pfree(0)
    {
      _construct(gnext_arg, n);
    }

    strstreambuf(signed char* gnext_arg, streamsize n, signed char* pbeg_arg = 0)
    : basic_streambuf(),/*alsize(alsize_arg), an unspecified value*/
      strmode(), palloc(0), pfree(0)
    {
      _construct(reinterpret_cast<char*>(gnext_arg), n, reinterpret_cast<char*>(pbeg_arg));
    }
    
    strstreambuf(const signed char* gnext_arg, streamsize n)
    : basic_streambuf(),/*alsize(alsize_arg), an unspecified value*/
      strmode(constant), palloc(0), pfree(0)
    {
      _construct(reinterpret_cast<const char*>(gnext_arg), n);
    }

    strstreambuf(unsigned char* gnext_arg, streamsize n, unsigned char* pbeg_arg = 0)
    : basic_streambuf(),/*alsize(alsize_arg), an unspecified value*/
      strmode(), palloc(0), pfree(0)
    {
      _construct(reinterpret_cast<char*>(gnext_arg), n, reinterpret_cast<char*>(pbeg_arg));
    }

    strstreambuf(const unsigned char* gnext_arg , streamsize n )
    : basic_streambuf(),/*alsize(alsize_arg), an unspecified value*/
      strmode(constant), palloc(0), pfree(0)
    {
      _construct(reinterpret_cast<const char*>(gnext_arg), n);
    }

    virtual ~strstreambuf()
    {
      if ( (strmode & allocated) && !(strmode & frozen) )
      {
        _free(eback());
      }
    }

    ///\name  D.7.1.2 Member functions [depr.strstreambuf.members]

    void freeze(bool freezefl = true)
    {
      strmode = strmode | frozen ^ (frozen & strstate(freezefl-1));
    }

    char* str()
    {
      freeze();
      return gptr();
    }
    
    streamsize pcount()
    {
      return !pptr() ? reinterpret_cast<streamsize>(pptr()) : pptr() - pbase();
    }

  ///////////////////////////////////////////////////////////////////////////
  protected:

    ///\name D.7.1.3 strstreambuf overridden virtual functions [depr.strstreambuf.virtuals]

    virtual int_type overflow(int_type c = EOF)
    {
      if ( c == EOF )
        return c + 1;
      if ( pptr() )
      {

      }

    }
    
    virtual int_type pbackfail(int_type c = EOF);
    virtual int_type underflow();
    virtual pos_type seekoff(off_type off , ios_base::seekdir way ,
    ios_base::openmode which
    = ios_base::in | ios_base::out);
    virtual pos_type seekpos(pos_type sp , ios_base::openmode which
    = ios_base::in | ios_base::out);
    virtual streambuf* setbuf(char* s , streamsize n );

  ///////////////////////////////////////////////////////////////////////////
  private:

    enum strstate
    { 
      allocated = 1 << 0,
      constant  = 1 << 1,
      dynamic   = 1 << 2,
      frozen    = 1 << 3
    };

    __ntl_bitmask_type(strstate, friend);

    strstate    strmode;
    streamsize  alsize;
    void*     (*palloc)(size_t);
    void      (*pfree)(void*);

    void _construct(const char* gnext_arg, streamsize n)
    {
      const streamsize _n = n > 0 ? n : !n ? static_cast<ssize_t>(strlen(gnext_arg)) : INT_MAX;
      setg(const_cast<char*>(gnext_arg),
           const_cast<char*>(gnext_arg),
           const_cast<char*>(gnext_arg + _n));
    }
    void _construct(char* gnext_arg, streamsize n, char* pbeg_arg = 0)
    {
      const streamsize _n = n > 0 ? n : !n ? static_cast<ssize_t>(strlen(gnext_arg)) : INT_MAX;
      if ( pbeg_arg == 0 )
        setg(gnext_arg, gnext_arg, gnext_arg + _n);
      else
      {
        setg(gnext_arg, gnext_arg, pbeg_arg);
        setp(pbeg_arg, pbeg_arg + _n);
      }
    }

    void* _alloc(size_t size)
    {
      return palloc ? palloc(size) : ::new char[size];
    }
    
    void _free(void* ptr)
    {
      if ( pfree ) pfree(ptr);
      else delete[] ptr;
    }
    

};//class strstreambuf

/**@} str_strstreams */

}//namespace std

#endif//#ifndef NTL__STLX_STRSTREAM
