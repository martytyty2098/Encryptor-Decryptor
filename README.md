# Encryptor-Decryptor
C++ program that can encrypt a file of _**`any`**_ type and then decrypt it back
# Usage
Just run the executable and follow displayed instructions.
# How does it work
User decides whether he wants to encrypt a file or decrypt his data that was already encrypted by **THIS** program.\
Then he inserts a file path with its name and extension like `C:\users\username\desktop\some_file.dat` and a **key** that will be used to **encrypt** or **decrypt** data **(its important that when you decrypt a file you must insert the same key that you inserted while encrypting it)**
## Encryption
- #### **Copying stage**
The program creates a file named `(original_filename).reserved_info` and copies the contents of your `original file` that you want to encrypt, in it.
- #### **Checking stage**
The program is checking if the contents of `original file` were copied correctly into `reserved_info` becuse even a small error can cause file corruption.
- #### **Encrypting stage**
The program reads some bytes from `reserved_info` and adds the ASCII value of characters in the key to every byte read.\
Then it writes these encrypted bytes into `original file` and does all that until it encrypts all bytes from `reserved_info` and writes them to `original file`
- #### **Cleaning stage**
When the `original file` was successfully encrypted, the program deletes the `reserved_info` because it contains unencrypted information from `original file`
## Decryption
The program creates a file named `original_filename.decrypted` then it reads bytes from `original file` that user wants to decrypt and subtracts the ASCII value of characters in the key from each byte read.\
Then it writes these bytes to freshly created `decrypted file`\
You will later need to add the needed extension to it (like `.exe` or `.mp4`)

# Troubleshooting
- If you were **encrypting** a file and your program for some reason was closed during **`Encryption stage`**, then the original file _may_ be rendered **useless** and you will not be able to restore it, in that case you must find the file named `(original_filename).reserved_info` **(the program should create it somewhere in the same directory as your original file)** and add the needed extension to it (like `.exe` or `.mp4`)
# Cool things
- This program can encrypt any type of file (text, photo, video, executables and so on) and then decrypt it back to the original state.
- This program should work on any OS since it only uses standard C++ features, but I only tested it on **Windows 10** so I don't know about **Linux** or **macOS**.
# Known issues
- The file path passed to the program must be in english characters.
- This program has a hard time dealing with large files (5 GB and more) and i dont know how to fix it.
