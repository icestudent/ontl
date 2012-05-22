#ifndef NTL__CRYPTO_MD5
#define NTL__CRYPTO_MD5
#pragma once

#include <nt/peb.hxx>
#include <pe/image.hxx>
#include "../stlx/ostream.hxx"
#include "../stlx/vector.hxx"

namespace ntl
{
  namespace crypto
  {
    struct md5_ctx
    {
      uint32_t  i[2];
      uint32_t  buf[4];
      uint8_t   in[64];
      uint8_t   hash[16];
    };

    class md5: md5_ctx
    {
      typedef void __stdcall MD5Init_t(md5_ctx* context);
      typedef void __stdcall MD5Final_t(md5_ctx* context);
      typedef void __stdcall MD5Update_t(md5_ctx* context, const void* input, uint32_t size);

      struct runtime
      {
        MD5Init_t* init;
        MD5Update_t* update;
        MD5Final_t* final;

        runtime()
        {
          init = nullptr,
            update = nullptr,
            final = nullptr;

          using namespace ntl::nt;
          peb::find_dll f(&peb::instance());
          const ntl::pe::image* ntdll = f("ntdll.dll");
          if(!ntdll)
            return;
          init = ntdll->find_export<MD5Init_t*>("MD5Init");
          update = ntdll->find_export<MD5Update_t*>("MD5Update");
          final = ntdll->find_export<MD5Final_t*>("MD5Final");
          if(!init){
            // for xp x86
            ntdll = f("advapi32.dll");
            if(!ntdll)
              return;
            init = ntdll->find_export<MD5Init_t*>("MD5Init");
            update = ntdll->find_export<MD5Update_t*>("MD5Update");
            final = ntdll->find_export<MD5Final_t*>("MD5Final");
          }
        }
      };

      runtime* funcs;
      bool finalized;
    public:
      enum { digest_size = 16 };
      typedef uint8_t digest[digest_size];

      md5()
        :finalized(false)
      {
        funcs = ntl::static_storage<runtime>::get_object();
        funcs->init(this);
      }

      md5& operator()(const void* input, size_t size)
      {
        finalized = false;
        funcs->update(this, input, static_cast<uint32_t>(size));
        return *this;
      }

      const digest& final()
      {
        if(!finalized){
          funcs->final(this);
          finalized = true;
        }
        return hash;
      }
    };

    template<typename T>
    inline md5& operator<<(md5& m, const T& x)
    {
      return m(&x, sizeof(T));
    }
    inline md5& operator<<(md5& m, const char* str)
    {
      return m(str, std::strlen(str));
    }
    
    inline md5& operator>>(md5& m, md5::digest& digest)
    {
      std::memcpy(&digest, &m.final(), m.digest_size);
      return m;
    }

    template<class charT, class traits, class Allocator>
    inline md5& operator<<(md5& m, const std::basic_string<charT,traits,Allocator>& str)
    {
      return m(str.data(), str.size()*sizeof(charT));
    }
    template<class T, class Allocator>
    inline md5& operator<<(md5& m, const std::vector<T,Allocator>& v)
    {
      return m(v.data(), v.size()*sizeof(T));
    }

  }
}
#endif // NTL__CRYPTO_MD5
