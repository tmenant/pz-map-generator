#pragma once

#include <string>
#include <vector>

#include "types.h"

struct Texture
{
    std::string name;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t ox;
    uint32_t oy;
    uint32_t ow;
    uint32_t oh;
};

struct Page
{
    int32_t version;
    std::string name;
    uint32_t hasAlpha;
    BytesBuffer png;
    std::vector<Texture> textures;
};

class TexturePack
{
public:
    std::string magic;
    uint32_t version;
    std::vector<Page> pages;

    TexturePack() = default;

    static TexturePack read(const BytesBuffer &buffer);
    static std::vector<Page> readPages(const BytesBuffer &buffer, uint32_t version, size_t &offset);
    static std::vector<Texture> readTextures(const BytesBuffer &buffer, size_t &offset);
};
