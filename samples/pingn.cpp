#include <consoleapp.hxx>
#include <iostream>
#include <iomanip>
#include <tr2/network.hxx>
#include <tr2/timer.hxx>
#include <functional>

namespace sys = std::tr2::sys;
namespace net = std::tr2::network;
namespace ip = net::ip;

using ntl::win::boolean;
using namespace net::placeholders;
using namespace std::literals;

static const auto& _error = std::tr2::network::placeholders::error;
static const auto& _bytes = std::tr2::network::placeholders::bytes_transferred;

namespace std
{
	inline std::ostream& operator<< (std::ostream& os, const std::chrono::milliseconds& duration)
	{
		auto d = duration.count() < 0 ? -duration : duration;
		const char sign = duration.count() < 0 ? '-' : (os.flags() & os.showpos ? '+' : '\0');
		using std::chrono::duration_cast;
		const auto h = duration_cast<std::chrono::hours>(d); d -= h;
		const auto min = duration_cast<std::chrono::minutes>(d); d -= min;
		const auto sec = duration_cast<std::chrono::seconds>(d); d -= sec;
		const auto ms = duration_cast<std::chrono::milliseconds>(d);

		os.unsetf(os.showpos);
		if(sign != '\0')
			os << sign;

		if(h.count())
			os << h.count() << ':';
		if(min.count() || h.count())
			os << std::setfill('0') << std::setw(2) << min.count() << ':' << std::setfill('0') << std::setw(2);
		os  << sec.count();
		if(ms.count())
			os << '.' << ms.count();
		os << 's';
		return os;
	}

	template <class Rep, class Period>
	inline std::ostream& operator<< (std::ostream& os, const std::chrono::duration<Rep, Period>& duration)
	{
		return os << std::chrono::duration_cast<std::chrono::milliseconds>(duration);
	}
}

// from boost_asio/example/cpp03/icmp/
class ipv4_header
{
public:
	ipv4_header() { ntl::bzero(rep_); }

	unsigned char version() const { return (rep_[0] >> 4) & 0xF; }
	unsigned short header_length() const { return (rep_[0] & 0xF) * 4; }
	unsigned char type_of_service() const { return rep_[1]; }
	unsigned short total_length() const { return decode(2, 3); }
	unsigned short identification() const { return decode(4, 5); }
	bool dont_fragment() const { return (rep_[6] & 0x40) != 0; }
	bool more_fragments() const { return (rep_[6] & 0x20) != 0; }
	unsigned short fragment_offset() const { return decode(6, 7) & 0x1FFF; }
	unsigned int time_to_live() const { return rep_[8]; }
	unsigned char protocol() const { return rep_[9]; }
	unsigned short header_checksum() const { return decode(10, 11); }

	ip::address_v4 source_address() const
	{
		ip::address_v4::bytes_type bytes
			= { { rep_[12], rep_[13], rep_[14], rep_[15] } };
		return ip::address_v4(bytes);
	}

	ip::address_v4 destination_address() const
	{
		ip::address_v4::bytes_type bytes
			= { { rep_[16], rep_[17], rep_[18], rep_[19] } };
		return ip::address_v4(bytes);
	}

	friend std::istream& operator>>(std::istream& is, ipv4_header& header)
	{
		is.read(reinterpret_cast<char*>(header.rep_), 20);
		if (header.version() != 4)
			is.setstate(std::ios::failbit);
		std::streamsize options_length = header.header_length() - 20;
		if (options_length < 0 || options_length > 40)
			is.setstate(std::ios::failbit);
		else
			is.read(reinterpret_cast<char*>(header.rep_) + 20, options_length);
		return is;
	}

private:
	unsigned short decode(int a, int b) const
	{ return (rep_[a] << 8) + rep_[b]; }

	unsigned char rep_[60];
};

// from boost_asio/example/cpp03/icmp/
class icmp_header
{
public:
	enum icmp_type: uint8_t {
    echo_reply = 0, destination_unreachable = 3, source_quench = 4,
		redirect = 5, echo_request = 8, time_exceeded = 11, parameter_problem = 12,
		timestamp_request = 13, timestamp_reply = 14, info_request = 15,
		info_reply = 16, address_request = 17, address_reply = 18
  };

	icmp_header() { ntl::bzero(rep_); }

	icmp_type type() const { return static_cast<icmp_type>(rep_[0]); }
	unsigned char code() const { return rep_[1]; }
	unsigned short checksum() const { return decode(2, 3); }
	unsigned short identifier() const { return decode(4, 5); }
	unsigned short sequence_number() const { return decode(6, 7); }

	void type(unsigned char n) { rep_[0] = n; }
	void code(unsigned char n) { rep_[1] = n; }
	void checksum(unsigned short n) { encode(2, 3, n); }
	void identifier(unsigned short n) { encode(4, 5, n); }
	void sequence_number(unsigned short n) { encode(6, 7, n); }

	friend std::istream& operator>>(std::istream& is, icmp_header& header)
	{ return is.read(reinterpret_cast<char*>(header.rep_), 8); }

	friend std::ostream& operator<<(std::ostream& os, const icmp_header& header)
	{ return os.write(reinterpret_cast<const char*>(header.rep_), 8); }

private:
	unsigned short decode(int a, int b) const
	{ return (rep_[a] << 8) + rep_[b]; }

	void encode(int a, int b, unsigned short n)
	{
		rep_[a] = static_cast<unsigned char>(n >> 8);
		rep_[b] = static_cast<unsigned char>(n & 0xFF);
	}

	unsigned char rep_[8];
};

template <typename Iterator>
void compute_checksum(icmp_header& header,
											Iterator body_begin, Iterator body_end)
{
	unsigned int sum = (header.type() << 8) + header.code()
		+ header.identifier() + header.sequence_number();

	Iterator body_iter = body_begin;
	while (body_iter != body_end)
	{
		sum += (static_cast<unsigned char>(*body_iter++) << 8);
		if (body_iter != body_end)
			sum += static_cast<unsigned char>(*body_iter++);
	}

	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	header.checksum(static_cast<unsigned short>(~sum));
}

class pinger:
  public std::enable_shared_from_this<pinger>
  , ntl::noncopyable
{
  typedef ip::icmp icmp;
  typedef std::chrono::system_clock clock;

public:
  explicit pinger(sys::io_service& io, std::string_ref host, size_t len = 32, size_t max_send = 0, unsigned delay = 1,
                  unsigned proto = 4, bool queit = false)
    : io(io)
    , timer(io)
    , socket(io)
    , ver(proto == 6 ? icmp::v6() : icmp::v4())
    , host(host)
    , endpoint(resolve(host))
    , length(len)
    , max(max_send)
    , delay(delay)
    , queit(queit)
    , id(int16_t(intptr_t(this)))
    , seq()
    , replied()
    , sent_count()
    , recv_count()
  {
  }

  ~pinger()
  {
    stop();
  }

  void start()
  {
    if(endpoint.address().is_unspecified())
      return;

    std::cout << "PING " << host << " (" << endpoint.address() << "): " << length << " data bytes" << std::endl;

    ec.clear();
    socket.open(ver, ec);
    start_time = clock::now();
    send();
    recv();
  }

  void stop()
  {
    if(!socket.is_open())
      return;

    stop_time = clock::now();
    std::error_code e;
    socket.close(e);
    timer.cancel(e);
  }

  int stats()
  {
    if(sent_count == 0)
      return 1;

    const auto lost = sent_count - recv_count;
    const auto loss = (lost*100) / sent_count;

    std::cout 
      << "--- " << host << " ping statistics ---\n"
      << sent_count << " packets transmitted, "
      << recv_count << " packets received, "
      << loss << "% packet loss, "
      << "time " << (stop_time-start_time) << "\n"
      << std::flush;

    return recv_count == 0 ? 1 : 0;
  }

  const std::error_code error() const { return ec; }

protected:
  icmp::endpoint resolve(std::string_ref const& host)
  {
    icmp::resolver dns(io);
    auto it = dns.resolve(icmp::resolver::query(ver, host, ""), ec);
    return ec ? icmp::endpoint() : *it;
  }

  std::string generate_data(size_t size) const
  {
    char sig[32];
    std::snprintf(sig, _countof(sig), "%04u", size);
    const uint32_t num = *(uint32_t*)sig;

    std::string s(size, '\0');
    for(size_t i = 0, end = size; i < end; i += 4) {
      *reinterpret_cast<uint32_t*>(&s[i]) = num;
    }
    s.c_str();
    return std::move(s);
  }

  auto echo_request()
  {
    const auto body = generate_data(length);

    icmp_header echo_request;
    echo_request.type(icmp_header::echo_request);
    echo_request.code(0);
    echo_request.identifier(id);
    echo_request.sequence_number(++seq);
    compute_checksum(echo_request, body.begin(), body.end());

    send_buf.consume(send_buf.size());
    std::ostream os(&send_buf);
    os << echo_request << body;
    return send_buf.data();
  }

protected:

  void send()
  {
    if(ec)
      return;

    if(max && sent_count == max) {
      stop();
      return;
    }

    const auto req = echo_request();
    send_time = clock::now();
    socket.send_to(req, endpoint, ec);
    
    sent_count++;
    replied = false;
    timer.expires_from_now(5s);
    timer.async_wait(std::bind(&pinger::on_timer, shared_from_this(), _error));
  }

  void recv()
  {
    recv_buf.consume(recv_buf.size());
    socket.async_receive(recv_buf.prepare(0xFFFF), std::bind(&pinger::on_recv, shared_from_this(), _error, _bytes));
  }

  void on_timer(const std::error_code& /*ec*/)
  {
    if(!replied && !queit)
      std::cout << "Request timed out." << std::endl;

    if(max && sent_count == max) {
      stop();
      return;
    }

    // send packet after 1sec
    timer.expires_at(send_time + delay);
    timer.async_wait(std::bind(&pinger::send, shared_from_this()));
  }

  void on_recv(const std::error_code& ec, size_t received)
  {
    if(ec) {
      stop();
      return;
    }

    replied = true;
    recv_count++;
    recv_time = clock::now();
    const auto elapsed = recv_time - send_time;

    recv_buf.commit(received);
    std::istream is(&recv_buf);

    ipv4_header ipv4_hdr;
    icmp_header icmp_hdr;
    is >> ipv4_hdr >> icmp_hdr;

    const auto type = icmp_hdr.type();

    if(!queit && is && icmp_hdr.type() == icmp_header::echo_reply && icmp_hdr.identifier() == id)
    {
      std::cout << received - ipv4_hdr.header_length() - sizeof(icmp_header)
        << " bytes from " << ipv4_hdr.source_address()
        << ": icmp_seq=" << icmp_hdr.sequence_number()
        << ", ttl=" << ipv4_hdr.time_to_live()
        << ", time=" << elapsed
        << std::endl;
    }

    // fire on_timer(aborted) event to restart timer
    timer.cancel(this->ec);
    recv();
  }

private:
  sys::io_service& io;
  sys::deadline_timer timer;
  std::error_code ec;

  const std::string host;
  const icmp ver;
  const icmp::endpoint endpoint;
  const std::chrono::seconds delay;
  const size_t length;
  const size_t max;
  const bool queit;

  size_t sent_count, recv_count;
  bool replied;

  const uint16_t id;
  uint16_t seq;

  icmp::socket socket;
  clock::time_point send_time, recv_time, start_time, stop_time;
  sys::streambuf send_buf, recv_buf;
};

std::shared_ptr<pinger> ping;

boolean __stdcall ctrl(uint32_t /*evt*/)
{
  if(ping)
    ping->stop();
  return true;
}

NTL_EXTERNAPI
  boolean __stdcall SetConsoleCtrlHandler(decltype(ctrl)*, boolean fAdd);

int ntl::consoleapp::main()
{

  // options
  unsigned max_count = 5;
  unsigned size = 32;
  unsigned proto = 4;
  unsigned delay = 1;
  bool quiet = false;

  ntl::consoleapp::command_line cmdl;
  if(cmdl.size() < 2) {
    std::cerr << "usage: ping [-n 5] [-l 32] [-w 1] [-4] [-6] [-q] host" << std::endl;
    return 1;
  }

  unsigned* value = nullptr;
  for(auto it = cmdl.begin() + 1, end = cmdl.end() - 1; it != end; ++it) {
    auto& cmd = *it;

    if(cmd.starts_with("-")) {
      value = nullptr;

    } else if(value) {
      *value = ntl::numeric::strtoul(cmd.data());
      value = nullptr;
      continue;
    }

    // common options
    if(cmd      == "-n")
      value = &max_count;
    else if(cmd == "-l")
      value = &size;
    else if(cmd == "-4")
      proto = 4;
    else if(cmd == "-6")
      proto = 6;
    else if(cmd == "-w")
      value = &delay;

    // linux options
    else if(cmd == "-q")
      quiet = true;


    else if(cmd.starts_with("-"))
      std::cerr << "error: unknown option " << cmd << std::endl;
  }

  std::error_code ec;
  sys::io_service io;

  SetConsoleCtrlHandler(&ctrl, true);
  ping = std::make_shared<pinger>(io, *cmdl.crbegin(), size, max_count, delay, proto, quiet);

  ping->start();

  io.run(ec);

  std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> cvt;
  if(ping->error())
    std::wcerr << "error " << ping->error() << ": " << cvt.from_bytes(ping->error().message()) << std::endl;

  int status = ping->stats();
  std::exit(status);
}
