#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

BOOL (__cdecl *InstallHook) (DWORD dwThreadId);
void (__cdecl *UninstallHook) (void);

char *GetError(void)
{
   static char szError[1024];

   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, szError, sizeof(szError), NULL);
   return szError;
}

// ------------------------------------------------------------------

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	STARTUPINFO si={0};
	PROCESS_INFORMATION pi={0};
	char szMsg[MAX_PATH+1280], szPath[MAX_PATH*2];
	DWORD cbSize, dwRet;
	HINSTANCE hMod;
	HKEY hKey;

	if ((dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Olympus\\DSSPlayerLite\\Directories", 0, KEY_QUERY_VALUE, &hKey)) == 0)
	{
		cbSize = sizeof(szPath);
		dwRet = RegQueryValueEx(hKey, "Path", NULL, NULL, szPath, &cbSize);
		RegCloseKey(hKey);
	}
	if (dwRet)
	{
		wsprintf (szMsg, "Unable to locate DSS Player lite: %s", GetError());
		MessageBox (NULL, szMsg, "Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	lstrcat(szPath, "\\DSSPly.exe ");
	lstrcat(szPath, lpCmdLine);

	si.cb = sizeof(si);
	si.dwFlags = STARTF_FORCEOFFFEEDBACK;
	pi.hProcess = pi.hThread = INVALID_HANDLE_VALUE;

	if (!(hMod = LoadLibrary ("dssplyhk.dll")))
	{
		wsprintf (szMsg, "Unable to load hook DLL: %s", GetError());
		MessageBox (NULL, szMsg, "Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	if (!(*(FARPROC *)&InstallHook = GetProcAddress (hMod, "InstallHook")) ||
		!(*(FARPROC *)&UninstallHook = GetProcAddress (hMod, "UninstallHook")))
	{
		wsprintf (szMsg, "Unable to locate export in Hooker DLL: %s", GetError());
		MessageBox (NULL, szMsg, "Error", MB_OK | MB_ICONERROR);
		FreeLibrary (hMod);
		return FALSE;
	}

	if (!CreateProcess (NULL, szPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		wsprintf (szMsg, "Failed to start application %s: %s", lpCmdLine, GetError());
		MessageBox (NULL, szMsg, "Error", MB_OK | MB_ICONERROR);
		FreeLibrary (hMod);
		return FALSE;
	}

	WaitForInputIdle (pi.hProcess, INFINITE);
	if (!InstallHook(pi.dwThreadId))
	{
		wsprintf (szMsg, "Failed to hook WndProc: %s", GetError());
		MessageBox (NULL, szMsg, "Error", MB_OK | MB_ICONERROR);
		FreeLibrary (hMod);
		return FALSE;
	}

	WaitForSingleObject (pi.hProcess, INFINITE);
	UninstallHook();
	FreeLibrary (hMod);

	return TRUE;
}

// ------------------------------------------------------------------

int WINAPI WinMainCRTStartup(void)
{
    STARTUPINFO				StartupInfo={sizeof(STARTUPINFO),0};
    int				        mainret;
    char				*lpszCommandLine = GetCommandLine();
    
    // skip past program name (first token in command line).
    if( *lpszCommandLine == '"' )  // check for and handle quoted program name
    {
        // scan, and skip over, subsequent characters until  another
        // double-quote or a null is encountered
		lpszCommandLine++;
        while( *lpszCommandLine && (*lpszCommandLine != '"') )
            lpszCommandLine++;

        // if we stopped on a double-quote (usual case), skip over it.
        if( *lpszCommandLine == '"' )
            lpszCommandLine++;
    }
    else    
    {
        // first token wasn't a quote
        while ( *lpszCommandLine > ' ' )
            lpszCommandLine++;
    }

    // skip past any white space preceeding the second token.
    while ( *lpszCommandLine && (*lpszCommandLine <= ' ') )
        lpszCommandLine++;

    GetStartupInfo(&StartupInfo);

    mainret = WinMain( GetModuleHandle(NULL),
                       NULL,
                       lpszCommandLine,
                       StartupInfo.dwFlags & STARTF_USESHOWWINDOW
                            ? StartupInfo.wShowWindow : SW_SHOWDEFAULT );

    ExitProcess(mainret);
    
    return mainret;
}
