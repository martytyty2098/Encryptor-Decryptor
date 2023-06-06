#include "boring_functions.h"

#define ENCRYPT true
#define DECRYPT false

#define ERROR std::cout << "ERROR : "
#define NERROR std::cout << "\nERROR : "
#define WARNING std::cout << "WARNING : "
#define NWARNING std::cout << "\nWARNING : "

constexpr int bar_length = 30;
std::streamsize global_buffer_size;

bool CheckFilesEquality_Safe(File& left, std::ios::openmode left_mode, File& right, std::ios::openmode right_mode, bool enable_bar = false, int bar_length = 10)
{
    reopen_as(left, std::ios::in | std::ios::binary);
    reopen_as(right, std::ios::in | std::ios::binary);
    bool result = CheckFilesEqualityBin(left.file, right.file, enable_bar, bar_length);
    reopen_as(left, left_mode | std::ios::binary);
    reopen_as(right, right_mode | std::ios::binary);
    return result;
}

bool encrypt_text(File& reserved, File& orig, const std::string& key)
{
    bool success = true;
    const size_t key_length = key.length();
    const std::streamsize buff_size = global_buffer_size > orig.size ? orig.size : global_buffer_size;
    char* buff = new char[buff_size];

    reopen_as(reserved, std::ios::in | std::ios::binary);
    reopen_as(orig, std::ios::out | std::ios::binary);

    ProgressBar bar(reserved.size - 1, 30);

    while (!reserved.file.fail())
    {
        reserved.file.read(buff, buff_size);
        std::streamsize bytes_read = reserved.file.gcount();
        for (std::streamsize i = 0; i < bytes_read; ++i)
        {
            buff[i] = safe_add(buff[i], key[i % key_length]);
        }
        orig.file.write(buff, bytes_read);
        bar.add(bytes_read);
        bar.display();
    }
    bar.fill();
    bar.display();

    delete[] buff;
    return success;
}

bool encrypt_file(File& orig, const std::string& key)
{
    reopen_as(orig, std::ios::in | std::ios::binary);
    bool result = true;
    File reserved;
    reserved.path = orig.path + ".reserved_info";
    reserved.file.open(reserved.path, std::ios::out | std::ios::binary);
    if (!reserved.file.is_open()) 
    {
        ERROR << "Unable to create a file \"" << reserved.path
            << "\"\nYou must create it manually\n"
            << "If the error presists it means that the program does not have the rights to write to that file\n"
            << "or the file is in \"read only\" mode\n"
            << "Try opening this program in administrator mode or taking off \"read only\" mode in that file\n\n";
        return false;
    }

    std::cout << "\nCopying...\n";

    copyFile(orig.file, reserved.file, orig.size, true, bar_length);
    std::cout << "Done!\n\n";

    std::cout << "Checking if file is copied correctly...\n";
    if (!CheckFilesEquality_Safe(orig, std::ios::out, reserved, std::ios::in, true, bar_length))
    {
        NERROR << "File was corrupted while copying\n"
            << "It means that the copying from\n\"" << orig.path << "\"\ninto\n\""
            << reserved.path << "\"\nprobably was disrupted by another program\n"
            << "Fix the issue and try again\n";
        result = false;
        goto cleaning;
    }
    std::cout << "\nYes\n\n";

    reserved.size = orig.size;

    std::cout << "Encrypting...\n";

    reopen_as(orig, std::ios::out | std::ios::binary);

    if (!orig.file.is_open())
    {
        NERROR << "Unable to write to a file \"" << orig.path
            << "\"\nIt means the file is probably in \"read only\" mode or is used by another program\n"
            << "Try taking off \"read only\" mode in that file and run this program again\n\n";
        result = false;
        goto cleaning;
    }

    if (!encrypt_text(reserved, orig, key)) 
    {
        result = false;
        goto cleaning;
    }
    std::cout << "\nDone!\n";

    cleaning:
    std::cout << "\nCleaning copied information...\nDont close the program!\n";
    reopen_as(reserved, std::ios::out | std::ios::binary);

    std::streamsize space_size = global_buffer_size;
    char* space = new char[space_size]{ ' ' };
    for (std::streamsize i = 0; i < space_size; ++i)
    {
        space[i] = ' ';
    }

    ProgressBar bar(reserved.size / space_size + 1, bar_length);
    for (std::streamsize i = 0; i < reserved.size / space_size + 1; ++i)
    {
        reserved.file.write(space, space_size);
        bar.update(i);
        bar.display();
    }
    bar.fill();
    bar.display();
    reserved.file.close();
    delete[] space;

    if (std::remove(reserved.path.c_str()) != 0)
    {
        WARNING << "Unable to delete file at \n\"" << reserved.path <<
            "\"\nYou must delete it manually because it may contain unencrypted data";
    }
    return result;
}

bool decrypt_text(File& orig, const std::string& key)
{
    bool success = true;
    const size_t key_length = key.length();
    const std::streamsize buff_size = global_buffer_size > orig.size ? orig.size : global_buffer_size;
    char* decrypted_buff = new char[buff_size];

    std::string decrypted_path = orig.path + ".decrypted";
    std::ofstream decrypted(decrypted_path, std::ios::binary);

    if (!decrypted.is_open())
    {
        ERROR << "Unable to create\\write to a file at \n\"" << decrypted_path
            << "\"\nyou must create it manually,\n"
            << "if the error presists it means that the program does not have the rights to open that file\n"
            << "or the file is in \"read only\" mode\n"
            << "Try opening this program in administrator mode or taking off \"read only\" mode in that file\n\n";
        success = false;
    }

    ProgressBar bar(orig.size, bar_length);
    std::streamsize bytes_read = 0;

    while ((!orig.file.fail()) && success)
    {
        orig.file.read(decrypted_buff, buff_size);

        bytes_read = orig.file.gcount();

        bar.add(bytes_read);
        bar.display();

        for (std::streamsize i = 0; i < bytes_read; ++i) {
            decrypted_buff[i] = safe_subtract(decrypted_buff[i], key[i % key_length]);
        }

        decrypted.write(decrypted_buff, bytes_read);
    }

    if (success) {
        bar.fill();
        bar.display();
    }
    else if (decrypted.is_open()) {
        decrypted.close();
        if (std::remove(decrypted_path.c_str()) != 0) {
            NWARNING << "Unable to delete file at \"" << decrypted_path <<
                "\"\nYou must delete it manually";
        }
    }

    delete[] decrypted_buff;
    return success;
}

bool decrypt_file(File& orig, const std::string& key)
{
    reopen_as(orig, std::ios::in | std::ios::binary);

    std::cout << "Decrypting...\n";

    if (!decrypt_text(orig, key))
    {
        return false;
    }

    std::cout << "\nDone!\n";

    return true;
}

int main()
{
    bool MODE;
    char input;
    std::cout << "Press 1 to encrypt or 2 to decrypt: ";
    std::cin >> input;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (input == '1') MODE = ENCRYPT;
    else if (input == '2') MODE = DECRYPT;
    else {
        ERROR << "Bad input\nPress Enter to exit . . . ";
        std::cin.get();
        return -1;
    }

    File file;
    std::cout << "Enter file path with its name and extension:\n";
    std::getline(std::cin, file.path, '\n');
    
    std::string key;
    std::cout << "Enter key:\n";
    while (true)
    {
        std::getline(std::cin, key, '\n');
        if (key.length() < 4) {
            std::cout << "Too short! (key must be at least 4 characters long)\n";
        }
        else if (key.length() > 120) {
            std::cout << "Too long! (key must be at most 120 characters long)\n";
        }
        else break;
    }

    file.file.open(file.path, std::ios::in | std::ios::binary);

    if (!file.file.is_open())
    {
        ERROR << "Unable to open file at " << file.path
            << "\nIt means that file does not exist or this program does not have the rights to open it\n"
            << "Try opening this program in administrator mode or renaming file to english characters\n"
            << "\nPress Enter to exit . . . ";
        std::cin.get();
        return -1;
    }

    file.file.seekg(0, std::ios::end);
    file.size = static_cast<std::streamsize>(file.file.tellg());
    file.file.seekg(0, std::ios_base::beg);

    if (file.size == 0) {
        ERROR << "The file at \n\"" << file.path << "\"\nIs empty\n"
            << "\nPress Enter to exit . . . ";
        std::cin.get();
        return -1;
    }
    
    global_buffer_size = file.size < MB(1) ? file.size : MB(1);

    if (MODE == DECRYPT)
    {
        std::cout << "\nThe program will create a decrypted file at \n\""
            << file.path + ".decrypted\"\n";
    }
    if (MODE == ENCRYPT)
    {
        std::cout << "\nThe program will encrypt the file \n\""
            << file.path << "\"\nthereby overwriting it.\n"
            << "It also will create a file \n\"" << file.path + ".reserved_info"
            << "\"\nto store the original information, and will delete it after encryption is done\n";
    }

    std::cout << "\nThis action will require approximately ";
    format_bytes((double)file.size);
    std::cout << " of free space\nPress Enter to continue . . .\n";
    std::cin.get();
    
    bool result = false;

    if (MODE == ENCRYPT)
    {
        result = encrypt_file(file, key);
    }
    if (MODE == DECRYPT)
    {
        result = decrypt_file(file, key);
    }

    std::cout << "\n\n" << (MODE == ENCRYPT ? "Encryption" : "Decryption")
        << (result ? " was successful" : " failed, follow the instructions above")
        << "\n\nPress Enter to exit . . . ";
	std::cin.get();
	return 0;
}