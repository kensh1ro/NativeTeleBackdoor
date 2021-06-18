#pragma once

#include <nlohmann\json.hpp>
#include "Chat.hpp"
#include "User.hpp"
#include "Sticker.hpp"
#include "Photo.hpp"

class Message {

public:
	Chat chat;
	User user;
	Sticker sticker;

	bool isSticker = false;
	bool isReply = false;

	std::vector<Photo> photos;
	int photosSize{ 0 };
	int messageID;

	std::string text{ "" };

	Message() {}

	Message(nlohmann::json message);
};