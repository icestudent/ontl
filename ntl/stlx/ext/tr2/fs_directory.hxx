#ifndef NTL__STLX_TR2_FILESYSTEM
#error internal header
#endif

namespace std
{
  namespace tr2
  {
    namespace sys
    {
      namespace filesystem
      {
        /**
         *	@brief Class template basic_directory_entry
         *  @details A basic_directory_entry object stores a basic_path object, a file_status object for non-symbolic link status, 
         *  and a file_status object for symbolic link status. 
         *  The file_status objects act as value caches.
         *  @note Because status() on a pathname may be a very expensive operation, some operating systems provide status information 
         *  as a byproduct of directory iteration. 
         *  Caching such status information can result is significant time savings. Cached and non-cached results may differ in the presence of race conditions.
         **/
        template <class Path> class basic_directory_entry;

        /** Specialization of basic_directory_entry for path */
        typedef basic_directory_entry<path> directory_entry;
        /** Specialization of basic_directory_entry for wpath */
        typedef basic_directory_entry<wpath> wdirectory_entry;

        /**
         *	@brief Class template basic_directory_iterator
         *  @details basic_directory_iterator satisfies the requirements of an input iterator (C++ Std, 24.1.1, Input iterators [lib.input.iterators]).
         *
         *  A basic_directory_iterator reads successive elements from the directory for which it was constructed, as if by calling POSIX readdir_r(). 
         *  After a basic_directory_iterator is constructed, and every time operator++ is called, it reads and stores a value of basic_directory_entry<Path> 
         *  and possibly stores associated status values. operator++ is not equality preserving; that is, i == j does not imply that ++i == ++j.
         *
         *  @note The practical consequence of not preserving equality is that directory iterators can be used only for single-pass algorithms.
         *
         *  If the end of the directory elements is reached, the iterator becomes equal to the end iterator value. 
         *  The constructor basic_directory_iterator() with no arguments always constructs an end iterator object, which is the only legitimate iterator 
         *  to be used for the end condition. The result of operator* on an end iterator is not defined. 
         *  For any other iterator value a const basic_directory_entry<Path>& is returned. The result of operator-> on an end iterator is not defined. 
         *  For any other iterator value a const basic_directory_entry<Path>* is returned.
         *
         *  Two end iterators are always equal. An end iterator is not equal to a non-end iterator.
         *
         *  The result of calling the path() member of the basic_directory_entry object obtained by dereferencing a basic_directory_iterator is a reference 
         *  to a basic_path object composed of the directory argument from which the iterator was constructed with filename of the directory entry appended as if by operator/=.
         **/
        template <class Path> class basic_directory_iterator;

        /** Specialization of basic_directory_iterator for path */
        typedef basic_directory_iterator<path> directory_iterator;
        /** Specialization of basic_directory_iterator for wpath */
        typedef basic_directory_iterator<wpath> wdirectory_iterator;

        /**
         *	@brief Class template basic_recursive_directory_iterator
         *  @details The behavior of a basic_recursive_directory_iterator is the same as a basic_directory_iterator unless otherwise specified.
         *
         *  - When an iterator is constructed, m_level is set to 0;
         *  - When an iterator it is incremented, if it->is_directory() is true and no_push() had not been called subsequent to the most recent increment operation (or construction, if no increment has occurred), then  m_level is incremented, the directory is visited, and its contents recursively iterated over.
         *  - When an iterator reaches the end of the directory currently being iterated over, or when pop() is called, m_level is decremented, and iteration continues with the parent directory, until the directory specified in the constructor argument is reached.
         *  - level() returns m_level.
         *  - level(), pop(), and no_push() all require that the iterator not be the end iterator.
         *  .
         *  @note One of the uses of no_push() is to prevent unwanted recursion into symlinked directories. This may be necessary to prevent loops on some operating systems.
         **/
        template <class Path> class basic_recursive_directory_iterator;

        /** Specialization of basic_recursive_directory_iterator for path */
        typedef basic_recursive_directory_iterator<path> recursive_directory_iterator;
        /** Specialization of basic_recursive_directory_iterator for wpath */
        typedef basic_recursive_directory_iterator<wpath> wrecursive_directory_iterator;

      }
    }
  }
}