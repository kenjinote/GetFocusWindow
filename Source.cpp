#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <psapi.h>

TCHAR szClassName[] = TEXT("Window");

BOOL GetExeFilePathFromWindowHandle(HWND hWnd, LPTSTR lpszExePath, int szPathSize)
{
	DWORD dwProcessID = 0;
	GetWindowThreadProcessId(hWnd, &dwProcessID);
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessID);
	if (!hProcess)
		return FALSE;
	HMODULE hModule = NULL;
	DWORD dwDummy = 0;
	if (!EnumProcessModules(hProcess, &hModule, sizeof(HMODULE), &dwDummy))
		return FALSE;
	if (!GetModuleFileNameEx(hProcess, hModule, lpszExePath, szPathSize))
		return FALSE;
	CloseHandle(hProcess);
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton1;
	static HWND hButton2;
	static HWND hEdit;
	static HFONT hFont;
	switch (msg)
	{
	case WM_CREATE:
		hFont = CreateFontW(-MulDiv(10, 96, 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, SHIFTJIS_CHARSET, 0, 0, 0, 0, L"MS Shell Dlg");
		hButton1 = CreateWindow(TEXT("BUTTON"), TEXT("開始"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SendMessage(hButton1, WM_SETFONT, (WPARAM)hFont, 0);
		hButton2 = CreateWindow(TEXT("BUTTON"), TEXT("停止"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDCANCEL, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SendMessage(hButton2, WM_SETFONT, (WPARAM)hFont, 0);
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_READONLY, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SendMessage(hEdit, EM_LIMITTEXT, 0, 0);
		SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, 0);
		SetTimer(hWnd, 0x1234, 1000, 0);
		break;
	case WM_TIMER:
		KillTimer(hWnd, 0x1234);
		{
			SetWindowText(hEdit, 0);
			GUITHREADINFO guiInfo = { sizeof(GUITHREADINFO) };
			if (GetGUIThreadInfo(0, &guiInfo))
			{
				SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)TEXT("フラグ: "));
				if (guiInfo.flags & GUI_CARETBLINKING)
				{
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)TEXT("GUI_CARETBLINKING "));
				}
				if (guiInfo.flags & GUI_INMENUMODE)
				{
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)TEXT("GUI_INMENUMODE "));
				}
				if (guiInfo.flags & GUI_INMOVESIZE)
				{
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)TEXT("GUI_INMOVESIZE "));
				}
				if (guiInfo.flags & GUI_POPUPMENUMODE)
				{
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)TEXT("GUI_POPUPMENUMODE "));
				}
				if (guiInfo.flags & GUI_SYSTEMMENUMODE)
				{
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)TEXT("GUI_SYSTEMMENUMODE "));
				}
				SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)TEXT("\r\n"));
				if (guiInfo.hwndActive)
				{
					{
						TCHAR szModulePath[MAX_PATH] = { 0 };
						GetExeFilePathFromWindowHandle(guiInfo.hwndActive, szModulePath, _countof(szModulePath));
						SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)TEXT("モジュールパス: "));
						SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szModulePath);
						SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)TEXT("\r\n"));
					}
					{
						RECT rect = { 0 };
						GetWindowRect(guiInfo.hwndActive, &rect);
						TCHAR szText[256];
						wsprintf(szText, TEXT("矩形: %d, %d, %d, %d\r\n"), rect.left, rect.top, rect.right, rect.bottom);
						SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					}
					{
						TCHAR szClassName[256];
						GetClassName(guiInfo.hwndActive, szClassName, _countof(szClassName));
						TCHAR szText[512];
						wsprintf(szText, TEXT("クラス名: %s\r\n"), szClassName);
						SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					}
					{
						TCHAR szWindowTitle[256];
						GetWindowText(guiInfo.hwndActive, szWindowTitle, _countof(szWindowTitle));
						TCHAR szText[512];
						wsprintf(szText, TEXT("ウィンドウタイトル: %s\r\n"), szWindowTitle);
						SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
					}
				}
				{
					TCHAR szText[1024];
					wsprintf(szText, TEXT("キーボードフォーカスを持つウィンドウのハンドル: 0x%x\r\n"), guiInfo.hwndCapture);
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);				
				}
				{
					TCHAR szText[1024];
					wsprintf(szText, TEXT("アクティブなメニューを所有するウィンドウのハンドル: 0x%x\r\n"), guiInfo.hwndMenuOwner);
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
				}
				{
					TCHAR szText[1024];
					wsprintf(szText, TEXT("移動またはサイズループでのウィンドウへのハンドル: 0x%x\r\n"), guiInfo.hwndMoveSize);
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
				}
				{
					TCHAR szText[1024];
					wsprintf(szText, TEXT("キャレットを表示しているウィンドウのハンドル: 0x%x\r\n"), guiInfo.hwndCaret);
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
				}
				{
					TCHAR szText[1024];
					wsprintf(szText, TEXT("hwndCaretメンバーによって指定されたウィンドウを基準にした、クライアント座標でのキャレットの境界矩形: %d, %d, %d, %d\r\n"), guiInfo.rcCaret.left, guiInfo.rcCaret.top, guiInfo.rcCaret.right, guiInfo.rcCaret.bottom);
					SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
				}
			}
		}
		SetTimer(hWnd, 0x1234, 1000, 0);
		break;
	case WM_SIZE:
		MoveWindow(hButton1, 10, 10, 256, 32, TRUE);
		MoveWindow(hButton2, 276, 10, 256, 32, TRUE);
		MoveWindow(hEdit, 10, 50, LOWORD(lParam) - 20, HIWORD(lParam) - 60, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			KillTimer(hWnd, 0x1234);
			SetTimer(hWnd, 0x1234, 1000, 0);
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			KillTimer(hWnd, 0x1234);
			SetForegroundWindow(hWnd);
			SetFocus(hEdit);
			SendMessage(hEdit, EM_SETSEL, 0, -1);
		}
		break;
	case WM_DESTROY:
		KillTimer(hWnd, 0x1234);
		DeleteObject(hFont);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_NOACTIVATE,
		szClassName,
		TEXT("フォーカスのあるウィンドウを取得する"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		0,
		0,
		640,
		480,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
