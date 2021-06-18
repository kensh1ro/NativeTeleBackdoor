#include "Errors.hpp"

namespace errors {
	TokenException::TokenException(std::string error, std::string token) {
		this->token = token;
		this->error = error;
	}

	const char* TokenException::what() const throw () {
		return error.c_str();
	};

	RequestsException::RequestsException(const char* message, const char* url, const char* result, long code) {
		this->url = url;
		this->code = code;
		this->result = result;
		this->message = message;
	}

	const char* RequestsException::what() const throw () {
		return message;
	};
}