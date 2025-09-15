#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define INITIAL_BUFFER_SIZE 1024
#define MAX_CONNECTIONS 5
#define PORT 42069

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
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	struct sockaddr *server_addr_ptr = (struct sockaddr *)&server_addr;
	socklen_t server_len = sizeof(server_addr);
	int yes = 1;
	if (setsockopt(sock_desc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) != 0) {
		perror("Failed to set socket options.");
		exit(EXIT_FAILURE);
	}
	
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
	
	// Reads request to a buffer
	int buffer_size = INITIAL_BUFFER_SIZE;
	char *buffer = malloc(INITIAL_BUFFER_SIZE);
	if (!buffer) {
		perror("malloc failed");
		exit(EXIT_FAILURE);
	}
	
	int total_bytes = 0;
	while (total_bytes < buffer_size - 1) {
		long bytes_received = recv(client_sock, buffer, buffer_size - 1 - total_bytes, 0);
		if (bytes_received < 0) {
			perror("Unable to receive from client");
			exit(EXIT_FAILURE);
		} else if (bytes_received == 0) {
			// connection was closed
		} else {
			total_bytes += bytes_received;
			buffer[total_bytes] = '\0';
			if (strstr(buffer, "\r\n\r\n")) break;
		}
	}
	printf("Request:\n%s\n", buffer);
	free(buffer);

	
	// Clean up
	close(client_sock);
	close(sock_desc);
	
	return 0;
}