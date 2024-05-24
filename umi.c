/*
* CLI command to have client connect to server
* ./umi {ipaddr} {port} {option} {file} 
*/

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 1024

const char* version = "0.1.0";

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s <arguments>\n", argv[0]);
		return 1;
	}

	if (strcmp(argv[1], "-v") == 0) {
		printf("%s\n", version);
		return 0;
	}
	const char* SERVER_IP = argv[1];
	const char* SERVER_PORT = argv[2];
	const char* options = argv[3];
	const char* file = argv[4]; 

	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		printf("WSAStartup failed: %d\n", result);
		exit(EXIT_FAILURE);
	}
	
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		printf("Socket failed to create: %d\n", WSAGetLastError());
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(SERVER_PORT));
	inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);
	
	result = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR) {
		printf("Failed to connect to server: %d\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	printf("Connected to %s:%s\n", SERVER_IP, SERVER_PORT);	

	if (strcmp(options, "push") == 0) {
		sendFile(clientSocket, file);	
	}

	if (strcmp(options, "pull") == 0) {
		printf("pull"); // search algo for file search
	}	
	
	closesocket(clientSocket);
	WSACleanup();

	return 0;
}

void sendFile(SOCKET socket, const char* filename) {
	FILE* file;
	char buffer[BUF_SIZE];
	size_t bytes_read;
	long file_size;

	file = fopen(filename, "rb") {
		if (file == NULL) {
			printf("Failed to open file %s\n", filename);
			return;
		}
	}

	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *base_filename = strrchr(filename, '\\');
	if (base_filename == NULL) {
		base_filename = (char*)filename;
	} else {
		base_filename++;
	}
	int filename_len = strlen(base_filename);
	result = send(clientSocket, (char*)&filename_len, sizeof(int), 0);
	if (result == SOCKET_ERROR) {
		printf("Failed to send data to %s:%s: %d\n", SERVER_IP, SERVER_PORT, WSAGetLastError());
		exit(EXIT_FAILURE);		
	}
	send(clientSocket, base_filename, filename_len, 0);
	
	send(clientSocket, (char*)&file_size, sizeof(long), 0);
	
	while ((bytes_read = fread(buffer, 1, BUF_SIZE, file)) > 0) {
		send(clientSocket, buffer, bytes_read, 0);
	}

	printf("File %s sent successfully\n", filename);

	fclose(file);
}



