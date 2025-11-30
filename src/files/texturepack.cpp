#include <vector>

#include "exceptions.h"
#include "io/binary_reader.h"
#include "texturepack.h"

TexturePack TexturePack::read(const BytesBuffer &buffer)
{
    TexturePack texturePack{};
    size_t offset = 0;

    texturePack.magic = BinaryReader::read_n_chars(buffer, 4, offset);
    texturePack.version = BinaryReader::readInt32(buffer, offset);
    texturePack.pages = TexturePack::readPages(buffer, texturePack.version, offset);

    if (offset != buffer.size())
    {
        throw Exceptions::FileEndNotReached(offset, buffer.size());
    }

    return texturePack;
}

std::vector<Page> TexturePack::readPages(const BytesBuffer &buffer, uint32_t version, size_t &offset)
{
    int32_t pagesCount = BinaryReader::readInt32(buffer, offset);
    std::vector<Page> pages(pagesCount);

    for (int i = 0; i < pagesCount; i++)
    {
        Page page{};

        page.version = version;
        page.name = BinaryReader::readStringWithLength(buffer, offset);
        page.textures = TexturePack::readTextures(buffer, offset);
        page.png = BinaryReader::readBytesWithLength(buffer, offset);

        pages[i] = page;
    }

    return pages;
}

std::vector<Texture> TexturePack::readTextures(const BytesBuffer &buffer, size_t &offset)
{
    int32_t texturesCount = BinaryReader::readInt32(buffer, offset);
    std::vector<Texture> textures(texturesCount);

    bool hasAlpha = BinaryReader::readInt32(buffer, offset);

    for (int i = 0; i < texturesCount; i++)
    {
        Texture texture{};

        texture.name = BinaryReader::readStringWithLength(buffer, offset);
        texture.x = BinaryReader::readInt32(buffer, offset);
        texture.y = BinaryReader::readInt32(buffer, offset);
        texture.width = BinaryReader::readInt32(buffer, offset);
        texture.height = BinaryReader::readInt32(buffer, offset);
        texture.ox = BinaryReader::readInt32(buffer, offset);
        texture.oy = BinaryReader::readInt32(buffer, offset);
        texture.ow = BinaryReader::readInt32(buffer, offset);
        texture.oh = BinaryReader::readInt32(buffer, offset);

        textures[i] = texture;
    }

    return textures;
}
