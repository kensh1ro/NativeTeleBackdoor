#pragma once

#include <nlohmann\json.hpp>
#include "Message.hpp"

class Update {
public:
	Message message;
	Message replyTo;
	nlohmann::json update_json;

	long updateID{ 0 };

	Update(nlohmann::json update);
};