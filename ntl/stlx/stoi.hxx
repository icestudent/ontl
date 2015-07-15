#pragma once

#include "string_ref.hxx"

#ifndef NTL__EXT_NUMERIC_CONVERSIONS
# include "ext/numeric_conversions.hxx"
#endif

namespace std
{
  namespace __
  {
    template<typename T>
    inline T stoi(const char* str, size_t length, size_t* idx, int base)
    {
      size_t l;
      typedef typename conditional<(sizeof(T) > sizeof(long)), unsigned long long, unsigned long>::type storage_type;
      storage_type value;
      ntl::numeric::convresult re = ntl::numeric::str2num<storage_type, typename make_signed<storage_type>::type>(value, str, static_cast<ssize_t>(length), base, numeric_limits<T>::__max, numeric_limits<T>::__min, &l);
      if(idx) *idx = l;
      if(re <= ntl::numeric::conv_result::bad_format){
#if STLX_USE_EXCEPTIONS
        __throw_invalid_argument("stoi: no conversion could be performed");
#else
        _assert_msg("stoi: no conversion could be performed");
#endif
      }else if(re == ntl::numeric::conv_result::overflow){
#if STLX_USE_EXCEPTIONS
        __throw_out_of_range("stoi: converted value is outside the range of representable values");
#else
        _assert_msg("stoi: converted value is outside the range of representable values");
#endif
      }
      return static_cast<T>(value);
    }
  }

  inline int stoi(const string& str, size_t *idx, int base)
  {
    return __::stoi<int>(str.data(), str.length(), idx, base);
  }

  inline long stol(const string& str, size_t *idx, int base)
  {
    return __::stoi<long>(str.data(), str.length(), idx, base);
  }

  inline unsigned long stoul(const string& str, size_t *idx, int base)
  {
    return __::stoi<unsigned long>(str.data(), str.length(), idx, base);
  }

  inline long long stoll(const string& str, size_t *idx, int base)
  {
    return __::stoi<long long>(str.data(), str.length(), idx, base);
  }

  inline unsigned long long stoull(const string& str, size_t *idx, int base)
  {
    return __::stoi<unsigned long long>(str.data(), str.length(), idx, base);
  }

  inline int stoi(const string_ref& str, size_t *idx, int base)
  {
    return __::stoi<int>(str.data(), str.length(), idx, base);
  }

  inline long stol(const string_ref& str, size_t *idx, int base)
  {
    return __::stoi<long>(str.data(), str.length(), idx, base);
  }

  inline unsigned long stoul(const string_ref& str, size_t *idx, int base)
  {
    return __::stoi<unsigned long>(str.data(), str.length(), idx, base);
  }

  inline long long stoll(const string_ref& str, size_t *idx, int base)
  {
    return __::stoi<long long>(str.data(), str.length(), idx, base);
  }

  inline unsigned long long stoull(const string_ref& str, size_t *idx, int base)
  {
    return __::stoi<unsigned long long>(str.data(), str.length(), idx, base);
  }

  inline int stoi(const wstring_ref& str, size_t *idx, int base)
  {
    char buf[ntl::numeric::max_number_size];
    const wchar_t* p = str.data();
    const size_t len = min(str.length(), ntl::numeric::max_number_size);
    for(size_t i = 0; i != len; i++, p++)
      buf[i] = static_cast<char>(*p);
    return __::stoi<int>(buf, len, idx, base);
  }

  inline long stol(const wstring_ref& str, size_t *idx, int base)
  {
    char buf[ntl::numeric::max_number_size];
    const wchar_t* p = str.data();
    const size_t len = min(str.length(), ntl::numeric::max_number_size);
    for(size_t i = 0; i != len; i++, p++)
      buf[i] = static_cast<char>(*p);
    return __::stoi<long>(buf, len, idx, base);
  }

  inline unsigned long stoul(const wstring_ref& str, size_t *idx, int base)
  {
    char buf[ntl::numeric::max_number_size];
    const wchar_t* p = str.data();
    const size_t len = min(str.length(), ntl::numeric::max_number_size);
    for(size_t i = 0; i != len; i++, p++)
      buf[i] = static_cast<char>(*p);
    return __::stoi<unsigned long>(buf, len, idx, base);
  }

  inline long long stoll(const wstring_ref& str, size_t *idx, int base)
  {
    char buf[ntl::numeric::max_number_size];
    const wchar_t* p = str.data();
    const size_t len = min(str.length(), ntl::numeric::max_number_size);
    for(size_t i = 0; i != len; i++, p++)
      buf[i] = static_cast<char>(*p);
    return __::stoi<long long>(buf, len, idx, base);
  }

  inline unsigned long long stoull(const wstring_ref& str, size_t *idx, int base)
  {
    char buf[ntl::numeric::max_number_size];
    const wchar_t* p = str.data();
    const size_t len = min(str.length(), ntl::numeric::max_number_size);
    for(size_t i = 0; i != len; i++, p++)
      buf[i] = static_cast<char>(*p);
    return __::stoi<unsigned long long>(buf, len, idx, base);
  }

  inline int stoi(const wstring& str, size_t *idx, int base)
  {
    return stoi(wstring_ref(str), idx, base);
  }

  inline long stol(const wstring& str, size_t *idx, int base)
  {
    return stol(wstring_ref(str), idx, base);
  }

  inline unsigned long stoul(const wstring& str, size_t *idx, int base)
  {
    return stoul(wstring_ref(str), idx, base);
  }


  inline long long stoll(const wstring& str, size_t *idx, int base)
  {
    return stoll(wstring_ref(str), idx, base);
  }

  inline unsigned long long stoull(const wstring& str, size_t *idx, int base)
  {
    return stoull(wstring_ref(str), idx, base);
  }

} // namespace std
