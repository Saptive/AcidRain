#include <iostream>
#include <fstream>
#include <vector>

void appendFileAtByte(const std::string& srcFile, const std::string& destFile, std::streampos position) {
    // Open the source file in binary mode
    std::ifstream src(srcFile, std::ios::binary);
    if (!src) 
    {
        printf("Error opening source file: %s\r\n", srcFile);
        return;
    }

    // Read entire source file into a buffer
    std::vector<char> buffer((std::istreambuf_iterator<char>(src)), std::istreambuf_iterator<char>());
    src.close();

    // Open the destination file in read-update binary mode
    std::fstream dest(destFile, std::ios::in | std::ios::out | std::ios::binary);
    if (!dest) 
    {
        std::cerr << "Error opening destination file: " << destFile << std::endl;        return;
    }

    // Move the file pointer to the desired position (512th byte, since index starts at 0)
    dest.seekp(position);
    if (dest.fail()) 
    {
        printf("seekp faileed\r\n");
        dest.close();
        return;
    }

    // Write the source file content at the specified position
    dest.write(buffer.data(), buffer.size());
    if (dest.fail()) 
    {
        std::cerr << "Error writing to destination file.\n";
    }


    dest.close();
}

int main() {
    std::string sourceFilename = "kernel.bin";  // Replace with your source file
    std::string destinationFilename = "disk.img";  // Replace with your destination file
    std::streampos insertPosition = 512;  // 513th byte (0-based index)

    appendFileAtByte(sourceFilename, destinationFilename, insertPosition);

    return 0;
}


