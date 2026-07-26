#include "level/level_parser.h"


static std::filesystem::path level_data_file_location = "~";

std::filesystem::path Core::LevelParser::SaveLevelData(Core::Level& data)
{
    std::filesystem::path _path = std::string(level_data_file_location.c_str()) + "/" + data.name + ".plvl";

    

    return _path;
}

void Core::LevelParser::SetLevelDataFileLocation(std::filesystem::path _path)
{

}