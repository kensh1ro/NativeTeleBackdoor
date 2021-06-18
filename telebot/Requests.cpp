#include "Requests.hpp"

Requests::Requests(std::string url) {
	this->url = url;
}

Requests Requests::firePost() {
	this->curl = curl_easy_init();
	if (!this->curl) {
		std::cout << "Critical error in MethodManager." << std::endl;
		throw EXCEPTION_ACCESS_VIOLATION;
	}

	std::string tmpUrl{ url + args };

	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist *headerlist = NULL;
	std::string jsonString{};

	curl_global_init(CURL_GLOBAL_ALL);

	// set up the header
	//curl_formadd(&formpost,
	//	&lastptr,
	//	CURLFORM_COPYNAME, "cache-control:",
	//	CURLFORM_COPYCONTENTS, "no-cache",
	//	CURLFORM_END);

	//curl_formadd(&formpost,
	//	&lastptr,
	//	CURLFORM_COPYNAME, "cache-control:",
	//	CURLFORM_COPYCONTENTS, "no-cache",
	//	CURLFORM_END);

	//curl_formadd(&formpost,
	//	&lastptr,
	//	CURLFORM_COPYNAME, "content-type:",
	//	CURLFORM_COPYCONTENTS, "multipart/form-data",
	//	CURLFORM_END);

	//curl_formadd(&formpost, &lastptr,
	//	CURLFORM_COPYNAME, fileType.c_str(),
	//	CURLFORM_FILE, fileName.c_str(),
	//	CURLFORM_BUFFERPTR, contents.data(),
	//	CURLFORM_BUFFERLENGTH, contents.size(),
	//	CURLFORM_END);
	curl_easy_setopt(curl, CURLOPT_URL, tmpUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&jsonString);


	if (!fileName.empty() && !fileType.empty())
	{
		// Adding POST data to a request body
		struct curl_httppost* post = nullptr;
		struct curl_httppost* last = nullptr;


		curl_formadd(&post, &last,
			CURLFORM_COPYNAME, fileType.c_str(),
			CURLFORM_FILE, fileName.c_str(),
			CURLFORM_END);

		curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);

	}
	headerlist = curl_slist_append(headerlist, jsonString.c_str());

	res = curl_easy_perform(curl);
	if (res != 0)
		Log::Debug("Invalid curl result: " + std::to_string(res));

	curl_easy_cleanup(curl);
	curl_formfree(formpost);
	curl_slist_free_all(headerlist);

	contents = "";

	return *this;
}

Requests Requests::fireGet() {
	this->curl = curl_easy_init();
	if (!this->curl) {
		std::cout << "Critical error in MethodManager." << std::endl;
		throw EXCEPTION_ACCESS_VIOLATION;
	}

	std::string tmpUrl{ url + args };

	curl_easy_setopt(curl, CURLOPT_URL, tmpUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

	res = curl_easy_perform(curl);
	if (res != 0) {
		Log::Error("Curl returned: " + std::to_string(res));
		Log::Error("Request response: " + readBuffer);
	}

	long http_code = 0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

	if (http_code != 200) {
		std::string message = "Request to '" + url + "' has returned response code '" + std::to_string(http_code) + "'.";
		throw errors::RequestsException(message.c_str(), url.c_str(), readBuffer.c_str(), http_code);
	}

	curl_easy_cleanup(curl);
	return *this;
}

// Setting the method means resetting the request
Requests Requests::setMethod(std::string method) {
	this->url += "/" + method;
	cleanUp();
	return *this;
}

Requests Requests::add(std::string name, std::string value) {
	args += ((nArgs == 0) ? "?" : "&") + name + "=" + curl_easy_escape(curl, value.c_str(), value.size());
	nArgs++;
	return *this;
}

Requests Requests::add(std::string name, int value) {
	return add(name, std::to_string(value));
}

Requests Requests::add(std::string name, long long value) {
	return add(name, std::to_string(value));
}

// Not implemented yet
Requests Requests::add(std::string name, std::string fileName, std::string contents) {
	this->fileType = name;
	this->fileName = fileName;
	this->contents = contents;
	this->isPost = true;
	//args += ((nArgs == 0) ? "?" : "&") + name + "=" + curl_easy_escape(curl, fileName.c_str(), fileName.size());
	//nArgs++;

	return *this;
}

Requests Requests::fire() {
	if (isPost) return firePost();
	else return fireGet();
}

Result Requests::getResult() {
	return Result(readBuffer);
}

void Requests::cleanUp() {
	isPost = false;
	curl = 0;
	args = "";
	nArgs = 0;
	readBuffer = std::string();
}