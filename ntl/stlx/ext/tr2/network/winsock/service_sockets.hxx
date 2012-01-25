#pragma once

#include "../io_service.hxx"
#include "../buffer.hxx"
#include "../socket_base.hxx"
#include "sockets.hxx"


namespace ntl { namespace network {
  namespace error = std::tr2::network::network_error;
  namespace stdnet = std::tr2::network;
  namespace ios = std::tr2::sys;

  namespace winsock
  {

    struct buffer_sequences
    {
      typedef wsa::wsabuf_t native_buffer;

      native_buffer* buffers()  { return native_buffers; }
      const native_buffer* buffers() const { return native_buffers; }

      bool empty() const { return buf_size == 0; }
      size_t count() const { return buf_count; }
      size_t total_size() const { return buf_size; }

    protected:
      buffer_sequences()
        :native_buffers(), buf_size(), buf_count()
      {}

      static void init_buffer(native_buffer& buf, const ios::mutable_buffer& from)
      {
        buf.buf = ios::buffer_cast<char*>(from);
        buf.len = static_cast<uint32_t>(ios::buffer_size(from));
      }
      static void init_buffer(native_buffer& buf, const ios::const_buffer& from)
      {
        buf.buf = const_cast<char*>(ios::buffer_cast<const char*>(from));
        buf.len = static_cast<uint32_t>(ios::buffer_size(from));
      }

    protected:
      native_buffer* native_buffers;
      size_t buf_count, buf_size;
    };


    template<class Buffer, class Buffers>
    struct buffer_sequence:
      buffer_sequences
    {
      typedef Buffer  buffer_t;
      typedef Buffers buffers_t;
    public:
      explicit buffer_sequence(const Buffers& buffers)
      {
        for(typename buffers_t::const_iterator i = buffers.begin(), e = buffers.end(); i != e && buf_count < max_count; ++i, ++buf_count)
        {
          const buffer_t buf(*i);
          init_buffer(this->buffs[buf_count], buf);
          buf_size += sys::buffer_size(buf);
        }
        native_buffers = buffs;
      }

      static buffer_t first(const buffers_t& buffers)
      {
        // first non-empty
        for(typename buffers_t::const_iterator i = buffers.begin(), e = buffers.end(); i != e; ++i)
        {
          const buffer_t buf(*i);
          if(stdnet::buffer_size(buf) > 0)
            return buf;
        }
        return buffer_t();
      }

      static bool empty(const buffers_t& buffers)
      {
        const buffer_t buf = first(buffers);
        return stdnet::buffer_size(buf) == 0;
      }
    protected:
      static const size_t max_count = 64;
      native_buffer buffs[max_count];
    };

    template<class Buffer, class Buffers>
    struct buffer_sequence_1
      :buffer_sequences

    {
      typedef Buffer  buffer_t;
      typedef Buffers buffers_t;

      explicit buffer_sequence_1(const buffers_t& buffers)
      {
        init_buffer(buf, Buffer(buffers));
        buf_size = ios::buffer_size(buffers);
        native_buffers = &buf;
        buf_count = 1;
      }

      static buffer_t first(const buffers_t& buffers)
      {
        return Buffer(buffers);
      }

      static bool empty(const buffers_t& buffers)
      {
        return stdnet::buffer_size(buffers) == 0;
      }
    protected:
      native_buffer buf;
    };

    template<class Buffer>
    struct buffer_sequence<Buffer, ios::mutable_buffers_1>
      :buffer_sequence_1<Buffer, ios::mutable_buffers_1>
    {
      explicit buffer_sequence(const buffers_t& buffers)
        :buffer_sequence_1(buffers)
      {}
    };

    template<class Buffer>
    struct buffer_sequence<Buffer, ios::const_buffers_1>
      :buffer_sequence_1<Buffer, ios::const_buffers_1>
    {
      explicit buffer_sequence(const buffers_t& buffers)
        :buffer_sequence_1(buffers)
      {}
    };


    namespace sockoptions
    {
      class outband_interface:
        public stdnet::__::network::integral_socket_option<int,
        constants::sol_ip, constants::ip_multicast_if,
        constants::sol_ipv6, constants::ipv6_multicast_if>
      {
      public:
        outband_interface()
          :v6()
        {
          v4.addr = 0;
        }
        template<class Protocol> void* data(const Protocol& p)
        { return p.family() == constants::af_inet6 ? &v6 : &v4.addr; }
        template<class Protocol> const void* data(const Protocol& p) const
        { return p.family() == constants::af_inet6 ? &v6 : &v4.addr; }
        template<class Protocol> size_t size(const Protocol& p) const
        { return p.family() == constants::af_inet6 ? sizeof(v6) : sizeof(v4); }

      protected:
        in_addr  v4;
        uint32_t v6;
      };

      class join_group:
        public stdnet::__::network::integral_socket_option<int,
        constants::sol_ip, constants::ip_add_membership,
        constants::sol_ipv6, constants::ipv6_join_group>
      {
      public:
        join_group()
        {
          std::memset(this, 0, sizeof(join_group));
        }
        template<class Protocol> void* data(const Protocol& p)
        { return p.family() == constants::af_inet6 ? (void*)&v6 : (void*)&v4; }
        template<class Protocol> const void* data(const Protocol& p) const
        { return p.family() == constants::af_inet6 ? (const void*)&v6 : (const void*)&v4; }
        template<class Protocol> size_t size(const Protocol& p) const
        { return p.family() == constants::af_inet6 ? sizeof(v6) : sizeof(v4); }

      protected:
        ip_mreq   v4;
        ipv6_mreq v6;
      };

      class leave_group:
        public stdnet::__::network::integral_socket_option<int,
        constants::sol_ip, constants::ip_drop_membership,
        constants::sol_ipv6, constants::ipv6_leave_group>
      {
      public:
        leave_group()
        {
          std::memset(this, 0, sizeof(join_group));
        }
        template<class Protocol> void* data(const Protocol& p)
        { return p.family() == constants::af_inet6 ? (void*)&v6 : (void*)&v4; }
        template<class Protocol> const void* data(const Protocol& p) const
        { return p.family() == constants::af_inet6 ? (const void*)&v6 : (const void*)&v4; }
        template<class Protocol> size_t size(const Protocol& p) const
        { return p.family() == constants::af_inet6 ? sizeof(v6) : sizeof(v4); }

      protected:
        ip_mreq   v4;
        ipv6_mreq v6;
      };

    }


    class socket_service_base:
      public winsock_service_base
    {
    protected:
      typedef stdnet::socket_base::shutdown_type shutdown_type;
      typedef stdnet::socket_base socket_base;

    public:
      typedef socket native_type;
      struct implementation_type
      {
        functions_t* funcs;
        socket s;

        // ip::protocol emulation
        int proto_family, proto_type, proto_protocol;
        bool ipv6, is_stream;

        int family() const { return proto_family;  }
        int type()   const { return proto_type;    }
        int protocol()const{ return proto_protocol;}
      };


    public:
      static void host_name(std::string& host, std::error_code& ec)
      {
        functions_t* funcs = winsock_runtime::instance();
        if(!funcs || !funcs->gethostname){
          ec = error::make_error_code(error::network_down);
          return;
        }
        host.resize(1025);
        if(!check_error(ec, funcs->gethostname(host.begin(), static_cast<int>(host.size()))))
          host.clear();
        winsock_runtime::cleanup();
      }

      void construct(implementation_type& impl)
      {
        std::memset(&impl, 0, sizeof(impl));
        impl.funcs = winsock_runtime::instance();
        impl.s = 0;
      }

      void destroy(implementation_type& impl)
      {
        if(impl.s) {
          impl.funcs->closesocket(impl.s);
          impl.s = 0;
        }
      }

      bool is_open(const implementation_type& impl) const
      {
        return impl.s != 0;
      }

      std::error_code close(implementation_type& impl, std::error_code& ec)
      {
        if(is_open(impl)){
          // cancel pending async operations
          if(check_error(ec, impl.funcs->closesocket(impl.s))){
            impl.s = 0;
          }else{
            return sockerror(ec);
          }
        }
        return success(ec);
      }

      std::error_code shutdown(implementation_type& impl, shutdown_type how, std::error_code& ec)
      {
        if(!check_open(impl, ec))
          return ec;
        check_error(ec, impl.funcs->shutdown(impl.s, static_cast<int>(how)));
        return ec;
      }

      std::error_code listen(implementation_type& impl, int backlog, std::error_code& ec)
      {
        if(!check_open(impl, ec))
          return ec;
        check_error(ec, impl.funcs->listen(impl.s, backlog ));
        return ec;
      }

      native_type native(implementation_type& impl)
      {
        return impl.s;
      }

      size_t available(const implementation_type& impl, std::error_code& ec) const
      {
        if(!check_open(impl, ec))
          return 0;
        uint32_t re = 0;
        check_error(ec, impl.funcs->ioctlsocket(impl.s, constants::fionread, &re));
        return re; //-V109
      }
      bool at_mark(const implementation_type& impl, std::error_code& ec) const
      {
        if(!check_open(impl, ec))
          return 0;
        uint32_t re = 0;
        check_error(ec, impl.funcs->ioctlsocket(impl.s, constants::siocatmark, &re));
        return re != 0;
      }

      bool wait(const implementation_type& impl, socket_base::wait_type check, std::error_code& ec)
      {
        if(!check_open(impl, ec))
          return ec;
        fd_set r, w, e;
        if(check & socket_base::read)
          r.set(impl.s);
        if(check & socket_base::write)
          w.set(impl.s);
        if(check & socket_base::error)
          e.set(impl.s);

        int re = impl.funcs->select(1, &r, &w, &e, nullptr);
        check_error(ec, re);
        if(re == 0)
          ec = std::make_error_code(stdnet::network_error::timed_out);
        return re == 1;
      }

      template <class Rep, class Period>
      bool wait_for(const implementation_type& impl, socket_base::wait_type check, const std::chrono::duration<Rep, Period>& rel_time, std::error_code& ec)
      {
        if(!check_open(impl, ec))
          return ec;
        fd_set r, w, e;
        if(check & socket_base::read)
          r.set(impl.s);
        if(check & socket_base::write)
          w.set(impl.s);
        if(check & socket_base::error)
          e.set(impl.s);
        
        timeval time;
        const std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(rel_time);
        time.sec  = (long)sec.count();
        time.usec = (long)std::chrono::duration_cast<std::chrono::seconds>(rel_time - sec).count();

        int re = impl.funcs->select(1, &r, &w, &e, &time);
        check_error(ec, re);
        if(re == 0)
          ec = std::make_error_code(stdnet::network_error::timed_out);
        return re == 1;
      }

    protected:
      // false if not open
      static bool check_open(const implementation_type& impl, std::error_code& ec)
      {
        if(impl.s == 0)
          ec = error::make_error_code(error::bad_file_descriptor);
        return impl.s != 0;
      }

    protected:
      std::error_code open(implementation_type& impl, int af, int type, int protocol, std::error_code& ec)
      {
        static const native_type invalid_socket = static_cast<socket>(-1);
        if(is_open(impl)){
          return ec = error::make_error_code(error::already_open);
        }
        impl.s = impl.funcs->socket(af, type, protocol);
        if(impl.s == invalid_socket){
          impl.s = 0;
          return sockerror(ec);
        }
        impl.proto_family = af,
          impl.proto_type = type,
          impl.proto_protocol = protocol,
          impl.ipv6 = af == constants::af_inet6,
          impl.is_stream = type == constants::sock_stream || constants::sock_seqpacket;
        return success(ec);
      }

      std::error_code assign(implementation_type& impl, const native_type& native_socket, std::error_code& ec)
      {
        if(is_open(impl)){
          return ec = error::make_error_code(error::already_open);
        }
        impl.s = native_socket;
        return success(ec);
      }

      size_t send(implementation_type& impl, const buffer_sequences& buffs, const sockaddr* addr, size_t addrlen, socket_base::message_flags flags, std::error_code& ec)
      {
        if(!check_open(impl, ec))
          return 0;
        const size_t max_size = buffs.total_size(), count = buffs.count();
        if(max_size == 0 && impl.is_stream)
          return 0;
        const buffer_sequences::native_buffer* buf = buffs.buffers();
        size_t transfered = 0;
        for(size_t i = 0; i < count && !ec; i++, buf++){
          uint32_t offset = 0;
          do {
            int re = addr
              ? impl.funcs->sendto(impl.s, buf->buf + offset, buf->len - offset, flags, addr, static_cast<int>(addrlen))
              : impl.funcs->send(impl.s, buf->buf + offset, buf->len - offset, flags);
            if(!check_error(ec, re))
              return transfered;
            assert(re >= 0);
            transfered += re;
            offset += re;
            if(addr)    // don't write more than one buffer without connection
              return transfered;
          } while(offset < buf->len);
        }
        return transfered;
      }

      size_t receive(implementation_type& impl, buffer_sequences& buffs, sockaddr* addr, size_t addrsize, socket_base::message_flags flags, std::error_code& ec)
      {
        if(!check_open(impl, ec))
          return 0;
        const size_t max_size = buffs.total_size(), count = buffs.count();
        if(max_size == 0 && impl.is_stream)
          return 0;
        int addrlen = static_cast<int>(addrsize);
        buffer_sequences::native_buffer* buf = buffs.buffers();
        size_t received = 0;
        for(size_t nbuf = 0; !ec && nbuf < count && received < max_size; nbuf++, buf++){
          uint32_t offset = 0;
          do {
            int re = addr
              ? impl.funcs->recvfrom(impl.s, buf->buf + offset, buf->len - offset, flags, addr, &addrlen)
              : impl.funcs->recv(impl.s, buf->buf + offset, buf->len - offset, flags);
            sockerror(ec);
            if(re == 0){
              if(!addr) // recv should fail with eof
                ec = std::make_error_code(error::eof);
              return received;
            } else if(re == socket_error){
              if(ec.value() != error::message_size)
                return received;
              continue;
            }
            assert(re > 0);
            received += re;
            offset += re;
            if(addr)    // don't read data without connection
              return received;
          } while(offset < buf->len);
        }
        return received;
      }

    };

    template<class Protocol>
    class socket_service:
      public socket_service_base
    {
      typedef socket_service_base daddy;
    public:
      typedef Protocol protocol_type;
      typedef typename Protocol::endpoint endpoint_type;

      struct implementation_type: socket_service_base::implementation_type
      {
        implementation_type()
          :proto(endpoint_type().protocol())
        {}
        Protocol proto;
      };

    public:
      socket_service(ios::io_service& /*io_service*/)
      {}

      // construct/destroy impl
      //void construct(implementation_type& impl)
      //{
      //  daddy::construct(impl);
      //}

      //void destroy(implementation_type& impl)
      //{
      //  daddy::destroy(impl);
      //}

      std::error_code open(implementation_type& impl, const protocol_type& protocol, std::error_code& ec)
      {
        daddy::open(impl, protocol.family(), protocol.type(), protocol.protocol(), ec);
        if(!ec)
          impl.proto = protocol;
        return ec;
      }

      std::error_code assign(implementation_type& impl, const protocol_type& protocol, const native_type& native_socket, std::error_code& ec)
      {
        daddy::assign(impl, native_socket, ec);
        if(!ec){
          impl.proto = protocol;
          impl.proto_family = protocol.family(),
            impl.proto_type = protocol.type(),
            impl.proto_protocol = protocol.protocol(),
            impl.ipv6 = impl.proto_family == constants::af_inet6,
            impl.is_stream = protocol.type() == constants::sock_stream || constants::sock_seqpacket;
        }
        return ec;
      }

      std::error_code cancel(implementation_type& impl, std::error_code& ec);

      template<class SettableSocketOption>
      std::error_code set_option(implementation_type& impl, const SettableSocketOption& option, std::error_code& ec)
      {
        static_assert(SettableSocketOption::level4 != -1, "socket control code should not be used as socket option!");
        if(!check_open(impl, ec))
          return ec;
        check_error(ec, impl.funcs->setsockopt(impl.s, option.level(impl.proto), option.name(impl.proto), 
          reinterpret_cast<const byte*>(option.data(impl.proto)), static_cast<int>(option.size(impl.proto))));
        return ec;
      }

      template<class GettableSocketOption>
      std::error_code get_option(const implementation_type& impl, GettableSocketOption& option, std::error_code& ec) const
      {
        static_assert(GettableSocketOption::level4 != -1, "socket control code should not be used as socket option!");
        if(!check_open(impl, ec))
          return ec;
        int len = static_cast<int>(option.size(impl.proto));
        check_error(ec, impl.funcs->getsockopt(impl.s, option.level(impl.proto), option.name(impl.proto), 
          reinterpret_cast<byte*>(option.data(impl.proto)), &len));
        return ec;
      }

      template<class IoControlCommand>
      std::error_code io_control(implementation_type& impl, IoControlCommand& command, std::error_code& ec)
      {
        static_assert(IoControlCommand::level4 == -1, "socket option code should not be used as socket control!");
        if(!check_open(impl, ec))
          return ec;
        check_error(ec, impl.funcs->ioctlsocket(impl.s, command.name(impl.proto), command.data(impl.proto)));
        return ec;
      }

      std::error_code bind(implementation_type& impl, const endpoint_type& endpoint, std::error_code& ec)
      {
        if(!check_open(impl, ec))
          return ec;
        check_error(ec, impl.funcs->bind(impl.s, endpoint.data(), static_cast<int>(endpoint.size()) ));
        return ec;
      }

      endpoint_type local_endpoint(const implementation_type& impl, std::error_code& ec) const
      {
        endpoint_type ep;
        if(!check_open(impl, ec))
          return ep;
        int size = static_cast<int>(ep.capacity());
        check_error(ec, impl.funcs->getsockname(impl.s, ep.data(), &size));
        return ep;
      }

      endpoint_type remote_endpoint(const implementation_type& impl, std::error_code& ec) const
      {
        endpoint_type ep;
        if(!check_open(impl, ec))
          return ep;
        int size = static_cast<int>(ep.capacity());
        check_error(ec, impl.funcs->getpeername(impl.s, ep.data(), &size));
        return ep;
      }

      std::error_code connect(implementation_type& impl, const endpoint_type& endpoint, std::error_code& ec)
      {
        if(!check_open(impl, ec))
          return ec;
        check_error(ec, impl.funcs->connect(impl.s, endpoint.data(), static_cast<int>(endpoint.size()) ));
        return ec;
      }

      std::error_code accept(implementation_type& impl, implementation_type& socket, endpoint_type* endpoint, std::error_code& ec)
      {
        if(!check_open(impl, ec))
          return ec;
        if(socket.s != 0)
          return ec = std::make_error_code(error::already_open);
        
        endpoint_type ep;
        int epsize = static_cast<int>(ep.size());
        socket.s = impl.funcs->accept(impl.s, ep.data(), &epsize);
        check_error(ec, static_cast<int>(socket.s));
        if(ec)
          socket.s = 0;
        else if(endpoint)
          *endpoint = ep;
        return ec;
      }

      template<class AcceptHandler>
      void async_accept(implementation_type& impl, implementation_type& socket, endpoint_type* endpoint, AcceptHandler handler);

      template<class ConnectHandler>
      void async_connect(implementation_type& impl, const endpoint_type& endpoint, ConnectHandler handler);

      template<class MutableBufferSequence>
      size_t receive(implementation_type& impl, const MutableBufferSequence& buffers, socket_base::message_flags flags, std::error_code& ec)
      {
        buffer_sequence<ios::mutable_buffer, MutableBufferSequence> buffs(buffers);
        return daddy::receive(impl, buffs, nullptr, 0, flags, ec);
      }

      template<class MutableBufferSequence>
      size_t receive_from(implementation_type& impl, const MutableBufferSequence& buffers, endpoint_type& sender, socket_base::message_flags flags, std::error_code& ec)
      {
        buffer_sequence<ios::mutable_buffer, MutableBufferSequence> buffs(buffers);
        return daddy::receive(impl, buffs, sender.data(), sender.capacity(), flags, ec);
      }

      template<class ConstBufferSequence>
      size_t send(implementation_type& impl, const ConstBufferSequence& buffers, socket_base::message_flags flags, std::error_code& ec)
      {
        buffer_sequence<ios::const_buffer, ConstBufferSequence> buffs(buffers);
        return daddy::send(impl, buffs, nullptr, 0, flags, ec);
      }

      template<class ConstBufferSequence>
      size_t send_to(implementation_type& impl, const ConstBufferSequence& buffers, const endpoint_type& destination, socket_base::message_flags flags, std::error_code& ec)
      {
        buffer_sequence<ios::const_buffer, ConstBufferSequence> buffs(buffers);
        return daddy::send(impl, buffs, destination.data(), destination.size(), flags, ec);
      }

      template<class MutableBufferSequence, class ReadHandler>
      void async_receive(implementation_type& impl, const MutableBufferSequence& buffers, socket_base::message_flags flags, ReadHandler handler);
      template<class MutableBufferSequence, class ReadHandler>
      void async_receive_from(implementation_type& impl, const MutableBufferSequence& buffers, endpoint_type& sender, socket_base::message_flags flags, ReadHandler handler);
      template<class ConstBufferSequence, class WriteHandler>
      void async_send(implementation_type& impl, const ConstBufferSequence& buffers, socket_base::message_flags flags, WriteHandler handler);
      template<class ConstBufferSequence, class WriteHandler>
      void async_send_to(implementation_type& impl, const ConstBufferSequence& buffers, const endpoint_type& destination, socket_base::message_flags flags, WriteHandler handler);
    };

  }
}}
