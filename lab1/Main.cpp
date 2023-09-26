#include <windows.h>
#include <tchar.h>

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

// define ID for the close button
#define BUTTON_ID 0x01

// serves for processing messages which receives window
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void DrawGradientCircle(HDC hdc, int x, int y, int radius);

const wchar_t* windowBitmapPath = L"cat.bmp";
const wchar_t* buttonBitmapPath = L"button.bmp";
const wchar_t* ClassName = L"CustomerWindow";

int WIDTH = 700;
int HEIGHT = 700;
int BUTTON_SIZE = 15;


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdLine, int nCmdLine)
{
	// initialization of GDI+ lib
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// creating a brush from a bitmap to use it as window background
	HBRUSH brush = CreatePatternBrush((HBITMAP)LoadImage(NULL, windowBitmapPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));

	// define and register the window class
	WNDCLASSEX windowClass;
	// zeroing the structure for the subsequent establishment of the necessary parameters
	ZeroMemory(&windowClass, sizeof(windowClass));
	windowClass.cbSize = sizeof(windowClass);
	windowClass.hInstance = hInst;
	windowClass.lpfnWndProc = WndProc;
	windowClass.hbrBackground = brush;
	windowClass.lpszClassName = ClassName;

	// exit if something went wrong
	if (!RegisterClassEx(&windowClass))
		return EXIT_FAILURE;

	// to center of our screen
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int x = (screenWidth - WIDTH) / 2;
	int y = (screenHeight - HEIGHT) / 2;
	
	HWND windowHandle = CreateWindowEx(WS_EX_LAYERED,
		ClassName,
		NULL, // without title
		WS_POPUP, // with flags
		x, y, WIDTH, HEIGHT, 
		NULL, NULL, hInst, NULL);

	if (windowHandle)
	{
		// white = transparent color
		COLORREF transparentColor = RGB(255, 255, 255);
		SetLayeredWindowAttributes(windowHandle, transparentColor, 0, LWA_COLORKEY);
		
		ShowWindow(windowHandle, nCmdLine);
		UpdateWindow(windowHandle);
	}
	else
	{
		MessageBox(NULL, L"Error", L"Something went wrong creating the window", NULL);
		return EXIT_FAILURE;
	}

	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);

		// to WndProc
		DispatchMessage(&message);
	}
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return message.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		// if creating window
		case WM_CREATE:
		{
			// creating button
			HWND closedButton = CreateWindow(L"button", NULL, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_BITMAP, 
											 WIDTH / 2, 0, BUTTON_SIZE, BUTTON_SIZE, hwnd, 
											 (HMENU)BUTTON_ID, GetModuleHandle(NULL), NULL);

			// loading button's image
			HBITMAP buttonBitmap = (HBITMAP)LoadImage(NULL, buttonBitmapPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
			if (!buttonBitmap)
				MessageBox(NULL, L"Error", L"Can't find such image!", NULL);

			// changing button's image
			SendMessage(closedButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)buttonBitmap);

			break;
		}

		// if painting window
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			DrawGradientCircle(hdc, 180, 170, 100);
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

		// if command
		case WM_COMMAND:
		{
			if (wParam == BUTTON_ID)
				DestroyWindow(hwnd);
			break;
		}

		// if window was closed
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;

		// if window was destroyed
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		// else go to default window procedure
		default:
			// roughly speaking super()
			return DefWindowProc(hwnd, message, wParam, lParam);
			break;
	}

	return 0;
}

void DrawGradientCircle(HDC hdc, int x, int y, int radius)
{
	Gdiplus::Graphics graphics(hdc);

	Gdiplus::Point center(x, y);
	Gdiplus::Color startColor(255, 250, 250);
	Gdiplus::Color endColor(255, 180, 122);

	Gdiplus::LinearGradientBrush gradientBrush(Gdiplus::Point(x - radius, y), 
											   Gdiplus::Point(x + radius, y), 
											   startColor, endColor);

	Gdiplus::GraphicsPath path;
	path.AddEllipse(x - radius, y - radius, 2 * radius, 2 * radius);

	graphics.FillPath(&gradientBrush, &path);
}
