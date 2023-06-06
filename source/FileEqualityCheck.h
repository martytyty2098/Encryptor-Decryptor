#pragma once

#include <fstream>
#include <iostream>
#include "ProgressBar.hpp"

static size_t getFileSize(std::fstream& file)
{
	std::streampos saved = file.tellg();
	file.seekg(0, std::ios_base::end);
	std::streampos file_size = file.tellg();
	file.seekg(saved, std::ios_base::beg);
	return static_cast<size_t>(file_size); 
}

static bool CheckFilesEqualityBin(std::fstream& left, std::fstream& right, bool enable_bar = false, int bar_length = 10)
{
	const size_t left_size = getFileSize(left);
	const size_t right_size = getFileSize(right);

	if (left_size != right_size) return false;
	if (left_size < 1) return true;

	const size_t buffer_size = 65536 > left_size ? left_size : 65536;

	char* left_buff = new char[buffer_size];
	char* right_buff = new char[buffer_size];

	left.seekg(0, std::ios::beg);
	right.seekg(0, std::ios::beg);

	ProgressBar bar(left_size - 1, bar_length);
	size_t bytes_read = 0;
	while ((!left.fail()) && (!right.fail()))
	{
		right.read(right_buff, buffer_size);
		left.read(left_buff, buffer_size);

		bytes_read = left.gcount();
		if (enable_bar) {
			bar.add(bytes_read);
			bar.display();
		}

		if (memcmp(left_buff, right_buff, bytes_read) != 0) return false;
	}
	if (enable_bar) {
		bar.fill();
		bar.display();
	}

	delete[] left_buff;
	delete[] right_buff;
	return true;
}