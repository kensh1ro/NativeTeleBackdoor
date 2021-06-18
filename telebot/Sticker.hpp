#pragma once

#include <nlohmann\json.hpp>

class Sticker {
public:
	std::string fileID;
	std::string emoji;
	std::string setName;

	int fileSize;
	int height;
	int width;

	Sticker() { }

	Sticker(nlohmann::json sticker);
};