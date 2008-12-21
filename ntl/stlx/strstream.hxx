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
#include "istream.hxx"
#include "ostream.hxx"


namespace std {
/**\addtogroup  lib_input_output ******* 27 Input/output library [input.output] **********
 *@{*/

/**\ingroup std_depr
 *@{*/
/**\defgroup  str_strstreams ********** D.7 Strings streams [depr.str.strstreams] ***************
 *@{*/

/// D.7.1 Class strstreambuf [depr.strstreambuf]
/// 1 The class strstreambuf associates the input sequence, and possibly
///   the output sequence, with an object of some character array type, whose
///   elements store arbitrary values. The array object has several attributes.
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
    //  if ( strmode & dynamic )
      strmode = strmode | frozen ^ (frozen & strstate(freezefl-1));
    }

    char* str()
    {
      freeze();
      return gptr();
    }

    ///\note int replaced with streamsize
    streamsize pcount()
    {
      return !pptr() ? reinterpret_cast<streamsize>(pptr()) : pptr() - pbase();
    }

  ///////////////////////////////////////////////////////////////////////////
  protected:

    ///\name D.7.1.3 strstreambuf overridden virtual functions [depr.strstreambuf.virtuals]

    /// 1 Effects: Appends the character designated by c to the output sequence,
    ///   if possible, in one of two ways:
    /// — If c != EOF and if either the output sequence has a write position
    ///   available or the function makes a write position available (as described below),
    ///   assigns c to *pnext++.
    /// 2 Returns (unsigned char)c.
    /// — If c == EOF, there is no character to append.
    /// 3 Returns a value other than EOF.
    /// 4 Returns EOF to indicate failure.
    /// 5 Remarks: The function can alter the number of write positions
    ///   available as a result of any call.
    /// 6 To make a write position available, the function reallocates
    ///   (or initially allocates) an array object with a sufficient number
    ///   of elements n to hold the current array object (if any), plus at least
    ///   one additional write position. How many additional write positions are
    ///   made available is otherwise unspecified. [...]
    ///   If the allocation fails, the function returns EOF.
    ///   Otherwise, it sets allocated in strmode.
    /// 8 If strmode & dynamic == 0, or if strmode & frozen != 0, the function
    ///   cannot extend the array (reallocate it with greater length)
    ///   to make a write position available.
    virtual int_type overflow(int_type c = EOF)
    {
      if ( c == EOF )
        return c + 1;
      if ( pptr() >= epptr()
        && strmode & dynamic
        && !(strmode & frozen) )
      {
        const streamsize newsize = __ntl_grow_heap_block_size(alsize);
        char * p = _alloc(newsize);
        if ( p )
        {
          alsize = newsize;
          strmode |= allocated;
          if ( pbase() )
          {
            std::copy(pbase(), pptr()/*epptr()*/, p);
            _free(pbase());
          }
          const ptrdiff_t pnextdiff = pptr() - pbase();
          setp(p, p + alsize);
          pnext += pnextdiff;//pbump(pnextdiff);
          setg(p, p + (gnext - gbeg), pnext + 1/*note pnext++ below*/);
        }
      }
      if ( pptr() >= epptr() )
        return EOF;
      *pnext++ = traits_type::to_char_type(c);
      return (unsigned char)c;
    }

    /// 9 Puts back the character designated by c to the input sequence,
    ///   if possible, in one of three ways:
    /// — If c != EOF, if the input sequence has a putback position available,
    ///   and if (char)c == gnext[-1], assigns gnext - 1 to gnext.
    /// 10 Returns c.
    /// — If c != EOF, if the input sequence has a putback position available,
    ///   and if strmode & constant is zero, assigns c to *--gnext.
    ///   11 Returns c.
    /// — If c == EOF and if the input sequence has a putback position available,
    ///   assigns gnext - 1 to gnext.
    /// 12 Returns a value other than EOF.
    /// 13 Returns EOF to indicate failure.
    /// 14 Remarks: If the function can succeed in more than one of these ways,
    ///    it is unspecified which way is chosen. The function can alter
    ///    the number of putback positions available as a result of any call.
    virtual int_type pbackfail(int_type c = EOF)
    {
      if ( gnext <= gbeg )
        return EOF;
      if ( c == EOF )
      {
        --gnext;
        return c + 1;
      }
      if ( strmode & constant )
      {
        if ( (char)c != gnext[-1] )
          return EOF;
        --gnext;
        return c;
      }
      *--gnext = traits_type::to_char_type(c);
      return c;
    }

    /// 15 Effects: Reads a character from the input sequence, if possible,
    ///    without moving the stream position past it, as follows:
    /// — If the input sequence has a read position available, the function
    ///   signals success by returning (unsigned char)*gnext.
    /// — Otherwise, if the current write next pointer pnext is not a null pointer
    ///   and is greater than the current read end pointer gend, makes a read
    ///   position available by assigning to gend a value greater than gnext and
    ///   no greater than pnext.
    /// 16 Returns (unsigned char*)gnext.
    /// 17 Returns EOF to indicate failure.
    /// 18 Remarks: The function can alter the number of read positions available
    ///    as a result of any call.
    virtual int_type underflow()
    {
      if ( gnext < gend )
        return (unsigned char)*gnext;
      if ( pnext && pnext > gend )
      {
        gend = pnext;
        return (unsigned char)*gnext;
      }
      return EOF;
    }

    /// 19 Effects: Alters the stream position within one of the controlled
    ///   sequences, if possible, as follows:
    /// - If (which & ios::in) != 0, positions the input sequence.
    /// - If (which & ios::out) != 0, positions the output sequence.
    /// - If (which & (ios::in | ios::out)) == (ios::in | ios::out)) and
    ///   way == either ios::beg or ios::end, positions both the input and the output sequences
    /// - Otherwise, the positioning operation fails.
    /// 20 For a sequence to be positioned, if its next pointer is a null pointer,
    ///   the positioning operation fails. Otherwise, the function determines
    ///   newoff, as follows:
    ///   condition           newoff value
    ///   way == ios::beg     0
    ///   way == ios::cur     the next pointer minus the beginning pointer (xnext - xbeg).
    ///   way == ios::end     seekhigh minus the beginning pointer (seekhigh - xbeg).
    /// - If (newoff + off) < (seeklow - xbeg), or (seekhigh - xbeg) < (newoff + off),
    ///   the positioning operation fails.
    /// 21 Otherwise, the function assigns xbeg + newoff + off to the next pointer xnext.
    /// 22 Returns: pos_type(newoff), constructed from the resultant offset newoff
    ///   (of type off_type), that stores the resultant stream position, if possible.
    ///   If the positioning operation fails, or if the constructed object cannot
    ///   represent the resultant stream position, the return value is pos_type(off_type(-1)).
    virtual
    pos_type
      seekoff(off_type            off,
              ios_base::seekdir   way,
              ios_base::openmode  which = ios_base::in | ios_base::out)
    {
      const pos_type failed = pos_type(off_type(-1));
      if ( (which & (ios::in | ios::out)) == (ios::in | ios::out)
        && way == ios::cur
        || !(which & (ios::in | ios::out)) )
        return failed;
      char_type * const sh = gnext ? (pend ? pend : gend) : 0;
      if ( !sh )
        return failed;
      char_type * const sl  = seeklow();
      if ( which & ios::in )
      {
        const off_type newoff = off + way == ios::beg ? 0
                                    : way == ios::cur ? gnext - gbeg
                                    : /*way == ios::end ?*/ sh - gbeg;
        if ( newoff < (sl - gbeg) || (sh - gbeg) < newoff )
          return failed;
        gnext = gbeg + newoff;
        if ( !(which & ios::out) )
          return pos_type(newoff);
      }
      // ( which & ios::out )
      if ( !pnext )
        return failed;
      const off_type newoff = off + way == ios::beg ? 0
                                  : way == ios::cur ? pnext - pbeg
                                  : /*way == ios::end ?*/ sh - pbeg;
      if ( newoff < (sl - pbeg) || (sh - pbeg) < newoff )
        return failed;
      pnext = pbeg + newoff;
      return pos_type(newoff);
    }

    /// 23 Effects: Alters the stream position within one of the controlled sequences,
    ///   if possible, to correspond to the stream position stored in sp (as described below).
    /// — If (which & ios::in) != 0, positions the input sequence.
    /// — If (which & ios::out) != 0, positions the output sequence.
    /// — If the function positions neither sequence, the positioning operation fails.
    /// 24 For a sequence to be positioned, if its next pointer is a null pointer,
    ///   the positioning operation fails. Otherwise, the function determines
    ///   newoff from sp.offset():
    /// — If newoff is an invalid stream position, has a negative value, or has
    ///  a value greater than (seekhigh - seeklow), the positioning operation fails
    /// — Otherwise, the function adds newoff to the beginning pointer xbeg and
    ///   stores the result in the next pointer xnext.
    /// 25 Returns: pos_type(newoff), constructed from the resultant offset
    ///   newoff (of type off_type), that stores the resultant stream position,
    ///   if possible. If the positioning operation fails, or if the constructed
    ///   object cannot represent the resultant stream position,
    ///   the return value is pos_type(off_type(-1)).
    virtual
    pos_type
      seekpos(pos_type sp,
              ios_base::openmode which = ios_base::in | ios_base::out)
    {
      const pos_type failed = pos_type(off_type(-1));
      if ( !(which & (ios::in | ios::out)) )
        return failed;
      const off_type newoff = sp;
      if ( newoff < 0 || newoff > seekhigh() - seeklow() )
        return failed;
      if ( which & ios::in )
      {
        if ( !gnext ) return failed;
        gnext = gbeg + newoff;
      }
      if ( which & ios::out )
      {
        if ( !pnext ) return failed;
        pnext = pbeg + newoff;
      }
      return pos_type(newoff);
    }

    /// 26 Effects: Implementation defined, except that setbuf(0, 0) has no effect.
    //virtual streambuf* setbuf(char* s , streamsize n);

    ///}

  ///////////////////////////////////////////////////////////////////////////
  private:

    /// 4 Each object of class strstreambuf has a seekable area, delimited by the
    ///   pointers seeklow and seekhigh. If gnext is a null pointer, the seekable
    ///   area is undefined. Otherwise, seeklow equals gbeg and seekhigh is either
    ///   pend, if pend is not a null pointer, or gend.
    char_type * seekhigh() const { return gnext ? (pend ? pend : gend) : gnext; }
    char_type * seeklow() const { return gnext ? gbeg : gnext/* 0 */; }

    enum strstate
    {
      /// set when a dynamic array object has been allocated, and
      /// hence should be freed by the destructor for the strstreambuf object;
      allocated = 1 << 0,
      /// set when the array object has const elements,
      /// so the output sequence cannot be written;
      constant  = 1 << 1,
      /// set when the array object is allocated (or reallocated) as necessary
      /// to hold a character sequence that can change in length;
      dynamic   = 1 << 2,
      /// set when the program has requested that the array object
      /// not be altered, reallocated, or freed.
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

    /// 6 ... If palloc is not a null pointer, the function calls (*palloc)(n)
    ///   to allocate the new dynamic array object.
    ///   Otherwise, it evaluates the expression new charT[n]
//    __forceinline
    char* _alloc(size_t size)
    {
      return palloc ? (char*)palloc(size) : new (std::nothrow) char[size];
    }

    /// 7 To free a previously existing dynamic array object whose first element
    ///   address is p:
    ///   If pfree is not a null pointer, the function calls (*pfree)(p).
    ///   Otherwise, it evaluates the expression delete[] p.
//    __forceinline
    void _free(char* p)
    {
      if ( pfree ) pfree(p);
      else delete[] p;
    }

};//class strstreambuf


/// D.7.2 Class istrstream [depr.istrstream].
/// 1 The class istrstream supports the reading of objects of class strstreambuf.
///   It supplies a strstreambuf object to control the associated array object.
class istrstream : public istream//basic_istream<char>
{
  public:

    ///\name D.7.2.1 istrstream constructors [depr.istrstream.cons]

    /// 1 Effects: Constructs an object of class istrstream, initializing
    ///   the base class with istream(&sb) and initializing sb with
    ///   strstreambuf(s,0)). s shall designate the first element of an NTBS.
    explicit istrstream(const char* s) : basic_istream<char>(&sb), sb(s, 0) {;}
    explicit istrstream(char* s) : basic_istream<char>(&sb), sb(s, 0) {;}

    /// 2 Effects: Constructs an object of class istrstream, initializing
    ///   the base class with istream(&sb) and initializing sb with
    ///   strstreambuf(s,n)). s shall designate the first element of
    ///   an array whose length is n elements, and n shall be greater than zero.
    istrstream(const char* s, streamsize n) : basic_istream<char>(&sb), sb(s, n) {;}
    istrstream(char* s, streamsize n) : basic_istream<char>(&sb), sb(s, n) {;}

    virtual ~istrstream() {;}

    ///\name D.7.2.2 Member functions [depr.istrstream.members]

    /// 1 Returns: const_cast<strstreambuf*>(&sb)
    strstreambuf* rdbuf() const { return const_cast<strstreambuf*>(&sb); }

    /// 2 Returns: rdbuf()->str().
    char *str() { return rdbuf()->str(); }

    ///\}

  private:
    strstreambuf sb;
};


/// D.7.3 Class ostrstream [depr.ostrstream].
/// 1 The class ostrstream supports the writing of objects of class strstreambuf.
///   It supplies a strstreambuf object to control the associated array object.
class ostrstream : public ostream//basic_ostream<char>
{
  public:

    ///\name D.7.3.1 ostrstream constructors [depr.ostrstream.cons]

    /// 1 Effects: Constructs an object of class ostrstream, initializing
    ///   the base class with ostream(&sb) and initializing sb with strstreambuf().
    ostrstream() : basic_ostream<char>(&sb), sb() {;}

    /// 2 Effects: Constructs an object of class ostrstream, initializing
    ///   the base class with ostream(&sb), and initializing sb with one of two
    ///   constructors:
    /// — If (mode & app) == 0, then s shall designate the first element of
    ///   an array of n elements. The constructor is strstreambuf(s, n, s).
    /// — If (mode & app) != 0, then s shall designate the first element of
    ///   an array of n elements that contains an NTBS whose first element is
    ///   designated by s. The constructor is strstreambuf(s, n, s + std::strlen(s)).
    ostrstream(char* s, int n, ios_base::openmode mode = ios_base::out)
    : basic_ostream<char>(&sb),
      sb(s, n, s + (mode & ios_base::app ? strlen(s) : 0))
    {;}

    virtual ~ostrstream() {;}

    ///\name D.7.3.2 Member functions [depr.ostrstream.members]

    /// 1 Returns: (strstreambuf*)&sb .
    strstreambuf* rdbuf() const { return const_cast<strstreambuf*>(&sb); }

    /// 2 Effects: Calls rdbuf()->freeze(freezefl).
    void freeze(bool freezefl = true) { rdbuf()->freeze(freezefl); }

    /// 3 Returns: rdbuf()->str().
    char *str() { return rdbuf()->str(); }

    /// 4 Returns: rdbuf()->pcount().
    streamsize pcount() const { return rdbuf()->pcount(); }

    ///\}

  private:
    strstreambuf sb;
};


/// D.7.4 Class strstream [depr.strstream].
/// 1 The class strstream supports reading and writing from objects
///   of class strstreambuf. It supplies a strstreambuf object to control
///   the associated array object.
class strstream : public iostream//basic_iostream<char>
{
  public:

    ///\name Types
    typedef char char_type;
    typedef char_traits<char>::int_type int_type;
    typedef char_traits<char>::pos_type pos_type;
    typedef char_traits<char>::off_type off_type;

    ///\name D.7.4.1 strstream constructors [depr.strstream.cons]

    /// 1 Effects: Constructs an object of class strstream,
    ///   initializing the base class with iostream(&sb).
    strstream() : basic_iostream<char>(&sb), sb() {;}

    /// 2 Effects: Constructs an object of class strstream, initializing
    ///   the base class with iostream(&sb) and initializing sb
    ///   with one of the two constructors:
    ///   — If (mode & app) == 0, then s shall designate the first element of
    ///     an array of n elements. The constructor is strstreambuf(s,n,s).
    ///   — If (mode & app) != 0, then s shall designate the first element of
    ///     an array of n elements that contains an NTBS whose first element
    ///     is designated by s. The constructor is strstreambuf(s,n,s + std::strlen(s)).
    strstream(char* s, int n, ios_base::openmode mode = ios_base::in|ios_base::out)
    : basic_iostream<char>(&sb),
      sb(s, n, s + (mode & ios_base::app ? strlen(s) : 0))
    {;}

    ///\name D.7.4.2 strstream destructor [depr.strstream.dest]
    virtual ~strstream() {}

    ///\name D.7.4.3 strstream operations [depr.strstream.oper]

    strstreambuf* rdbuf() const { return const_cast<strstreambuf*>(&sb); }

    /// 1 Effects: Calls rdbuf()->freeze(freezefl).
    void freeze(bool freezefl = true) { rdbuf()->freeze(freezefl); }

    /// 2 Returns: rdbuf()->str().
    char *str() { return rdbuf()->str(); }

    /// 3 Returns: rdbuf()->pcount().
    streamsize pcount() const { return rdbuf()->pcount(); }

    ///\}

  private:
    strstreambuf sb;
};

/**@} str_strstreams */
/**@} std_depr */
/**@} lib_input_output */

}//namespace std

#endif//#ifndef NTL__STLX_STRSTREAM
