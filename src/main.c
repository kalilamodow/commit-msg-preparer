#define UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <consoleapi.h>
#include <minwindef.h>
#include <processenv.h>
#include <winbase.h>
#include <windef.h>
#include <wingdi.h>
#include <winuser.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "gdi32")

#include <wchar.h>

#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT 200

UINT currentDpi;
HANDLE hConsole;
HFONT font;

int DpiScale(UINT input)
{
    return MulDiv(input, currentDpi, 96); // 96 is the windows dpi scale factor or something
}

void InitFont()
{
    font = CreateFont(DpiScale(24), 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                      CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE, L"Segoe UI");
}

void SizeMainWindow(HWND hwnd)
{
    SetWindowPos(hwnd, NULL, 0, 0, DpiScale(WINDOW_WIDTH), DpiScale(WINDOW_HEIGHT),
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void print(const wchar_t *text)
{
    WriteConsole(hConsole, text, wcslen(text), NULL, NULL);
    WriteConsole(hConsole, L"\r\n", 2, NULL, NULL);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE: {
        return 0;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        SelectObject(hdc, font);
        TextOut(hdc, 8, 8, L"hi", 2);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY: {
        PostQuitMessage(22);
        return 0;
    }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main(void)
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    const wchar_t CLASS_NAME[] = L"pre commit window";
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hInstance = GetModuleHandle(NULL);

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Pre-commit", WS_CAPTION | WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT,
                               WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

    currentDpi = GetDpiForWindow(hwnd);
    InitFont();
    SizeMainWindow(hwnd);
    ShowWindow(hwnd, SW_SHOW);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
