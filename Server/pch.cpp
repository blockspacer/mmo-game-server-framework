#include "pch.h"

namespace NETWORK_UTIL
{
	namespace ERROR_HANDLING
	{
		_NORETURN void ERROR_QUIT(const WCHAR *msg)
		{
			LPVOID lpMsgBuf;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				WSAGetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&lpMsgBuf,
				0,
				NULL
			);

			MessageBox(NULL, (LPTSTR)lpMsgBuf, msg, MB_ICONERROR);
			LocalFree(lpMsgBuf);
			exit(1);
		};

		/*_DEPRECATED*/ void ERROR_DISPLAY(const CHAR *msg)
		{
			LPVOID lpMsgBuf;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				WSAGetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&lpMsgBuf,
				0,
				NULL
			);

			printf(" [%s]  %s", msg, (LPTSTR)&lpMsgBuf);
			LocalFree(lpMsgBuf);
		};
	}
}