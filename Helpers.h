#include <WinSock2.h>

#define SHUTDOWN(s, rtn) cleanup(&s); return rtn;

#define MAX_MSG_LEN 1024
#define MAX_NAME_LEN 100
#define true 1
#define false 0

typedef char bool;

void cleanup(SOCKET* s) {
    closesocket(*s);
	WSACleanup();
}

const char* setusername = "\\setusername";