/**\file*********************************************************************
 *                                                                     \brief
 *  URI Library for C++
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_TR2_NETWORK_URI
#define NTL__STLX_TR2_NETWORK_URI
#pragma once

#include <string>        // std::basic_string
#include <system_error>  // std::error_code
#include <iosfwd>        // std::istream, std::ostream, std::wistream, std::wostream
#include <iterator>      // std::iterator_traits
#include <exception>
#include <string_ref>

namespace std { namespace tr2 { namespace network {

  ///\name class declarations
  class uri;
  class uri_syntax_error;
  class uri_builder;

  ///\name factory functions
  template <class String>
  uri make_uri(const String &u, std::error_code &e) noexcept;

  ///\name swap functions
  void swap(uri &lhs, uri &rhs);

  ///\name hash functions
  std::size_t hash_value(const uri &u);

  ///\name equality and comparison operators
  enum uri_comparison_level {  };
  bool compare(const uri &lhs, const uri &rhs, uri_comparison_level level);
  bool operator == (const uri &lhs, const uri &rhs);
  bool operator != (const uri &lhs, const uri &rhs);
  bool operator <  (const uri &lhs, const uri &rhs);
  bool operator <= (const uri &lhs, const uri &rhs);
  bool operator >  (const uri &lhs, const uri &rhs);
  bool operator >= (const uri &lhs, const uri &rhs);

  ///\name percent encoding and decoding
  template <typename InputIterator, typename OutputIterator>
  OutputIterator encode_user_info(InputIterator first, InputIterator last, OutputIterator out);
  template <typename InputIterator, typename OutputIterator>
  OutputIterator encode_host(InputIterator first, InputIterator last, OutputIterator out);
  template <typename InputIterator, typename OutputIterator>
  OutputIterator encode_port(InputIterator first, InputIterator last, OutputIterator out);
  template <typename InputIterator, typename OutputIterator>
  OutputIterator encode_path(InputIterator first, InputIterator last, OutputIterator out);
  template <typename InputIterator, typename OutputIterator>
  OutputIterator encode_query(InputIterator first, InputIterator last, OutputIterator out);
  template <typename InputIterator, typename OutputIterator>
  OutputIterator encode_fragment(InputIterator first, InputIterator last, OutputIterator out);
  template <typename InputIterator, typename OutputIterator>
  OutputIterator decode(InputIterator first, InputIterator last, OutputIterator out);

  template <class String>
  String encode_user_info(const String &user_info);
  template <class String>
  String encode_host(const String &host);
  template <class String>
  String encode_port(const String &port);
  template <class String>
  String encode_path(const String &path);
  template <class String>
  String encode_query(const String &query);
  template <class String>
  String encode_fragment(const String &fragment);
  template <class String>
  String decode(const String &source);

  ///\name stream operators
  std::ostream &operator <<  (std::ostream &os, const uri &u);
  std::wostream &operator << (std::wostream &os, const uri &u);
  std::istream &operator >>  (std::istream &os, uri &u);
  std::wistream &operator >> (std::wistream &os, uri &u);
  ///\}

  //////////////////////////////////////////////////////////////////////////
  /**
   *	@brief 
   **/
  class uri_syntax_error: public std::runtime_error
  {
  public:
    // constructors and destructor
    uri_syntax_error(std::error_code e);
    virtual ~uri_syntax_error();

    // accessors
    std::error_code error_code() const noexcept;
    virtual const char *what() const noexcept;
  };




  /**
   *	@brief 
   **/
  class uri
  {
    std::wstring str_;
  public:
    ///\name typedefs
    typedef wstring string_type;
    typedef string_type::const_iterator iterator;
    typedef string_type::const_iterator const_iterator;
    typedef std::iterator_traits<iterator>::value_type value_type;

    ///\name range types
    class part_range;

    ///\name constructors and destructor
    uri()
    {}

    template <typename InputIterator>
    uri(const InputIterator &first, const InputIterator &last)
    {
      assign(first, last);
    }

    template <class Source>
    explicit uri(const Source &source)
    {
      assign(source);
    }

    uri(const uri &other)
      :str_(other.str_)
    {}

    uri(uri &&other) noexcept
    {
      swap(other);
    }

    ~uri()
    {}

    ///\name assignment
    uri &operator = (const uri &other)
    {
      str_ = other.str_; return *this;
    }

    uri &operator = (uri &&other)
    {
      str_.clear();
      swap(other);
      return *this;
    }

    template <typename InputIterator>
    void assign(const InputIterator &first, const InputIterator &last);

    template <class Source>
    void assign(const Source &source);

    ///\name swap
    void swap(uri &other) noexcept
    {
      str_.swap(other.str_);
    }

    ///\name iterators
    const_iterator begin() const;
    const_iterator end() const;

    ///\name accessors
    part_range scheme() const;
    part_range user_info() const;
    part_range host() const;
    part_range port() const;
    part_range path() const;
    part_range authority() const;
    part_range query() const;
    part_range fragment() const;

    ///\name query
    bool empty() const noexcept { return str_.empty(); }
    bool absolute() const noexcept;
    bool opaque() const noexcept;

    ///\name transformers
    uri normalize() const;
    uri relativize(const uri &other) const;
    uri resolve(const uri &other) const;

    template <class String>
    uri resolve(const String &other) const;

    ///\name string accessors
    string string() const;
    wstring wstring() const;
    u16string u16string() const;
    u32string u32string() const;
    ///\}
  };


  /**
   *	@brief 
   **/
  class uri::part_range
  {
  public:
    typedef uri::iterator iterator;
    typedef uri::const_iterator const_iterator;

    const_iterator begin() const;
    const_iterator end() const;

    // string accessors
    uri::string_type native() const;
    std::string string() const;
    std::wstring wstring() const;
    std::u16string u16string() const;
    std::u32string u32string() const;
  };

  /**
   *	@brief 
   **/
  class uri_builder
  {
    uri_builder(const uri_builder &) __deleted;
    uri_builder &operator= (const uri_builder &) __deleted;
  public:

    uri_builder();
    explicit uri_builder(const uri &base);
    template <typename Source>
    explicit uri_builder(const Source &base);
    ~uri_builder();

    template <typename Source>
    uri_builder &scheme(const Source &scheme);

    template <typename Source>
    uri_builder &user_info(const Source &user_info);

    template <typename Source>
    uri_builder &host(const Source &host);

    template <typename Source>
    uri_builder &port(const Source &port);

    template <typename Source>
    uri_builder &authority(const Source &authority);

    template <typename Source>
    uri_builder &authority(const Source &user_info, const Source &host, const Source &port);

    template <typename Source>
    uri_builder &path(const Source &path);

    template <typename Source>
    uri_builder &append_path(const Source &path);

    template <typename Source>
    uri_builder &query(const Source &query);

    template <class Key, class Param>
    uri_builder &query(const Key &key, const Param &param);

    template <typename Source>
    uri_builder &fragment(const Source &fragment);

    uri uri() const;

  };

}}}
#endif // NTL__STLX_TR2_NETWORK_URI
