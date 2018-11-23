// CEBacktrace.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CEBacktrace.h"
#include <windows.h>
#include <commctrl.h>

////////////////////////////////////////////
#pragma comment(lib, "toolhelp.lib")
#include <pkfuncs.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE			g_hInst;			// current instance
HWND				g_hWndCommandBar;	// command bar handle

// Forward declarations of functions included in this code module:
ATOM			MyRegisterClass(HINSTANCE, LPTSTR);
BOOL			InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

//////////////////////////////////////////////////////////////////////////////////////
void MyProcessFrames(DWORD dwCnt, CallSnapshot *lpFrames)
{
	printf("Backtrace:\n");
	for(DWORD i=0; i< dwCnt; i++){
		printf("MyProcessFrames:[%d] dwReturnAddr=0x%X\n", i, lpFrames[i].dwReturnAddr);
	}	 
}


LONG WINAPI MyVectoredExceptionHandler(struct _EXCEPTION_POINTERS *pExceptionInfo)  //异常处理函数
{  
	printf("################ My Exception Handler ####################\n");
  //typedef ULONG (WINAPI *lpGetThreadCallStack)(HANDLE,ULONG,LPVOID,DWORD,DWORD);  
 
  // 打印 Dump 信息   
	
  // 打印 SP 堆栈  

  ULONG *punSp = (ULONG *)pExceptionInfo->ContextRecord->Sp;  

  // 获取线程堆栈调用  
  //HMODULE hCore = LoadLibrary(L"coredll.dll");  
  //if(NULL != hCore)  
  //{  
  //  lpGetThreadCallStack pGetThreadCallStack = (lpGetThreadCallStack)GetProcAddress(hCore,L"GetThreadCallStack");  
  //  if(NULL != pGetThreadCallStack)  
  //  {  

  //  }  
  //}  

	//CallSnapshot frames[100];
	//HANDLE hThread = GetCurrentThread();
	//SetLastError(ERROR_SUCCESS);
	//int funcCount = GetThreadCallStack(hThread, 100, frames, STACKSNAP_RETURN_FRAMES_ON_ERROR, 0);
	//bool success = GetLastError() == ERROR_SUCCESS;
	//msdn:
#define MAX_FRAMES 100
  HANDLE hThread = GetCurrentThread();
  printf("except thread id=0x%x\n", GetCurrentThreadId());
  CallSnapshot lpFrames[MAX_FRAMES];
DWORD dwCnt, dwSkip = 0;
do {
      dwCnt = GetThreadCallStack (hThread, MAX_FRAMES, lpFrames, 0, dwSkip);
      if (dwCnt) {
         // Process the frames retrieved so far
         MyProcessFrames (dwCnt, lpFrames);		 
         dwSkip += dwCnt;
      }
   } while (MAX_FRAMES == dwCnt);

//还有 GetCallStackSnapshot
//#define MAX_FRAME_COUNT 100
//CallSnapshot lpFramesShot[MAX_FRAME_COUNT];
//int nRet = GetCallStackSnapshot(MAX_FRAME_COUNT, lpFramesShot, 0, 1);
//MyProcessFrames (nRet, lpFramesShot);

  //// 获取进程内 dll 信息  
  ////MODULEENTRY32 CurrentModule;  
  //HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());  
  //if((HANDLE)-1 != hSnapShot)  
  //{  
  //  // 调用  Module32First  和 Module32Next 完成 Module 枚举  
  //}  

  return 1;//
}

//调用例子： fun1 -> fun2 -> fun3 -> fun4 -> fun5  (fun5函数内异常)
void fun5(int a, int b, int c)
{
	int aa = 100+a;
	int bb = aa/2 + c/5;
	int cc = 40*aa;
	cc = aa+bb+cc;
	char *p = (char*)0xdead;
	char src[100]={5};
	memcpy(p, src, 100); //异常位置
	aa = 100 * cc + 5;
	aa = bb * aa + c;
	
}
void fun4(int a, int b, int c, int d)
{
	int aa = a;
	aa = aa /2;
	for (int i=0; i<100; i++)
		aa = aa/2+i;
	int bb = b;
	int cc = c/5;
	int dd = bb + cc/6;
	cc = dd /(aa+bb);
	fun5(aa, bb, cc);
}
void fun3(int a)
{
	int aa = a;
	aa = aa /2;
	for (int i=0; i<100; i++)
		aa = aa/2+i;
	int bb = 60;
	int cc = bb/5;
	int dd = bb + cc/6;
	fun4(aa, bb, cc, dd);
}
void fun2(int a, int b, int c)
{
	int aa = a;
	int bb = b;
	int cc = c;
	aa = aa + bb +cc;
	bb = bb +cc;
	cc = aa /bb;
	fun3(cc);
}
void fun1(int a, int b)
{
	int aa = a;
	int bb = b/2;
	int cc = (aa + bb)/5;
	b = a/b;
	fun2(aa, bb, cc);
}

//////////////////////////////////////////////////////////////////////////////////////

DWORD WINAPI ThreadProFunc(LPVOID lpParam)
{
	fun1(10, 20); //测试线程里调用
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
	MSG msg;

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) 
	{
		return FALSE;
	}
	/////////////////////////////////////////////////////////////
	AddVectoredExceptionHandler(1,MyVectoredExceptionHandler);  //注册异常处理函数

	printf("funs addr: %x %x %x %x %x\n", fun1,fun2,fun3,fun4,fun5);
	printf("main thread id=0x%x\n", GetCurrentThreadId());
	
	//fun1(10, 20); //测试主函数里调用
	DWORD dwThreadId;
	CreateThread(NULL, 0, ThreadProFunc, NULL, 0, &dwThreadId);
	printf("creeate thread id=0x%x\n", GetCurrentThreadId());


	/////////////////////////////////////////////////////////////

	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CEBACKTRACE));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CEBACKTRACE));
	wc.hCursor       = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = szWindowClass;

	return RegisterClass(&wc);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    TCHAR szTitle[MAX_LOADSTRING];		// title bar text
    TCHAR szWindowClass[MAX_LOADSTRING];	// main window class name

    g_hInst = hInstance; // Store instance handle in our global variable


    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING); 
    LoadString(hInstance, IDC_CEBACKTRACE, szWindowClass, MAX_LOADSTRING);


    if (!MyRegisterClass(hInstance, szWindowClass))
    {
    	return FALSE;
    }

    hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }


    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    if (g_hWndCommandBar)
    {
        CommandBar_Show(g_hWndCommandBar, TRUE);
    }

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

	
    switch (message) 
    {
        case WM_COMMAND:
            wmId    = LOWORD(wParam); 
            wmEvent = HIWORD(wParam); 
            // Parse the menu selections:
            switch (wmId)
            {
                case IDM_HELP_ABOUT:
                    DialogBox(g_hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, About);
                    break;
                case IDM_FILE_EXIT:
                    DestroyWindow(hWnd);
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        case WM_CREATE:
            g_hWndCommandBar = CommandBar_Create(g_hInst, hWnd, 1);
            CommandBar_InsertMenubar(g_hWndCommandBar, g_hInst, IDR_MENU, 0);
            CommandBar_AddAdornments(g_hWndCommandBar, 0, 0);
            break;
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            
            // TODO: Add any drawing code here...
            
            EndPaint(hWnd, &ps);
            break;
        case WM_DESTROY:
            CommandBar_Destroy(g_hWndCommandBar);
            PostQuitMessage(0);
            break;


        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
            RECT rectChild, rectParent;
            int DlgWidth, DlgHeight;	// dialog width and height in pixel units
            int NewPosX, NewPosY;

            // trying to center the About dialog
            if (GetWindowRect(hDlg, &rectChild)) 
            {
                GetClientRect(GetParent(hDlg), &rectParent);
                DlgWidth	= rectChild.right - rectChild.left;
                DlgHeight	= rectChild.bottom - rectChild.top ;
                NewPosX		= (rectParent.right - rectParent.left - DlgWidth) / 2;
                NewPosY		= (rectParent.bottom - rectParent.top - DlgHeight) / 2;
				
                // if the About box is larger than the physical screen 
                if (NewPosX < 0) NewPosX = 0;
                if (NewPosY < 0) NewPosY = 0;
                SetWindowPos(hDlg, 0, NewPosX, NewPosY,
                    0, 0, SWP_NOZORDER | SWP_NOSIZE);
            }
            return (INT_PTR)TRUE;

        case WM_COMMAND:
            if ((LOWORD(wParam) == IDOK) || (LOWORD(wParam) == IDCANCEL))
            {
                EndDialog(hDlg, LOWORD(wParam));
                return TRUE;
            }
            break;

        case WM_CLOSE:
            EndDialog(hDlg, message);
            return TRUE;

    }
    return (INT_PTR)FALSE;
}
