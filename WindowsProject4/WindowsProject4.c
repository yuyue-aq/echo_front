#include "framework.h"
#include "WindowsProject4.h"
#include "cli_core.h"
#include <windowsx.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_LOADSTRING 100
#define IDC_MY_BUTTON 102 // 定义按钮ID
#define IDC_CHAT_DISPLAY 1001  // 聊天显示区域
#define IDC_CHAT_INPUT   1002  // 聊天输入框
#define IDC_SEND_BUTTON  1003  // 发送按钮

// 定义回调函数类型
typedef int (*MessageCallback)(const char*);

// 全局变量:
HINSTANCE hInst;                                // 当前实例
HWND hWndMain;                                  // 主窗口句柄（全局）
BOOL g_bConnected = FALSE;                      // 连接状态（全局）
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
MessageCallback g_messageCallback = NULL;       // 消息回调函数指针
SERVER* g_server=NULL;                          // 服务器连接实例

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    divLink(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    handleLink(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Linksuccess(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Linkfail(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Linkfailhandle(HWND, UINT, WPARAM, LPARAM);
void AppendChatMessage(const wchar_t* msg);
void AppendChatMessageFromChar(const char* msg);

// 设置消息回调函数
void SetMessageCallback(MessageCallback callback)
{
    g_messageCallback = callback;
}

// 调用回调函数
void CallMessageCallback(const char* msg)
{
    if (g_messageCallback && msg)
    {
        g_messageCallback(msg);
    }
}

// 消息回调函数实现
int OnMessageReceived(const char* msg, int len)
{
    if (msg && len > 0)
    {
        // 将接收到的消息显示在聊天框中
        AppendChatMessageFromChar(msg);
    }
    return 0;
}

// 初始化网络
BOOL InitializeNetwork()
{
    if (cli_core_init() != 0)
    {
        MessageBoxW(hWndMain, L"网络初始化失败", L"错误", MB_ICONERROR);
        return FALSE;
    }
    return TRUE;
}

// 连接到服务器
BOOL ConnectToServer(const char* ip, int port)
{
    int wsaerror = 0;
    if (cli_core_login(ip, port, OnMessageReceived, &wsaerror) != 0)
    {
        MessageBoxW(hWndMain, L"连接服务器失败", L"错误", MB_ICONERROR);
        return FALSE;
    }
    g_bConnected = TRUE;
    return TRUE;
}

// 断开连接
void DisconnectFromServer()
{
    if (g_bConnected)
    {
        cli_core_logout(g_server);
        g_bConnected = FALSE;
    }
}

// 发送消息
BOOL SendChatMessage(const char* msg)
{
    if (!g_bConnected || !msg)
        return FALSE;

    // 调用新添加的 cli_core_send 接口
    if (cli_core_send(g_server, msg, (int)strlen(msg)) != 0)
    {
        MessageBoxW(hWndMain, L"发送消息失败", L"错误", MB_ICONERROR);
        return FALSE;
    }
    return TRUE;
}

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

    // 初始化网络
    if (cli_core_init())
    {
        MessageBoxW(hWndMain, L"网络初始化失败", L"错误", MB_ICONERROR);
        return FALSE;
    }

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
            DialogBox(hInst, MAKEINTRESOURCE(IDD_LINKBOX), hWndMain, divLink);
            break;
        case IDM_LINK_SUB2:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_handlelinkbox), hWndMain, handleLink);
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
                // 将宽字符转换为GBK字节字符串用于检测
                char msgGbk[1024] = { 0 };
                int len = WideCharToMultiByte(CP_ACP, 0, inputText, -1, msgGbk, sizeof(msgGbk), NULL, NULL);

                // 检测是否包含脏话
                if (detect_swear_words(msgGbk)) {
                    MessageBoxW(hWndMain, L"消息包含脏话，发送被拒绝", L"警告", MB_ICONWARNING);
                    SetWindowTextW(hInput, L""); // 清空输入框
                    break;
                }

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
                wcscat_s(newText, sizeof(newText) / sizeof(newText[0]), L"你: ");
                wcscat_s(newText, sizeof(newText) / sizeof(newText[0]), inputText);

                // 更新显示内容
                SetWindowTextW(hDisplay, newText);

                // 滚动到底部
                SendMessageW(hDisplay, EM_LINESCROLL, 0, INT_MAX);

                // 发送消息到服务器（保持原有的UTF-8发送方式）
                char msg[256] = { 0 };
                WideCharToMultiByte(CP_ACP, 0, inputText, -1, msg, sizeof(msg), NULL, NULL);
                SendChatMessage(msg);

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
        DisconnectFromServer();
        cli_core_cleanup();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// "关于"框的消息处理程序。
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

INT_PTR CALLBACK divLink(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        // 将 wmId 声明移到此处，扩大作用域
        int wmId = LOWORD(wParam);
        
        if (wmId == IDC_OKDIV) // 处理"确定"按钮
        {
            if (g_bConnected) // 检查连接状态
            {
                cli_core_logout(g_server); // 断开连接
                g_bConnected = FALSE;       // 更新状态
                MessageBoxW(hDlg, L"已成功断开连接。", L"提示", MB_OK);
            }
            EndDialog(hDlg, wmId);
            return (INT_PTR)TRUE;
        }
        else if (wmId == IDCANCEL) // 处理"取消"按钮
        {
            EndDialog(hDlg, wmId);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// 其他对话框函数（保持不变）
INT_PTR CALLBACK handleLink(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        // 设置默认IP和端口（修正：使用小写 IDC_port）
        SetDlgItemText(hDlg, IDC_IP_ADDR, L"127.0.0.1");
        SetDlgItemInt(hDlg, IDC_port, 8080, FALSE);  // 小写 IDC_port
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_CONNECT)
        {
            // 获取IP地址
            wchar_t ipAddrW[16] = { 0 };
            GetDlgItemTextW(hDlg, IDC_IP_ADDR, ipAddrW, 16);

            // 获取端口（修正：使用小写 IDC_port）
            int port = GetDlgItemInt(hDlg, IDC_port, NULL, FALSE);  // 小写 IDC_port

            // 其余代码保持不变...
            char ipAddr[16] = { 0 };
            WideCharToMultiByte(CP_ACP, 0, ipAddrW, -1, ipAddr, 16, NULL, NULL);

            // 验证输入
            if (port <= 0 || port > 65535) {
                MessageBoxW(hDlg, L"无效的端口号", L"错误", MB_ICONERROR);
                return (INT_PTR)TRUE;
            }

            int wsaError;
            // 连接服务器
            SERVER* serverPtr = cli_core_login(ipAddr, port, OnMessageReceived, &wsaError);
            if (serverPtr == NULL) {
                MessageBoxW(hDlg, L"连接服务器失败", L"错误", MB_ICONERROR);
                return (INT_PTR)TRUE;
            }

            // 保存服务器指针到全局变量
            g_server = serverPtr;
            g_bConnected = TRUE;

            // 显示连接成功对话框
            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;
        }
        if (LOWORD(wParam) == IDCANCEL) {
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

void AppendChatMessage(const wchar_t* msg)
{
    HWND hDisplay = GetDlgItem(hWndMain, IDC_CHAT_DISPLAY);
    if (!hDisplay || !msg) return;
    wchar_t displayText[1024] = { 0 };
    GetWindowTextW(hDisplay, displayText, 1024);
    wchar_t newText[1024] = { 0 };
    if (wcslen(displayText) > 0)
    {
        wcscat_s(newText, sizeof(newText) / sizeof(newText[0]), displayText);
        wcscat_s(newText, sizeof(newText) / sizeof(newText[0]), L"\r\n");
    }
    wcscat_s(newText, sizeof(newText) / sizeof(newText[0]), msg);
    SetWindowTextW(hDisplay, newText);
    // 滚动到底部
    SendMessageW(hDisplay, EM_LINESCROLL, 0, INT_MAX);
}

void AppendChatMessageFromChar(const char* msg)
{
    if (!msg) return;

    // 计算需要的缓冲区大小
    int wlen = MultiByteToWideChar(CP_ACP, 0, msg, -1, NULL, 0);
    if (wlen == 0) return;

    // 分配缓冲区
    wchar_t* wmsg = (wchar_t*)malloc(wlen * sizeof(wchar_t));
    if (!wmsg) return;

    // 转换字符串
    if (MultiByteToWideChar(CP_ACP, 0, msg, -1, wmsg, wlen) > 0)
    {
        // 构造显示文本
        wchar_t displayText[1024] = { 0 };
        wcscat_s(displayText, sizeof(displayText) / sizeof(displayText[0]), L"对方: ");
        wcscat_s(displayText, sizeof(displayText) / sizeof(displayText[0]), wmsg);

        // 添加到聊天框
        AppendChatMessage(displayText);
    }

    // 释放内存
    free(wmsg);
}