#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <CommCtrl.h>
#include <consoleapi.h>
#include <corecrt_search.h>
#include <libloaderapi.h>
#include <minwinbase.h>
#include <minwindef.h>
#include <processenv.h>
#include <processthreadsapi.h>
#include <winbase.h>
#include <windef.h>
#include <wingdi.h>
#include <winnt.h>
#include <winuser.h>

// makes it look modern
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <wchar.h>

#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT 150

UINT currentDpi;
HANDLE hConsole;
HFONT font;
enum
{
    IDC_OKBUTTON = 100,
    IDC_CANCELBUTTON,
    IDC_NAMEINPUT,
    IDC_TITLETEXT
};

int DpiScale(UINT input)
{
    return MulDiv(input, currentDpi, 96); // 96 is the windows dpi scale factor or something
}

void InitFont()
{
    font = CreateFont(DpiScale(16), 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                      CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE, "Segoe UI");
}

void SizeMainWindow(HWND hwnd)
{
    SetWindowPos(hwnd, NULL, 0, 0, DpiScale(WINDOW_WIDTH), DpiScale(WINDOW_HEIGHT),
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void print(const char *text, const size_t length)
{
    WriteConsole(hConsole, text, length, NULL, NULL);
    WriteConsole(hConsole, "\r\n", 2, NULL, NULL);
}

#pragma function(memset)
void *__cdecl memset(void *dest, int byte, size_t length)
{
    char *d = (char *)dest;
    while (length--)
        *d++ = (char)byte;
    return dest;
}

LPSTR ValidateName(LPSTR buf)
{
    char *character = buf;

    while (*character)
    {
        if (*character < L'a' || *character > L'z')
            return "Lowercase letters only";

        character++;
    }

    if (character - buf < 3)
        return "Name too short";

    if (character - buf > 8)
        return "Name too long";

    return NULL;
}

HWND titleHwnd;
HWND nameInputHwnd;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE: {
        currentDpi = GetDpiForWindow(hwnd);
        InitFont();
        SizeMainWindow(hwnd);
        HINSTANCE hInstance = GetModuleHandle(NULL);

        titleHwnd = CreateWindowEx(0, "STATIC", "Enter your name:", WS_CHILD | WS_VISIBLE, DpiScale(10), DpiScale(10),
                                   DpiScale(265), DpiScale(24), hwnd, (HMENU)IDC_TITLETEXT, hInstance, NULL);
        SendMessage(titleHwnd, WM_SETFONT, (WPARAM)font, TRUE);

        nameInputHwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE, DpiScale(10), DpiScale(36),
                                       DpiScale(265), DpiScale(24), hwnd, (HMENU)IDC_NAMEINPUT, hInstance, NULL);
        SendMessage(nameInputHwnd, WM_SETFONT, (WPARAM)font, TRUE);

        HWND okBtnHwnd =
            CreateWindowEx(0, "BUTTON", "OK", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, DpiScale(140),
                           DpiScale(80), DpiScale(64), DpiScale(24), hwnd, (HMENU)IDC_OKBUTTON, hInstance, NULL);
        SendMessage(okBtnHwnd, WM_SETFONT, (WPARAM)font, TRUE);

        HWND cancelBtnHwnd =
            CreateWindowEx(0, "BUTTON", "Cance", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, DpiScale(210),
                           DpiScale(80), DpiScale(64), DpiScale(24), hwnd, (HMENU)IDC_CANCELBUTTON, hInstance, NULL);
        SendMessage(cancelBtnHwnd, WM_SETFONT, (WPARAM)font, TRUE);

        return 0;
    }
    case WM_COMMAND: {
        switch (wParam)
        {
        case IDC_OKBUTTON: {
            char buffer[128];
            int length = GetWindowText(nameInputHwnd, buffer, 128);

            LPSTR error = ValidateName(buffer);
            if (error)
            {
                MessageBox(hwnd, error, "Invalid name", MB_OK | MB_ICONERROR);
                PostQuitMessage(1);
            }
            else
            {
                print(buffer, length);
                PostQuitMessage(0);
            }

            return 0;
        }
        case IDC_CANCELBUTTON: {
            print("Cancelled", 9);
            PostQuitMessage(1);
            return 0;
        }
        }
        return 0;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wParam;
        HWND control = (HWND)lParam;

        if (control == titleHwnd)
        {
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(0, 0, 0));
            return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
        }

        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(1);
        return 0;
    }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int CustomEntry(void)
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    const char CLASS_NAME[] = "p";
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hInstance = GetModuleHandle(NULL);

    WNDCLASS wc = {0};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Pre-commit", WS_CAPTION | WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT,
                               WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, SW_SHOW);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    ExitProcess(msg.wParam);
}
