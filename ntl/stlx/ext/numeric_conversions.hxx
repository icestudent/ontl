/**\file*********************************************************************
 *                                                                     \brief
 *  Class template circular_buffer
 *
 ****************************************************************************
 */
#ifndef NTL__EXT_NUMERIC_CONVERSIONS
#define NTL__EXT_NUMERIC_CONVERSIONS
#pragma once

#ifndef NTL__STLX_LIMITS
# include "../limits.hxx"
#endif

#ifndef NTL__STLX_TYPE_TRAITS
# include "../type_traits.hxx"
#endif

namespace ntl { namespace numeric {

  namespace detail
  {
    static const std::int16_t ascii_mask[256] =
    {
      0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x068, 0x028, 0x028, 0x028, 0x028, 0x020, 0x020,
      0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020,
      0x048, 0x010, 0x010, 0x010, 0x000, 0x010, 0x010, 0x010, 0x010, 0x010, 0x010, 0x000, 0x010, 0x010, 0x010, 0x010,
      0x084, 0x084, 0x084, 0x084, 0x084, 0x084, 0x084, 0x084, 0x084, 0x084, 0x010, 0x010, 0x000, 0x000, 0x000, 0x010,
      0x010, 0x181, 0x181, 0x181, 0x181, 0x181, 0x181, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101,
      0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x010, 0x010, 0x010, 0x000, 0x010,
      0x000, 0x182, 0x182, 0x182, 0x182, 0x182, 0x182, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102,
      0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x010, 0x000, 0x010, 0x000, 0x020,
      0x020, 0x020, 0x020, 0x020, 0x020, 0x028, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020,
      0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020,
      0x048, 0x010, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x102, 0x010, 0x000, 0x020, 0x000, 0x000,
      0x000, 0x000, 0x000, 0x000, 0x000, 0x102, 0x000, 0x010, 0x000, 0x000, 0x102, 0x010, 0x000, 0x000, 0x000, 0x010,
      0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101,
      0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x000, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x102,
      0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102,
      0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x000, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102
    };

    class mini_ctype
    {
      static const std::int16_t __ct_upper = 0x0001;
      static const std::int16_t __ct_lower = 0x0002;
      static const std::int16_t __ct_digit = 0x0004;
      static const std::int16_t __ct_space = 0x0008;
      static const std::int16_t __ct_punct = 0x0010;
      static const std::int16_t __ct_cntrl = 0x0020;
      static const std::int16_t __ct_blank = 0x0040;
      static const std::int16_t __ct_xdigit= 0x0080;
      static const std::int16_t __ct_alpha = 0x0100;
    public:
      typedef std::int16_t mask;
      static const mask
        /** is lower */
        lower = __ct_lower,
        /** is upper */
        upper = __ct_upper,
        /** is letter */
        alpha = lower|upper|__ct_alpha,
        /** is control character */
        cntrl = __ct_cntrl,
        /** is digit */
        digit = __ct_digit,
        /** is hexadecimal digit */
        xdigit = __ct_xdigit,
        /** is whitespace */
        space = __ct_space|__ct_blank,
        /** is punctuation */
        punct = __ct_punct,
        /** is printable, including space */
        print = alpha|digit|punct|__ct_blank,// __ct_space?
        /** is alphanumeric */
        alnum = alpha|digit,
        /** is graphic character, excluding space */
        graph = alnum|punct;

    public:
      /** Classifies a character. */
      static bool is(mask m, char c) 
      {
        return (detail::ascii_mask[static_cast<unsigned char>(c)] & m) != 0;
      }

    private:
    };
  }

  namespace conv_result
  {
    enum type {
      // success values
      ok,
      eof,
      // error values
      overflow    = -1,
      bad_format  = -2,
      bad_base    = -3
    };
  }
  typedef conv_result::type convresult;

  static const std::size_t max_number_size = 68;

  template<typename storage_type, typename signed_storage_type>
  static convresult str2num(storage_type& value, const char* str, std::ssize_t len, unsigned int base, storage_type max_val, signed_storage_type min_val, 
                              std::size_t* taken = 0, char thousands_sep = '\0', char decimal_sep = '\0')
  {
    using std::size_t;
    convresult result = conv_result::bad_format;
    value = 0;
    if(taken) *taken = 0;
    (void)decimal_sep; // reserved for floating point numbers

    if(!(base == 0 ||(base >= 2 && base <= 36)))
      return conv_result::bad_base;

    // skip whitespace
    const char* in = str;
    while(len && *in){
      if(!detail::mini_ctype::is(detail::mini_ctype::space, *in))
        break;
      len--, in++;
    }
    if(!len || !*in){
      if(taken) *taken = in-str;
      return conv_result::eof;
    }

    // check sign
    bool minus = false;
    if(*in == '-' || *in == '+'){
      minus = *in++ == '-';
      len--;
    }
    if(!len || !*in){
      if(taken) *taken = in-str;
      return conv_result::eof;
    }

    // detect or skip base prefix (only on unknown base or base = 8|16)
    unsigned lbase = 10;
    if((base == 0 || base == 8 || base == 16) && in[0] == '0' && len && in[1]){
      in++, len--;
      if(in[0] == 'x' || in[0] == 'X')
        lbase = 16, in++, len--;
      else
        lbase = 8;
    }
    if(!base) base = lbase;

    // initialization
    const storage_type max_val2 = minus ? static_cast<storage_type>(min_val) : max_val;
    const unsigned rem = static_cast<unsigned>(max_val2 % base);
    const storage_type max_base_val = max_val2 / base;

    // parse state
    //const bool use_dcep = base == 10 && decimal_sep != '\0';
    bool overflow = false;

    size_t ic = 0; // number of digits extracted
    do{
      char c = *in;
      if(!c)
        break;

      if(value != 0 && c == thousands_sep) // skip group separator in the middle of digits (only in octal or decimal base)
        continue;

      #define InRange(ch, a, b)  (unsigned(ch - a) <= b - a)
      #define InRangeI(ch, a, b) (unsigned((ch & ~(1<<5)) - a) <= b - a)

      unsigned digit;
      if(detail::mini_ctype::is(detail::mini_ctype::alpha, c))
        digit = (c & ~(1 << 5)) - 'A' + 10;
      else
        digit = c - '0';
      if(digit >= base)
        break;

      ++ic;
      if(value < max_base_val || (value == max_base_val && digit <= rem))
        value = value * base + digit;
      else{
        overflow = true;
        break;
      }
    }while(++in && --len && *in);

   if(taken) *taken = in - str;
    if(!ic)
      result = conv_result::bad_format;
    else if(len == 0 || !*in)
      result = conv_result::eof;
    else
      result = conv_result::ok;

    if(overflow){
      value = minus ? min_val : max_val;
      result = conv_result::overflow;
    }else if(minus)
      value = static_cast<storage_type>( -static_cast<signed_storage_type>(value) );
    
    return result;
  }


  template<typename storage_type>
  static convresult num2str(storage_type value, bool is_signed, char* const str, std::size_t len, int base = 10, std::size_t* written = 0)
  {
    if(written)
      *written = 0;
    if(!str || !len)
      return conv_result::eof;
    else if(!(base >= 2 && base <= 36))
      return conv_result::bad_base;

    char buf[max_number_size];
    buf[0] = '+';
    char *p = buf+1, *s = str;

    if(is_signed && base == 10 && static_cast<typename std::make_signed<storage_type>::type>(value) < 0)
      buf[0] = '-',
      value = static_cast<storage_type>(-static_cast<typename std::make_signed<storage_type>::type>(value));

    do {
      char c = static_cast<char>( value % base );
      c = c + (c > 10 ? 'a' : '0');
      *p++ = c;
    }while((value /= base) != 0);

    const size_t size = (p-buf) - (buf[0] == '+');
    if(size > len)
      return conv_result::eof;

    if(written) 
      *written = size;

    if(*buf == '-')
      *s++ = '-';

    while(--p > buf)
      *s++ = *p;
    if(size < len)
      *s = '\0';

    return value == 0 ? conv_result::ok : conv_result::eof;
  }


  //////////////////////////////////////////////////////////////////////////
  inline int strtoi(const char * __restrict nptr, const char ** __restrict endptr = 0, int base = 0)
  {
    typedef int value_t;
    std::size_t taken;
    unsigned long value;
    const convresult re = str2num<unsigned long, long>(value, nptr, -1, base, std::numeric_limits<value_t>::__max, std::numeric_limits<value_t>::__min, &taken);
    if(endptr) *endptr = re <= conv_result::bad_format ? nptr : nptr + taken;
    return static_cast<value_t>(value);
  }

  inline long strtol(const char * __restrict nptr, const char ** __restrict endptr = 0, int base = 0)
  {
    typedef long value_t;
    std::size_t taken;
    unsigned long value;
    const convresult re = str2num<unsigned long, long>(value, nptr, -1, base, std::numeric_limits<value_t>::__max, std::numeric_limits<value_t>::__min, &taken);
    if(endptr) *endptr = re <= conv_result::bad_format ? nptr : nptr + taken;
    return static_cast<value_t>(value);
  }

  inline unsigned long strtoul(const char * __restrict nptr, const char ** __restrict endptr = 0, int base = 0)
  {
    typedef unsigned long value_t;
    std::size_t taken;
    unsigned long value;
    const convresult re = str2num<unsigned long, long>(value, nptr, -1, base, std::numeric_limits<value_t>::__max, std::numeric_limits<value_t>::__min, &taken);
    if(endptr) *endptr = re <= conv_result::bad_format ? nptr : nptr + taken;
    return static_cast<value_t>(value);
  }

  inline long long strtoll(const char * __restrict nptr, const char ** __restrict endptr = 0, int base = 0)
  {
    typedef long long value_t;
    std::size_t taken;
    unsigned long long value;
    const convresult re = str2num<unsigned long long, long long>(value, nptr, -1, base, std::numeric_limits<value_t>::__max, std::numeric_limits<value_t>::__min, &taken);
    if(endptr) *endptr = re <= conv_result::bad_format ? nptr : nptr + taken;
    return static_cast<value_t>(value);
  }

  inline unsigned long long strtoull(const char * __restrict nptr, const char ** __restrict endptr = 0, int base = 0)
  {
    typedef unsigned long long value_t;
    std::size_t taken;
    unsigned long long value;
    const convresult re = str2num<unsigned long long, long long>(value, nptr, -1, base, std::numeric_limits<value_t>::__max, std::numeric_limits<value_t>::__min, &taken);
    if(endptr) *endptr = re <= conv_result::bad_format ? nptr : nptr + taken;
    return static_cast<value_t>(value);
  }

  //////////////////////////////////////////////////////////////////////////
  namespace detail
  {
    template<typename T>
    struct select
    {
      typedef typename std::conditional<sizeof(T) <= sizeof(long), unsigned long, unsigned long long>::type type;
    };
  }

  template<typename T>
  inline char* itoa(T value, char* str, std::size_t strLen, int radix = 10, std::size_t* written = 0)
  {
    static_assert(std::is_integral<T>::value, "only integral types supported");
    typedef detail::select<T>::type storage_type;
    num2str<storage_type>(static_cast<storage_type>(value), std::is_signed<T>::value, str, strLen, radix, written);
    return str;
  }

  template<typename T, std::size_t N>
  inline char* itoa(T value, char (&str)[N], int radix = 10, std::size_t* written = 0)
  {
    static_assert(std::is_integral<T>::value, "only integral types supported");
    typedef detail::select<T>::type storage_type;
    num2str<storage_type>(static_cast<storage_type>(value), std::is_signed<T>::value, str, N, radix, written);
    return str;
  }

  template<typename T>
  inline wchar_t* itow(T value, wchar_t* str, std::size_t strLen, int radix = 10, std::size_t* written = 0)
  {
    static_assert(std::is_integral<T>::value, "only integral types supported");
    typedef detail::select<T>::type storage_type;
    char buf[max_number_size];
    std::size_t lwritten;
    const convresult re = num2str<storage_type>(static_cast<storage_type>(value), std::is_signed<T>::value, buf, strLen, radix, &lwritten);
    if(re != conv_result::ok)
      return str;
    for(std::size_t i = 0; i < lwritten; i++)
      str[i] = buf[i];
    if(strLen > lwritten)
      str[lwritten] = 0;
    if(written) *written = lwritten;
    return str;
  }

  template<typename T, std::size_t N>
  inline wchar_t* itow(T value, wchar_t (&str)[N], int radix = 10, std::size_t* written = 0)
  {
    static_assert(std::is_integral<T>::value, "only integral types supported");
    typedef detail::select<T>::type storage_type;
    char buf[max_number_size];
    std::size_t lwritten;
    const convresult re = num2str<storage_type>(static_cast<storage_type>(value), std::is_signed<T>::value, buf, N, radix, &lwritten);
    if(re != conv_result::ok)
      return str;
    for(std::size_t i = 0; i < lwritten; i++)
      str[i] = buf[i];
    if(N > lwritten)
      str[lwritten] = 0;
    if(written) *written = lwritten;
    return str;
  }

}}
#endif // NTL__EXT_NUMERIC_CONVERSIONS

