#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

const int BUFFER_LENGTH = 2048;
HANDLE hPipe;

void Receive()
{
    char receivingBuffer[BUFFER_LENGTH] = {};

    while (true)
    {
        DWORD bytesRead;
        DWORD bytesAvailable;

        if (PeekNamedPipe(hPipe, NULL, 0, NULL, &bytesAvailable, NULL) && bytesAvailable > 0)
            if (ReadFile(hPipe, receivingBuffer, BUFFER_LENGTH, &bytesRead, NULL))
                cout << "[Client] said: " << receivingBuffer << endl;
            else
            {
                cout << "Failed to receive the message!" << endl;
                break;
            }
    }
}

void Send()
{
    string input;
    char sendingBuffer[BUFFER_LENGTH] = {};

    while (true)
    {
        getline(cin, input);
        strncpy_s(sendingBuffer, input.c_str(), BUFFER_LENGTH);

        DWORD bytesWritten;
        if (!WriteFile(hPipe, sendingBuffer, strlen(sendingBuffer) + 1, &bytesWritten, NULL))
        {
            cout << "Failed to send the message!" << endl;
            break;
        }
        else
            cout << "I said: " << sendingBuffer << endl;
    }
}

int main()
{
    hPipe = CreateNamedPipe(
        L"\\\\.\\pipe\\MyPipe",
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1, BUFFER_LENGTH, BUFFER_LENGTH,
        0,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE)
    {
        cout << "Failed to create the pipe!" << endl;
        return 1;
    }

    cout << "Waiting for client connection..." << endl;

    if (ConnectNamedPipe(hPipe, NULL))
    {
        cout << "Client connected to the pipe." << endl;
        Sleep(1000);

        HANDLE hThreadReceive = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Receive, NULL, 0, NULL);
        HANDLE hThreadSend = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Send, NULL, 0, NULL);

        WaitForSingleObject(hThreadReceive, INFINITE);
        WaitForSingleObject(hThreadSend, INFINITE);
    }
    else
        cout << "Failed to connect to the pipe!" << endl;

    CloseHandle(hPipe);
    cout << "Server stopped!" << endl;

    return 0;
}
