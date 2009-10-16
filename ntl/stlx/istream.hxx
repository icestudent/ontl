/**\file*********************************************************************
 *                                                                     \brief
 *  N2315 27.6.1 Input streams [input.streams]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_ISTREAM
#define NTL__STLX_ISTREAM

#include "ostream.hxx"

namespace std
{

/**\addtogroup  lib_input_output ******* 27 Input/output library [input.output]
 *@{*/

/**\addtogroup  lib_iostream_format **** 27.6 Formatting and manipulators
 *@{*/

/**\addtogroup  lib_input_streams ****** 27.6.1 Input streams
 *@{*/

/// 27.6.1.1 Class template basic_istream [istream]
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_istream : virtual public basic_ios<charT, traits>
{
    typedef basic_ios<charT, traits> this_base;

  ///////////////////////////////////////////////////////////////////////////
  public:

    // Types (inherited from basic_ios (27.4.4)):
    typedef typename this_base::char_type   char_type;
    typedef typename this_base::int_type    int_type;
    typedef typename this_base::pos_type    pos_type;
    typedef typename this_base::off_type    off_type;
    typedef typename this_base::traits_type traits_type;

    // 27.6.1.1.1 basic_istream constructors [istream.cons]
    explicit basic_istream(basic_streambuf<charT, traits>* sb)
    : ccount(0)
    {
      basic_ios::init(sb);
    }

#ifdef NTL__CXX_RV
    basic_istream(basic_istream&& rhs);
      :ccount(0)
    {
      basic_ios::move(rhs);
      swap(ccount, rhs.ccount);
    }
#endif

    virtual ~basic_istream() {/**/}

    // 27.6.1.1.2 Class basic_istream assign and swap [istream.assign]

#ifdef NTL__CXX_RV
    basic_istream& operator=(basic_istream&& rhs)
    {
      swap(rhs); return *this;
    }
#endif

    void swap(basic_istream& rhs)
    {
      if(this != &rhs){
        using std::swap;
        basic_ios::swap(rhs);
        swap(ccount, rhs.ccount);
      }
    }


    // 27.6.1.1.3 Class basic_istream::sentry [istream::sentry]
    class sentry
    {
      typedef traits traits_type;
    public:
      // constructor for formated input
      //explicit sentry(basic_istream& is);// bool noskipws = false

      // constructor for un/formated input
      explicit sentry(basic_istream& is, bool noskipws = false)
        : ok_(false)
      {
        if ( is.good() )
        {
          if ( is.tie() ) is.tie()->flush();

          if(noskipws == false && is.flags() & ios_base::skipws){
            // formatted input, discard all input spaces
            basic_streambuf<char_type, traits_type>* buf = is.rdbuf();
            const ctype<char_type>& l = use_facet<ctype<char_type> >(is.getloc());
            const int_type eof = traits_type::eof();
            int_type c = buf->sgetc(); 
            while(!traits_type::eq_int_type(c, eof) && l.is(ctype_base::space, traits_type::to_char_type(c)))
              c = buf->snextc();

            if(traits_type::eq_int_type(c, eof))
              is.setstate(ios_base::eofbit|ios_base::failbit);  // can throw
          }
        }
        ok_ = is.good();
      }

      ~sentry() {/**/}

#ifdef NTL__CXX_EXPLICITOP
      explicit operator bool() const { return ok_; }
#else
      operator __::explicit_bool_type() const { return __::explicit_bool(ok_); }
#endif

    private:
      bool ok_;

      sentry(const sentry&) __deleted;
      sentry& operator=(const sentry&) __deleted;
    };

    // 27.6.1.2 Formatted input functions [istream.formatted]

    ///\name 27.6.1.2.2 Arithmetic Extractors [istream.formatted.arithmetic]
    basic_istream<charT,traits>& operator>>(bool& n)          { return get_impl(n); }
    basic_istream<charT,traits>& operator>>(short& n)         { return get_impl_signed(n); }
    basic_istream<charT,traits>& operator>>(unsigned short& n){ return get_impl(n); }
    basic_istream<charT,traits>& operator>>(int& n)           { return get_impl_signed(n); }
    basic_istream<charT,traits>& operator>>(unsigned int& n)  { return get_impl(n); }
    basic_istream<charT,traits>& operator>>(long& n)          { return get_impl(n); }
    basic_istream<charT,traits>& operator>>(unsigned long& n) { return get_impl(n); }
    basic_istream<charT,traits>& operator>>(long long& n)     { return get_impl(n); }
    basic_istream<charT,traits>& operator>>(unsigned long long& n)  { return get_impl(n); }
    basic_istream<charT,traits>& operator>>(float& f)         { return get_impl(f); }
    basic_istream<charT,traits>& operator>>(double& f)        { return get_impl(f); }
    basic_istream<charT,traits>& operator>>(long double& f)   { return get_impl(f); }
    basic_istream<charT,traits>& operator>>(void*& p)         { return get_impl(p); }


  protected:
    template<typename ValueT>
    basic_istream<charT,traits>& get_impl(ValueT& value)
    {
      const sentry ok(*this);
      ios_base::iostate state = ios_base::goodbit;
      if(ok)
        __ntl_try
      {
        typedef istreambuf_iterator<charT> iterator;
        typedef num_get<charT, iterator> facet_t;
        use_facet<facet_t>(getloc()).get(iterator(*this), iterator(), *this, state, value);
      }
      __ntl_catch(...){
        state |= ios_base::badbit;
      }
      if(state) setstate(state);
      return *this;
    }
    template<typename ValueT>
    basic_istream<charT,traits>& get_impl_signed(ValueT& n)
    {
      const sentry ok(*this);
      ios_base::iostate state = ios_base::goodbit;
      if(ok)
        __ntl_try
      {
        typedef istreambuf_iterator<charT> iterator;
        typedef num_get<charT, iterator> facet_t;

        long value;
        use_facet<facet_t>(getloc()).get(iterator(*this), iterator(), *this, state, value);

        if(state == ios_base::goodbit){
          typedef numeric_limits<ValueT> lim;
          if(!(value < lim::__min || value > lim::__max))
            n = static_cast<ValueT>(value);
          else {
            state |= ios_base::failbit;
            value = value < lim::__min ? lim::__min : lim::__max;
          }
        }
      }
      __ntl_catch(...){
        state |= ios_base::badbit;
      }
      if(state) setstate(state);
      return *this;
    }
  public:

    ///\name 27.6.1.2.3 basic_istream::operator>> [istream::extractors]
    basic_istream<charT, traits>& operator>>(basic_istream<charT,traits>&(*pf)(basic_istream<charT,traits>&))
    {
      return pf(*this);
    }

    basic_istream<charT, traits>& operator>>(basic_ios<charT,traits>& (*pf)(basic_ios<charT,traits>&))
    {
      pf(*this); return *this;
    }

    basic_istream<charT, traits>& operator>>(ios_base& (*pf)(ios_base&))
    {
      pf(*this); return *this;
    }

    basic_istream<charT, traits>& operator>>(basic_streambuf<char_type, traits>* sb)
    {
      const sentry ok(*this, true);
      ios_base::iostate state = sb ? ios_base::goodbit : ios_base::failbit;
      streamsize n = 0;
      if (ok && sb)
      __ntl_try
      {
        basic_streambuf<char_type, traits_type>* buf = rdbuf();
        const int_type eof = traits_type::eof();
        int_type c = buf->sgetc();
        while(!traits_type::eq_int_type(c, eof)){
          if(traits_type::eq_int_type(sb->sputc(traits_type::to_char_type(c)), eof))
            break;
          ++n;
        }
        if(traits_type::eq_int_type(c, eof))
          state |= ios_base::eofbit;
      }
      __ntl_catch(...)
      {
        state |= ios_base::badbit;
        // If it inserted no characters because it caught an exception thrown while extracting characters from
        // *this and failbit is on in exceptions() (27.5.4.3), then the caught exception is rethrown.
      }
      if(!n)
        state |= ios_base::failbit;
      this->setstate(state);
      return *this;
    }

    ///\todo non-member operator>>

    ///\name 27.6.1.3 Unformatted input functions [istream.unformatted]

    streamsize gcount() const { return ccount; }

    int_type get()
    {
      ccount = 0;
      const sentry ok(*this, true);
      ios_base::iostate state = ok ? ios_base::goodbit : ios_base::failbit;
      int_type c = traits_type::eof();
      if (ok)
      __ntl_try
      {
        c = this->rdbuf()->sbumpc();
        if ( traits_type::eq_int_type(traits_type::eof(), c) )
          state |= ios_base::eofbit | ios_base::failbit;
        else ++ccount;
      }
      __ntl_catch(...)
      {
        state |= ios_base::badbit;
        // NOTE: why this is commented?
        //if ( this->exceptions() & ios_base::badbit ) __ntl_rethrow;
      }
      if(state) this->setstate(state);
      return c;
    }

    basic_istream<charT,traits>& get(char_type& c)
    {
      const int_type ic = get();
      if ( !traits_type::eq_int_type(traits_type::eof(), ic) )
        c = traits_type::to_char_type(ic);
      return *this;
    }

    basic_istream<charT,traits>& get(char_type* s, streamsize n)
    {
      return get(s, n, this_base::widen('\n'));
    }

    basic_istream<charT,traits>& get(char_type* s, streamsize n, char_type delim)
    {
      ccount = 0;
      /* unformatted input functions taking a character array of non-zero size as an argument 
         shall also store a null character in the first location of the array. */
      if(n > 0) *s = char_type();
      const sentry ok(*this, true);
      ios_base::iostate state = ios_base::goodbit;
      if (ok){ // && n > 0?
        __ntl_try
        {
          const int_type eof = traits_type::eof();
          basic_streambuf<char_type, traits_type>* buf = rdbuf();
          for(int_type c = buf->sgetc(); 0 < --n; c = buf->snextc()){
            if(traits_type::eq_int_type(eof, c))
            {state |= ios_base::eofbit; break;}
            const char_type cc = traits_type::to_char_type(c);
            if(traits_type::eq(delim, cc))
              break;
            *s++ = cc;
            ++ccount;
          }
        }
        __ntl_catch(...)
        {
          state |= ios_base::badbit;
        }
        if ( n > 0 ) *s = char_type();
      }
      this->setstate(state | ccount ? state : ios_base::failbit);
      return *this;
    }

    basic_istream<charT,traits>& get(basic_streambuf<char_type,traits>& sb)
    {
      return get(sb, this_base::widen('\n'));
    }

    basic_istream<charT,traits>& get(basic_streambuf<char_type,traits>& dest, char_type delim)
    {
      ccount = 0;
      const sentry ok(*this, true);
      ios_base::iostate state = ios_base::goodbit;
      if (ok)
        __ntl_try
      {
        basic_streambuf<char_type, traits_type>* buf = rdbuf();
        for (int_type c = buf->sgetc(), eof = traits_type::eof(); ; c = buf->snextc() )
        {
          if ( traits_type::eq_int_type(eof, c) )
          {
            state |= ios_base::eofbit;
            break;
          }
          const char_type cc = traits_type::to_char_type(c);
          if (traits_type::eq(delim, cc))
            break;
          if ( traits_type::eq_int_type(eof, dest.sputc(cc)) )
            break;
          ++ccount;
        }
      }
      __ntl_catch(...)
      {
        state |= ios_base::badbit;
      }
      this->setstate(state | ccount ? state : ios_base::failbit);
      return *this;
    }

    basic_istream<charT,traits>& getline(char_type* s, streamsize n)
    {
      return getline(s, n, this_base::widen('\n'));
    }

    basic_istream<charT,traits>& getline(char_type* s, streamsize n, char_type delim)
    {
      ccount = 0;
      if(n > 0) *s = char_type();
      const sentry ok(*this, true);
      ios_base::iostate state = ios_base::goodbit;
      if(ok && n > 0){
        __ntl_try
        {
          basic_streambuf<char_type, traits_type>* buf = rdbuf();
          const int_type eof = traits_type::eof(), d = traits_type::to_int_type(delim);
          int_type c;

          while(ccount+1 < n){ // space for null-terminator
            c = buf->sbumpc();
            if(traits_type::eq_int_type(c, eof) || traits_type::eq_int_type(c, d))
              break;
            *s++ = traits_type::to_char_type(c);
            ++ccount;
          } 

          if(traits_type::eq_int_type(c, eof))
            state |= ios_base::eofbit;
          else if(traits_type::eq_int_type(c, d))
            ++ccount;
          else //if(n <= 1)
            state |= ios_base::failbit; // line too long
        }
        __ntl_catch(...)
        {
          state |= ios_base::badbit;
        }
        if(n > 0)
          *s = char_type();
      }
      if(!ccount)
        state |= ios_base::failbit;
      if(state) this->setstate(state);
      return *this;
    }

    basic_istream<charT,traits>& ignore(streamsize n = 1, int_type delim = traits::eof())
    {
      ccount = 0;
      const sentry ok(*this, true);
      if (ok && n > 0 && n != numeric_limits<streamsize>::__max){
        ios_base::iostate state = ios_base::goodbit;
        __ntl_try
        {
          basic_streambuf<char_type, traits_type>* buf = rdbuf();
          const int_type eof = traits_type::eof();
          int_type c;
          do{
            c = buf->sbumpc();
            if(traits_type::eq_int_type(c, eof) || traits_type::eq_int_type(c, delim))
              break;
          } while(++ccount, --n);
          if(traits_type::eq_int_type(c, eof))
            state |= ios_base::eofbit;
          else
            ++ccount;
        }
        __ntl_catch(...)
        {
          state |= ios_base::badbit;
        }
        if(state) this->setstate(state);
      }
      return *this;
    }

    int_type peek()
    {
      ccount = 0;
      int_type c = traits_type::eof();
      const sentry ok(*this, true);
      if (ok){
        ios_base::iostate state = ios_base::goodbit;
        __ntl_try
        {
          c = this->rdbuf()->sgetc();
          if ( traits_type::eq_int_type(traits_type::eof(), c) )
            state |= ios_base::eofbit;
        }
        __ntl_catch(...)
        {
          state |= ios_base::badbit;
        }
        if(state) this->setstate(state);
      }
      return c;
    }

    basic_istream<charT,traits>& read(char_type* s, streamsize n)
    {
      ccount = 0;
      const sentry ok(*this, true);
      ios_base::iostate state = ok ? ios_base::goodbit : ios_base::failbit;
      if (ok && n > 0)
      __ntl_try
      {
        ccount = rdbuf()->sgetn(s, n);
        if(ccount != n)
          state |= ios_base::eofbit|ios_base::failbit;
      }
      __ntl_catch(...)
      {
        state |= ios_base::badbit;
      }
      if(state) this->setstate(state);
      return *this;
    }

    streamsize readsome(char_type* s, streamsize n)
    {
      ccount = 0;
      const sentry ok(*this, true);
      ios_base::iostate state = ok ? ios_base::goodbit : ios_base::failbit;
      if (ok && n > 0)
      __ntl_try
      {
        const streamsize avail = rdbuf()->in_avail();
        if(avail == -1)
          state |= ios_base::eofbit;
        else if(avail > 0)
          ccount = rdbuf()->sgetn(s, min(avail, n));
      }
      __ntl_catch(...)
      {
        state |= ios_base::badbit;
      }
      if(state) this->setstate(state);
      return ccount;
    }

    basic_istream<charT,traits>& putback(char_type c)
    {
      ccount = 0;
      const sentry ok(*this, true);
      ios_base::iostate state = ok ? ios_base::goodbit : ios_base::failbit;
      if (ok)
      __ntl_try
      {
        basic_streambuf<char_type, traits_type>* buf = rdbuf();
        if(!buf || traits_type::eq_int_type(buf->sputbackc(c), traits_type::eof()))
          state |= ios_base::badbit;
      }
      __ntl_catch(...)
      {
        state |= ios_base::badbit;
      }
      if(state) this->setstate(state);
      return *this;
    }

    basic_istream<charT,traits>& unget()
    {
      ccount = 0;
      const sentry ok(*this, true);
      ios_base::iostate state = ok ? ios_base::goodbit : ios_base::failbit;
      if (ok)
      __ntl_try
      {
        basic_streambuf<char_type, traits_type>* buf = rdbuf();
        if(!buf || traits_type::eq_int_type(buf->sungetc(), traits_type::eof()))
          state |= ios_base::badbit;
      }
      __ntl_catch(...)
      {
        state |= ios_base::badbit;
      }
      if(state) this->setstate(state);
      return *this;
    }

    int sync()
    {
      int re = -1;
      const sentry ok(*this, true);
      if (ok){
        ios_base::iostate state = ios_base::goodbit;
        __ntl_try
        {
          basic_streambuf<char_type, traits_type>* buf = rdbuf();
          if(buf){
            if(buf->pubsync() == -1)
              state |= ios_base::badbit;
            else
              re = 0;
          }
        }
        __ntl_catch(...)
        {
          state |= ios_base::badbit;
        }
        if(state) this->setstate(state);
      }
      return re;
    }

    pos_type tellg()
    {
      pos_type re = pos_type(-1);
      const sentry ok(*this, true);
      if(!fail()){
        ios_base::iostate state = ios_base::goodbit;
        __ntl_try{
          re = rdbuf()->pubseekoff(0, ios_base::cur, ios_base::in);
        }
        __ntl_catch(...){
          state |= ios_base::badbit;
        }
        this->setstate(state);
      }
      return re;
    }

    basic_istream<charT,traits>& seekg(pos_type pos)
    {
      const sentry ok(*this, true);
      if(!fail()){
        ios_base::iostate state = ios_base::goodbit;
        __ntl_try{
          if(rdbuf()->pubseekpos(pos, ios_base::in) == pos_type(off_type(-1)))
            state |= ios_base::failbit;
        }
        __ntl_catch(...){
          state |= ios_base::badbit;
        }
        this->setstate(state);
      }
      return *this;
    }

    basic_istream<charT,traits>& seekg(off_type off, ios_base::seekdir dir)
    {
      const sentry ok(*this, true);
      if(!fail()){
        ios_base::iostate state = ios_base::goodbit;
        __ntl_try{
          // NOTE: is error checking required here?
          if(rdbuf()->pubseekoff(off, dir, ios_base::in) == pos_type(off_type(-1)))
            state |= ios_base::failbit;
        }
        __ntl_catch(...){
          state |= ios_base::badbit;
        }
        this->setstate(state);
      }
      return *this;
    }

  ///////////////////////////////////////////////////////////////////////////
  private:
    streamsize ccount;

    basic_istream(const basic_istream&) __deleted;
    basic_istream& operator=(const basic_istream&) __deleted;
};

///\name 27.7.1.2.3 character extraction templates [istream::extractors]

template<class charT, class traits>
inline basic_istream<charT,traits>& operator>>(basic_istream<charT,traits>& is, charT* s)
{
  const basic_istream<charT,traits>::sentry ok(is, true);
  ios_base::iostate state = ios_base::goodbit;
  streamsize ccount = 0;
  if(ok)
  __ntl_try
  {
    const ctype<charT>& ct = use_facet< ctype<charT> >(is.getloc());
    const traits::int_type eof = traits::eof();
    const streamsize max = (is.width() > 0 ? is.width() : numeric_limits<streamsize>::__max) - 1;
    basic_streambuf<charT, traits>* sb = is.rdbuf();

    traits::int_type c = sb->sgetc();
    while(ccount < max && !traits::eq_int_type(c, eof)){
      const charT cc = traits::to_char_type(c);
      if(ct.is(ctype_base::space, cc))
        break;
      *s++ = cc;
      ++ccount;
      c = sb->snextc();
    }
    if(traits::eq_int_type(c, eof))
      state |= ios_base::eofbit;
  }
  __ntl_catch(...){
    state |= ios_base::badbit;
  }
  *s = charT();
  is.width(0);
  if(!ccount)
    state |= ios_base::failbit;
  return is.setstate(state), is;
}

template<class traits>
basic_istream<char, traits>& operator>>(basic_istream<char, traits>& is,   signed char* s) { return is >> reinterpret_cast<char*>(s); }

template<class traits>
basic_istream<char, traits>& operator>>(basic_istream<char, traits>& is, unsigned char* s) { return is >> reinterpret_cast<char*>(s); }

// single character input
template<class charT, class traits>
inline basic_istream<charT,traits>& operator>>(basic_istream<charT,traits>& is, charT& c)
{
#if 1
  return is.get(c);
#else
  const basic_istream<charT,traits>::sentry ok(is, true);
  ios_base::iostate state = ios_base::goodbit;
  if(ok)
  __ntl_try
  {
    const traits::int_type ic = is.rdbuf()->sbumpc();
    if(!traits::eq_int_type(ic, traits::eof()))
      c = traits::to_char_type(ic);
    else
      state |= ios_base::eofbit|ios_base::failbit;
  }
  __ntl_catch(...){
    state |= ios_base::badbit;
  }
  return is.setstate(state), is;
#endif
}

template<class traits>
basic_istream<char, traits>& operator>>(basic_istream<char, traits>& is,   signed char& c) { return is >> reinterpret_cast<char&>(c); }

template<class traits>
basic_istream<char, traits>& operator>>(basic_istream<char, traits>& is, unsigned char& c) { return is >> reinterpret_cast<char&>(c); }

///\name  Inserters and extractors [21.4.8.9 string.io]
template<class charT, class traits, class Allocator>
inline basic_istream<charT,traits>& operator>>(basic_istream<charT,traits>& is, basic_string<charT,traits,Allocator>& str)
{
  const basic_istream<charT,traits>::sentry ok(is); // formatted input
  ios_base::iostate state = ios_base::goodbit;
  typedef basic_string<charT,traits,Allocator>::size_type size_type;
  size_type ccount = 0;
  if(ok)
    __ntl_try
  {
    const ctype<charT>& ct = use_facet< ctype<charT> >(is.getloc());
    const traits::int_type eof = traits::eof();
    const size_type max = is.width() > 0 ? static_cast<size_type>(is.width()) : str.max_size();
    basic_streambuf<charT, traits>* sb = is.rdbuf();

    size_t bufc = 0;
    charT buf[128]; // we extracting characters by 128-chars length portions to avoid unnecessary reallocates
    str.erase();

    traits::int_type c = sb->sgetc();
    while(ccount < max && !traits::eq_int_type(c, eof)){
      const charT cc = traits::to_char_type(c);
      if(ct.is(ctype_base::space, cc))
        break;
      buf[bufc++] = cc;
      ++ccount;
      if(bufc == _countof(buf)){
        str.append(buf, bufc);
        bufc = 0;
      }
      c = sb->snextc();
    }
    str.append(buf, bufc);
    // its a formatted input, so make string a bit pretty:
    str.c_str();
    if(traits::eq_int_type(c, eof))
      state |= ios_base::eofbit;
    is.width(0);
  }
  __ntl_catch(...){
    state |= ios_base::badbit;
  }
  if(!ccount)
    state |= ios_base::failbit;
  return is.setstate(state), is;
}

template<class charT, class traits, class Allocator>
inline basic_istream<charT,traits>& getline(basic_istream<charT,traits>& is, basic_string<charT,traits,Allocator>& str)
{
  return getline(is, str, is.widen('\n'));
}

template<class charT, class traits, class Allocator>
inline basic_istream<charT,traits>& getline(basic_istream<charT,traits>& is, basic_string<charT,traits,Allocator>& str, charT delim)
{
  const basic_istream<charT,traits>::sentry ok(is, true); // unformatted input
  ios_base::iostate state = ios_base::goodbit;
  basic_string<charT,traits,Allocator>::size_type ccount = 0, max = str.max_size();
  if(ok){
    __ntl_try
    {
      basic_streambuf<charT,traits>* sb = is.rdbuf();
      const traits::int_type eof = traits::eof(), d = traits::to_int_type(delim);
      traits::int_type c;

      size_t bufc = 0;
      charT buf[128]; // we extracting characters by 128-chars length portions to avoid unnecessary reallocates
      str.erase();

      while(ccount < max){
        c = sb->sbumpc();
        if(traits::eq_int_type(c, eof) || traits::eq_int_type(c, d))
          break;
        buf[bufc++] = traits::to_char_type(c);
        ++ccount;
        if(bufc == _countof(buf)){
          str.append(buf, bufc);
          bufc = 0;
        }
      }
      str.append(buf, bufc);
      if(traits::eq_int_type(c, eof))
        state |= ios_base::eofbit;
      else if(ccount == max)
        state |= ios_base::failbit; // line too long
    }
    __ntl_catch(...)
    {
      state |= ios_base::badbit;
    }
  }
  if(!ccount)
    state |= ios_base::failbit;
  if(state) is.setstate(state);
  return is;
}


///\}

template <class charT, class traits>
inline void swap(basic_istream<charT, traits>& x, basic_istream<charT, traits>& y) { x.swap(y); }


/// 27.6.1.4 Standard basic_istream manipulators [istream.manip]
/// 1 Effects: Behaves as an unformatted input function (as described in 27.6.1.3/1),
///   except that it does not count the number of characters extracted and
///   does not affect the value returned by subsequent calls to is.gcount().
///   After constructing a sentry object extracts characters as long as
///   the next available character c is whitespace or until there are no more
///   characters in the sequence. Whitespace characters are distinguished with
///   the same criterion as used by sentry::sentry (27.6.1.1.3).
///   If ws stops extracting characters because there are no more available it
///   sets eofbit, but not failbit.
/// 2 Returns: is.
template <class charT, class traits>
inline basic_istream<charT,traits>& ws(basic_istream<charT,traits>& is)
{
  const basic_istream<charT,traits>::sentry ok(is, true);
  ios_base::iostate state = ios_base::goodbit;
  if(ok)
  __ntl_try{
    const ctype<charT>& ct = use_facet< ctype<charT> >(is.getloc());
    const traits::int_type eof = traits::eof();

    basic_streambuf<charT, traits>* sb = is.rdbuf();
    traits::int_type c = sb->sgetc();
    while(!traits::eq_int_type(c, eof) && ct.is(ctype_base::space, traits::to_char_type(c)))
      c = sb->snextc();
    if(traits::eq_int_type(c, eof))
      state |= ios_base::eofbit;
  }
  __ntl_catch(...){
    state |= ios_base::badbit;
  }
  if(state) is.setstate(state);
  return is;
}


/// 27.6.1.5 Class template basic_iostream [iostreamclass].
/// 1 The class basic_iostream inherits a number of functions that allow
///   reading input and writing output to sequences controlled by a stream buffer.
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_iostream:
  public basic_istream<charT, traits>,
  public basic_ostream<charT, traits>
{
  public:
    ///\name Types
    typedef charT char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits traits_type;

    ///\name 27.6.1.5.1 basic_iostream constructors [iostream.cons]

    /// 1 Effects: Constructs an object of class basic_iostream, assigning
    ///   initial values to the base classes by calling
    ///   <tt>basic_istream<charT,traits>(sb)</tt> (27.6.1.1)
    ///   and <tt>basic_ostream<charT,traits>(sb)</tt> (27.6.2.1)
    /// 2 Postcondition: <tt>rdbuf()==sb</tt> and <tt>gcount()==0</tt>.
    explicit basic_iostream(basic_streambuf<charT,traits>* sb)
    : basic_istream<charT, traits>(sb), basic_ostream<charT, traits>(sb)
    {;}

#ifdef NTL__CXX_RV
    /// 3 Effects: Move constructs from the rvalue \p rhs by constructing
    ///   the basic_istream base class with <tt>move(rhs)</tt>.
    basic_iostream(basic_iostream&& rhs)
      :basic_istream(move(rhs))
    {}
#endif

    ///\name 27.6.1.5.2 basic_iostream destructor [iostream.dest]
    /// 1 Effects: Destroys an object of class basic_iostream.
    /// 2 Remarks: Does not perform any operations on \c rdbuf().
    virtual ~basic_iostream() {;}

    ///\name 27.6.1.5.3 basic_iostream assign and swap [iostream.assign]
#ifdef NTL__CXX_RV

    /// 1 Effects: swap(rhs).
    basic_iostream& operator=(basic_iostream&& rhs)
    {
      swap(rhs); return *this;
    }
#endif

    /// 2 Effects: Calls <tt>basic_istream<charT, traits>::swap(rhs)</tt>.
    void swap(basic_iostream& rhs)
    {
      basic_istream::swap(rhs);
    }
    ///\}
};

/// 3 Effects: x.swap(y).
template <class charT, class traits>
void swap(basic_iostream<charT, traits>& x, basic_iostream<charT, traits>& y)  { x.swap(y); }

#ifdef NTL__CXX_RV__0 // disabled in N2857+
template <class charT, class traits>
void swap(basic_iostream<charT, traits>&& x, basic_iostream<charT, traits>& y) { x.swap(y); }
template <class charT, class traits>
void swap(basic_iostream<charT, traits>& x, basic_iostream<charT, traits>&& y) { x.swap(y); }

/// 27.7.1.6 Rvalue stream extraction [istream.rvalue]
template <class charT, class traits, class T>
inline basic_istream<charT, traits>& operator>>(basic_istream<charT, traits>&& is, T& x)
{
  return is >> x;
}
#endif
/**@} lib_input_streams */
/**@} lib_iostream_format */
/**@} lib_input_output */

}//namespace std

#endif//#ifndef NTL__STLX_ISTREAM
