#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "wininet.lib")
#include <WinSock2.h>

#include <iostream>

#include <cwchar>

#include <string>
#include <string_view>

#include <functional>

#include <array>

#define		_NORETURN			[[noreturn]]
#define		_NODISCARD			[[nodiscard]]
#define		_DEPRECATED			[[deprecated]]
#define		_MAYBE_UNUSED		[[maybe_unused]]
#define		_FALLTHROUGH		[[fallthrough]]

namespace NETWORK_UTIL {
	namespace ERROR_HANDLING {
		_NORETURN void ERROR_QUIT(const WCHAR *msg);
		/*_DEPRECATED*/ void ERROR_DISPLAY(const CHAR *msg);
	}
}
#endif //PCH_H