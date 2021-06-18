#include "Photo.hpp"

Photo::Photo(nlohmann::json photo) {
	photoID = photo["file_id"].get<std::string>();
	fileSize = photo["file_size"].get<int>();
	width = photo["width"].get<int>();
	height = photo["height"].get<int>();
}