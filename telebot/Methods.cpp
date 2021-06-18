#include "Methods.hpp"

Methods::Methods(std::string token) {
	this->token = token;
}

Requests Methods::sendMessage(long long chatID, std::string text) {
	return Requests(baseUrl + token)
			.setMethod("sendMessage")
			.add("chat_id", chatID)
			.add("text", text);
}

Requests Methods::getMe() {
	return Requests(baseUrl + token)
			.setMethod("getMe");
}

Requests Methods::getUpdates() {
	return Requests(baseUrl + token)
			.setMethod("getUpdates");
}

Requests Methods::sendChatAction(long long chatID, chatAction action) {
	return Requests(baseUrl + token)
			.setMethod("sendChatAction")
			.add("chat_id", chatID)
			.add("action", chatActions[action]);
}

Requests Methods::sendPhoto(long long chatID, std::string fileName, std::string contents) {
	return Requests(baseUrl + token)
			.setMethod("sendPhoto")
			.add("chat_id", chatID)
			.add("photo", fileName, contents);
}

Requests Methods::sendPhoto(long long chatID, std::string fileID) {
	return Requests(baseUrl + token)
			.setMethod("sendPhoto")
			.add("chat_id", chatID)
			.add("photo", fileID);
}

Requests Methods::sendSticker(long long chatID, std::string stickerID) {
	return Requests(baseUrl + token)
			.setMethod("sendSticker")
			.add("chat_id", chatID)
			.add("sticker", stickerID);
}

Requests Methods::sendDocument(long long chatID, std::string fileName, std::string contents) {
	return Requests(baseUrl + token)
			.setMethod("sendDocument")
			.add("chat_id", chatID)
			.add("document", fileName, contents);
}

Requests Methods::sendDocument(long long chatID, std::string fileID) {
	return Requests(baseUrl + token)
			.setMethod("sendDocument")
			.add("chat_id", chatID)
			.add("document", fileID);
}