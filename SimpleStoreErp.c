// SimpleStoreErp.c : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include "common.h"

HINSTANCE yinyue200_hInstance;
int yinyue200_nCmdShow;
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    yinyue200_hInstance = hInstance;
    yinyue200_nCmdShow = nCmdShow;
    CreateMainWindow();
    return MessageLoop();
}

int MessageLoop()
{
    // Run the message loop.

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}



