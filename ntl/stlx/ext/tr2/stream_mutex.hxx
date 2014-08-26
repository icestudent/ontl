/**\file*********************************************************************
 *                                                                     \brief
 *  C++ Stream Mutexes [N3535]
 *
 ****************************************************************************
 */
#pragma once

#include <mutex>
#include <unordered_map>

namespace std
{
  namespace tr2
  {
    namespace __
    {
      class stream_mutex_base
      {
        std::recursive_mutex* mtx;
      public:
        explicit stream_mutex_base(const void* tag)
          : mtx(acquire(tag))
        {}

        void lock() { mtx->lock(); }
        void unlock() { mtx->unlock(); }
        bool try_lock() { return mtx->try_lock(); }

      private:
        static std::recursive_mutex* acquire(const void* tag)
        {
          static std::mutex mutex;
          static std::unordered_map<const void*, std::recursive_mutex*> map(19);
          std::lock_guard<std::mutex> lock(mutex);
          std::recursive_mutex*& m = map[tag];
          if(!m)
            m = new std::recursive_mutex();
          return m;
        }
      };
    } // __ namespace


    template <class Stream>	class stream_mutex;
    template <class Stream> class stream_guard;


    template <typename charT, typename traits>
    class stream_mutex< std::basic_ostream<charT, traits> >: public __::stream_mutex_base
    {
      typedef std::basic_ostream<charT, traits> Stream;
      Stream& s;

    public:
      explicit stream_mutex(Stream& stm)
        : stream_mutex_base(&stm), s(stm)
      {}

      using stream_mutex_base::lock;
      using stream_mutex_base::unlock;
      using stream_mutex_base::try_lock;

      Stream& bypass() { return s; }

      stream_guard<Stream> hold() { return stream_guard<Stream>(*this); }

      stream_guard<Stream> operator<< (basic_ostream<charT, traits>& (*pf)(basic_ostream<charT, traits>&))
      {
        lock();
        s << pf;
        return stream_guard<Stream>(*this, std::defer_lock);
      }
    };


    template <typename charT, typename traits>
    class stream_guard< std::basic_ostream<charT, traits> >
    {
      typedef std::basic_ostream<charT, traits> Stream;
      stream_mutex<Stream>& m;

    public:
      explicit stream_guard(stream_mutex<Stream>& mtx)
        : m(mtx)
      {
        m.lock();
      }

      explicit stream_guard(stream_mutex<Stream>& mtx, defer_lock_t)
        : m(mtx)
      {}

      ~stream_guard()
      {
        m.unlock();
      }

      Stream& bypass() const { return m.bypass(); }

      const stream_guard& operator<< (basic_ostream<charT, traits>& (*pf)(basic_ostream<charT, traits>&)) const
      {
        bypass() << pf;
        return *this;
      }
    };

    ///\name Stream locks
    template <typename Stream>
    inline stream_guard<Stream> lock_stream(stream_mutex<Stream>& mtx)
    {
      return stream_guard<Stream>(mtx);
    }


    ///\name Stream Operators
    template <typename Stream, typename T>
    inline const stream_guard<Stream>& operator<<(const stream_guard<Stream>& lck, const T& arg)
    {
      lck.bypass() << arg;
      return lck;
    }

    template <typename Stream, typename T>
    inline const stream_guard<Stream>& operator>>(const stream_guard<Stream>& lck, T& arg)
    {
      lck.bypass() << arg;
      return lck;
    }

    template <typename Stream, typename T>
    inline stream_guard<Stream> operator<<(stream_mutex<Stream>& mtx, const T& arg)
    {
      mtx.lock();
      mtx.bypass() << arg;
      return stream_guard<Stream>(mtx, std::defer_lock);
    }

    template <typename Stream, typename T>
    inline stream_guard<Stream> operator>>(stream_mutex<Stream>& mtx, T& arg)
    {
      mtx.lock();
      mtx.bypass() << arg;
      return stream_guard<Stream>(mtx, std::defer_lock);
    }
    ///\}
  }
}
