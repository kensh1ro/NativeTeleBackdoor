#include "Message.hpp"

Message::Message(nlohmann::json message) {
	// std::cout << message.dump() << std::endl;
	if (message["text"].is_string()) {
		text = message["text"].get<std::string>();
	}

	chat = Chat(message["chat"].get<nlohmann::json>());
	user = User(message["from"].get<nlohmann::json>());

	messageID = message["message_id"].get<int>();

	if (message.find("reply_to_message") != message.end())
		isReply = true;

	if (message.find("sticker") != message.end()) {
		sticker = Sticker(message["sticker"].get<nlohmann::json>());
		isSticker = true;
	}

	try {
		nlohmann::json::array_t photos_array = message.at("photo").get<nlohmann::json::array_t>();
		photosSize = photos_array.size();
		for (int i{ 0 }; i < photosSize; i++) {
			photos.push_back(Photo(photos_array[i]));
		}
	} catch (...) {
	}
}