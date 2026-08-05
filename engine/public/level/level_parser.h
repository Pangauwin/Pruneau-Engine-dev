#pragma once

#include "level.h"
#include <cstdint>
#include <filesystem>

namespace Core
{
    
using Entity = std::uint32_t;

class LevelParser
{
public:
    static Core::Level* LoadLevelData(std::filesystem::path _path);
    static std::filesystem::path SaveLevelData(Core::Level& _data);

    static void SetLevelDataFileLocation(std::filesystem::path _path);

private:
    LevelParser();
    ~LevelParser();    
};

struct LevelHeader
{
    std::uint32_t magic = 'LVL\0';
    std::uint16_t format_version = 1;
};
}