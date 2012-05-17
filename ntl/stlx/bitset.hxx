/**\file*********************************************************************
 *                                                                     \brief
 *  Class template bitset [20.5 template.bitset]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_BITSET
#define NTL__STLX_BITSET
#pragma once

#include "cstddef.hxx"
#include "stdexcept.hxx"
#include "stdstring.hxx"
#ifndef NTL__STLX_IOSFWD
#include "iosfwd.hxx"    // for ios
#endif

namespace std {

  /**\addtogroup  lib_utilities *** 20 General utilities library [utilities]
  *@{*/

  /**\addtogroup  lib_bitset *****  20.5 Class template bitset [template.bitset]
  *@{*/

  template<size_t N>
  class bitset
  {
  public:
    // bit reference
    class reference
    {
      friend class bitset;
      reference();

      reference(bitset& b, const size_t pos)
        :bitset_(b), pos_(pos)
      {}
    public:
      ~reference(){}

      reference& operator=(bool x)
      {
        bitset_.set(pos_, x);
        return *this;
      }

      reference& operator=(const reference& rhs)
      {
        bitset_.set(pos_, rhs.bitset_.test(rhs.pos_));
        return *this;
      }

      bool operator~() const
      {
        return !bitset_.test(pos_);
      }

      operator bool() const
      {
        return bitset_.test(pos_);
      }

      reference& flip()
      {
        bitset_.flip(pos_);
        return *this;
      }

    private:
      bitset<N>& bitset_;
      const size_t pos_;
    };

    ///\name constructors [20.5.1]
    /*constexpr*/ bitset()  // can't make constexpr atm
    {
      reset();
    }

    /*constexpr*/ bitset(unsigned long long val)
    {
      // compiler cannot shift more than 32
      val &= tidy<N, N >= 32>::make_tidy();
      if(N > sizeof(unsigned long long)*8)
        reset();

      // split val to i elements
      const size_t count = min(elements_count_, sizeof(unsigned long long) / sizeof(storage_type));
      for(size_t x = 0; x < count; ++x)
        storage_[x] = static_cast<storage_type>((val >> x * element_size_) & set_bits_);
    }

    template<class charT, class traits, class Allocator>
    explicit bitset(
      const basic_string<charT,traits,Allocator>& str,
      typename basic_string<charT,traits,Allocator>::size_type pos = 0,
      typename basic_string<charT,traits,Allocator>::size_type n =
      basic_string<charT,traits,Allocator>::npos,
      charT zero = charT('0'), charT one = charT('1')
      ) __ntl_throws(out_of_range, invalid_argument)
    {
      typedef basic_string<charT,traits,Allocator> string_type;

      // 20.5.1/4
      if(pos > str.size())
        __ntl_throw(out_of_range("pos > str.size()"));

      // 20.5.1/5
      const unsigned rlen = static_cast<unsigned>(min(n, str.size()-pos));

      // 20.5.1/7
      if(rlen < elements_count_*element_size_)
        reset();

      const unsigned count = min(rlen, static_cast<unsigned>(N));
      for(unsigned i = 0, rpos = static_cast<unsigned>(pos) + count - 1; i < count; ++i, --rpos){
        const typename traits::char_type c = str[rpos];
        // 20.5.1/5.2
        if(!(c == zero || c == one))
          __ntl_throw(invalid_argument("str"));

        storage_type xval = storage_[i/element_size_];
        const unsigned mod = i & element_mod_;
        xval &= ~(native_one_ << mod);
      #ifndef _M_X64
        xval |= ((c == one) << mod);
      #else
        if(c == one)
          xval |= 1ui64 << mod;
      #endif
        storage_[i/element_size_] = xval;
      }
    }

    /// @name bitset operations [20.5.2]
    bitset<N>& operator&=(const bitset<N>& rhs)
    {
      for(unsigned i = 0; i < elements_count_; ++i)
        storage_[i] &= ~rhs.storage_[i];
      return *this;
    }

    bitset<N>& operator|=(const bitset<N>& rhs)
    {
      for(unsigned i = 0; i < elements_count_; ++i)
        storage_[i] |= rhs.storage_[i];
      return *this;
    }

    bitset<N>& operator^=(const bitset<N>& rhs)
    {
      for(unsigned i = 0; i < elements_count_; ++i)
        storage_[i] ^= rhs.storage_[i];
      return *this;
    }

    bitset<N>& operator<<=(size_t pos)
    {
      if(pos >= N)
        return reset();
      else if(pos == 0)
        return *this; // nothing to do
      else if(pos >= element_size_){
        large_shift_left(pos);
        return *this;
      }

      storage_type preval = 0, curval = 0;
      for(unsigned i = 0; i < elements_count_; ++i){
        // save shifted bits
        curval = storage_[i] >> (element_size_ - pos);
        // shift
        storage_[i] <<= pos;
        // restore previous bits
        storage_[i] |= preval;
        preval = curval;
      }

      // cut garbage bits
      storage_[elements_count_-1] &= digits_mod_;
      return *this;
    }

    bitset<N>& operator>>=(size_t pos)
    {
      if(pos > N)
        return reset();
      else if(pos == 0)
        return *this; // nothing to do
      else if(pos >= element_size_){
        large_shift_right(pos);
        return *this;
      }

      storage_type xval = 0, yval = 0;
      for(int i = elements_count_-1; i >= 0; --i){
        // save shifted bits
        yval = storage_[i] & ((native_one_ << pos) - 1);
        // shift
        storage_[i] >>= pos;
        // restore previous bits
        storage_[i] |= xval << (element_size_ - pos);
        xval = yval;
      }
      return *this;
    }

    bitset<N>& set()
    {
      for(unsigned i = 0; i < elements_count_; ++i)
        storage_[i] = set_bits_;
      return *this;
    }

    bitset<N>& set(size_t pos, bool val = true) __ntl_throws(out_of_range)
    {
      check_bounds(pos);
      storage_type xval = storage_[pos / element_size_];
      const size_t mod = pos & element_mod_;
      xval &= ~(1 << mod);
      xval |= (val << mod);
      storage_[pos / element_size_] = xval;
      return *this;
    }

    bitset<N>& reset()
    {
      for(unsigned i = 0; i < elements_count_; ++i)
        storage_[i] = 0;
      return *this;
    }

    bitset<N>& reset(size_t pos) __ntl_throws(out_of_range)
    {
      check_bounds(pos);
      storage_type val = storage_[pos / element_size_];
      const size_t mod = pos & element_mod_;
      val &= ~(1 << mod);
      storage_[pos / element_size_] = val;
      return *this;
    }

    bitset<N> operator~() const
    {
      return bitset<N>(*this).flip();
    }

    bitset<N>& flip() __ntl_nothrow
    {
      for(unsigned i = 0; i < elements_count_; ++i)
        storage_[i] = ~storage_[i];
      return *this;
    }

    bitset<N>& flip(size_t pos) __ntl_throws(out_of_range)
    {
      check_bounds(pos);
      storage_type val = storage_[pos / element_size_];
      const size_t mod = pos & element_mod_;
      val ^= (native_one_ << mod);
      storage_[pos / element_size_] = val;
      return *this;
    }

    constexpr bool operator[](size_t pos) const __ntl_nothrow
    { return test(pos); }

    reference operator[](size_t pos) __ntl_nothrow
    {
      return reference(*this, pos);
    }

    unsigned long to_ulong() const __ntl_throws (overflow_error)
    {
      return to_T<unsigned long>();
    }

    unsigned long long to_ullong() const __ntl_throws (overflow_error)
    {
      return to_T<unsigned long long>();
    }

    template <class charT, class traits, class Allocator>
    basic_string<charT, traits, Allocator> to_string(charT zero = charT('0'), charT one = charT('1')) const
    {
      return to_stringT<charT, traits, Allocator>(zero, one);
    }
    template <class charT, class traits>
    basic_string<charT, traits, allocator<charT> > to_string(charT zero = charT('0'), charT one = charT('1')) const
    {
      return to_stringT<charT, traits, allocator<charT> >(zero, one);
    }
    template <class charT>
    basic_string<charT, char_traits<charT>, allocator<charT> > to_string(charT zero = charT('0'), charT one = charT('1')) const
    {
      return to_stringT<charT, char_traits<charT>, allocator<charT> >(zero, one);
    }
    basic_string<char, char_traits<char>, allocator<char> > to_string() const
    {
      return to_stringT<char, char_traits<char>, allocator<char> >('0', '1');
    }

    size_t count() const
    {
      static const char table[] = "\0\1\1\2\1\2\2\3\1\2\2\3\2\3\3\4";
      unsigned count_ = 0;
      for(storage_type j = storage_[elements_count_-1] & digits_mod_; j; j >>= 4)
        count_ += table[j & 0xF];
      for(ssize_t i = elements_count_-2; i >= 0; --i)
        for(storage_type j = storage_[i]; j; j >>= 4)
          count_ += table[j & 0xF];
      return count_;
    }

    constexpr size_t size() const { return N; }

    bool operator==(const bitset<N>& rhs) const
    {
      return !memcmp(storage_, rhs.storage_, elements_count_*sizeof(storage_type));
    }

    bool operator!=(const bitset<N>& rhs) const
    {
      return !(*this==rhs);
    }

    bool test(size_t pos) const __ntl_throws (out_of_range)
    {
      check_bounds(pos);
      const storage_type val = storage_[pos / element_size_];
      return (val & static_cast<storage_type>(1 << (pos & element_mod_)) ) != 0;
    }

    bool none() const { return !any(); }
    bool all()  const { return count() == size(); }
    bool any()  const
    {
      for(uint32_t i = 0; i < elements_count_; i++)
        if(storage_[i])
          return true;
      return false;
    }

    bitset<N> operator<<(size_t pos) const
    {
      return bitset<N>(*this) <<= pos;
    }

    bitset<N> operator>>(size_t pos) const
    {
      return bitset<N>(*this) >>= pos;
    }

  private:
    void check_bounds(const size_t pos) const __ntl_throws (out_of_range)
    {
      if(pos >= N)
        __ntl_throw(out_of_range(__FUNCTION__));
    }

    template<typename T>
    T to_T() const __ntl_throws (overflow_error)
    {
      if(sizeof(T)*8 < N){
        // check that all upper bits are zero
        for(unsigned pos = elements_count_-1; pos >= sizeof(T) / sizeof(storage_type); --pos){
          if(storage_[pos] != 0)
            __ntl_throw(overflow_error("bitset cannot be represented as specified type"));
        }
      }
      T val = 0;
      for(unsigned pos = 0; pos < min(sizeof(T) / sizeof(storage_type), elements_count_); ++pos)
        val |= (T)storage_[pos] << (pos*element_size_);
      return val;
    }

    template <class charT, class traits, class Allocator>
    basic_string<charT, traits, Allocator> to_stringT(charT zero, charT one) const
    {
      basic_string<charT, traits, Allocator> str;
      str.resize(N);
      for(unsigned word = 0; word < elements_count_; ++word){
        const unsigned rpos = N - word*element_size_ - 1;
        const storage_type xval = storage_[word];
        const size_t count = min((size_t)rpos+1, celement_size_);
        for(unsigned bit = 0; bit < count; ++bit){
          str[rpos-bit] = static_cast<charT>(((xval & (native_one_ << bit)) != 0) ? one : zero);
        }
      }
      return str;
    }

    void large_shift_left(size_t pos)
    {
      const size_t shift = pos % element_size_;
      const unsigned lookup = static_cast<unsigned>(pos / element_size_);
      if(shift == 0){
        // shift by words, fastest pass
        for(unsigned i = elements_count_-1; i >= lookup; --i)
          storage_[i] = storage_[i-lookup];
        for(int i = lookup-1; i >= 0; --i)
          storage_[i] = 0;
      }else{
        // shift by bits
        for(unsigned i = elements_count_-1; i > lookup; --i){
          storage_[i]  = storage_[i-lookup] << shift;
          storage_[i] |= storage_[i-lookup-1] >> (element_size_-shift);
        }
        storage_[lookup] = storage_[0] << shift;
        // clear remaining bits
        for(int i = lookup-1; i >= 0; --i)
          storage_[i] = 0;
      }
      // cut garbage bits
      storage_[elements_count_-1] &= digits_mod_;
    }

    void large_shift_right(size_t pos)
    {
      const size_t shift = pos % element_size_;
      const unsigned lookup = static_cast<unsigned>(pos / element_size_);
      if(shift == 0){
        // shift by words, fastest pass
        for(unsigned i = 0; i < elements_count_-lookup; ++i)
          storage_[i] = storage_[i+lookup];
        for(unsigned i = elements_count_-lookup; i < elements_count_; ++i)
          storage_[i] = 0;
      }else{
        // shift by bits
        for(unsigned i = 0; i < elements_count_-lookup-1; ++i){
          storage_[i]  = storage_[i+lookup] >> shift;
          storage_[i] |= storage_[i+lookup+1] << (element_size_-shift);
        }
        storage_[elements_count_-lookup-1] = storage_[elements_count_-1] >> shift;
        // clear remaining bits
        for(unsigned i = elements_count_-lookup; i < elements_count_; ++i)
          storage_[i] = 0;
      }
    }

  private:
    typedef uintptr_t storage_type; // native platform type

    enum { digits = N };
    enum { element_size_ = sizeof(storage_type) * 8 }; // bits count

    static const size_t native_one_ = 1;
    static const storage_type set_bits_ = static_cast<storage_type>(-1);
    static const size_t element_mod_ = element_size_ - 1;
    static const size_t digits_mod_val_ = static_cast<size_t>(native_one_ << (N % element_size_)) - 1;//static_cast<size_t>(1 << (N & (element_size_-1))) - 1;
    static const size_t digits_mod_ = digits_mod_val_ ? digits_mod_val_ : set_bits_;
    static const size_t elements_count_ = N / element_size_ + ((N & (element_size_-1)) ? 1 : 0);
    static const size_t celement_size_ = sizeof(storage_type) * 8;

    template<size_t size, bool large> struct tidy
    {
      static const uint64_t make_tidy()
      {
        return (1 << size) - 1;
      }
    };
    template<size_t size> struct tidy<size, true>
    {
      static const uint64_t make_tidy()
      {
        uint64_t value = 1;
        value <<= size;
        return --value;
      }
    };

    storage_type storage_[elements_count_];
  };


  // 20.5.3 bitset operators:
  template <size_t N>
  bitset<N> operator&(const bitset<N>& lhs, const bitset<N>& rhs)
  {
    return lhs &= rhs;
  }

  template <size_t N>
  bitset<N> operator|(const bitset<N>& lhs, const bitset<N>& rhs)
  {
    return lhs |= rhs;
  }

  template <size_t N>
  bitset<N> operator^(const bitset<N>& lhs, const bitset<N>& rhs)
  {
    return lhs ^= rhs;
  }

  template <class charT, class traits, size_t N>
  inline basic_istream<charT, traits>& operator>>(basic_istream<charT, traits>& is, bitset<N>& x)
  {
    basic_string<charT,traits> str;
    str.reserve(N);
    charT valid[2] = {is.widen('0'), is.widen('1')};
    while(str.size() < N){
      charT ch;
      is >> ch;
      if(!is || !(ch == valid[0] || ch == valid[1]))
        break;
      str.push_back(ch);
    }
    x = bitset<N>(str);
    return is;
  }

  template <class charT, class traits, size_t N>
  inline basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const bitset<N>& x)
  {
    const ctype<charT>& ct = use_facet< ctype<charT> >(os.getloc());
    return os << x.template to_string<charT,traits,allocator<char> >(ct.widen('0'),ct.widen('1'));
  }

  /**@} lib_bitset */
  /**@} lib_utilities */
} // namespace std
#endif // NTL__STLX_BITSET
