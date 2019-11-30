
#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <stdio.h> // DEBUG

#include "abeep.h"

#define BEEP_DEFAULT_FREQ	800
#define BEEP_DEFAULT_DURATION	200
#define BEEP_DEFAULT_PERIOD	1500

char lpszDefaultPromptMessage [] = "Press any key to stop beeping...";

DWORD dwFreq = BEEP_DEFAULT_FREQ;	/* 37 - 32767 */
DWORD dwDuration = BEEP_DEFAULT_DURATION;
DWORD dwPeriod = BEEP_DEFAULT_PERIOD;
ULONG ulTimeout = 0;

HANDLE hConsole;
HANDLE hConsoleOutput;
HANDLE hWaitableTimer = NULL;
HANDLE hPeriodTimer = NULL;
HANDLE hEvent = NULL;
BOOL bBeepDeviceOpened = FALSE;
BOOL bCtrlHandlerSet = FALSE;

int iNumArgs;
LPWSTR *ArgvW;
HANDLE hWaitObjects [4];
LARGE_INTEGER liDueTime;

enum {
	exitOK = 0,
	exitTimeout,
	exitBreak,
	exitError
};

enum {
	parseOK = 0,
	parseExit
};

static INT ParseCommandLine (VOID);
static VOID ParseAndWritePromptMessage (VOID);
BOOL IsKeyPressed (void);
BOOL WINAPI HandlerRoutine (DWORD dwCtrlType);

int __cdecl main (void)
{
	DWORD dw;
	INT iInd;
	WCHAR wChar;
	BOOL bPressed = FALSE, bPromptMessage;

	hConsoleOutput = GetStdHandle (STD_OUTPUT_HANDLE);
	if (hConsoleOutput == INVALID_HANDLE_VALUE || hConsoleOutput == NULL)
		return exitError;

	ArgvW = CommandLineToArgvW (GetCommandLineW (), &iNumArgs);
	if (ArgvW == NULL)
		return exitError;

	if (ParseCommandLine ()) {
		LocalFree (ArgvW);
		return exitOK;
	}

	hConsole = GetStdHandle (STD_INPUT_HANDLE);
	if (hConsole == INVALID_HANDLE_VALUE || hConsole == NULL)
		return exitError;

	__try {
		hWaitableTimer = CreateWaitableTimer (NULL, TRUE, NULL);
		if (hWaitableTimer == NULL)
			return exitError;
		hPeriodTimer = CreateWaitableTimer (NULL, FALSE, NULL);
		if (hPeriodTimer == NULL)
			return exitError;
		if (!OpenBeepDevice ())
			return exitError;
		bBeepDeviceOpened = TRUE;
		hEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
		if (hEvent == NULL)
			return exitError;
		if (!SetConsoleCtrlHandler (HandlerRoutine, TRUE))
			return exitError;
		bCtrlHandlerSet = TRUE;
		bPromptMessage = FALSE;
		dw = L' ';
		for (iInd = 1; iInd < iNumArgs; iInd++) {
			wChar = ArgvW [iInd][0];
			if (wChar != '-' && wChar != '/') {
				if (bPromptMessage) WriteConsoleW (hConsoleOutput, &dw, 1,
					NULL, NULL);
				WriteConsoleW (hConsoleOutput, ArgvW [iInd],
					lstrlenW (ArgvW [iInd]), NULL, NULL);
				bPromptMessage |= TRUE;
			}
		}
		LocalFree (ArgvW);
		if (!bPromptMessage)
			WriteConsoleA (hConsoleOutput, (LPSTR)lpszDefaultPromptMessage,
				sizeof (lpszDefaultPromptMessage)-1, NULL, NULL);
		FlushConsoleInputBuffer (hConsole);
		if (ulTimeout) {
			liDueTime.QuadPart = - UInt32x32To64 (ulTimeout, 10000000);
			if (!SetWaitableTimer (hWaitableTimer, &liDueTime, 0, NULL, NULL, FALSE))
				return exitError;
		}
		liDueTime.QuadPart = - UInt32x32To64 (dwPeriod, 10000);
		if (!SetWaitableTimer (hPeriodTimer, &liDueTime, dwPeriod, NULL, NULL, FALSE))
			return exitError;

		hWaitObjects [0] = hConsole;
		hWaitObjects [1] = hPeriodTimer;
		hWaitObjects [2] = hWaitableTimer;
		hWaitObjects [3] = hEvent;
		while (1) {
			if (!AsyncBeep (dwFreq, dwDuration))
				return exitError;
			if (IsKeyPressed ())
				return exitOK;
			do	dw = WaitForMultipleObjects (4, hWaitObjects, FALSE, INFINITE);
			while (!dw && !(bPressed = IsKeyPressed ()));
			if (dw - WAIT_OBJECT_0 >= 2 || bPressed)
				break;
		}
	} __finally {
		if (bBeepDeviceOpened) {
			AsyncMakeBeep (0);
			CloseBeepDevice ();
		}
		if (hWaitableTimer != NULL)
			CloseHandle (hWaitableTimer);
		if (hPeriodTimer != NULL)
			CloseHandle (hPeriodTimer);
		if (bCtrlHandlerSet)
			SetConsoleCtrlHandler (HandlerRoutine, FALSE);
		if (hEvent != NULL)
			CloseHandle (hEvent);
	}
	WriteConsoleA (hConsoleOutput, (LPSTR)"\n", 1, NULL, NULL);
	if (!bPressed)
		switch (dw - WAIT_OBJECT_0) {
			case 2: return exitTimeout;
			case 3:	return exitBreak;
			default: return exitError;
		}
	return exitOK;
}

BOOL IsKeyPressed (void)
{
	DWORD dwNumberOfEvents;
	INPUT_RECORD Record;

	while (1) {
		GetNumberOfConsoleInputEvents (hConsole, &dwNumberOfEvents);
		if (dwNumberOfEvents == 0)
			return FALSE;
		if (!ReadConsoleInput (hConsole, &Record, 1, &dwNumberOfEvents) ||
				dwNumberOfEvents == 0)
			return TRUE;
		if (Record.EventType == KEY_EVENT && Record.Event.KeyEvent.bKeyDown == TRUE)
			return TRUE;
	};
}

BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	SetEvent (hEvent);
	return TRUE;
}

CONST CHAR lpszUsage [] =
"Suspends execution of a batch script and starts beeping until a key is pressed.\n\
Usage:\n\
beepwarn \"prompt message\" [/t<timeout>] [/p<period>] [/f<freq>] [/d<durat>] [/? | /h]\n\
Arguments (default value):\n\
  <timeout>\t- timeout in seconds (no);\n\
  <period>\t- interval between beeps, ms (1.5 s);\n\
  <freq>\t- frequency from 37 to 32767, Hz (800);\n\
  <durat>\t- beep duration, ms (200).\n\
Exit code:\n\
  0\t- key pressed,\n\
  1\t- timeout,\n\
  2\t- break signal,\n\
  3\t- error.\n";

static LPCWSTR SkipColon (LPCWSTR lpcwStr);

CONST CHAR lpcszm1 [] = "Warning (beepwarn): option ignored: '";
CONST CHAR lpcszm2 [] = "'.\n";

INT ParseCommandLine (VOID)
{
	DWORD dw;
	BOOL bOptIgnored, bDisplayUsage = FALSE;
	WCHAR wChar;
	LPCWSTR lpcwStr;
	INT iInd;

	for (iInd = 1; iInd < iNumArgs; iInd++) {
		bOptIgnored = FALSE;
		wChar = ArgvW [iInd][0];
		if (wChar == '-' || wChar == '/') {
			wChar = ArgvW [iInd][1];
			CharLowerBuffW (&wChar, 1);
			switch (wChar) {
			case 't':
				lpcwStr = SkipColon (ArgvW [iInd] + 2);
				if (*lpcwStr)
					ulTimeout = StrToIntW (lpcwStr);
				else	bOptIgnored = TRUE;
				break;
			case 'p':
				lpcwStr = SkipColon (ArgvW [iInd] + 2);
				if (*lpcwStr)
					dwPeriod = StrToIntW (lpcwStr);
				else	bOptIgnored = TRUE;
				break;
			case 'f':
				lpcwStr = SkipColon (ArgvW [iInd] + 2);
				if (*lpcwStr) {
					dw = StrToIntW (lpcwStr);
					if (dw >= 37 && dw <= 32767)
						dwFreq = dw;
					else	bOptIgnored = TRUE;
				} else	bOptIgnored = TRUE;
				break;
			case 'd':
				lpcwStr = SkipColon (ArgvW [iInd] + 2);
				if (*lpcwStr)
					dwDuration = StrToIntW (lpcwStr);
				else	bOptIgnored = TRUE;
				break;
			case '?':
			case 'h':
				if (ArgvW [iInd][2] == '\0')
					bDisplayUsage = TRUE;
				else if (lstrcmpiW (ArgvW [iInd] + 1, L"help") == 0)
					bDisplayUsage = TRUE;
				else	bOptIgnored = TRUE;
				break;
			default:
				bOptIgnored = TRUE;
			}
		}
		if (bOptIgnored) {
			WriteConsoleA (hConsoleOutput, lpcszm1, sizeof(lpcszm1)-1,
				NULL, NULL);
			WriteConsoleW (hConsoleOutput, ArgvW [iInd], lstrlenW (ArgvW [iInd]),
				NULL, NULL);
			WriteConsoleA (hConsoleOutput, lpcszm2, sizeof(lpcszm2)-1,
				NULL, NULL);
		}
	}
	if (bDisplayUsage) {
		WriteConsoleA (hConsoleOutput, (LPSTR)lpszUsage,
			sizeof(lpszUsage)-1, NULL, NULL);
		return parseExit;
	}
	return parseOK;
}

LPCWSTR SkipColon (LPCWSTR lpcwszStr)
{
	if (!lpcwszStr)
		return NULL;
	if (*lpcwszStr == ':')
		return lpcwszStr + 1;
	return	lpcwszStr;
}
