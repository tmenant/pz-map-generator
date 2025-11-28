#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "io/binary_reader.h"

const std::string LOTHEADER_PATH = "data/B42/27_38.lotheader";

struct MyStruct
{
    std::string magic;
    int32_t version;
};

int read_header(std::string path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
    {
        std::cerr << "Impossible d'ouvrir le fichier: " << path << std::endl;
        return 1;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char *>(buffer.data()), size))
    {
        std::cerr << "Impossible de lire le fichier.\n";
        return 1;
    }

    BinaryReader reader(buffer);

    size_t offset = 0;

    std::string magic = reader.read_n_chars(4, offset);
    uint32_t version = reader.read_int32(offset);

    std::cout << "magic: " << magic << ", version: " << version << std::endl;
}

int main()
{
    read_header(LOTHEADER_PATH);
}
