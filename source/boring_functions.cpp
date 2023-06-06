#include "boring_functions.h"

void copyFile(std::fstream& source, std::fstream& dest, std::streamsize source_size, bool enable_bar, int bar_length)
{
    const std::streamsize buff_size = global_buffer_size;
    char* buff = new char[buff_size];
    
    ProgressBar bar(source_size, bar_length);
    std::streamsize bytes_read = 0;
    while (source.good())
    {
        source.read(buff, buff_size);
        bytes_read = source.gcount();
        dest.write(buff, bytes_read);

        if (enable_bar) {
            bar.add(bytes_read);
            bar.display();
        }
    }

    if (enable_bar) {
        bar.fill();
        bar.display();
    }

    dest.flush();
    delete[] buff;
}

void reopen_as(File& file, std::ios::openmode mode)
{
    file.file.close();
    file.file.open(file.path, mode);
}

void format_bytes(double bytes)
{
    std::string extension = "Bytes";
    if (bytes >= 1000000000000) {
        extension = "Terabytes";
        bytes /= 1000000000000.0;
    }
    else if (bytes >= 1000000000) {
        extension = "Gigabytes";
        bytes /= 1000000000.0;
    }
    else if (bytes >= 1000000) {
        extension = "Megabytes";
        bytes /= 1000000.0;
    }
    else if (bytes >= 1000) {
        extension = "Kilobytes";
        bytes /= 1000.0;
    }
    std::cout << std::setprecision(std::to_string((int)bytes).length() + 2) << bytes << " " << extension;
}