#include "Utils.hpp"

std::string replaceAll(std::string* str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str->find(from, start_pos)) != std::string::npos) {
		str->replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return *str;
}

// The directory path returned by native GetCurrentDirectory() no end backslash
std::string getDirectory() {
	const unsigned long maxDir = 260;
	TCHAR currentDir[maxDir];
	GetCurrentDirectory(maxDir, currentDir);
	std::string cDirectory = currentDir;
	std::string str(cDirectory.begin(), cDirectory.end());
	return str;
}

bool contains(std::string string, std::string substring, bool lower) {
	if (lower) {
		substring = lowerCase(substring);
		string = lowerCase(string);
	}
	return (string.find(substring) != std::string::npos);
}

bool startsWith(std::string string, std::string substring) {
	if (_strnicmp(string.c_str(), substring.c_str(), substring.size()) == 0) return true;
	return false;
}

bool endsWith(std::string string, std::string substring) {
	if (substring.size() > string.size()) return false;
	return std::equal(substring.rbegin(), substring.rend(), string.rbegin());
}

std::string lowerCase(std::string string) {
	std::transform(string.begin(), string.end(), string.begin(), ::tolower);
	return string;
}

std::vector<std::string> splitByChar(std::string string, std::string splitter) {
	std::stringstream sstring(string);
	std::string segment;
	std::vector<std::string> seglist;

	while (std::getline(sstring, segment, splitter.c_str()[0])) {
		seglist.push_back(segment);
	}

	return seglist;
}

std::vector<std::string> splitByString(std::string s, std::string delim) {

	std::vector<std::string> result;

	auto start = 0U;
	auto end = s.find(delim);
	while (end != std::string::npos) {
		result.push_back(s.substr(start, end - start));
		start = end + delim.length();
		end = s.find(delim, start);
	}

	result.push_back(s.substr(start, end));

	return result;
}

std::vector<std::string> split(std::string string) {
	std::string buf; // Have a buffer string
	std::stringstream ss(string); // Insert the string into a stream

	std::vector<std::string> tokens; // Create vector to hold our words

	while (ss >> buf)
		tokens.push_back(buf);

	return tokens;
}

std::string readFile(std::string fileName) {
	std::ifstream t(fileName);
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	return str;
}

std::string readBytes(std::string fileName) {
	std::string contents;
	std::ifstream in(fileName, std::ios::binary);
	in.seekg(0, std::ios::end);
	contents.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&contents[0], contents.size());
	in.close();

	return contents;
}