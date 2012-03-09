/**\file*********************************************************************
 *                                                                     \brief
 *  28 Regular expressions library [re]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_REGEX
#define NTL__STLX_REGEX
#pragma once

#include "stdstring.hxx"
#include "iosfwd.hxx"
#include "stdexcept.hxx"
#include "locale.hxx"

namespace std
{

  // 28.5, regex constants:
  namespace regex_constants {
    typedef unsigned syntax_option_type;
    static const syntax_option_type icase;
    static const syntax_option_type nosubs;
    static const syntax_option_type optimize;
    static const syntax_option_type collate;
    static const syntax_option_type ECMAScript;
    static const syntax_option_type basic;
    static const syntax_option_type extended;
    static const syntax_option_type awk;
    static const syntax_option_type grep;
    static const syntax_option_type egrep;

    typedef unsigned match_flag_type;
    static const match_flag_type match_default = 0;
    static const match_flag_type match_not_bol;
    static const match_flag_type match_not_eol;
    static const match_flag_type match_not_bow;
    static const match_flag_type match_not_eow;
    static const match_flag_type match_any;
    static const match_flag_type match_not_null;
    static const match_flag_type match_continuous;
    static const match_flag_type match_prev_avail;
    static const match_flag_type format_default = 0;
    static const match_flag_type format_sed;
    static const match_flag_type format_no_copy;
    static const match_flag_type format_first_only;

    enum error_type 
    {
      error_collate,
      error_ctype,
      error_escape,
      error_backref,
      error_brack,
      error_paren,
      error_brace,
      error_badbrace,
      error_range,
      error_space,
      error_badrepeat,
      error_complexity,
      error_stack
    };
  } // namespace regex_constants

  // 28.6, class regex_error:
  class regex_error: public std::runtime_error 
  {
  public:
    explicit regex_error(regex_constants::error_type ecode);
    regex_constants::error_type code() const;
  };

  // 28.7, class template regex_traits:
  template <class charT>
  struct regex_traits
  {
    typedef charT char_type;
    typedef std::basic_string<char_type> string_type;
    typedef std::locale locale_type;
    typedef bitmask_type char_class_type;

    regex_traits();

    static std::size_t length(const char_type* p);
    charT translate(charT c) const;
    charT translate_nocase(charT c) const;
    
    template <class ForwardIterator>
    string_type transform(ForwardIterator first, ForwardIterator last) const;
    template <class ForwardIterator>
    string_type transform_primary(ForwardIterator first, ForwardIterator last) const;
    template <class ForwardIterator>
    string_type lookup_collatename(ForwardIterator first, ForwardIterator last) const;

    template <class ForwardIterator>
    char_class_type lookup_classname(ForwardIterator first, ForwardIterator last, bool icase = false) const;

    bool isctype(charT c, char_class_type f) const;
    int value(charT ch, int radix) const;
    locale_type imbue(locale_type l);
    locale_type getloc()const;
  };

  // 28.8, class template basic_regex:
  template <class charT, class traits = regex_traits<charT> >
  class basic_regex
  {
  public:
    // types:
    typedef charT value_type;
    typedef regex_constants::syntax_option_type flag_type;
    typedef typename traits::locale_type locale_type;

    // 28.8.1, constants:
    static const regex_constants::syntax_option_type icase      = regex_constants::icase;
    static const regex_constants::syntax_option_type nosubs     = regex_constants::nosubs;
    static const regex_constants::syntax_option_type optimize   = regex_constants::optimize;
    static const regex_constants::syntax_option_type collate    = regex_constants::collate;
    static const regex_constants::syntax_option_type ECMAScript = regex_constants::ECMAScript;
    static const regex_constants::syntax_option_type basic      = regex_constants::basic;
    static const regex_constants::syntax_option_type extended   = regex_constants::extended;
    static const regex_constants::syntax_option_type awk        = regex_constants::awk;
    static const regex_constants::syntax_option_type grep       = regex_constants::grep;
    static const regex_constants::syntax_option_type egrep      = regex_constants::egrep;

    // 28.8.2, construct/copy/destroy:
    basic_regex();

    explicit basic_regex(const charT* p, flag_type f = regex_constants::ECMAScript);
    basic_regex(const charT* p, size_t len, flag_type f);
    basic_regex(const basic_regex&);
    template <class ST, class SA>
    explicit basic_regex(const basic_string<charT, ST, SA>& p, flag_type f = regex_constants::ECMAScript);
    template <class ForwardIterator>
    basic_regex(ForwardIterator first, ForwardIterator last, flag_type f = regex_constants::ECMAScript);
    basic_regex(initializer_list<charT>, flag_type = regex_constants::ECMAScript);
    ~basic_regex();

    basic_regex& operator=(const basic_regex&);
    basic_regex& operator=(const charT* ptr);
    template <class ST, class SA>
    basic_regex& operator=(const basic_string<charT, ST, SA>& p);
    
    // 28.8.3, assign:
    basic_regex& assign(const basic_regex& that);
    basic_regex& assign(const charT* ptr, flag_type f = regex_constants::ECMAScript);
    basic_regex& assign(const charT* p, size_t len, flag_type f);
    template <class string_traits, class A>
    basic_regex& assign(const basic_string<charT, string_traits, A>& s, flag_type f = regex_constants::ECMAScript);
    template <class InputIterator>
    basic_regex& assign(InputIterator first, InputIterator last, flag_type f = regex_constants::ECMAScript);
    basic_regex& assign(initializer_list<charT>, flag_type = regex_constants::ECMAScript);

    // 28.8.4, const operations:
    unsigned mark_count() const;
    flag_type flags() const;
    
    // 28.8.5, locale:
    locale_type imbue(locale_type loc);
    locale_type getloc() const;
    
    // 28.8.6, swap:
    void swap(basic_regex&);
  };
  typedef basic_regex<char> regex;
  typedef basic_regex<wchar_t> wregex;

  // 28.8.6, basic_regex swap:
  template <class charT, class traits>
  void swap(basic_regex<charT, traits>& e1, basic_regex<charT, traits>& e2)
  {
    e1.swap(e2);
  }

  // 28.9, class template sub_match:
  template <class BidirectionalIterator>
  class sub_match: public std::pair<BidirectionalIterator, BidirectionalIterator> 
  {
  public:
    typedef typename iterator_traits<BidirectionalIterator>::value_type value_type;
    typedef typename iterator_traits<BidirectionalIterator>::difference_type difference_type;
    typedef BidirectionalIterator iterator;

    bool matched;

    difference_type length() const;
    operator basic_string<value_type>() const;
    basic_string<value_type> str() const;

    int compare(const sub_match& s) const;
    int compare(const basic_string<value_type>& s) const;
    int compare(const value_type* s) const;
  };
  typedef sub_match<const char*> csub_match;
  typedef sub_match<const wchar_t*> wcsub_match;
  typedef sub_match<string::const_iterator> ssub_match;
  typedef sub_match<wstring::const_iterator> wssub_match;

  // 28.9.2, sub_match non-member operators:
  template <class BiIter>
  bool operator==(const sub_match<BiIter>& lhs, const sub_match<BiIter>& rhs);
  template <class BiIter>
  bool operator!=(const sub_match<BiIter>& lhs, const sub_match<BiIter>& rhs);
  template <class BiIter>
  bool operator<(const sub_match<BiIter>& lhs, const sub_match<BiIter>& rhs);
  template <class BiIter>
  bool operator<=(const sub_match<BiIter>& lhs, const sub_match<BiIter>& rhs);
  template <class BiIter>
  bool operator>=(const sub_match<BiIter>& lhs, const sub_match<BiIter>& rhs);
  template <class BiIter>
  bool operator>(const sub_match<BiIter>& lhs, const sub_match<BiIter>& rhs);
  
  template <class BiIter, class ST, class SA>
  bool operator==(const basic_string<typename iterator_traits<BiIter>::value_type, ST, SA>& lhs, const sub_match<BiIter>& rhs);
  template <class BiIter, class ST, class SA>
  bool operator!=(const basic_string<typename iterator_traits<BiIter>::value_type, ST, SA>& lhs, const sub_match<BiIter>& rhs);
  template <class BiIter, class ST, class SA>
  bool operator<(const basic_string<typename iterator_traits<BiIter>::value_type, ST, SA>& lhs, const sub_match<BiIter>& rhs);
  template <class BiIter, class ST, class SA>
  bool operator>(const basic_string<typename iterator_traits<BiIter>::value_type, ST, SA>& lhs, const sub_match<BiIter>& rhs);
  template <class BiIter, class ST, class SA>
  bool operator>=(const basic_string<typename iterator_traits<BiIter>::value_type, ST, SA>& lhs, const sub_match<BiIter>& rhs);
  template <class BiIter, class ST, class SA>
  bool operator<=(const basic_string<typename iterator_traits<BiIter>::value_type, ST, SA>& lhs, const sub_match<BiIter>& rhs);
  template <class BiIter, class ST, class SA>
  bool operator==(const sub_match<BiIter>& lhs, const basic_string<typename iterator_traits<BiIter>::value_type, ST, SA>& rhs);
  template <class BiIter, class ST, class SA>
  bool operator!=(const sub_match<BiIter>& lhs, const basic_string<typename iterator_traits<BiIter>::value_type, ST, SA>& rhs);
  template <class BiIter, class ST, class SA>
  bool operator<(const sub_match<BiIter>& lhs, const basic_string<typename iterator_traits<BiIter>::value_type, ST, SA>& rhs);
  template <class BiIter, class ST, class SA>
  bool operator>(const sub_match<BiIter>& lhs, const basic_string<typename iterator_traits<BiIter>::value_type, ST, SA>& rhs);
  template <class BiIter, class ST, class SA>
  bool operator>=(const sub_match<BiIter>& lhs, const basic_string<typename iterator_traits<BiIter>::value_type, ST, SA>& rhs);
  template <class BiIter, class ST, class SA>
  bool operator<=(const sub_match<BiIter>& lhs, const basic_string<typename iterator_traits<BiIter>::value_type, ST, SA>& rhs);

  template <class BiIter>
  bool operator==(typename iterator_traits<BiIter>::value_type const* lhs, const sub_match<BiIter>& rhs);
  template <class BiIter>
  bool operator!=(typename iterator_traits<BiIter>::value_type const* lhs, const sub_match<BiIter>& rhs);
  template <class BiIter>
  bool operator<(typename iterator_traits<BiIter>::value_type const* lhs, const sub_match<BiIter>& rhs);
  template <class BiIter>
  bool operator>(typename iterator_traits<BiIter>::value_type const* lhs, const sub_match<BiIter>& rhs);
  template <class BiIter>
  bool operator>=(typename iterator_traits<BiIter>::value_type const* lhs, const sub_match<BiIter>& rhs);
  template <class BiIter>
  bool operator<=(typename iterator_traits<BiIter>::value_type const* lhs, const sub_match<BiIter>& rhs);
  template <class BiIter>
  bool operator==(const sub_match<BiIter>& lhs, typename iterator_traits<BiIter>::value_type const* rhs);
  template <class BiIter>
  bool operator!=(const sub_match<BiIter>& lhs, typename iterator_traits<BiIter>::value_type const* rhs);
  template <class BiIter>
  bool operator<(const sub_match<BiIter>& lhs, typename iterator_traits<BiIter>::value_type const* rhs);
  template <class BiIter>
  bool operator>(const sub_match<BiIter>& lhs, typename iterator_traits<BiIter>::value_type const* rhs);
  template <class BiIter>
  bool operator>=(const sub_match<BiIter>& lhs, typename iterator_traits<BiIter>::value_type const* rhs);
  template <class BiIter>
  bool operator<=(const sub_match<BiIter>& lhs, typename iterator_traits<BiIter>::value_type const* rhs);
  template <class BiIter>
  bool operator==(typename iterator_traits<BiIter>::value_type const& lhs, const sub_match<BiIter>& rhs);
  template <class BiIter>
  bool operator!=(typename iterator_traits<BiIter>::value_type const& lhs, const sub_match<BiIter>& rhs);
  template <class BiIter>
  bool operator<(typename iterator_traits<BiIter>::value_type const& lhs, const sub_match<BiIter>& rhs);
  template <class BiIter>
  bool operator>(typename iterator_traits<BiIter>::value_type const& lhs, const sub_match<BiIter>& rhs);
  template <class BiIter>
  bool operator>=(typename iterator_traits<BiIter>::value_type const& lhs, const sub_match<BiIter>& rhs);
  template <class BiIter>
  bool operator<=(typename iterator_traits<BiIter>::value_type const& lhs, const sub_match<BiIter>& rhs);
  template <class BiIter>
  bool operator==(const sub_match<BiIter>& lhs, typename iterator_traits<BiIter>::value_type const& rhs);
  template <class BiIter>
  bool operator!=(const sub_match<BiIter>& lhs, typename iterator_traits<BiIter>::value_type const& rhs);
  template <class BiIter>
  bool operator<(const sub_match<BiIter>& lhs, typename iterator_traits<BiIter>::value_type const& rhs);
  template <class BiIter>
  bool operator>(const sub_match<BiIter>& lhs, typename iterator_traits<BiIter>::value_type const& rhs);
  template <class BiIter>
  bool operator>=(const sub_match<BiIter>& lhs, typename iterator_traits<BiIter>::value_type const& rhs);
  template <class BiIter>
  bool operator<=(const sub_match<BiIter>& lhs, typename iterator_traits<BiIter>::value_type const& rhs);
  template <class charT, class ST, class BiIter>
  basic_ostream<charT, ST>& operator<<(basic_ostream<charT, ST>& os, const sub_match<BiIter>& m);


  // 28.10, class template match_results:
  template <class BidirectionalIterator, class Allocator = allocator<sub_match<BidirectionalIterator> > >
  class match_results
  {
  public:
    typedef sub_match<BidirectionalIterator> value_type;
    typedef typename Allocator::const_reference const_reference;
    typedef const_reference reference;
    typedef implementation defined const_iterator;
    typedef const_iterator iterator;
    typedef typename iterator_traits<BidirectionalIterator>::difference_type difference_type;
    typedef typename Allocator::size_type size_type;
    typedef Allocator allocator_type;
    typedef typename iterator_traits<BidirectionalIterator>::value_type char_type;
    typedef basic_string<char_type> string_type;

    // 28.10.1, construct/copy/destroy:
    explicit match_results(const Allocator& a = Allocator());
    match_results(const match_results& m);
    match_results& operator=(const match_results& m);
    ~match_results();
    
    // 28.10.2, size:
    size_type size() const;
    size_type max_size() const;
    bool empty() const;
    
    // 28.10.3 element access:
    difference_type length(size_type sub = 0) const;
    difference_type position(size_type sub = 0) const;

    string_type str(size_type sub = 0) const;
    const_reference operator[](size_type n) const;

    const_reference prefix() const;
    const_reference suffix() const;

    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    
    // 28.10.4, format:
    template <class OutputIter>
    OutputIter format(OutputIter out, const string_type& fmt, regex_constants::match_flag_type flags = regex_constants::format_default) const;
    string_type format(const string_type& fmt, regex_constants::match_flag_type flags = regex_constants::format_default) const;

    // 28.10.5, allocator:
    allocator_type get_allocator() const;

    // 28.10.6, swap:
    void swap(match_results& that);
  };
  typedef match_results<const char*> cmatch;
  typedef match_results<const wchar_t*> wcmatch;
  typedef match_results<string::const_iterator> smatch;
  typedef match_results<wstring::const_iterator> wsmatch;
  
  // match_results comparisons
  template <class BidirectionalIterator, class Allocator>
  bool operator== (const match_results<BidirectionalIterator, Allocator>& m1, const match_results<BidirectionalIterator, Allocator>& m2);
  template <class BidirectionalIterator, class Allocator>
  bool operator!= (const match_results<BidirectionalIterator, Allocator>& m1, const match_results<BidirectionalIterator, Allocator>& m2);
  
  // 28.10.6, match_results swap:
  template <class BidirectionalIterator, class Allocator>
  void swap(match_results<BidirectionalIterator, Allocator>& m1, match_results<BidirectionalIterator, Allocator>& m2)
  {
    m1.swap(m2);
  }

  // 28.11.2, function template regex_match:
  template <class BidirectionalIterator, class Allocator, class charT, class traits>
    bool regex_match(BidirectionalIterator first, BidirectionalIterator last,
                    match_results<BidirectionalIterator, Allocator>& m,
                    const basic_regex<charT, traits>& e,
                    regex_constants::match_flag_type flags =
                    regex_constants::match_default);
  template <class BidirectionalIterator, class charT, class traits>
  bool regex_match(BidirectionalIterator first, BidirectionalIterator last,
                  const basic_regex<charT, traits>& e,
                  regex_constants::match_flag_type flags =
                  regex_constants::match_default);
  template <class charT, class Allocator, class traits>
  bool regex_match(const charT* str, match_results<const charT*, Allocator>& m,
                  const basic_regex<charT, traits>& e,
                  regex_constants::match_flag_type flags =
                  regex_constants::match_default);
  template <class ST, class SA, class Allocator, class charT, class traits>
  bool regex_match(const basic_string<charT, ST, SA>& s,
                  match_results< typename basic_string<charT, ST, SA>::const_iterator, Allocator>& m,
                  const basic_regex<charT, traits>& e,
                  regex_constants::match_flag_type flags =
                  regex_constants::match_default);
  template <class charT, class traits>
  bool regex_match(const charT* str,
                  const basic_regex<charT, traits>& e,
                  regex_constants::match_flag_type flags =
                  regex_constants::match_default);
  template <class ST, class SA, class charT, class traits>
  bool regex_match(const basic_string<charT, ST, SA>& s,
                  const basic_regex<charT, traits>& e,
                  regex_constants::match_flag_type flags =
                  regex_constants::match_default);


  // 28.11.3, function template regex_search:
  template <class BidirectionalIterator, class Allocator, class charT, class traits>
  bool regex_search(BidirectionalIterator first, BidirectionalIterator last,
                    match_results<BidirectionalIterator, Allocator>& m,
                    const basic_regex<charT, traits>& e,
                    regex_constants::match_flag_type flags =
                    regex_constants::match_default);
  template <class BidirectionalIterator, class charT, class traits>
  bool regex_search(BidirectionalIterator first, BidirectionalIterator last,
                    const basic_regex<charT, traits>& e,
                    regex_constants::match_flag_type flags =
                    regex_constants::match_default);
  template <class charT, class Allocator, class traits>
  bool regex_search(const charT* str,
                    match_results<const charT*, Allocator>& m,
                    const basic_regex<charT, traits>& e,
                    regex_constants::match_flag_type flags =
                    regex_constants::match_default);
  template <class charT, class traits>
  bool regex_search(const charT* str,
                    const basic_regex<charT, traits>& e,
                    regex_constants::match_flag_type flags =
                    regex_constants::match_default);
  template <class ST, class SA, class charT, class traits>
  bool regex_search(const basic_string<charT, ST, SA>& s,
                    const basic_regex<charT, traits>& e,
                    regex_constants::match_flag_type flags =
                    regex_constants::match_default);
  template <class ST, class SA, class Allocator, class charT, class traits>
  bool regex_search(const basic_string<charT, ST, SA>& s,
                    match_results<typename basic_string<charT, ST, SA>::const_iterator, Allocator>& m,
                    const basic_regex<charT, traits>& e,
                    regex_constants::match_flag_type flags =
                    regex_constants::match_default);


  // 28.11.4, function template regex_replace:
  template <class OutputIterator, class BidirectionalIterator,class traits, class charT>
    OutputIterator regex_replace(OutputIterator out,
                                BidirectionalIterator first, BidirectionalIterator last,
                                const basic_regex<charT, traits>& e,
                                const basic_string<charT>& fmt,
                                regex_constants::match_flag_type flags =
                                regex_constants::match_default);
  template <class traits, class charT>
  basic_string<charT> regex_replace(const basic_string<charT>& s,
                                const basic_regex<charT, traits>& e,
                                const basic_string<charT>& fmt,
                                regex_constants::match_flag_type flags =
                                regex_constants::match_default);


  // 28.12.1, class template regex_iterator:
  template <class BidirectionalIterator, class charT = typename iterator_traits<BidirectionalIterator>::value_type, class traits = regex_traits<charT> >
  class regex_iterator
  {
  public:
    typedef basic_regex<charT, traits> regex_type;
    typedef match_results<BidirectionalIterator> value_type;
    typedef std::ptrdiff_t difference_type;
    typedef const value_type* pointer;
    typedef const value_type& reference;
    typedef std::forward_iterator_tag iterator_category;

    regex_iterator();
    regex_iterator(BidirectionalIterator a, BidirectionalIterator b, const regex_type& re,
                  regex_constants::match_flag_type m = regex_constants::match_default);
    regex_iterator(const regex_iterator&);

    regex_iterator& operator=(const regex_iterator&);
    bool operator==(const regex_iterator&) const;
    bool operator!=(const regex_iterator&) const;
    const value_type& operator*() const;
    const value_type* operator->() const;
    regex_iterator& operator++();
    regex_iterator operator++(int);
  private:
    // these members are shown for exposition only:
    BidirectionalIterator begin;
    BidirectionalIterator end;
    const regex_type* pregex;
    regex_constants::match_flag_type flags;
    match_results<BidirectionalIterator> match;
  };
  typedef regex_iterator<const char*> cregex_iterator;
  typedef regex_iterator<const wchar_t*> wcregex_iterator;
  typedef regex_iterator<string::const_iterator> sregex_iterator;
  typedef regex_iterator<wstring::const_iterator> wsregex_iterator;
  
  // 28.12.2, class template regex_token_iterator:
  template <class BidirectionalIterator, class charT = typename iterator_traits<BidirectionalIterator>::value_type, class traits = regex_traits<charT> >
  class regex_token_iterator
  {
  public:
    typedef basic_regex<charT, traits> regex_type;
    typedef sub_match<BidirectionalIterator> value_type;
    typedef std::ptrdiff_t difference_type;
    typedef const value_type* pointer;
    typedef const value_type& reference;
    typedef std::forward_iterator_tag iterator_category;

    regex_token_iterator();
    regex_token_iterator(BidirectionalIterator a, BidirectionalIterator b, const regex_type& re, int submatch = 0,regex_constants::match_flag_type m = regex_constants::match_default);
    regex_token_iterator(BidirectionalIterator a, BidirectionalIterator b, const regex_type& re, const std::vector<int>& submatches, regex_constants::match_flag_type m = regex_constants::match_default);
    template <std::size_t N>
    regex_token_iterator(BidirectionalIterator a, BidirectionalIterator b, const regex_type& re, const int (&submatches)[N], regex_constants::match_flag_type m = regex_constants::match_default);
    regex_token_iterator(const regex_token_iterator&);
    regex_token_iterator& operator=(const regex_token_iterator&);

    bool operator==(const regex_token_iterator&) const;
    bool operator!=(const regex_token_iterator&) const;
    const value_type& operator*() const;
    const value_type* operator->() const;
    regex_token_iterator& operator++();
    regex_token_iterator operator++(int);
  private: // data members for exposition only:
    typedef regex_iterator<BidirectionalIterator, charT, traits> position_iterator;
    position_iterator position;
    const value_type *result;
    value_type suffix;
    std::size_t N;
    std::vector<int> subs;
  };
  typedef regex_token_iterator<const char*> cregex_token_iterator;
  typedef regex_token_iterator<const wchar_t*> wcregex_token_iterator;
  typedef regex_token_iterator<string::const_iterator> sregex_token_iterator;
  typedef regex_token_iterator<wstring::const_iterator> wsregex_token_iterator;

} // std

#endif // NTL__STLX_REGEX
