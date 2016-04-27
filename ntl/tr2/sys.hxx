#pragma once

// sys::
#ifdef NTL__STLX_TR2_IOSERVICE_FWD
	namespace sys = std::tr2::sys;
#endif

// fs::
#ifdef NTL__STLX_TR2_FILES
	namespace fs = std::tr2::files;
#endif

#ifdef NTL__STLX_TR2_NETWORKFWD
	namespace net = std::tr2::network;
#endif

#ifdef NTL__STLX_TR2_NETWORK_PROTOCOL
	using std::tr2::network::ip::tcp;
	using std::tr2::network::ip::udp;
#endif

// string and chrono literals
using namespace std::literals;
