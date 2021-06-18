#include "Sticker.hpp"

Sticker::Sticker(nlohmann::json sticker) {
	emoji = sticker["emoji"].get<std::string>();
	fileID = sticker["file_id"].get<std::string>();

	fileSize = sticker["file_size"].get<int>();
	height = sticker["height"].get<int>();
	width = sticker["width"].get<int>();
	setName = sticker["set_name"].get<std::string>();
}