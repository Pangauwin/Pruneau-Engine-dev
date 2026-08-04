#include "level/level_parser.h"
#include "cereal/external/rapidjson/document.h"
#include "core/application.h"
#include "core/component/component.h"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "level/level.h"
#include "level/level_manager.h"

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <string>

#include <entt/entt.hpp>

static std::filesystem::path level_data_file_location = "/home/jerome-gauwin/Documents/dev/Pruneau-Engine-dev/ressources/saves";

std::filesystem::path Core::LevelParser::SaveLevelData(Core::Level& data)
{
    Core::LogMessageInfo(std::string(level_data_file_location.c_str()) + "/" + data.name + ".plvl");
    std::filesystem::path _path = std::string(level_data_file_location.c_str()) + "/" + data.name + ".plvl";

    std::ofstream file(_path);
    Core::SaveArchive archive( file  );

    Core::Level* _lvl = Core::LevelManager::GetCurrentLevel();
    entt::registry& _reg = _lvl->GetRegistry();
    auto view = _reg.view<Entity_info>();

    const std::size_t size = view.size();

    archive(cereal::make_nvp("Name", _lvl->name));
    archive(cereal::make_nvp("Camera Index", _lvl->camera_index));

    archive(cereal::make_nvp("Entities", cereal::make_size_tag(size)));

    for(auto& _ent : view)
    {
        archive.startNode();

        archive(cereal::make_nvp("ID", entt::to_integral(_ent)));

        Entity_info& info = _lvl->GetComponent<Entity_info>(_ent);
        archive(cereal::make_nvp("Name", info.name));

        archive.setNextName("Components");
        archive.startNode();

        for(const auto& [id, component]: ComponentRegistry::m_component_registry)
        {
            if(!component.Has(_reg, _ent)) continue;
            
            archive.setNextName(component.name.c_str());
            archive.startNode();
        
            component.Save(archive, _reg, _ent);

            archive.finishNode();
        }

        archive.finishNode();

        archive.finishNode();
    }

    return _path;
}


void Core::LevelParser::SetLevelDataFileLocation(std::filesystem::path _path)
{
    level_data_file_location = _path;
}

Core::Level* Core::LevelParser::LoadLevelData(std::filesystem::path _path)
{
    std::ifstream input_file(_path);


    std::string json ((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

    rapidjson::Document doc;

    doc.Parse(json.c_str());

    if(doc.HasParseError())
    {
        Core::LogMessageError("Failed to load level: JSON parse error");
        return nullptr;
    }

    Core::Level* new_level = new Level(doc.HasMember("Name") && doc["Name"].IsString() ? doc["Name"].GetString() : "Anonyme level");

    return new_level;
}


struct SavedEntity
{
    std::string name = "";
    std::uint32_t id = 0;

    
};