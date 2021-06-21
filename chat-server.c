#include <stdio.h>
#include <WinSock2.h>

#include "Helpers.h"

int main(int argc , char* argv[])
{
	WSADATA wsa;
	SOCKET s, new_socket;
	struct sockaddr_in server, client;
	int c, recv_len;
    char* message;
    char client_reply[MAX_MSG_LEN];

	// Initialize WinSock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		return 1;
	}
	puts("Initialised.");

	// Create socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket : %d\n", WSAGetLastError());
        return 1;
	}
	puts("Socket created.");

    // Perpare socket sockaddr_in struct
	server.sin_addr.S_un.S_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);

    // Bind socket
    if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d", WSAGetLastError());
        SHUTDOWN(s,1);
    }

    // Listen for connections
    listen(s,3);

    // Accept connection
    puts("Waiting for incoming connection...");

    c = sizeof(struct sockaddr_in);
    while ((new_socket = accept(s, (struct sockaddr*)&client, &c)) != INVALID_SOCKET) {
        char* client_ip = inet_ntoa(client.sin_addr);
        int client_port = ntohs(client.sin_port);
        printf("Connection accepted from %s:%d\n", client_ip, client_port);

        //Receive a message from client
        while ( (recv_len = recv(new_socket , client_reply , MAX_MSG_LEN , 0)) > 0 ) {
            //Send the message back to client
            printf("%s:%d: %s\n", client_ip, client_port, client_reply);
            send(new_socket , client_reply , strlen(client_reply), 0);
        }
    }

    if (new_socket == INVALID_SOCKET){
        printf("accept failed with error code: %d", WSAGetLastError());
        SHUTDOWN(s,1);
    }

    // Shutdown
    SHUTDOWN(s,0);
}
