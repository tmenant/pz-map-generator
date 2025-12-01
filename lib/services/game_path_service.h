#include <string>
#include <vector>

class GamePathService
{
public:
    std::string gamePath;
    std::vector<std::string> lotpackFiles;
    std::vector<std::string> lotheaderFiles;

    GamePathService(std::string gamePath) : gamePath(gamePath) {}

    std::string mapDirectory(std::string mapName);
};