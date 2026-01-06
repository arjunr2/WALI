// Client side C/C++ program to demonstrate Socket
// programming

#include "common.h"
#define PORT 8080

int main()
{
	int sock = 0, valread, client_fd;
	struct sockaddr_in serv_addr;
	char* hello = "Hello from client";
	char buffer[1024] = { 0 };
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		print("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
		<= 0) {
		print(
			"\nInvalid address/ Address not supported \n");
		return -1;
	}

	if ((client_fd
		= connect(sock, (struct sockaddr*)&serv_addr,
				sizeof(serv_addr)))
		< 0) {
		print("\nConnection Failed \n");
		return -1;
	}
	send(sock, hello, strlen(hello), 0);
	print("Hello message sent\n");
	valread = read(sock, buffer, 1024);
	PRINT_STR("CLIENT Buffer", buffer);

	// closing the connected socket
	close(client_fd);
	return 0;
}

