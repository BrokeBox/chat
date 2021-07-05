#include <stdio.h>
#include <WinSock2.h>
#include <conio.h>

#include "Helpers.h"

int initialize(SOCKET* sock, const char* name)
{
	WSADATA wsa;
	u_long mode = 1;  // 1 to enable non-blocking socket
	struct sockaddr_in server;
	char message[MAX_MSG_LEN];

	// Initialize WinSock
	printf("\nInitialising...");
	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
		printf("Failed. Error Code : %d",WSAGetLastError());
		return 1;
	}

	// Create socket
	if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket : %d\n", WSAGetLastError());
		return 1;
	}

	// Connect socket to server
	server.sin_addr.S_un.S_addr = inet_addr("192.168.0.20");
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);
	if (connect(*sock, (struct sockaddr*) &server, sizeof(server)) < 0) {
		puts("connect error");
		return 2;
	}

	sprintf(message, "%s %s\0", setusername, name);
	send(*sock, message, strlen(message), 0); 
	puts("Connected");

	ioctlsocket(*sock, FIONBIO, &mode); // Make socket non-blocking
}

int main(int argc, char* argv[])
{
	SOCKET sock;
	char message[MAX_MSG_LEN], input[MAX_MSG_LEN], server_reply[MAX_MSG_LEN], name[MAX_NAME_LEN];
	unsigned int recv_size;
	
	printf("username:");
	scanf("%s", name);
	
	int initcode = initialize(&sock, name);
	switch (initcode) {
	case 1:
		return 1;
		break;

	case 2:
		SHUTDOWN(sock, 1);
		break;

	default:
		break;	
	}

	// Main execution loop
	while (true){
		// Get user input
		if (_kbhit()){
			gets(input);
			if (strlen(input) > 0) {
			// Quit gracefully
				if (strcmp(input, "quit") == 0) {
					send(sock, "", strlen(0), 0); // Let the server know we're disconnecting
					SHUTDOWN(sock,0)
				}

				// Send message to server
				sprintf(message, "%s: %s\0", name, input);
				if (send(sock, message, strlen(message), 0) < 0) {
					puts("Send failed");
				}
				printf("%s: ", name);
			}
		}

		// Get server traffic
		if ((recv_size = recv(sock, server_reply, MAX_MSG_LEN, 0)) != SOCKET_ERROR) {
			server_reply[recv_size] = '\0';
			printf("\r%s\n", server_reply);
			printf("%s: ", name);
		} 
	}

	SHUTDOWN(sock,0)
}
