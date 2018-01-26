#include "Utility.h"
#include <fstream>
#include <cassert>
#include <iostream>

char* readTextResourceFile(const std::string &_filename)
{
	std::ifstream file(_filename, std::ios::ate);
	file.exceptions(std::ifstream::badbit);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file " + _filename + "!");
	}
	size_t fileSize = (size_t)file.tellg();
	char *buffer = new char[fileSize + 1];
	file.seekg(0);

	//file.read(buffer, fileSize);
	char c;
	size_t position = 0;
	while (file.get(c)) {
		//if (c != '\r') {
		buffer[position] = c;
		++position;
		assert(position <= fileSize);
	}
	buffer[position] = '\0';

	file.close();

	return buffer;
}