#include <stdio.h>
#include <WinSock2.h>
#include <string.h>
#include "Helpers.h"

#define MAX_CLIENTS 30
#define FOR_EACH_CLIENT for (int i = 0; i < MAX_CLIENTS; ++i)

const char* welcome = "Welcome to chat! Type \"quit\" to exit.\n";
SOCKET clients[MAX_CLIENTS];
char client_names[MAX_CLIENTS][MAX_NAME_LEN];

/*
* Setup sockets to prepare for client connections  
*/
void sendMessage(const char* msg, SOCKET exclude_client)
{
	for (int j = 0; (j < MAX_CLIENTS) && (clients[j] != 0); ++j) {
		if (clients[j] != exclude_client) //...except the sender
			send(clients[j], msg, strlen(msg), 0);
	}
}

/*
* Accept new client connections. 
*/
void accept_new_connection(SOCKET master_sock) 
{
	int c = sizeof(struct sockaddr_in);
	int recv_len;
	SOCKET new_socket;
	struct sockaddr_in client;
	char message[MAX_MSG_LEN];

	if ((new_socket = accept(master_sock, (struct sockaddr*) &client, (int*) &c)) < 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}

	printf("%s:%d: Connected on socket %d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), new_socket);
	recv_len = recv(new_socket, message, MAX_MSG_LEN, 0);
	message[recv_len] = '\0';

	// Send greeting
	if (send(new_socket, welcome, strlen(welcome), 0) != strlen(welcome)) 
		perror("send failed");

	// Add to list of clients
	FOR_EACH_CLIENT {
		if (clients[i] == 0) {
			clients[i] = new_socket;

			if (strstr(message, setusername) == message) {
				strcpy(client_names[i], message + strlen(setusername) + 1);
				sprintf(message, "Server: %s has joined the server\0", client_names[i]);
				printf("%s\n", message);
				sendMessage(message, clients[i]);
			}

			break;
		}
	}
}

/*
* Receive and log messages from clients and send them back out to all other clients. 
*/
void receive_client_messages(const FD_SET* readfds) 
{
	char message[MAX_MSG_LEN];
	struct sockaddr_in client;
	int c, recv_len;

	// Otherwise its activity from an already connected client
	for (int i = 0; (i < MAX_CLIENTS) && (clients[i] != 0); ++i) {
		if(FD_ISSET(clients[i], readfds)) {
			getpeername(clients[i] , (struct sockaddr*)&client , (int*)&c);

			recv_len = recv(clients[i], message, MAX_MSG_LEN, 0);

			// Handle any disconnects first
			if ((recv_len == SOCKET_ERROR) || (recv_len == 0)) {
				printf("%s:%d: Disconnected on socket %d\n" , inet_ntoa(client.sin_addr) , ntohs(client.sin_port), clients[i]);

				if (strlen(client_names[i]) > 0) {
					sprintf(message, "Server: %s has left the server\0", client_names[i]);
					printf("%s\n", message);
					sendMessage(message, clients[i]);
				}

				closesocket( clients[i] );
				clients[i] = 0;
			} else { // Otherwise assume its a normal message
				message[recv_len] = '\0';
				printf("%s:%d: %s\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), message);
				sendMessage(message, clients[i]);
			}
		}
	}
}

/*
* Setup sockets to prepare for client connections  
*/
int initialize(SOCKET* master_sock)
{
	WSADATA wsa;
	struct sockaddr_in server;
	FOR_EACH_CLIENT{ 
		clients[i] = 0; 
		strcpy(client_names[i], "\0"); 
	}

	// Initialize WinSock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
		printf("Failed. Error Code : %d",WSAGetLastError());
		return 1;
	}
	puts("Initialised.");

	// Create master socket that will receive connections from clients
	if ((*master_sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket : %d\n", WSAGetLastError());
		return 1;
	}
	puts("Master socket created.");

	// Perpare socket sockaddr_in struct
	server.sin_addr.S_un.S_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);

	// Bind socket
	if (bind(*master_sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("Bind failed with error code: %d", WSAGetLastError());
		SHUTDOWN(*master_sock,1);
	}
}

/*
* Main 
*/
int main(int argc , char* argv[])
{
	SOCKET master_sock;
	
	FD_SET readfds;

	int initcode = initialize(&master_sock);
	switch (initcode) {
	case 1:
		return 1;
		break;

	case 2:
		SHUTDOWN(master_sock, 1);
		break;

	default:
		break;	
	}

	// Listen for connections
	listen(master_sock,3);

	// Accept connection
	puts("Waiting for incoming connection...");

	// Main execution loop
	
	while (true) {
		// Refresh the fd structure
		FD_ZERO(&readfds);
		FD_SET(master_sock, &readfds);
		FOR_EACH_CLIENT {
			if (clients[i] > 0) { FD_SET(clients[i], &readfds); }
		}

		// Wait for activity on a socket
		select(0, &readfds, NULL, NULL, NULL);

		// Activity on master means a new connection coming in
		if (FD_ISSET(master_sock, &readfds)) {
			accept_new_connection(master_sock);
		}

		receive_client_messages(&readfds);
	}

	SHUTDOWN(master_sock, 0);
}
