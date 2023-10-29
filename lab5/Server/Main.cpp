#include <windows.h>
#include <iostream>  
#include <process.h>

using namespace std;

#pragma comment(lib, "WS2_32.lib") 

const int BUFFER_LENGTH = 2048;

void Receive(void* param)
{
	while (1)
	{
		SOCKET serverSocket = *(SOCKET*)(param);
		char receivingBuffer[BUFFER_LENGTH] = {};

		if (recv(serverSocket, receivingBuffer, BUFFER_LENGTH, 0) == SOCKET_ERROR)
			cout << "Failed to receive the message!" << endl;
		else
			cout << "[Client] said: " << receivingBuffer << endl;
	}
}

void Send(void* param)
{
	while (1)
	{
		SOCKET serverSocket = *(SOCKET*)(param);
		char sendingBuffer[BUFFER_LENGTH] = {};

		cin.getline(sendingBuffer, BUFFER_LENGTH);

		if (send(serverSocket, sendingBuffer, strlen(sendingBuffer), 0) == SOCKET_ERROR)
			cout << "Failed to send the message!" << endl;
		else
			cout << "I said: " << sendingBuffer << endl;
	}
}

int main()
{
	// socket initialization
	WSADATA wsa; // WSA - Windows sockets API
	WORD version = MAKEWORD(2, 2);
	if (WSAStartup(version, &wsa) != 0) 
	{
		cout << "Failed to initializate the socket!" << endl;
		return 0;
	}

	// socket creation
	SOCKET serverSocket;
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
	{
		cout << "Failed to create the socket!" << endl;
		return 0;
	}

	// socket binding
	struct sockaddr_in ServerAddress;
	memset(&ServerAddress, 0, sizeof(sockaddr_in)); 
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // local IP
	ServerAddress.sin_port = htons(60000); // port number
	if (bind(serverSocket, (sockaddr*)&ServerAddress, sizeof(ServerAddress)) == SOCKET_ERROR)
	{
		cout << "Failed to bind the socket!" << endl;
		return 0;
	}
	
	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cout << "Monitoring failure!" << endl;
		return 0;
	}
	else
		cout << "Server is monitoring." << endl;

	// receiving of client connection
	sockaddr_in clientAddress;
	SOCKET clientSocket = INVALID_SOCKET;
	int addressLength = sizeof(clientAddress);
	if ((clientSocket = accept(serverSocket, (sockaddr*)&clientAddress, &addressLength)) == INVALID_SOCKET)
	{
		cout << "Failed to receive the client connection!" << endl;
		return 0;
	}
	else
		cout << "Server received the client connection." << endl;

	_beginthread(Receive, 0, &clientSocket);
	_beginthread(Send, 0, &clientSocket);

	Sleep(INFINITE);

	closesocket(clientSocket);
	closesocket(serverSocket);

	WSACleanup();
	cout << "Server stopped!" << endl;

	return 0;
}
