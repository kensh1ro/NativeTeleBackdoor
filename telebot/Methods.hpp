#pragma once
#include <string>
#include <map>
#include "Requests.hpp"


namespace Arguments {
	static constexpr const char* replyToMessage = "reply_to_message_id";
	static constexpr const char* parseMode = "parse_mode";
	static constexpr const char* markdown = "markdown";
};

enum chatAction {
	TYPING = 0,
	UPLOAD_PHOTO = 1,
	UPLOAD_DOCUMENT = 6
};

class Methods {

	std::string baseUrl{ "https://api.telegram.org/bot" };

	std::string token;

	std::map<int, std::string> chatActions{
		{ TYPING, "typing" },
		{ UPLOAD_PHOTO, "upload_photo" },
		{ UPLOAD_DOCUMENT, "upload_document" }
	};

public:
	
	Methods(std::string token);

	Requests sendMessage(long long chatID, std::string text);

	Requests getMe();

	Requests getUpdates();

	Requests sendChatAction(long long chatID, chatAction action);

	Requests sendPhoto(long long chatID, std::string fileName, std::string contents);

	Requests sendPhoto(long long chatID, std::string fileID);

	Requests sendSticker(long long chatID, std::string stickerID);

	Requests sendDocument(long long chatID, std::string fileName, std::string contents);

	Requests sendDocument(long long chatID, std::string fileID);
};