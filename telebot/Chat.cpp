#include "Chat.hpp"

Chat::Chat(nlohmann::json chat) {
	chatID = chat["id"].get<long long>();

	if (std::find(chat.begin(), chat.end(), "title") != chat.end()) {
		title = chat["title"].get<std::string>();
	}
}