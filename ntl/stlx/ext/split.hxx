/**\file*********************************************************************
 *                                                                     \brief
 *  String split
 *
 ****************************************************************************
 */
#ifndef NTL__EXT_SPLIT
#define NTL__EXT_SPLIT
#pragma once

#include "../string_ref.hxx"
#include "../forward_list.hxx"

namespace std
{
  ///\name Standard split predicates

  /** Skips empty substrings in the std::split() output collection. */
  struct skip_empty
  {
    bool operator()(const string_ref& sref) const
    {
      return !sref.empty();
    }
  };

  /**
   *	@brief A string delimiter.
   *	
   *	This is the default delimiter used if a string is given as the delimiter argument to \c std::split().
   *	Alternatively, this delimiter could be named differently, such as \c std::literal_delimiter.
   **/
  class literal
  {
    string_ref sref;
  public:
    explicit literal(const string_ref& sref)
      :sref(sref)
    {}

    string_ref find(const string_ref& text) const
    {
      if(text.empty())
        return string_ref();
      const string_ref::size_type pos = text.find(sref);
      return pos == text.npos ? string_ref() : string_ref(text, pos, sref.length());
    }
  };


  /**
   *	@brief Each character in the given string is a delimiter.
   *	
   *	This is different from the \c std::any_of algorithm [alg.any_of], but overload resolution should disambiguate this delimiter. 
   *	Alternatively, this delimiter could be named differently, such as \c std::any_of_delimiter.
   **/
  class split_any
  {
    string_ref sref;
  public:
    explicit split_any(const string_ref& sref)
      :sref(sref)
    {}

    string_ref find(const string_ref& text) const
    {
      if(text.empty())
        return string_ref();
      //else if(sref.empty())
      //  return string_ref(text, 0, 1);
      size_t pos = text.npos;
      for(string_ref::const_iterator t = text.begin(), te = text.end(), se = sref.end(); t != te; ++t){
        for(string_ref::const_iterator s = sref.begin(); s != se; ++s){
          if(*t == *s)
            return string_ref(t, 1);
        }
      }
      return string_ref();
    }
  };
  ///\}


  /**
   *	@brief Holds split results
   **/
  template <typename Delimiter>
  class splitter
  {
    typedef std::forward_list<string_ref> refs_type;
  public:
    typedef typename refs_type::const_iterator const_iterator;
    typedef typename refs_type::iterator       iterator;
    typedef typename refs_type::value_type     value_type;
    
  public:
    splitter(refs_type& inrefs)
    {
      refs.swap(inrefs);
    }

#ifdef NTL_CXX_RV
    splitter(splitter&& r)
      :refs(std::move(r.refs))
    {}
#endif

    const_iterator begin() const { return refs.begin(); }
    const_iterator end()   const { return refs.end();   }

    template <typename Container>
    operator Container()
    {
      return Container(begin(), end());
    }

  private:
    refs_type refs;
  };

  namespace __
  {
    template<class T>
    class split_is_delimiter
    {
      template<typename U, std::string_ref (U::*)(const std::string_ref&) const> struct SFINAE_C {};
      template<typename U, std::string_ref (U::*)(std::string_ref) const> struct SFINAE {};
      template<typename U> static sfinae_passed_tag check(SFINAE<U, &U::find>*);
      template<typename U> static sfinae_passed_tag check(SFINAE_C<U, &U::find>*);
      template<typename U> static sfinae_failed_tag check(...);
    public:
      static const bool value = sizeof(check<T>(0)) == sizeof(sfinae_passed_tag);
    };

    struct split_default_predicate
    {
      bool operator()(const string_ref& /*sref*/) const { return true; }
    };

    template <typename Delimiter, typename Predicate>
    inline typename enable_if<__::split_is_delimiter<Delimiter>::value,splitter<Delimiter> >::type split(const std::string_ref& text, Delimiter d, Predicate filter)
    {
      typedef std::forward_list<string_ref> sequence;
      sequence seq; sequence::iterator seq_pos = seq.before_begin();
      for(string_ref cur = text, pos = d.find(cur); !cur.empty(); pos = d.find(cur)){
        const bool notfound = pos.empty();
        // range [current] delim
        const string_ref::size_type len = notfound ? cur.length() : (pos.data()-cur.data());
        const string_ref sub(cur.data(), len);
        if(filter(sub))
          seq_pos = seq.insert_after(seq_pos, sub);
        if(notfound)
          break;
        // current = delim [rest]
        cur = string_ref(pos.end(), text.length() - (pos.end()-text.begin()));
        if(cur.empty() && pos.end() == text.end()){
          // if delimiter at end of string, treat end as additional empty result
          const string_ref sub(pos.end());
          if(filter(sub))
            seq_pos = seq.insert_after(seq_pos, sub);
        }
      }
      splitter<Delimiter> result(seq);
      return std::move(result);
    }

  }


  /** The function called to split an input string into a collection of substrings. */
  template <typename Delimiter, typename Predicate>
  inline splitter<Delimiter> split(const std::string_ref& text, Delimiter d, Predicate filter)
  {
    static_assert(__::split_is_delimiter<Delimiter>::value, "string_ref Delimiter::find(string_ref) is not found in <Delimiter>");
    return __::split(text, d, filter);
  }

  template <typename Delimiter>
  inline splitter<Delimiter> split(const std::string_ref& text, Delimiter d)
  {
    return std::split(text, d, __::split_default_predicate());
  }

  template <typename Predicate>
  inline splitter<literal> split(const std::string_ref& text, const string_ref& delim, Predicate filter)
  {
    if(delim.empty()){
      typedef std::forward_list<string_ref> sequence;
      sequence seq; sequence::iterator seq_pos = seq.before_begin();
      for(string_ref::const_iterator p = text.begin(), end = text.end(); p != end; ++p){
        string_ref sub(p, 1);
        if(filter(sub))
          seq_pos = seq.insert_after(seq_pos, sub);
      }
      splitter<literal> result(seq);
      return std::move(result);
    }
    return __::split(text, literal(delim), filter);
  }

  template <typename Predicate>
  inline splitter<literal> split(const std::string_ref& text, const char* delim, Predicate filter)
  {
    return std::split(text, string_ref(delim), filter);
  }

  inline splitter<literal> split(const std::string_ref& text, const char* delim)
  {
    return std::split(text, delim, __::split_default_predicate());
  }


}
#endif // NTL__EXT_SPLIT

