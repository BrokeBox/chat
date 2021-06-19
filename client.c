#include <stdio.h>
#include <WinSock2.h>

int main(int argc , char* argv[])
{
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char* message;
	char server_reply[2000];
	int recv_size;

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
	}
	puts("Socket created.");

	// Connect socket to server
	server.sin_addr.S_un.S_addr = inet_addr("192.168.0.20"); // Google homepage
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);

	if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0) {
		puts("connect error");
		return 1;
	}
	puts("Connected");

	// Send some data
	message = "Hello?";
	if (send(s, message, strlen(message), 0) < 0) {
		puts("Send failed");
		return 1;
	}
	puts("Data Send\n");

	// Receive some data
	if ((recv_size = recv(s, server_reply, 2000, 0)) == SOCKET_ERROR){
		puts("recv failed");
	}
	puts("Reply received");
	server_reply[recv_size] = '\0';
	puts(server_reply);

	// Shutdown
	closesocket(s);
	WSACleanup();

	return 0;
}