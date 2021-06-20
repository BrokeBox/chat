#include <stdio.h>
#include <WinSock2.h>

#include "Helpers.h"

int main(int argc , char* argv[])
{
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char message[MAX_MSG_LEN];
	char name[100];

    printf( "username:");
    scanf("%s", name);

	// Initialize WinSock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		return 1;
	}

	// Create socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket : %d\n", WSAGetLastError());
	}

    // Connect socket to server
	server.sin_addr.S_un.S_addr = inet_addr("192.168.0.20");
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);

	if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0) {
		puts("connect error");
		SHUTDOWN(s,1)
	}
	puts("Connected");

	// Receive some data
    unsigned int recv_size;
    char server_reply[MAX_MSG_LEN];

    while (true){
        gets(message);
        if (send(s, message, strlen(message), 0) < 0)
        {
            puts("Send failed");
        }
        printf("%s: ", name);
	}

    SHUTDOWN(s,0)
}
