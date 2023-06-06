#pragma once

#include <fstream>
#include <bitset>
#include <string>
#include <iostream>
#include <limits>
#include <iomanip>
#include "FileEqualityCheck.h"
#include "ProgressBar.hpp"

#define safe_add(ch, term) unsigned char(ch + term)
#define safe_subtract(ch, sub) unsigned char(ch - sub)
#define KB(amount) 1024 * amount
#define MB(amount) KB(1024) * amount

typedef unsigned char uchar;

extern std::streamsize global_buffer_size;

struct File
{
    std::fstream file;
    std::string path;
    std::streamsize size;
};

void copyFile(std::fstream& source, std::fstream& dest, std::streamsize source_size = 0, bool enable_bar = false, int bar_length = 10);

void reopen_as(File& file, std::ios::openmode mode);

void format_bytes(double bytes);