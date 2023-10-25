#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <algorithm>
#include <mutex>
#include <chrono>
#include <string>

using namespace std;

const int MAX_THREADS = 10;

mutex inputMtx;
mutex outputMtx;
mutex coutMtx;

struct ThreadData
{
    int id = -1;
    vector<int> data;
    vector<int> lines;
    bool ready = false;
};

void ReadFileLine(ifstream& input, ThreadData* threadData, int line)
{
    threadData->data.clear();

    string lineContent;
    getline(input, lineContent);

    if (lineContent == "") return;

    // splitting
    auto ptr = new char;
    char* char_array = new char[lineContent.length() + 1];
    strcpy_s(char_array, lineContent.length() + 1, lineContent.c_str());
    char* token = strtok_s(char_array, " ", &ptr);
    while (token != NULL)
    {
        double num = stof(token);

        if (num > 0)
            threadData->data.push_back(num);

        token = strtok_s(NULL, " ", &ptr);
    }

    delete[] char_array;
}

void SortThread(ThreadData* threadData)
{
    int dataSize = (int) threadData->data.size();
    while (dataSize--)
    {
        bool swapped = false;

        for (int i = 0; i < dataSize; i++)
        {
            if (threadData->data[i] > threadData->data[i + 1])
            {
                swap(threadData->data[i], threadData->data[i + 1]);
                swapped = true;
            }
        }

        if (swapped == false)
            break;
    }
}

void WriteFileLine(ofstream& output, ThreadData* threadData)
{
    for (int number : threadData->data)
        output << number << ' ';
    output << endl;
}

void ThreadFunction(ifstream& input, ofstream& output, ThreadData* threadData)
{
    unique_lock<mutex> coutLock1(coutMtx);
    cout << "Thread " << threadData->id << " is not ready." << endl;
    coutLock1.unlock();

    auto start_time = chrono::high_resolution_clock::now();

    for (int line : threadData->lines)
    {
        unique_lock<mutex> inputLock(inputMtx);
        ReadFileLine(input, threadData, line);
        inputLock.unlock();

        SortThread(threadData);

        unique_lock<mutex> outputLock(outputMtx);
        WriteFileLine(output, threadData);
        outputLock.unlock();
    }

    auto end_time = chrono::high_resolution_clock::now();

    unique_lock<mutex> coutLock2(coutMtx);
    cout << "Thread " << threadData->id << " is ready." << endl;
    coutLock2.unlock();

    threadData->ready = true;

    chrono::duration<double> elapsed_time = end_time - start_time;

    unique_lock<mutex> coutLock3(coutMtx);
    cout << "Thread " << threadData->id << " completed in " << elapsed_time.count() << " seconds." << endl;
    coutLock3.unlock();
}

int main() 
{
    int numArrays, arraySize, numThreads;

    std::cout << "Enter the number of arrays: ";
    std::cin >> numArrays;
    std::cout << "Enter the size of each array: ";
    std::cin >> arraySize;
    std::cout << "Enter the number of threads: ";
    std::cin >> numThreads;

    numThreads = min(numThreads, MAX_THREADS);

    ofstream inputFile("input.txt");
    for (int i = 0; i < numArrays; i++) 
    {
        for (int j = 0; j < arraySize; ++j)
            inputFile << rand() % 1000 << ' ';
        inputFile << '\n';
    }
    inputFile.close();

    ifstream input("input.txt");
    ofstream output("output.txt");

    vector<thread> threads(numThreads);
    vector<ThreadData> threadDataArray(numThreads);
    
    // lines initialization
    int fragmentSize = (int) ceil((double) numArrays / numThreads);
    for (int i = 0; i < numThreads; i++)
    {
        for (int j = 0; j < fragmentSize; j++)
        {
            int numLine = i * fragmentSize + j + 1;

            if (numLine > numArrays)
                break;
            else
                threadDataArray[i].lines.push_back(numLine);
        }
    }
    
    // initializing of the threads array
    for (int i = 0; i < numThreads; i++)
    {
        threadDataArray[i].id = i + 1;
        threadDataArray[i].ready = false;

        threads[i] = thread(ThreadFunction, ref(input), ref(output), &threadDataArray[i]);
    }

    std::cout << "Sorting array fragments with " << numThreads << " threads started." << endl;

    for (std::thread& thread : threads)
        thread.join();

    // waiting for all threads to end
    while (true)
    {
        bool allThreadsReady = true;
        for (const ThreadData& threadData : threadDataArray)
            if (!threadData.ready)
            {
                allThreadsReady = false;
                break;
            }
        if (allThreadsReady) break;
    }

    input.close();
    output.close();

    cout << "Sorting completed." << endl;

    return 0;
}
