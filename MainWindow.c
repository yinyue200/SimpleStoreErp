#include "common.h"
#include "EditItemWindow.h"
#define ID_MENU_ABOUT 1
#define ID_MENU_VWS 2
#define ID_MENU_ADDRECORD 3
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateMainWindow()
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.SimpleStoreErp.MainWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = MainWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);
    //printf("%d", result);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"我的小店出入库管理系统",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        yinyue200_hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        int a = GetLastError();
        return 0;
    }

    ShowWindow(hwnd, yinyue200_nCmdShow);


    return 0;
}
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        AddWindowCount();

        HMENU hMenubar = CreateMenu();
        HMENU hFile = CreateMenu();
        HMENU hFind = CreateMenu();
        HMENU hAdd = CreateMenu();
        HMENU hAddItem = CreateMenu();
        HMENU hAns = CreateMenu();
        HMENU hUsr = CreateMenu();
        HMENU hHelp = CreateMenu();
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFile, L"文件");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFind, L"查询");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hAdd, L"新增");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hAns, L"统计");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hUsr, L"用户管理");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hHelp, L"帮助");



        AppendMenu(hHelp, MF_STRING, ID_MENU_VWS, L"GitHub");
        AppendMenu(hHelp, MF_STRING, ID_MENU_ABOUT, L"关于");
        AppendMenu(hAdd, MF_STRING, ID_MENU_ADDRECORD, L"添加单个条目");

        SetMenu(hwnd, hMenubar);

        HWND hwndButton = CreateWindow(//see https://docs.microsoft.com/en-us/windows/win32/controls/buttons
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"OK",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
            10,         // x position 
            10,         // y position 
            100,        // Button width
            100,        // Button height
            hwnd,     // Parent window
            NULL,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.
    }
    return 0;
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == ID_MENU_VWS)
        {
            ShellExecute(NULL, L"Open", L"https://github.com/yinyue200", NULL, NULL, SW_SHOWNORMAL);
        }
        else if (LOWORD(wParam) == ID_MENU_ABOUT)
        {
            MessageBox(NULL, L"版本 0.0.1\r\nA GUI program\r\nold fashion design", L"关于", 0);
        }
        else if (LOWORD(wParam) == ID_MENU_ADDRECORD)
        {
            CreateEditItemWindow();
        }
    }
    return 0;
    case WM_DESTROY:
        DecreaseWindowCount();
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);



        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
