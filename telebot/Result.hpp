#pragma once

#include <nlohmann\json.hpp>

class Result {
	std::string result;

public:
	Result(std::string result);

	std::string asString();

	nlohmann::json asJson();

	nlohmann::json::array_t asJsonArray(std::string keyName);
};