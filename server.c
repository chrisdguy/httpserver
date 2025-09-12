#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_CONNECTIONS 5

int main() {
	// Initialize the socket
	int sock_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_desc == -1) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	
	// Binds socket to an ip address and port number & starts listening
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(42069);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	struct sockaddr *server_addr_ptr = (struct sockaddr *)&server_addr;
	socklen_t server_len = sizeof(server_addr);
	
	if (bind(sock_desc, server_addr_ptr, server_len) < 0) {
		perror("Failed to bind.");
		exit(EXIT_FAILURE);
	}
	
	if (listen(sock_desc, MAX_CONNECTIONS) < 0) {
		perror("Unable to listen.");
		exit(EXIT_FAILURE);
	}
	
	// Accepts incomming connection & prints client info
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	struct sockaddr *client_addr_ptr = (struct sockaddr *)&client_addr;
	int client_sock = accept(sock_desc, client_addr_ptr, &client_len);
	if (client_sock < 0) {
		perror("Unable to accept connection.");
		exit(EXIT_FAILURE);
	} else {
		printf("Client connected from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
	}
	
	// Clean up
	close(client_sock);
	close(sock_desc);
	
	return 0;
}