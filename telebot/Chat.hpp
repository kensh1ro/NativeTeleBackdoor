#pragma once

#include <nlohmann\json.hpp>
#include <string>

class Chat {

public:
	long long chatID{ 0 };
	std::string title{ "" };

	Chat() { }

	Chat(nlohmann::json chat);
};