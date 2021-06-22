#include <stdio.h>
#include <WinSock2.h>
#include "Helpers.h"

#define MAX_CLIENTS 30
#define FOR_EACH_CLIENT for (int i = 0; i < MAX_CLIENTS; ++i)

const char* welcome = "Welcome to chat! Type \"quit\" to exit.\n";

int main(int argc , char* argv[])
{
	WSADATA wsa;
	SOCKET sock, master_sock, new_socket, clients[MAX_CLIENTS];
	struct sockaddr_in server, client;
	int c, recv_len;
    char* message;
    char client_msg[MAX_MSG_LEN];
    FD_SET readfds;

    FOR_EACH_CLIENT{ clients[i] = 0; }

	// Initialize WinSock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		return 1;
	}
	puts("Initialised.");

	// Create master socket that will receive connections from clients
	if ((master_sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket : %d\n", WSAGetLastError());
        return 1;
	}
	puts("Master socket created.");

    // Perpare socket sockaddr_in struct
	server.sin_addr.S_un.S_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);

    // Bind socket
    if (bind(master_sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d", WSAGetLastError());
        SHUTDOWN(master_sock,1);
    }

    // Listen for connections
    listen(master_sock,3);

    // Accept connection
    puts("Waiting for incoming connection...");

    // Main execution loop
    c = sizeof(struct sockaddr_in);
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
            if ((new_socket = accept(master_sock , (struct sockaddr *)&client, (int *)&c))<0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("%s:%d: Connected on socket %d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), new_socket);
            
            // Send greeting
            if (send(new_socket, welcome, strlen(welcome), 0) != strlen(welcome)) { perror("send failed"); }

            // Add to list of clients
            FOR_EACH_CLIENT {
                if (clients[i] == 0) {
                    clients[i] = new_socket;
                    break;
                }
            }
        } 

        // Otherwise its activity from an already connected client
        for (int i = 0; (i < MAX_CLIENTS) && (clients[i] != 0); ++i) {
            if(FD_ISSET(clients[i], &readfds)) {
                getpeername(clients[i] , (struct sockaddr*)&client , (int*)&c);

                recv_len = recv(clients[i], client_msg, MAX_MSG_LEN, 0);

                // Handle any disconnects first
                if ((recv_len == SOCKET_ERROR) || (recv_len == 0)) {
                    //Somebody disconnected , get his details and print
                    printf("%s:d: Disconnected on socket %d\n" , inet_ntoa(client.sin_addr) , ntohs(client.sin_port), clients[i]);
                    
                    //Close the socket and mark as 0 in list for reuse
                    closesocket( clients[i] );
                    clients[i] = 0;
                }
                else { // Otherwise assume its a normal message
                    client_msg[recv_len] = '\0';
                    printf("%s:%d: %s\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), client_msg);
                    // Send the message out to all clients...
                    for (int j = 0; (j < MAX_CLIENTS) && (clients[j] != 0); ++j) {
                        if (j != i) { //...except the sender
                            send(clients[j] , client_msg , strlen(client_msg), 0);
                        }
                    }
                }
            }
        }
    }

    SHUTDOWN(master_sock,0);
}
