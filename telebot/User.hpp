#pragma once

#include <nlohmann\json.hpp>

class User {
public:
	long long userID{0};

	std::string name{ "" };
	std::string last_name{ "" };
	std::string username{""};

	User(nlohmann::json user);

	User() {};
};