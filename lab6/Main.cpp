#include <iostream>
#include <thread>
#include <Windows.h>
#include <mutex>

using namespace std;

HANDLE semaphore;
mutex outputMtx;

int resource = 0;

void PrintCurrentTime()
{
    SYSTEMTIME currentTime;
    GetLocalTime(&currentTime);

    printf("[%02d:%02d:%02d] ", currentTime.wHour, currentTime.wMinute, currentTime.wSecond);
}

void Reader(int id) 
{
    while (true) 
    {
        DWORD result = WaitForSingleObject(semaphore, 0);

        if (result == WAIT_OBJECT_0) 
        {
            unique_lock<mutex> lock(outputMtx);
           
            PrintCurrentTime();
            cout << "Reader " << id << " read resource." << endl;

            lock.unlock();

            ReleaseSemaphore(semaphore, 1, NULL);
        }
        else if (result == WAIT_TIMEOUT) 
        {
            unique_lock<mutex> lock(outputMtx);

            PrintCurrentTime();
            cout << "Reader " << id << " does not have access." << endl;

            lock.unlock();
        }

        // reading emulation
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

void Writer(int id) 
{
    while (true) 
    {
        WaitForSingleObject(semaphore, INFINITE);

        resource = id;

        unique_lock<mutex> lock(outputMtx);

        PrintCurrentTime();
        cout << "Writer " << id << " wrote resource." << endl;
        
        lock.unlock();

        ReleaseSemaphore(semaphore, 1, NULL);

        // writing emulation
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

int main() 
{
    semaphore = CreateSemaphore(NULL, 1, 1, NULL);

    int readersNum, writersNum;

    cout << "Enter the number of readers: ";
    cin >> readersNum;
    cout << "Enter the number of writers: ";
    cin >> writersNum;

    std::thread* readers = new std::thread[readersNum];
    std::thread* writers = new std::thread[writersNum];

    for (int i = 0; i < readersNum; i++)
        readers[i] = thread(Reader, i + 1);
    for (int i = 0; i < writersNum; i++)
        writers[i] = thread(Writer, i + 1);

    for (int i = 0; i < readersNum; i++)
        readers[i].join();
    for (int i = 0; i < writersNum; i++) 
        writers[i].join();

    CloseHandle(semaphore);

    return 0;
}