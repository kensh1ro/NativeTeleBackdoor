#include "Log.hpp"

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

namespace Log {

	void applyCout(std::string text, std::string type, colors color) {
		SetConsoleTextAttribute(hConsole, WHITE);
		std::cout << "[";
		SetConsoleTextAttribute(hConsole, color);
		std::cout << type;
		SetConsoleTextAttribute(hConsole, WHITE);
		std::cout << "] - " << text << std::endl;
	}

	void Debug(std::string text) {
		applyCout(text, " Debug  ", GREEN);
	}

	void Action(std::string text) {
		applyCout(text, " Action ", PURPLE);
	}

	void Info(std::string text) {
		applyCout(text, " Info   ", LIGHT_BLUE);
	}

	void Error(std::string text) {
		applyCout(text, " Error  ", RED);
	}
}