#include "Update.hpp"

Update::Update(nlohmann::json update) {
	updateID = update["update_id"].get<long>();
	message = Message(update["message"].get<nlohmann::json>());

	if (message.isReply) {
		replyTo = Message(update["message"].get<nlohmann::json>()["reply_to_message"].get<nlohmann::json>());
	}

	// Let's save the update, maybe we could need it
	update_json = update;
}