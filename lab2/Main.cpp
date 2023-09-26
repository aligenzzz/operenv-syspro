#include <windows.h>
#include <fstream>

// define ID for the increment button
#define BUTTON_ID 0x01
#define WS_CUSTOMWSTYLE (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_POPUP)


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void LoadData();
void SaveData(int counter, int number);
void StartNewProcess();

const wchar_t* ClassName = L"RecoveredProcess";

int COUNTER = 0;
int NUMBER = 1;
int MAX_COUNTER = 5;

int WIDTH = 180;
int HEIGHT = 120;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdLine) 
{
    LoadData();
    COUNTER++;

    WNDCLASSEX windowClass;
    ZeroMemory(&windowClass, sizeof(windowClass));
    windowClass.cbSize = sizeof(windowClass);
    windowClass.hInstance = hInst;
    windowClass.lpfnWndProc = WndProc;
    windowClass.lpszClassName = ClassName;

    if (!RegisterClassEx(&windowClass))
        return EXIT_FAILURE;

    // to center of our screen
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenWidth - WIDTH) / 2;
    int y = (screenHeight - HEIGHT) / 2;

    HWND windowHandle = CreateWindow(
            ClassName,
            NULL,
            WS_CUSTOMWSTYLE,
            x, y, WIDTH, HEIGHT, 
            NULL, NULL, hInst, NULL);

    if (windowHandle) 
    {
        ShowWindow(windowHandle, nCmdLine);
        UpdateWindow(windowHandle);
        
    }
    else
    {
        MessageBox(NULL, L"Error", L"Something went wrong creating the window", NULL);
        return EXIT_FAILURE;
    }    

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
        {
            HWND button = CreateWindowW(L"button", L"Click me", WS_VISIBLE | WS_CHILD,
                                        30, 10, 100, 30, 
                                        hwnd, (HMENU)BUTTON_ID, 
                                        GetModuleHandle(NULL), NULL);
            break;
        }
        
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            wchar_t buffer[100];
            swprintf_s(buffer, L"Number: %d", NUMBER);

            TextOut(hdc, 30, 50, buffer, wcslen(buffer));

            EndPaint(hwnd, &ps);

            break;
        }

        // if mouse click
        case WM_NCHITTEST:
        {
            // if the inner area of the window return window caption => move window without caption
            if (DefWindowProc(hwnd, message, wParam, lParam) == HTCLIENT)
                return HTCAPTION;
        }

        
        case WM_COMMAND:
        {
            if (wParam == BUTTON_ID)
            {
                NUMBER++;
                RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
            }
                
            break;
        }

        case WM_CLOSE:
        {
            SaveData(COUNTER, NUMBER);

            if (COUNTER < MAX_COUNTER)
                StartNewProcess();
            else
                SaveData(0, 1);

            DestroyWindow(hwnd);

            break;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}

void LoadData()
{
    int counter = 0, number = 0;
    std::ifstream file("data.txt");

    if (file.is_open()) 
    {
        file >> counter;
        file >> number;
        file.close();
    }

    COUNTER = counter;
    NUMBER = number;
}

void SaveData(int counter, int number)
{
    std::ofstream file("data.txt");

    if (file.is_open())
    {
        file << counter << std::endl;
        file << number;
        file.close();
    }
}

void StartNewProcess()
{
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);

    // startup new process with the same exe
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    CreateProcess(exePath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
