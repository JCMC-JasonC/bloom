#include <TTK/IO.h>
#include <iostream>
#include <fstream>

std::string TTK::IO::loadFile(std::string fileName)
{
	// std::ios::in		- read
	// std::ios::binary	- treat data as binary, not text
	// std::ios::ate	- start stream at end of file (useful for getting length without seeking)
	std::ifstream file(fileName, std::ios::in /*| std::ios::binary*/ | std::ios::ate);

	if (!file.is_open())
	{
		std::cout << "File IO Error: Cannot open file: " << fileName << std::endl;
		return nullptr;
	}

	size_t end;
	
	// Get last element in stream (std::ios::ate)
	end = file.tellg();

	// Alloc memory for file
	std::string ret(end, ' ');	

	// Move back to beginning of file
	file.seekg(0, std::ios::beg); 
	
	// Load file into memory
	//file.read(&ret[0], end);
	file.read(&ret[0], end);

	// Close file
	file.close();

	 //std::string ret(mem);
	 //delete[] mem;
	 return ret;
}

