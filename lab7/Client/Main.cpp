#include <windows.h>
#include <iostream>
#include <process.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

const int BUFFER_LENGTH = 2048;

void Receive(void* param)
{
	while (1)
	{
		SOCKET clientSocket = *(SOCKET*)(param);
		char receivingBuffer[BUFFER_LENGTH] = {};	

		if (recv(clientSocket, receivingBuffer, 2048, 0) == SOCKET_ERROR)
			cout << "Failed to receive the message!" << endl;
		else
			cout << "[Server] said: " << receivingBuffer << endl;
	}
}

void Send(void* param)
{
	while (1)
	{
		SOCKET clientSocket = *(SOCKET*)(param);
		char sendingBuffer[BUFFER_LENGTH] = {};

		cin.getline(sendingBuffer, BUFFER_LENGTH);

		if (send(clientSocket, sendingBuffer, strlen(sendingBuffer), 0) == SOCKET_ERROR)
			cout << "Failed to send the message!" << endl;
		else
			cout << "I said: " << sendingBuffer << endl;
	}
}

int main()
{
	WSADATA wsa;
	WORD version = MAKEWORD(2, 2);
	if (WSAStartup(version, &wsa) != 0)
		cout << "Failed to initializate the socket!" << endl;

	SOCKET clientSocket;
	if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
		cout << "Failed to create the socket!" << endl;

	struct sockaddr_in ServerAddress;
	memset(&ServerAddress, 0, sizeof(sockaddr_in));
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); 
	ServerAddress.sin_port = htons(60000);

	if (connect(clientSocket, (sockaddr*)&ServerAddress, sizeof(ServerAddress)) == SOCKET_ERROR)
	{
		cout << "Failed to connect to the server!" << endl;
		return 0;
	}

	_beginthread(Receive, 0, &clientSocket);
	_beginthread(Send, 0, &clientSocket);

	Sleep(INFINITE);	

	if (clientSocket != INVALID_SOCKET) 
	{
		closesocket(clientSocket);
		clientSocket = INVALID_SOCKET;
	}

	WSACleanup();
	cout << "Client is leaving!" << endl;
	return 0;
}
