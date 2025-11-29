#include "lotpack.h"
#include "io/binary_reader.h"

Lotpack::Lotpack(const LotHeader &header) : header(header) {}

Lotpack Lotpack::read(const std::string &filename, const LotHeader &header)
{
    throw std::runtime_error("not implemented");
}

Lotpack Lotpack::read(const BytesBuffer &buffer, const LotHeader &header)
{
    Lotpack lotpack(header);
    size_t offset = 0;

    lotpack.magic = BinaryReader::read_n_chars(buffer, 4, offset);
    lotpack.version = BinaryReader::readInt32(buffer, offset);
    lotpack.squareMap = Lotpack::readSquareMap(buffer, offset);

    return lotpack;
}

std::unordered_map<CellCoord, SquareData> Lotpack::readSquareMap(const BytesBuffer &buffer, size_t offset)
{
    return std::unordered_map<CellCoord, SquareData>{};
}
