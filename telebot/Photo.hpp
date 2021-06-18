#pragma once

#include <nlohmann\json.hpp>

class Photo {

public:
	std::string photoID;

	int fileSize{ 0 };
	int width{ 0 };
	int height{ 0 };

	Photo() {}

	Photo(nlohmann::json photo);
};