#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "types.h"

class TexturePack
{
public:
    struct Texture
    {
        std::string name;
        int32_t x;
        int32_t y;
        int32_t width;
        int32_t height;
        int32_t ox;
        int32_t oy;
        int32_t ow;
        int32_t oh;
    };

    struct Page
    {
        int32_t version;
        std::string name;
        uint32_t hasAlpha;
        BytesBuffer png;
        std::vector<Texture> textures;
    };

    std::string name;
    std::string magic;
    uint32_t version;
    std::vector<Page> pages;

    TexturePack() = default;

    static TexturePack read(const std::filesystem::path &path);
    static TexturePack read(const std::string &name, const BytesBuffer &buffer);
    static int32_t readVersion(const BytesBuffer &buffer, std::string magic, size_t &offset);
    static BytesBuffer readPNG(const BytesBuffer &buffer, int32_t version, size_t &offset);
    static std::vector<Page> readPages(const BytesBuffer &buffer, int32_t version, size_t &offset);
    static std::vector<Texture> readTextures(const BytesBuffer &buffer, size_t &offset);
};
