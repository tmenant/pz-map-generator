#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "file_reader.h"

BytesBuffer FileReader::read(std::string path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
    {
        throw std::runtime_error("Impossible d'ouvrir le fichier: " + path);
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char *>(buffer.data()), size))
    {
        throw std::runtime_error("Impossible d'ouvrir le fichier: " + path);
    }

    return buffer;
}