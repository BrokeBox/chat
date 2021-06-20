#include <WinSock2.h>

#define SHUTDOWN(s, rtn) cleanup(&s); return rtn;

#define MAX_MSG_LEN 1024
#define true 1
#define false 0

void cleanup(SOCKET* s) {
    closesocket(*s);
	WSACleanup();
}
