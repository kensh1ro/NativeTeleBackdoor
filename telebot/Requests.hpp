#pragma once

#include <curl\curl.h>
#include <string>
#include <iostream>

#include "Log.hpp"
#include "Result.hpp"
#include "Errors.hpp"

class Requests {
private:
	CURL * curl;
	CURLcode res;

	int nArgs;
	bool isPost = false;
	std::string args = "";
	std::string url = "";
	std::string readBuffer;
	std::string fileType;
	std::string fileName;
	std::string contents;

	static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
		((std::string*)userp)->append((char*)contents, size * nmemb);
		return size * nmemb;
	}

	void cleanUp();

	Requests firePost();

	Requests fireGet();

public:
	Requests(std::string url);

	Requests setMethod(std::string method);

	Requests add(std::string name, std::string value);

	Requests add(std::string name, int value);

	Requests add(std::string name, long long value);

	Requests add(std::string name, std::string fileName, std::string contents);

	Requests fire();

	Result getResult();
};