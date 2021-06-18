#pragma once
#include <exception>
#include <string>

namespace errors {

	struct TokenException : public std::exception {
		std::string token;
		std::string error;

		TokenException(std::string error, std::string token);

		const char* what() const throw ();
	};

	struct RequestsException : public std::exception {
		const char* url;
		const char* result;
		const char* message;
		long code;

		RequestsException(const char* message, const char* url, const char* result, long code);

		const char* what() const throw ();
	};
}