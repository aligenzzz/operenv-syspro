#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;

const int MAX_THREADS = 10;
mutex mtx;

struct ThreadData 
{
    int id = -1;
    vector<int> data;
    bool ready = false;
};

void SortThread(ThreadData* threadData) 
{
    auto start_time = std::chrono::high_resolution_clock::now();

    unique_lock<mutex> lock1(mtx);
    cout << "Thread " << threadData->id << " is not ready." << endl;
    lock1.unlock();

    int dataSize = threadData->data.size();
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

    unique_lock<mutex> lock2(mtx);
    cout << "Thread " << threadData->id << " is ready." << endl;
    lock2.unlock();

    auto end_time = std::chrono::high_resolution_clock::now();

    threadData->ready = true;

    chrono::duration<double> elapsed_time = end_time - start_time;

    unique_lock<mutex> lock3(mtx);
    cout << "Thread " << threadData->id << " completed in " << elapsed_time.count() << " seconds.\n";
    lock3.unlock();
}

int main() 
{
    int arraySize, numThreads;

    cout << "Enter the size of the array: ";
    cin >> arraySize;
    cout << "Enter the number of threads: ";
    cin >> numThreads;

    numThreads = min(numThreads, MAX_THREADS);

    vector<int> arrayToSort(arraySize);
    vector<thread> threads(numThreads);
    vector<ThreadData> threadDataArray(numThreads);

    // initializing of the array with random values
    for (int i = 0; i < arraySize; i++)
        arrayToSort[i] = rand() % 1000;

    // dividing of the array into equal parts
    int fragmentSize = arraySize / numThreads;

    // initializing of the threads array
    for (int i = 0; i < numThreads; i++) 
    {
        threadDataArray[i].id = i + 1;
        threadDataArray[i].data = std::vector<int>(
            arrayToSort.begin() + i * fragmentSize,
            arrayToSort.begin() + (i + 1) * fragmentSize
        );
        threadDataArray[i].ready = false;

        threads[i] = thread(SortThread, &threadDataArray[i]);
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

    // merging of the sorted fragments
    vector<int> result(arraySize); 
    for (const ThreadData& threadData : threadDataArray) 
        result.insert(result.end(), threadData.data.begin(), threadData.data.end());
    cout << "Sorting completed." << endl;

    return 0;
}
