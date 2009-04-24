/**\file*********************************************************************
 *                                                                     \brief
 *  Filesystem library
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_TR2_FILESYSTEM
#define NTL__STLX_TR2_FILESYSTEM

#include "../../iosfwd.hxx"
#include "../../string.hxx"
#include "../../system_error.hxx"
#include "../../algorithm.hxx"
#include "../../ctime.hxx"
#include "../../locale.hxx"

#include "../../type_traits.hxx"

// implementation
#include "../../../nt/string.hxx"

#ifndef NTL__SUBSYSTEM_KM
# include "../../../nt/new.hxx"
#else
# include "../../../km/new.hxx"
#endif

namespace std
{
  /// C++ Standard Library Technical Report 2
  namespace tr2
  {
    /**\defgroup tr2 Technical Report 2 on C++ Library Extensions
     *@{*/

    /// sys namespace
    namespace sys {
      /// Filesystem Library
      namespace filesystem
      {
      /**
       *  \addtogroup tr2
       *@{
       *  \defgroup tr2_filesystem Filesystem Library
       *
       *  This paper proposes addition of a filesystem library component to the C++ Standard Library Technical Report 2.\n
       *  The proposal is based on the Boost Filesystem Library (see http://boost.org/libs/filesystem).\n
       *  The library provides portable facilities to query and manipulate paths, files, and directories.\n
       *
       *  \b Definitions
       *
       *  The following definitions shall apply to this clause:
       *  \arg \e File: An object that can be written to, or read from, or both. A file has certain attributes, including type. File types include regular file, symbolic link, and directory. Other types of files may be supported by the implementation.
       *  \arg <i>File system</i>: A collection of files and certain of their attributes.
       *  \arg \e Filename: The name of a file. The format is as specified by the POSIX Filename base definition.
       *  \arg \e Path: A sequence of elements which identify a location within a filesystem. The elements are the root-name, root-directory, and each successive filename. See Pathname grammar.
       *  \arg \e Pathname: A character string that represents a path.
       *  \arg \e Link: A directory entry object that associates a filename with a file. On some file systems, several directory entries can associate names with the same file.
       *  \arg <i>Hard link</i>: A link to an existing file. Some file systems support multiple hard links to a file. If the last hard link to a file is removed, the file itself is removed.
       *  @note A hard link can be thought of as a shared-ownership smart pointer to a file.
       *  \arg <i>Symbolic link</i>: A type of file with the property that when the file is encountered during pathname resolution, a string stored by the file is used to modify the pathname resolution.
       *  @note A symbolic link can be thought of as a raw pointer to a file. If the file pointed to does not exist, the symbolic link is said to be a "dangling" symbolic link.
       *  \arg \e Slash: The character '/', also known as solidus.
       *  \arg \e Dot: The character '.', also known as period.
       *  \arg <i>Race condition</i>: The condition that occurs when multiple threads, processes, or computers interleave access and modification of the same object within a file system.
       *@{
       **/

      struct path_traits;
      struct wpath_traits;
      template <class String, class Traits>  class basic_path;

      /** Specialization of basic_path for string */
      typedef basic_path<std::string, path_traits>    path;
      /** Specialization of basic_path for wstring */
      typedef basic_path<std::wstring, wpath_traits>  wpath;


      #pragma region Path traits
      ///\name Path traits
      /** This subclause defines requirements on classes representing path behavior traits, and defines two classes that satisfy those requirements for paths based on string and wstring. */
      struct path_traits
      {
        /** A typedef which is a specialization of basic_string. The \c value_type is a character type used by the operating system to represent pathnames. */
        typedef wstring external_string_type;
        /** A typedef which is a specialization of basic_string. The value_type is a character type to be used by the program to represent pathnames. */
        typedef  string internal_string_type;

        /** \a is, converted by the \c m_locale \c codecvt facet to external_string_type. */
        static external_string_type to_external(const path& p, const internal_string_type& is);
        /** \a xs, converted by the \c m_locale \c codecvt facet to to internal_string_type. */
        static internal_string_type to_internal(const path& p, const external_string_type& xs);

        static void imbue(const locale& loc);
        static bool imbue(const locale& loc, std::nothrow_t);
      };

      /** This subclause defines requirements on classes representing path behavior traits, and defines two classes that satisfy those requirements for paths based on string and wstring. */
      struct wpath_traits
      {
        /** A typedef which is a specialization of basic_string. The \c value_type is a character type used by the operating system to represent pathnames. */
        typedef wstring external_string_type;
        /** A typedef which is a specialization of basic_string. The value_type is a character type to be used by the program to represent pathnames. */
        typedef wstring internal_string_type;

        /** \a is, converted by the \c m_locale \c codecvt facet to external_string_type. */
        static external_string_type to_external(const wpath&, const internal_string_type& is) { return is; }
        /** \a xs, converted by the \c m_locale \c codecvt facet to to internal_string_type. */
        static internal_string_type to_internal(const wpath&, const external_string_type& xs) { return xs; }

        static void imbue(const locale& loc);
        static bool imbue(const locale& loc, std::nothrow_t);
      };

      /** Portable %slash character (\e solidus) */
      template<class Path> struct slash { static const typename Path::value_type value = '/'; };
      /** Portable %dot character (\e period) */
      template<class Path> struct dot   { static const typename Path::value_type value = '.'; };
      /** Portable %colon character */
      template<class Path> struct colon { static const typename Path::value_type value = ':'; };

      /** Slash character which used by path */
      template<> struct slash <path>{ static const char value = '/'; };
      /** Dot character which used by path */
      template<> struct dot   <path>{ static const char value = '.'; };
      /** Colon character which used by path */
      template<> struct colon <path>{ static const char value = ':'; };

      /** Slash character which used by wpath */
      template<> struct slash <wpath>{ static const wchar_t value = '/'; };
      /** Dot character which used by wpath */
      template<> struct dot   <wpath>{ static const wchar_t value = '.'; };
      /** Colon character which used by wpath */
      template<> struct colon <wpath>{ static const wchar_t value = ':'; };
      ///\}
      #pragma endregion

      /**
       *  @brief Class template basic_path
       *
       *	Class template basic_path provides a portable mechanism for representing paths in C++ programs, using a portable generic pathname grammar.
       *  When portability is not a requirement, native file system specific formats can be used. 
       *  Class template basic_path is concerned only with the lexical and syntactic aspects of a path.
       *  The path does not have to exist in the operating system's file system, and may contain names which are not even valid for the current operating system. 
       *
       *  @note If the library's functions trafficked only in C++ or C-style strings, they would provide only the illusion of portability 
       *  since while the syntax of function calls would be portable, the semantics of the strings they operate on would not be portable.
       **/
      template <class String, class Traits>
      class basic_path
      {
        typedef typename String::difference_type pos_type;
      public:
        typedef basic_path<String, Traits>            path_type;
        typedef String                                string_type;
        typedef Traits                                traits_type;
        typedef typename String::value_type           value_type;
        typedef typename Traits::external_string_type external_string_type; 

        ///\name constructors/destructor
        basic_path()
        {}
        basic_path(const basic_path& p)
          :path_(p.path_)
        {}
        basic_path(const string_type& s)
        {
          operator /=(s);
        }
        basic_path(const value_type* s)
        {
          operator /=(s);
        }

        template <class InputIterator>
        basic_path(InputIterator first, InputIterator last)
        {
          append(first, last);
        }

        ~basic_path()
        {}

        ///\name assignments
        basic_path& operator=(const basic_path& p)
        {
          path_.clear();
          return operator /=(p);
        }

        basic_path& operator=(const string_type& s)
        {
          path_.clear();
          return operator /=(s);
        }

        basic_path& operator=(const value_type* s)
        {
          path_.clear();
          return operator /=(s);
        }

        template <class InputIterator>
        basic_path& assign(InputIterator first, InputIterator last)
        {
          path_.clear();
          return append(first, last);
        }

        ///\name modifiers
        basic_path& operator/=(const basic_path& rhs) { return append(rhs.string().c_str(), rhs.string().size()); }
        basic_path& operator/=(const string_type& s)  { return append(s.c_str(), s.size()); }
        basic_path& operator/=(const value_type* s)   { return append(s, s ? string_type::traits_type::length(s) : 0); }

        template <class InputIterator>
        basic_path& append(InputIterator first, InputIterator last);

        /** Clears the path */
        void clear() { path_.clear(); }
        /** Swaps the paths */
      #ifdef NTL__CXX_RV
        void swap(basic_path&& rhs)
      #else
        void swap(basic_path& rhs)
      #endif
        {
          std::swap(path_, rhs.path_);
        }

        /** Removes the last \e filename from the path */
        basic_path& remove_leaf();

        /** Replaces the extension of \e filename with the new extension */
        basic_path& replace_extension(const string_type& new_ext = string_type());

        ///\name observers

        /** Returns the stored path */
        const string_type string() const { return path_; }
        /** Returns the stored path, formatted according to the Native subsystem rules for file names */
        const string_type file_string(bool normalize = true) const;
        /** Returns the stored path, formatted according to the Native subsystem rules for directory names (same as file_string()) */
        const string_type directory_string(bool normalize = true) const { return file_string(); }

        /** Returns the stored path, formatted and encoded according to the Native subsystem rules for file names */
        const external_string_type external_file_string(bool normalize = true) const
        {
          if(normalize){
            external_string_type xs = traits_type::to_external(*this, path_);
            ntl::nt::rtl::relative_name rel(xs.c_str());
            if(rel)
              return rel.path.get_string();
          }
          return traits_type::to_external(*this, file_string(normalize));
        }

        /** Returns the stored path, formatted and encoded according to the Native subsystem rules for directory names */
        const external_string_type external_directory_string(bool normalize = true) const{ return traits_type::to_external(*this, directory_string(normalize)); }

        /** extension: returns UNC server name if any */
        string_type  unc_name() const;
        /** Returns the \e root-name, if any (e.g. UNC server name or drive name) */
        string_type  root_name() const;
        /** Returns the \e root-directory, if any (currently its always slash) */
        string_type  root_directory() const;
        /** Returns the \e root-path, if any (\e root-name with trailing slash) */
        basic_path   root_path() const;

        /** Returns the \e branch-path, if any (aka \e parent-path) */
        basic_path   branch_path() const;
        /** Returns the relative path, if any (path without \e root-name) */
        basic_path   relative_path() const;
        
        /** Returns the base object name (without path), if any */
        string_type  leaf() const;
        /** Returns the base object name without \e extension, if any */
        string_type  stem() const;
        /** Returns the extension of the base object name, if any */
        string_type  extension() const;

        /** Returns \c true if path is empty */
        bool empty() const { return path_.empty(); }

        /** Returns \c true if elements of root_path() uniquely identify a directory */
        bool is_complete() const;

        /** extension: checks is path have a UNC prefix */
        bool is_unc() const { return path_.size() > 2 && path_[0] == slashval && path_[1] == slashval; }

        /** Returns \c true if path has the \e root-name */
        bool has_root_name() const;
        /** Returns \c true if path has the \e root-directory */
        bool has_root_directory() const;
        /** Returns \c true if path has the \e root-path */
        bool has_root_path() const;
        /** Returns \c true if path has the branch (parent) path */
        bool has_branch_path() const;
        /** Returns \c true if path has the relative (without \e root-name) path */
        bool has_relative_path() const;
        /** Returns \c true if path has the base object name */
        bool has_leaf() const;
        /** Returns \c true if path has the base object name without extension (useless) */
        bool has_stem() const;
        /** Returns \c true if path has the extension of the base object name */
        bool has_extension() const;

        ///\name iterators
        /**
         *	@brief A basic_path::iterator is a constant iterator satisfying all the requirements of a bidirectional iterator
         *  The forward traversal order is as follows:
         *  - The root-name element, if present.
         *  - The root-directory element, if present.
         *  - Each successive filename element, if present.
         *  - Dot, if one or more trailing non-root slash characters are present.
         *  .
         *  The backward traversal order is the reverse of forward traversal.
         **/
        struct iterator:
          std::iterator<bidirectional_iterator_tag, const string_type>
        {
          iterator()
            :path(), pos(), element()
          {}

          static_assert((is_same<const string_type&, reference>::value), "brb");

          reference operator*() const { return name;  }
          pointer operator->()  const { return &name; }

          iterator& operator++();
          iterator& operator--();
          iterator  operator++(int)
          {
            iterator tmp(*this);
            operator++();
            return tmp;
          }
          iterator  operator--(int)
          {
            iterator tmp(*this);
            operator--();
            return tmp;
          }

          friend bool operator==(const iterator& x, const iterator& y) { return x.path == y.path && x.pos == y.pos; }
          friend bool operator!=(const iterator& x, const iterator& y) { return !(x == y); }

        protected:
          friend iterator basic_path::begin() const;
          friend iterator basic_path::end() const;

          iterator(const string_type& p, bool start)
            :path(&p), pos(0), element(begin)
          {
            if(!start)
              pos = value_type::npos, element = end;
            else
              operator++();
          }

          enum elements {
            begin, rootname, rootdir, filename, dot, end
          };

          static bool is_unc(reference p) { return p.size() > 2 && p[0] == slashval && p[1] == slashval; }
          static bool has_drive(reference p) { return p.size() >= 2 && p[1] == colonval; }
        private:
          string_type name;
          const string_type* path;
          pos_type pos;
          elements element;
        };

        /** Same as iterator. */
        typedef iterator const_iterator;

        /** Returns an iterator for the first present element in the traversal list above. If no elements are present, the end iterator. */
        iterator begin() const { return iterator(path_, true ); }
        /** Returns the end iterator. */
        iterator end()   const { return iterator(path_, false); }

        /** Returns an iterator for the first present element in the traversal list above. If no elements are present, the end iterator. */
        const_iterator cbegin() const { return begin(); }
        /** Returns the end iterator. */
        const_iterator cend() const { return end(); }
        ///\}

        /** Compares a two path objects */
        friend bool operator==(const basic_path& a, const basic_path& b) { return a.path_ == b.path_; }

      private:
        basic_path& append(const value_type* s, typename string_type::size_type size);
        
        bool has_drive() const { return path_.size() >= 2 && path_[1] == colonval; }
        static bool is_sep(value_type c) { return c == slashval || c == backslash; }

        template<bool>
        bool do_normalize(string_type& native, std::__::int2type<true>) const
        {
          ntl::nt::rtl::relative_name rel(path_.c_str());
          if(rel)
            native = rel.path.get_string();
          return rel;
        }

        template<bool>
        bool do_normalize(string_type& native, std::__::int2type<false>) const
        {
          external_string_type xs = traits_type::to_external(*this, path_);
          ntl::nt::rtl::relative_name rel(xs.c_str());
          if(rel)
            native = traits_type::to_internal(*this, rel.path.get_string());
          return rel;
        }

        pos_type filename_pos() const;
      private:
        string_type path_;
        static const value_type dotval = dot<path_type>::value;
        static const value_type slashval = slash<path_type>::value;
        static const value_type colonval = colon<path_type>::value;
        static const value_type backslash = '\\';
        static const value_type qmark = '?';
        static const pos_type npos = string_type::npos;
      };

      /// Type \c is_basic_path shall be specialized for path, wpath, and any user-specialized basic_path types.
      template<class Path>
      struct is_basic_path: false_type{};

      /// Type \c is_basic_path shall be specialized for path, wpath, and any user-specialized basic_path types.
      template <class String, class Traits>
      struct is_basic_path<basic_path<String, Traits> >: true_type{};

      #pragma region basic_path non-member operators

      ///\name basic_path non-member operators (basic_path comparsion)
      /** Swaps two path objects */
      template<class String, class Traits>
      inline void swap(basic_path<String, Traits> & lhs, basic_path<String, Traits> & rhs) { lhs.swap(rhs); }
      
    #ifdef NTL__CXX_RV
      template<class String, class Traits>
      inline void swap(basic_path<String, Traits>&& lhs, basic_path<String, Traits> & rhs) { lhs.swap(rhs); }
      template<class String, class Traits>
      inline void swap(basic_path<String, Traits> & lhs, basic_path<String, Traits>&& rhs) { lhs.swap(rhs); }
    #endif

    #ifndef __DOXYGEN__
      template<class String, class Traits> inline bool operator<(basic_path<String, Traits>& a, basic_path<String, Traits>& b) { return lexicographical_compare(a.begin(), a.end(), b.begin(), b.end()); }
      template<class String, class Traits> bool operator==(basic_path<String, Traits>& a, basic_path<String, Traits>& b);
      template<class String, class Traits> inline bool operator!=(basic_path<String, Traits>& a, basic_path<String, Traits>& b){ return rel_ops::operator !=(a,b); }
      template<class String, class Traits> inline bool operator>(basic_path<String, Traits>& a, basic_path<String, Traits>& b) { return rel_ops::operator >(a,b);  }
      template<class String, class Traits> inline bool operator<=(basic_path<String, Traits>& a, basic_path<String, Traits>& b){ return rel_ops::operator <=(a,b); }
      template<class String, class Traits> inline bool operator>=(basic_path<String, Traits>& a, basic_path<String, Traits>& b){ return rel_ops::operator >=(a,b); }
      template<class String, class Traits> inline basic_path<String, Traits> operator/(basic_path<String, Traits>& a, basic_path<String, Traits>& b) { return basic_path<String, Traits>(a) /= b; }

      template<class String, class Traits> inline bool operator<(const typename basic_path<String, Traits>::string_type& a, basic_path<String, Traits>& b) { return basic_path<String, Traits>(a) < b; }
      template<class String, class Traits> inline bool operator==(const typename basic_path<String, Traits>::string_type& a, basic_path<String, Traits>& b){ return basic_path<String, Traits>(a) == b; }
      template<class String, class Traits> inline bool operator!=(const typename basic_path<String, Traits>::string_type& a, basic_path<String, Traits>& b){ return basic_path<String, Traits>(a) != b; }
      template<class String, class Traits> inline bool operator>(const typename basic_path<String, Traits>::string_type& a, basic_path<String, Traits>& b) { return basic_path<String, Traits>(a) > b; }
      template<class String, class Traits> inline bool operator<=(const typename basic_path<String, Traits>::string_type& a, basic_path<String, Traits>& b){ return basic_path<String, Traits>(a) <= b; }
      template<class String, class Traits> inline bool operator>=(const typename basic_path<String, Traits>::string_type& a, basic_path<String, Traits>& b){ return basic_path<String, Traits>(a) >= b; }
      template<class String, class Traits> inline basic_path<String, Traits> operator/(const typename basic_path<String, Traits>::string_type& a, basic_path<String, Traits>& b) { return basic_path<String, Traits>(a) /= b; }

      template<class String, class Traits> inline bool operator<(const typename basic_path<String, Traits>::string_type::value_type* a, basic_path<String, Traits>& b) { return basic_path<String, Traits>(a) < b; }
      template<class String, class Traits> inline bool operator==(const typename basic_path<String, Traits>::string_type::value_type* a, basic_path<String, Traits>& b){ return basic_path<String, Traits>(a) == b; }
      template<class String, class Traits> inline bool operator!=(const typename basic_path<String, Traits>::string_type::value_type* a, basic_path<String, Traits>& b){ return basic_path<String, Traits>(a) != b; }
      template<class String, class Traits> inline bool operator>(const typename basic_path<String, Traits>::string_type::value_type* a, basic_path<String, Traits>& b) { return basic_path<String, Traits>(a) > b; }
      template<class String, class Traits> inline bool operator<=(const typename basic_path<String, Traits>::string_type::value_type* a, basic_path<String, Traits>& b){ return basic_path<String, Traits>(a) <= b; }
      template<class String, class Traits> inline bool operator>=(const typename basic_path<String, Traits>::string_type::value_type* a, basic_path<String, Traits>& b){ return basic_path<String, Traits>(a) >= b; }
      template<class String, class Traits> inline basic_path<String, Traits> operator/(const typename basic_path<String, Traits>::string_type::value_type* a, basic_path<String, Traits>& b) { return basic_path<String, Traits>(a) /= b; }

      template<class String, class Traits> inline bool operator<(const basic_path<String, Traits>& a, typename basic_path<String, Traits>::string_type& b) { return a < basic_path<String, Traits>(b); }
      template<class String, class Traits> inline bool operator==(const basic_path<String, Traits>& a, typename basic_path<String, Traits>::string_type& b){ return a == basic_path<String, Traits>(b); }
      template<class String, class Traits> inline bool operator!=(const basic_path<String, Traits>& a, typename basic_path<String, Traits>::string_type& b){ return a != basic_path<String, Traits>(b); }
      template<class String, class Traits> inline bool operator>(const basic_path<String, Traits>& a, typename basic_path<String, Traits>::string_type& b) { return a > basic_path<String, Traits>(b); }
      template<class String, class Traits> inline bool operator<=(const basic_path<String, Traits>& a, typename basic_path<String, Traits>::string_type& b){ return a <= basic_path<String, Traits>(b); }
      template<class String, class Traits> inline bool operator>=(const basic_path<String, Traits>& a, typename basic_path<String, Traits>::string_type& b){ return a >= basic_path<String, Traits>(b); }
      template<class String, class Traits> inline basic_path<String, Traits> operator/(const basic_path<String, Traits>& a, typename basic_path<String, Traits>::string_type& b) { return basic_path<String, Traits>(a) /= basic_path<String, Traits>(b); }

      template<class String, class Traits> inline bool operator<(const basic_path<String, Traits>& a, typename basic_path<String, Traits>::string_type::value_type* b) { return a < basic_path<String, Traits>(b); }
      template<class String, class Traits> inline bool operator==(const basic_path<String, Traits>& a, typename basic_path<String, Traits>::string_type::value_type* b){ return a == basic_path<String, Traits>(b); }
      template<class String, class Traits> inline bool operator!=(const basic_path<String, Traits>& a, typename basic_path<String, Traits>::string_type::value_type* b){ return a != basic_path<String, Traits>(b); }
      template<class String, class Traits> inline bool operator>(const basic_path<String, Traits>& a, typename basic_path<String, Traits>::string_type::value_type* b) { return a > basic_path<String, Traits>(b); }
      template<class String, class Traits> inline bool operator<=(const basic_path<String, Traits>& a, typename basic_path<String, Traits>::string_type::value_type* b){ return a <= basic_path<String, Traits>(b); }
      template<class String, class Traits> inline bool operator>=(const basic_path<String, Traits>& a, typename basic_path<String, Traits>::string_type::value_type* b){ return a >= basic_path<String, Traits>(b); }
      template<class String, class Traits> inline basic_path<String, Traits> operator/(const basic_path<String, Traits>& a, typename basic_path<String, Traits>::string_type::value_type* b) { return a /= basic_path<String, Traits>(b); }
    #endif
      ///\}
      #pragma endregion 

      /** Prints basic_path to the %stream */
      template<class Path>
      inline basic_ostream<typename Path::string_type::value_type, typename Path::string_type::traits_type> &
        operator<<(basic_ostream<typename Path::string_type::value_type, typename Path::string_type::traits_type>& os, const Path& ph)
      {
        return os << ph.string();
      }

      /** Reads the basic_path from the %stream */
      template<class Path>
      inline basic_istream<typename Path::string_type::value_type, typename Path::string_type::traits_type> &
        operator>>(basic_istream<typename Path::string_type::value_type, typename Path::string_type::traits_type>& is, Path & ph)
      {
        typename Path::string_type str;
        is >> str;
        ph = str;
        return is;
      }


      /**
       *	@brief Class template basic_filesystem_error
       *  @details The class template basic_filesystem_error defines the type of objects thrown as exceptions 
       *  to report file system errors from functions described in this clause.
       **/
      template <class Path>
      class basic_filesystem_error:
        public system_error
      {
        Path p1,p2;
        mutable string msg;
      public:
        typedef Path path_type;

        ///\name Constructors
        explicit basic_filesystem_error(const std::string& what_arg, error_code ec)
          :system_error(ec, what_arg)
        {}
        basic_filesystem_error(const std::string& what_arg, const path_type& p1, error_code ec)
        :system_error(ec, what_arg), p1(p1)
        {}
        basic_filesystem_error(const std::string& what_arg, const path_type& p1, const path_type& p2, error_code ec)
        :system_error(ec, what_arg), p1(p1), p2(p2)
        {}

        ///\name Observers
        /** Returns the stored path #1 */
        const path_type& path1() const { return p1; }
        /** Returns the stored path #2 */
        const path_type& path2() const { return p2; }

        inline const char* what() const;
        ///\}
      private:
        template<bool>
        static string to_string(const path& p)
        {
          return p.file_string();
        }
        template<bool>
        static string to_string(const wpath& p)
        {
          const wpath::string_type& ws = p.string();
          string s; s.resize(ws.size());
          typedef ctype<wpath::string_type::value_type> ctype;
          const ctype& ct = use_facet<ctype>(locale::classic());
          ct.narrow(ws.cbegin(),ws.cend(),'\0',s.begin());
          return s;
        }
        template<bool, class PathT>
        static string to_string(const PathT&);
      };

      /** Specialization of basic_filesystem_error for path */
      typedef basic_filesystem_error<path> filesystem_error;
      /** Specialization of basic_filesystem_error for wpath */
      typedef basic_filesystem_error<wpath> wfilesystem_error;

      #ifndef __DOXYGEN__
      //////////////////////////////////////////////////////////////////////////
      // IMPLEMENTATION
      /************************************************************************/
      /* basic_path                                                           */
      /************************************************************************/
      template <class String, class Traits>
      const String basic_path<String,Traits>::file_string(bool normalize) const
      {
        if(path_.empty())
          return string_type();

        string_type native;
        if(normalize){
          static const bool same = is_same<string_type,external_string_type>::value;
          do_normalize<same>(native, std::__::int2type<same>());
          if(!native.empty())
            return native;
        }

        // prepend the NT prefixes if path has drive
        static const char *ntprefix = "\\??\\", *uncprefix = "UNC\\";

        const bool abs = has_drive(), unc = is_unc();
        native.resize(abs*4 + path_.size() + unc*2); // '\??\' + (?unc 'UNC\' - '//') + path

        if(abs || unc){
          const ctype<value_type>& ct = use_facet<ctype<value_type> >(locale::classic());
          if(abs)
            ct.widen(ntprefix, ntprefix+4, native.begin());
          if(unc)
            ct.widen(uncprefix, uncprefix+4, native.begin()+4*abs);
        }

        // convert separators
        string_type::const_iterator j = path_.cbegin()+unc*2;
        for(string_type::iterator i = native.begin()+4*abs+4*unc, endi = native.end(); i != endi; ++i, ++j){
          *i = *j != slashval ? *j : backslash;
        }
        return native;
      }

      template <class String, class Traits>
      typename String::difference_type basic_path<String,Traits>::filename_pos() const
      {
        if(path_.empty())
          return string::npos;

        // '//C:/path/to/file.txt
        pos_type slashpos = path_.rfind(slash<path_type>::value);
        if(slashpos == npos) // path is filename
          return 0;
        else if(slashpos == static_cast<pos_type>(path_.size()-1))  // path ends with slash
          return npos;
        return slashpos + 1;
      }

      template <class String, class Traits>
      bool basic_path<String,Traits>::is_complete() const
      {
        return has_root_name() && has_root_directory();
      }

      template <class String, class Traits>
      bool basic_path<String,Traits>::has_root_name() const
      {
        // 'C:' or '//HOME'
        return is_unc() || has_drive();
      }

      template <class String, class Traits>
      bool basic_path<String,Traits>::has_root_directory() const
      {
        // has directory
        pos_type pos = path_.find(slashval, is_unc() ? 2 : 0);
        return pos != npos;
      }

      template <class String, class Traits>
      bool basic_path<String,Traits>::has_root_path() const
      {
        return has_root_name() || (!path_.empty() && path_[0] == slashval);
      }

      template <class String, class Traits>
      bool basic_path<String,Traits>::has_relative_path() const
      {
        return !relative_path().empty();
      }

      template <class String, class Traits>
      bool basic_path<String,Traits>::has_leaf() const
      {
        return !leaf().empty();
      }

      template <class String, class Traits>
      bool basic_path<String,Traits>::has_stem() const
      {
        return !stem().empty();
      }

      template <class String, class Traits>
      bool basic_path<String,Traits>::has_extension() const
      {
        return !extension().empty();
      }

      template <class String, class Traits>
      bool basic_path<String,Traits>::has_branch_path() const
      {
        return !branch_path().empty();
      }

      template <class String, class Traits>
      String basic_path<String,Traits>::root_name() const
      {
        if(!has_root_name())
          return string_type();

        // '//unc[/path]
        pos_type pos = path_.find(slashval, is_unc() ? 2 : 0);
        return path_.substr(0, pos);
      }

      template <class String, class Traits>
      String basic_path<String,Traits>::root_directory() const
      {
        // root dir always '/'?
        return has_root_directory() ? string_type(1, slashval) : string_type();
      }

      template <class String, class Traits>
      basic_path<String,Traits> basic_path<String,Traits>::root_path() const
      {
        // if '/root[...]' return '/'
        if(!has_root_name() && has_root_path())
          return string_type(1, slashval);

        // root name with trailing slash, if any
        string_type root = root_name();
        if(!root.empty() && path_.size() > root.size())
          root += slashval; // += root_directory()
        return root;
      }

      template <class String, class Traits>
      basic_path<String,Traits> basic_path<String,Traits>::relative_path() const
      {
        // right part from root path if any
        if(path_.empty())
          return basic_path();
        
        pos_type from = root_path().string().size();
        return path_.substr(from);
      }

      template <class String, class Traits>
      basic_path<String,Traits> basic_path<String,Traits>::branch_path() const
      {
        // path - leaf
        if(path_.empty())
          return basic_path();

        pos_type fpos = filename_pos();
        return path_.substr(0, fpos == 0 ? npos : fpos-1);
      }


      template <class String, class Traits>
      String basic_path<String,Traits>::leaf() const
      {
        // filename
        if(path_.empty())
          return string_type();

        pos_type pos = filename_pos();
        if(pos == npos)
          return string_type();
        else if(pos == 0 && path_.size() <= 2 && path_[0] == dotval) // '.' or '..'
          return string_type();
        else
          return path_.substr(pos);
      }

      template <class String, class Traits>
      String basic_path<String,Traits>::stem() const
      {
        // filename without extension, if any
        string_type fname = leaf();
        pos_type dotpos = fname.rfind(dotval);
        return dotpos == npos ? fname : fname.substr(0, dotpos);
      }

      template <class String, class Traits>
      String basic_path<String,Traits>::extension() const
      {
        // filename's extension, if any
        string_type fname = leaf();
        pos_type dotpos = fname.rfind(dotval);
        return dotpos == npos ? string_type() : fname.substr(dotpos+1, npos);
      }

      template <class String, class Traits>
      String basic_path<String,Traits>::unc_name() const
      {
        return is_unc() ? root_name() : string_type();
      }

      template <class String, class Traits>
      basic_path<String,Traits>& basic_path<String,Traits>::append(const typename basic_path<String,Traits>::value_type* p, typename String::size_type size)
      {
        if(!p || size == 0 || !*p)
          return *this;

        // check Native subsystem name '\??\'
        const bool native = path_.empty() && size > 4 && p[0] == backslash && p[1] == qmark && p[2] == qmark && p[3] == backslash;
        bool unc = false;
        if(native){
          p += 4, size -= 4;
          // check the UNC name 'UNC\'
          unc = size > 4 && p[0] == 'U' && p[1] == 'N' && p[2] == 'C' && p[3] == backslash;
          if(unc){
            p += 4, size -= 4;
            // prepend the portable UNC prefix '//'
            path_ += slashval, path_ += slashval;
          }
          if(!size)
            return *this;
        }

        // explicit native sequence (posix?) '//:'
        // NOTE: can this sequence be inside path?
        // All basic_path string or sequence arguments that describe a path shall accept the portable pathname format, 
        // and shall accept the native format if explicitly identified by a native format escape sequence prefix of slash slash colon ('//:').
        const bool pnative = size > 3 && p[0] == slashval && p[1] == slashval && p[2] == colonval;
        if(pnative){
          p += 3, size -= 3;
          if(!size)
            return *this;
        }

        // check trailing separator
        if(!path_.empty() && !is_sep(*p) && path_[path_.size()-1] != slashval)
          path_ += slashval;

        // NOTE: should be it?
        //if(pnative){
        //  path_.append(p,size);
        //  return *this;
        //}

        // append
        while(size--){
          path_ += *p == backslash ? slashval : *p;
          p++;
        }
        path_.c_str();
        return *this;
      }

      template <class String, class Traits>
      basic_path<String,Traits>& basic_path<String,Traits>::remove_leaf()
      {
        path_.erase(filename_pos());
        return *this;
      }

      template <class String, class Traits>
      basic_path<String,Traits>& basic_path<String,Traits>::replace_extension(const typename basic_path<String,Traits>::string_type& new_ext)
      {
        string_type ext = extension();
        if(!ext.empty())
          path_.erase(path_.size() - ext.size());
        if(!new_ext.empty() && new_ext[0] != dotval)
          path_ += dotval;
        path_ += new_ext;
        return *this;
      }

      // iterator functions
      template <class String, class Traits>
      typename basic_path<String,Traits>::iterator& basic_path<String,Traits>::iterator::operator++()
      {
        // root name, root directory, elements, dot
        assert(element != end && "out of range");
        if(element == end)
          return *this;

        value_type& p = *path;
        const typename value_type::size_type size = path->size();
        if(pos == npos || pos == static_cast<pos_type>(size)){
          element = end;
          pos = npos;
          name.clear();
          return *this;
        }

        // skip leading slashes
        if(!pos){
          while(pos < static_cast<pos_type>(size) && p[pos] == slashval)
            pos++;
        }

        pos_type cur = pos;
        pos = p.find(slashval, cur);
        if(pos == npos)
          pos = size;
        name = p.substr(cur, pos-cur);
        if(pos != size)
          pos++;
      #if _DEBUG
        name.c_str();
      #endif
        return *this;
      }

      template <class String, class Traits>
      typename basic_path<String,Traits>::iterator& basic_path<String,Traits>::iterator::operator--()
      {
        assert(element != begin && "out of range");
        return *this;
      }

      /************************************************************************/
      /* basic_filesystem_error                                               */
      /************************************************************************/
      template <class Path>
      const char* basic_filesystem_error<Path>::what() const
      {
        if(msg.empty()){
          __ntl_try{
            string tmp = system_error::what();
            if(!p1.empty() || !p2.empty()){
              if(!tmp.empty() && tmp != "")
                tmp += " with ";
              if(!p1.empty()){
                tmp += "path1 is '";
                tmp += to_string<true>(p1);
                tmp += "'";
              }
              if(!p2.empty()){
                if(!p1.empty())
                  tmp += " and ";
                tmp += "path2 is '";
                tmp += to_string<true>(p2);
                tmp += "'";
              }
              tmp += ".";
            }
            msg = move(tmp);
          }
          __ntl_catch(...){
            return system_error::what();
          }
        }
        return msg.c_str();
      }

      /************************************************************************/
      /* Path traits                                                          */
      /************************************************************************/
      path_traits::external_string_type path_traits::to_external(const path&, const path_traits::internal_string_type& is)
      {
        if(is.empty())
          return external_string_type();

        // for why path here?!
        using namespace ntl::nt;
        unicode_string us;
        const_ansi_string as(is);
        ntstatus st = RtlAnsiStringToUnicodeString(us, as, true);
        if(!success(st))
          return external_string_type();
        external_string_type xs = us.get_string();
        RtlFreeUnicodeString(us);
        // convert separators
        for(external_string_type::iterator i = xs.begin(), iend = xs.end(); i != iend; ++i)
          if(*i == '/')
            *i = '\\';
        return xs;
      }

      path_traits::internal_string_type path_traits::to_internal(const path&, const path_traits::external_string_type& xs)
      {
        if(xs.empty())
          return internal_string_type();

        using namespace ntl::nt;
        ansi_string as;
        const_unicode_string us(xs);
        ntstatus st = RtlUnicodeStringToAnsiString(as, us, true);
        if(!success(st))
          return internal_string_type();
        internal_string_type is = as.get_string();
        RtlFreeAnsiString(as);
        // convert separators
        for(internal_string_type::iterator i = is.begin(), iend = is.end(); i != iend; ++i)
          if(*i == '\\')
            *i = '/';
        return is;
      }

      #endif // __DOXYGEN__

      /** @} tr2_filesystem */
      /** @} tr2 */
    } // filesystem
    } // sys
    /** @} tr2 */
  } // tr2
}// std

#include "fs_operations.hxx"
#include "fs_directory.hxx"

#endif
