#pragma once

#include <windows.h>
#include <string>
#include <chrono>
#include <curl\curl.h>
#include <thread>
#include <vector>
#include <iostream>
#include <map>
#include <nlohmann\json.hpp>
#include "Log.hpp"
#include "Chat.hpp"
#include "User.hpp"
#include "Sticker.hpp"
#include "Photo.hpp"
#include "Message.hpp"
#include "Update.hpp"
#include "Result.hpp"
#include "Methods.hpp"
#include "Errors.hpp"
#include "Utils.hpp"

static bool stopProgram = false;

static BOOL WINAPI console_ctrl_handler(DWORD dwCtrlType);

class Bot : public Methods {
public:

	long ID;
	long offset = 0;

	std::string name{ "" };
	std::string username{ "" };
	
	Bot(std::string token);

	void addCommandHandler(std::string command, void(*commandHandler)(Bot* bot, Update update, std::vector<std::string> args));

	void addCommandHandler(std::string command, void(*commandHandler)(Bot* bot, Update update));

	void setErrorHandler(void(*errorHandler)(Bot* bot, Update update, std::string func, std::string error));

	void setUpdateHandler(void(*updateHandler)(Bot* bot, Update update));

	void startPolling(bool clean);

	void startPolling();
	
	void stopPolling();

	void idle();

	void defaultUpdateHandlerFoo(Bot* bot, Update update);

	void defaultErrorHandlerFoo(Bot* bot, Update update, std::string func, std::string error);

	int getElapsedTime();

private:

	bool stop{ false };

	std::string botToken;
	std::string commandSymbol{ "/" };

	std::chrono::steady_clock::time_point updateStartTime;

	std::vector<std::pair<std::string, void(*)(Bot*, Update, std::vector<std::string>)>> commandHandlersArgs{
		std::vector<std::pair<std::string, void(*)(Bot*, Update, std::vector<std::string>)>>()
	};

	std::vector<std::pair<std::string, void(*)(Bot*, Update)>> commandHandlers{
		std::vector<std::pair<std::string, void(*)(Bot*, Update)>>()
	};

	void(*updateHandlerFoo)(Bot* bot, Update update) { NULL };

	void(*errorHandlerFoo)(Bot* bot, Update update, std::string func, std::string error) { NULL };

	void cleanUpdates();

	void _startPolling();

	void handleUpdate(Update* update);

	void elaborateUpdates(nlohmann::json::array_t updates);

	void pollingTask();
};