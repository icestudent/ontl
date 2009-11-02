/**\file*********************************************************************
 *                                                                     \brief
 *  Secure Hash Algorithm as declared in FIPS PUB 180-2
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 *
 *  @note at the moment only SHA1 is implemented.
 *
 ****************************************************************************
 */
#ifndef NTL__SHA
#define NTL__SHA
#pragma once

#include <stdint.h>
#include <stdlib.hxx>

namespace ntl
{

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4710) // operator()(const block & m) & hash_tail not inlined
#endif

class sha1
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef uint8_t  octet;

    enum { block_size = 512, block_bytes = block_size/8 };
    typedef octet block[block_bytes];

    struct digest
    {
        enum { size = 160 };

        inline
          digest(octet d00, octet d01, octet d02, octet d03, octet d04,
                 octet d05, octet d06, octet d07, octet d08, octet d09,
                 octet d10, octet d11, octet d12, octet d13, octet d14,
                 octet d15, octet d16, octet d17, octet d18, octet d19)
        {
          _[ 0] = d00; _[ 1] = d01; _[ 2] = d02;  _[ 3] = d03; _[ 4] = d04;
          _[ 5] = d05; _[ 6] = d06; _[ 7] = d07;  _[ 8] = d08; _[ 9] = d09;
          _[10] = d10; _[11] = d11; _[12] = d12;  _[13] = d13; _[14] = d14;
          _[15] = d15; _[16] = d16; _[17] = d17;  _[18] = d18; _[19] = d19;
        }

        const octet & operator [](int pos) const { return _[pos]; }

      friend static
        bool operator ==(const digest & d, const digest & d2)
          { return binary_equal(d, d2); }

      friend static
        bool operator !=(const digest & d, const digest & d2)
          { return ! (d == d2); }

        void * operator new(size_t, void * p) __ntl_nothrow { return p; }
        void operator delete(void * /*p*/, void * /*placement*/) __ntl_nothrow {/**/}

      private:
        octet _[size/8];
    };//struct digest

    sha1() { reset(); }

    operator const digest&() const { return *reinterpret_cast<const digest*>(&h[0]); }

    /// hash message
    /// @note the size is in bytes, not bits.
    const digest & operator()(const void * const message, const size_t bytes)
    {
      hash_complete_blocks(message, bytes);
      return hash_tail(message, bytes);
    }

    /// hash one block
    const digest & operator()(const block & m)
    {
      const uint32_t * w0 = reinterpret_cast<const uint32_t*>(&m[0]);
      uint32_t w[80];
      uint32_t a = big_endian(h[0]);  uint32_t b = big_endian(h[1]);
      uint32_t c = big_endian(h[2]);  uint32_t d = big_endian(h[3]);
      uint32_t e = big_endian(h[4]);  unsigned t = 0;
      do
      {
        w[t] = big_endian(w0[t]);
        // it's Ok to use 0 instead of t in f() and k() because t < 20 here
        const uint32_t temp = rotl(a, 5) + f(0, b, c, d) + e + w[t] - k(0);
        e = d;  d = c;  c = rotr(b, 2); b = a;  a = temp;
      }
      while ( ++t < 16 );
      do
      {
        w[t] = rotl(w[t-3] ^ w[t-8] ^ w[t-14] ^ w[t-16], 1);
        const uint32_t temp = rotl(a, 5) + f(t, b, c, d) + e + w[t] - k(t);
        e = d;  d = c;  c = rotr(b, 2); b = a;  a = temp;
      }
      while ( ++t < 80 );
      h[0] = big_endian(big_endian(h[0]) + a);
      h[1] = big_endian(big_endian(h[1]) + b);
      h[2] = big_endian(big_endian(h[2]) + c);
      h[3] = big_endian(big_endian(h[3]) + d);
      h[4] = big_endian(big_endian(h[4]) + e);
      return *this;
    }

    void hash_complete_blocks(const void * const message, size_t bytes)
    {
      const block * pm = reinterpret_cast<const block*>(message);
      const size_t ready_blocks = bytes / block_bytes;
      for ( size_t i = 0; i < ready_blocks; ++i )
        operator()(*pm++);
    }

    const digest & hash_tail(const void * const message, const size_t bytes)
    {
      const block * const pm = reinterpret_cast<const block*>(message);
      // copy the message tail
      const octet * tail = reinterpret_cast<const octet*>(&pm[bytes/block_bytes]);
      block _b[2]; octet * pad = reinterpret_cast<octet*>(_b);
      unsigned j = 0;
      for ( ; j < bytes % block_bytes; ++j )
        pad[j] = tail[j];
      // add `1' bit and fill with `0'.
      pad[j++] = 0x80;
      while ( (j % block_bytes) != block_bytes - sizeof(uint64_t) )
        pad[j++] = 0x00;
      // add 64 bit size and do hash
      *reinterpret_cast<uint64_t*>(&pad[j]) =
                                  big_endian(static_cast<uint64_t>(bytes) * 8);
      operator()(_b[0]);
      if ( j > block_bytes )  operator()(_b[1]);
      return *this;
    }

    void inline reset()
    {
      //set_state(digest());
      new (&h[0]) digest( 0x67, 0x45, 0x23, 0x01,   //h0 = 0x67452301;
                          0xEF, 0xCD, 0xAB, 0x89,   //h1 = 0xEFCDAB89;
                          0x98, 0xBA, 0xDC, 0xFE,   //h2 = 0x98BADCFE;
                          0x10, 0x32, 0x54, 0x76,   //h3 = 0x10325476;
                          0xC3, 0xD2, 0xE1, 0xF0 ); //h4 = 0xC3D2E1F0;
    }

    void inline set_state(const digest & state)
    {
      new (&h[0]) digest(state);
    }

#ifdef NTL_TEST
    /// @return 0 - Ok;
    static inline
    const char * test__implementation()
    {
      //  A.1 SHA-1 Example (One-Block Message)
      {
        static char msg[3] = { 'a', 'b', 'c' };
        digest etalon( 0xa9, 0x99, 0x3e, 0x36,  0x47, 0x06, 0x81, 0x6a,
                       0xba, 0x3e, 0x25, 0x71,  0x78, 0x50, 0xc2, 0x6c,
                       0x9c, 0xd0, 0xd8, 0x9d );
        sha1 hash;
        if ( etalon != hash(msg, sizeof(msg)) )
          return "A.1 SHA-1 Example (One-Block Message)";
      }
      //  A.2 SHA-1 Example (Multi-Block Message)
      {
        static char msg[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
        STATIC_ASSERT(448 / 8 == sizeof(msg) - 1);
        digest etalon( 0x84, 0x98, 0x3e, 0x44,  0x1c, 0x3b, 0xd2, 0x6e,
                       0xba, 0xae, 0x4a, 0xa1,  0xf9, 0x51, 0x29, 0xe5,
                       0xe5, 0x46, 0x70, 0xf1 );
        sha1 hash;
        if ( hash(msg, 448 / 8) != etalon )
          return "A.2 SHA-1 Example (Multi-Block Message)";
      }
      //  A.3 SHA-1 Example (Long Message)
      {
        // don't want to depend on allocators, anywhy this is just a test...
        static char msg[1000000];
        for ( unsigned i = 0; i < sizeof(msg); ++i )
          msg[i] = 'a';
        digest etalon( 0x34, 0xaa, 0x97, 0x3c,  0xd4, 0xc4, 0xda, 0xa4,
                       0xf6, 0x1e, 0xeb, 0x2b,  0xdb, 0xad, 0x27, 0x31,
                       0x65, 0x34, 0x01, 0x6f );
        sha1 hash;
        if ( hash(msg, sizeof(msg)) != etalon )
          return "A.3 SHA-1 Example (Long Message)";
      }
      return 0;
    }
#endif//#ifdef NTL_TEST

  ///////////////////////////////////////////////////////////////////////////
  private:

    uint32_t h[sizeof(digest)/sizeof(uint32_t)];

    /// SHA round constant
    /// @note the sign is changed to full some crypto searchers.
    static inline uint32_t k(unsigned const t)
    {
      return  t < 20 ?  -0x5A827999
            : t < 40 ?  -0x6ED9EBA1
            : t < 60 ?  0-0x8F1BBCDC
            :/* < 80 */ 0-0xCA62C1D6;
    }

    static inline uint32_t f(unsigned const t, uint32_t b,
                                  uint32_t c, uint32_t d)
    {
      return  t < 20 ?  (c ^ d) & b ^ d      // b & c | ~b & d
            : t < 40 ?  b ^ c ^ d
            : t < 60 ?  (c | d) & b | c & d  //b & c | b & d | c & d
            :/* < 80 */ b ^ c ^ d;
    }

};// class sha1

#ifdef _MSC_VER
#pragma warning(pop)
#endif

}//namespace ntl

#endif//#ifndef NTL__SHA
