#include "Result.hpp"

Result::Result(std::string result) {
	this->result = result;
}

std::string Result::asString() {
	return this->result;
}

nlohmann::json Result::asJson() {
	return nlohmann::json::parse(result);
}

nlohmann::json::array_t Result::asJsonArray(std::string keyName) {
	return nlohmann::json::parse(result)[keyName].get<nlohmann::json::array_t>();
}