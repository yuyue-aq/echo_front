#include "framework.h"
#include "WindowsProject4.h"
#include <windowsx.h>
#include<stdio.h>

#define MAX_LOADSTRING 100
#define IDC_MY_BUTTON 102 // 定义按钮ID
#define IDC_CHAT_DISPLAY 1001  // 聊天显示区域
#define IDC_CHAT_INPUT   1002  // 聊天输入框
#define IDC_SEND_BUTTON  1003  // 发送按钮

// 全局变量:
HINSTANCE hInst;                                // 当前实例
HWND hWndMain;                                  // 主窗口句柄（全局）
BOOL g_bConnected = FALSE;                      // 连接状态（全局）
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Link(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    handleLink(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Linksuccess(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Linkfail(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Linkfailhandle(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT4, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT4));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT4));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT4);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 将实例句柄存储在全局变量中

    // 设定窗口大小（宽度300，高度700）
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 360, 700, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    hWndMain = hWnd; // 保存主窗口句柄到全局变量
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        // 创建聊天显示区域（多行编辑控件）
        CreateWindowW(
            L"EDIT",                  // 控件类名
            L"",                      // 初始文本
            WS_CHILD | WS_VISIBLE | WS_BORDER |
            ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | ES_WANTRETURN | ES_RIGHT,
            10, 10, 300, 400,        // 位置和大小
            hWnd, (HMENU)IDC_CHAT_DISPLAY,
            hInst, NULL);

        // 创建聊天输入框（多行编辑控件）
        CreateWindowW(
            L"EDIT",                  // 控件类名
            L"",                      // 初始文本
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL |
            ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
            10, 420, 300, 100,       // 位置和大小（高度调整为100以容纳多行文本）
            hWnd, (HMENU)IDC_CHAT_INPUT,
            hInst, NULL);

        // 创建发送按钮
        CreateWindowW(
            L"BUTTON",                // 控件类名
            L"发送",                  // 按钮文本
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            220, 540, 90, 30,         // 位置和大小
            hWnd, (HMENU)IDC_SEND_BUTTON,
            hInst, NULL);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWndMain, About);
            break;
        case IDM_Link:
            // 主菜单点击逻辑（如需弹出子菜单需额外处理）
            break;
        case IDM_LINK_SUB1:
            // 模拟连接逻辑（实际应调用真实连接函数）
            // 模拟连接逻辑，设置连接状态
            g_bConnected = FALSE; // 这里设置为连接失败，实际应调用真实连接函数

            // 显示 Link 对话框，它会自动根据 g_bConnected 状态处理后续逻辑
            DialogBox(hInst, MAKEINTRESOURCE(IDD_LINKBOX), hWndMain, Link);
            break;
        case IDM_LINK_SUB2:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_handlelinkbox), hWndMain, handleLink);
            // 模拟第二个连接逻辑
            g_bConnected = FALSE; // 0表示连接失败，1表示成功
            if (g_bConnected) {
                DialogBox(hInst, MAKEINTRESOURCE(IDD_connected), hWndMain, Linksuccess);
            }
            else {
                DialogBox(hInst, MAKEINTRESOURCE(IDD_failconnecthandle), hWndMain, Linkfailhandle);
            }
            break;
        case IDM_EXIT:
            DestroyWindow(hWndMain);
            break;
        case IDC_SEND_BUTTON:
        {
            // 获取输入框文本
            wchar_t inputText[256] = { 0 };
            HWND hInput = GetDlgItem(hWndMain, IDC_CHAT_INPUT);
            GetWindowTextW(hInput, inputText, 256);

            if (wcslen(inputText) > 0)
            {
                // 获取当前显示内容
                wchar_t displayText[1024] = { 0 };
                HWND hDisplay = GetDlgItem(hWndMain, IDC_CHAT_DISPLAY);
                GetWindowTextW(hDisplay, displayText, 1024);

                // 构造新的显示内容
                wchar_t newText[1024] = { 0 };
                if (wcslen(displayText) > 0)
                {
                    wcscat_s(newText, sizeof(newText) / sizeof(newText[0]), displayText);
                    wcscat_s(newText, sizeof(newText) / sizeof(newText[0]), L"\r\n");
                }
                wcscat_s(newText, sizeof(newText) / sizeof(newText[0]), L"You: ");
                wcscat_s(newText, sizeof(newText) / sizeof(newText[0]), inputText);

                // 更新显示内容
                SetWindowTextW(hDisplay, newText);

                // 滚动到底部
                SendMessageW(hDisplay, EM_LINESCROLL, 0, INT_MAX);

                // 清空输入框
                SetWindowTextW(hInput, L"");
            }
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_KEYDOWN:
        if (GetFocus() == GetDlgItem(hWndMain, IDC_CHAT_INPUT))
        {
            if (wParam == VK_RETURN)
            {
                // 模拟点击发送按钮
                SendMessageW(hWndMain, WM_COMMAND,
                    MAKEWPARAM(IDC_SEND_BUTTON, BN_CLICKED), 0);
            }
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 在此处添加使用 hdc 的任何绘图代码...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Link(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_INITDIALOG:
        // 对话框初始化后立即检查连接状态
        if (!g_bConnected) // 如果连接失败
        {
            // 关闭当前对话框
            EndDialog(hDlg, IDCANCEL);

            // 显示连接失败对话框
            DialogBox(hInst, MAKEINTRESOURCE(IDD_failconnect), hWndMain, Linkfail);
        }
        else // 如果连接成功
        {
            // 可以选择显示连接成功对话框或执行其他操作
            EndDialog(hDlg, IDOK);
            DialogBox(hInst, MAKEINTRESOURCE(IDD_connected), hWndMain, Linksuccess);
        }
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// 其他对话框函数（保持不变）
INT_PTR CALLBACK handleLink(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Linksuccess(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Linkfail(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Linkfailhandle(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}