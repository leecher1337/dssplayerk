#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>

// ------------------------------------------------------------------

HINSTANCE	g_hModule	= NULL;

// ------------------------------------------------------------------

#define ID_REWIND	0x3E8
#define REWIND	 0
#define STOP	 1
#define PLAY	 2
#define FFORWARD 3
#define SKIP	 4
#define MAX_BTN	SKIP

#pragma data_seg(".SHARED")
HHOOK		g_hWndHook	= NULL;
HHOOK		g_hMsgHook	= NULL;
HWND		g_hWndMain  = NULL;
#pragma data_seg()
#pragma comment(linker,"/SECTION:.SHARED,RWS")

// ------------------------------------------------------------------

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			g_hModule = (HINSTANCE)hModule;
			break;
		}
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

// ------------------------------------------------------------------

BOOL WINAPI _DllMainCRTStartup(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpReserved)
{
	return DllMain(hinstDLL,fdwReason,lpReserved);
}

static void ClickButton(int nID)
{
	HWND hWnd = GetDlgItem(g_hWndMain, ID_REWIND + nID);

	SendMessage(hWnd, WM_KEYDOWN, ' ', 0x390001);
	SendMessage(hWnd, WM_CHAR, ' ', 0x390001);
	SendMessage(hWnd, WM_KEYUP, ' ', 0xC0390001);
}

// ------------------------------------------------------------------
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	MSG *msg = (MSG*)lParam;

	if (msg->message == WM_KEYDOWN)
	{
		switch (msg->wParam)
		{
		case VK_F9:	// Play/Stop
			OutputDebugString("PLAY\n");
			if (IsWindowEnabled(GetDlgItem(g_hWndMain, ID_REWIND + PLAY))) ClickButton(PLAY); else ClickButton(STOP);
			break;
		case VK_F8: // Fast forward
			OutputDebugString("FFORWARD\n");
			ClickButton(FFORWARD);
			break;
		case VK_F7: // Rewind
			OutputDebugString("REWIND\n");
			ClickButton(REWIND);
			break;
		case VK_F6:
			OutputDebugString("SKIP\n");
			ClickButton(SKIP);
			break;
		}
	}
	return CallNextHookEx(g_hWndHook, nCode, wParam, lParam); 
}

// ------------------------------------------------------------------


__declspec (dllexport) BOOL InstallHook(DWORD dwThreadId)
{
	GUITHREADINFO gti={0};
	char szDbg[64];

	if (!(g_hWndHook = SetWindowsHookEx(WH_GETMESSAGE, HookProc, g_hModule, dwThreadId)))
	{
		wsprintf(szDbg, "SetWindowsHookEx failed: %d\n", GetLastError());
		OutputDebugString(szDbg);
		return FALSE;
	}
	gti.cbSize = sizeof(gti);
	GetGUIThreadInfo(dwThreadId, &gti);
	g_hWndMain = gti.hwndActive;
	wsprintf(szDbg, "Active main window: %08X\n", g_hWndMain);
	OutputDebugString(szDbg);
	return TRUE;
}

// ------------------------------------------------------------------

__declspec (dllexport) BOOL UninstallHook(void)
{
	return UnhookWindowsHookEx(g_hWndHook);
}
