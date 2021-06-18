#pragma once
#include <string>
#include <iostream>
#include <nlohmann\json.hpp>
#include <windows.h>

namespace Log {
	enum colors {
		GREEN = 10,
		RED = 12,
		WHITE = 15,
		LIGHT_BLUE = 11,
		GRAY = 8,
		PURPLE = 13
	};

	void applyCout(std::string text, std::string type, colors color);

	void Debug(std::string text);

	void Info(std::string text);

	void Error(std::string text);

	void Action(std::string text);
}