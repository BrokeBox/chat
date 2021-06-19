#include <stdio.h>
#include <WinSock2.h>

int main(int argc , char* argv[])
{
	WSADATA wsa;
	SOCKET s, new_socket;
	struct sockaddr_in server, client;
	int c;
    char* message;

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

    // Perpare socket sockaddr_in struct
	server.sin_addr.S_un.S_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);

    // Bind socket
    if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d", WSAGetLastError());
    }
    puts("Bind done");

    // Listen for connections
    listen(s,3);

    // Accept connection
    puts("Waiting for incoming connection...");

    c = sizeof(struct sockaddr_in);
    while ((new_socket = accept(s, (struct sockaddr*)&client, &c)) != INVALID_SOCKET) {
        puts("Connection accepted");
        
        // Reply
        message = "Hello Client, I have received your connection. But I have to go now, bye\n";
        send(new_socket, message, strlen(message), 0);
    }

    if (new_socket == INVALID_SOCKET){
        printf("accept failed with error code: %d", WSAGetLastError());
    }

    // Shutdown
	closesocket(s);
	WSACleanup();

    return 0;
}