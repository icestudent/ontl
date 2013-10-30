/**\file*********************************************************************
 *                                                                     \brief
 *  URI Library for C++ [N3792]
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
	class uri_builder;
	class uri_syntax_error;
	class base_uri_error;
	class uri_builder_error;
	class percent_decoding_error;

	enum class uri_error {
		// uri syntax errors
		invalid_syntax = 1,

		// uri reference and resolution errors
		base_uri_is_empty,
		base_uri_is_not_absolute,
		base_uri_is_opaque,
		base_uri_does_not_match,

		// builder errors
		invalid_uri,
		invalid_scheme,
		invalid_user_info,
		invalid_host,
		invalid_port,
		invalid_path,
		invalid_query,
		invalid_fragment,

		// decoding errors
		not_enough_input,
		non_hex_input,
		conversion_failed,
	};

	template <>
	struct is_error_code_enum<uri_error>:
	true_type {};

	enum class uri_normalization_level {
		string_comparison,
		syntax_based,
	};

  ///\name factory functions
	template <class Source>
	uri make_uri(const Source& source, std::error_code& ec);

	template <class InputIterator>
	uri make_uri(InputIterator first, InputIterator last, std::error_code& ec);
	
	template <class Source, class Allocator>
	uri make_uri(const Source& source, const Allocator& alloc, std::error_code& ec);
	
	template <class InputIterator, class Allocator>
	uri make_uri(InputIterator first, InputIterator last, const Allocator& alloc, std::error_code& ec);

  ///\name swap functions
  void swap(uri& lhs, uri& rhs);

  ///\name hash functions
  std::size_t hash_value(const uri &u);

  ///\name equality and comparison operators
  bool operator == (const uri& lhs, const uri& rhs);
  bool operator != (const uri& lhs, const uri& rhs);
  bool operator <  (const uri& lhs, const uri& rhs);
  bool operator <= (const uri& lhs, const uri& rhs);
  bool operator >  (const uri& lhs, const uri& rhs);
  bool operator >= (const uri& lhs, const uri& rhs);

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
  String encode_user_info(const String& user_info);
  template <class String>
  String encode_host(const String& host);
  template <class String>
  String encode_port(const String& port);
  template <class String>
  String encode_path(const String& path);
  template <class String>
  String encode_query(const String& query);
  template <class String>
  String encode_fragment(const String& fragment);
  template <class String>
  String decode(const String& source);

  ///\name stream operators
	template <typename CharT, class CharTraits = std::char_traits<CharT> >
	std::basic_ostream<CharT, CharTraits>& operator<< (std::basic_ostream<CharT, CharTraits>& os, const uri& u);
	template <typename CharT, class CharTraits = std::char_traits<CharT> >
	std::basic_istream<CharT, CharTraits>& operator>> (std::basic_istream<CharT, CharTraits>& is, uri& u);
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
		std::wstring str;
  public:
		///\name typedefs
		typedef std::wstring string_type;
		typedef typename string_type::iterator iterator;
		typedef typename string_type::const_iterator const_iterator;
		typedef std::iterator_traits<iterator>::value_type value_type;
		typedef basic_string_ref<value_type> string_view;

		///\name constructors and destructor
		uri()
		{}

		template <class Source, class Allocator = std::allocator<value_type> >
		explicit uri(const Source& source, const Allocator& alloc = Allocator());

		template <typename InputIterator, class Allocator = std::allocator<value_type> >
		uri(InputIterator begin, InputIterator end, const Allocator& alloc = Allocator());

		uri(const uri& other);

		uri(uri&& other) noexcept;

		~uri() noexcept;

		///\name assignment
		uri& operator= (const uri& other);
		uri& operator= (uri&& other) noexcept;

		///\name modifiers
		void swap(uri& other) noexcept;

		///\name iterators
		const_iterator begin() const;
		const_iterator end() const;
		const_iterator cbegin() const;
		const_iterator cend() const;

		///\name accessors
		std::optional<string_view> scheme() const;
		std::optional<string_view> user_info() const;
		std::optional<string_view> host() const;
		std::optional<string_view> port() const;
		template <typename IntT>
		std::optional<IntT> port(typename std::is_integral<IntT>::type* = 0) const;
		std::optional<string_view> path() const;
		std::optional<string_view> authority() const;
		std::optional<string_view> query() const;
		std::optional<string_view> fragment() const;

		///\name string accessors
		template <typename CharT,
		class CharTraits = std::char_traits<CharT>,
		class Allocator = std::allocator<CharT>>
			std::basic_string<CharT, CharTraits, Allocator>
			to_string(const Allocator& alloc = Allocator()) const;
		std::string string() const;
		std::wstring wstring() const;
		std::string u8string() const;
		std::u16string u16string() const;
		std::u32string u32string() const;

		///\name query
		bool empty() const noexcept;
		bool is_absolute() const;
		bool is_opaque() const;

		///\name transformers
		uri normalize(uri_normalization_level level) const;
		template <class Allocator>
		uri normalize(uri_normalization_level level, const Allocator& alloc) const;
		uri normalize(uri_normalization_level level, std::error_code& ec) const;
		template <class Allocator>
		uri normalize(uri_normalization_level level, const Allocator& alloc,
			std::error_code& ec) const;

		uri make_relative(const uri& base) const;
		template <class Allocator>
		uri make_relative(const uri& base, const Allocator& alloc) const;
		uri make_relative(const uri& base, std::error_code& ec) const;
		template <class Allocator>
		uri make_relative(const uri& base, const Allocator& alloc,
			std::error_code& ec) const;

		uri resolve(const uri& base) const;
		template <class Allocator>
		uri resolve(const uri& base, const Allocator& alloc) const;
		uri resolve(const uri& base, std::error_code& ec) const;
		template <class Allocator>
		uri resolve(const uri& base, const Allocator& alloc,
			std::error_code& ec) const;

		///\name comparison
		int compare(const uri& other, uri_normalization_level level) const noexcept;

		///\name percent encoding and decoding
		template <typename InputIterator, typename OutputIterator>
		static OutputIterator encode_user_info(InputIterator begin, InputIterator end,
			OutputIterator out);
		template <typename InputIterator, typename OutputIterator>
		static OutputIterator encode_host(InputIterator begin, InputIterator end,
			OutputIterator out);
		template <typename InputIterator, typename OutputIterator>
		static OutputIterator encode_port(InputIterator begin, InputIterator end,
			OutputIterator out);
		template <typename InputIterator, typename OutputIterator>
		static OutputIterator encode_path(InputIterator begin, InputIterator end,
			OutputIterator out);
		template <typename InputIterator, typename OutputIterator>
		static OutputIterator encode_query(InputIterator begin, InputIterator end,
			OutputIterator out);
		template <typename InputIterator, typename OutputIterator>
		static OutputIterator encode_fragment(InputIterator begin, InputIterator end,
			OutputIterator out);
		template <typename InputIterator, typename OutputIterator>
		static OutputIterator decode(InputIterator begin, InputIterator end,
			OutputIterator out);
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

    ///\name string accessors
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
