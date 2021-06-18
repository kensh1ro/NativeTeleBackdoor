#include "User.hpp"

User::User(nlohmann::json user) {
	userID = user["id"].get<long long>();
	name = user["first_name"].get<std::string>();
	username = user["username"].get<std::string>();

	if (std::find(user.begin(), user.end(), "last_name") != user.end()) {
		last_name = user["last_name"].get<std::string>();
	}
}