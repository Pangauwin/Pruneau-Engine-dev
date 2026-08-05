#include "level/level_parser.h"
#include "entt/entity/entity.hpp"
#include <cstdio>

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/document.h"
#include "core/application.h"
#include "core/component/component.h"
#include "entt/entity/fwd.hpp"
#include "level/level.h"
#include "level/level_manager.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"

#include <string>

#include <entt/entt.hpp>

static std::filesystem::path level_data_file_location = "/home/jerome-gauwin/Documents/dev/Pruneau-Engine-dev/ressources/saves";

std::filesystem::path Core::LevelParser::SaveLevelData(Core::Level& data)
{
    Core::LogMessageInfo(std::string(level_data_file_location.c_str()) + "/" + data.name + ".plvl");
    std::filesystem::path _path = std::string(level_data_file_location.c_str()) + "/" + data.name + ".plvl";

    std::FILE* file = fopen(_path.c_str(), "w");

    Core::Level* _lvl = Core::LevelManager::GetCurrentLevel();
    entt::registry& _reg = _lvl->GetRegistry();
    auto view = _reg.view<Entity_info>();

    rapidjson::Document doc;
    doc.SetObject();

    rapidjson::Value level(rapidjson::kObjectType);

    level.AddMember("Name", _lvl->name, doc.GetAllocator());
    level.AddMember("camera_index", _lvl->camera_index, doc.GetAllocator());

    rapidjson::Value entities(rapidjson::kObjectType);

    for(auto& _ent: view)
    {
        rapidjson::Value ent(rapidjson::kObjectType);

        ent.AddMember("Id", entt::to_integral(_ent), doc.GetAllocator());
        
        Parent& _parent = _lvl->GetComponent<Parent>(_ent);
        Entity_info& _info = _lvl->GetComponent<Entity_info>(_ent);
        
        ent.AddMember("parent", _parent.entity, doc.GetAllocator());
        ent.AddMember("name", _info.name, doc.GetAllocator());

        rapidjson::Value parsed_comp(rapidjson::kObjectType);
        for(const auto& [id, component]: ComponentRegistry::m_component_registry)
        {
            if(!component.Has(_reg, _ent)) continue;
            
            component.Save(parsed_comp, doc.GetAllocator(), _reg, _ent);
        }

        ent.AddMember("Components", parsed_comp, doc.GetAllocator());

        std::string id_str = std::to_string(entt::to_integral(_ent));

        rapidjson::Value key_value(id_str.c_str(), doc.GetAllocator());

        entities.AddMember(key_value, ent, doc.GetAllocator());
    }

    level.AddMember("Entities", entities, doc.GetAllocator());

    doc.AddMember("level", level, doc.GetAllocator());

    char buffer[65536];
    rapidjson::FileWriteStream os(file, buffer, sizeof(buffer));
    rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);

    writer.SetIndent('\t', 1);

    doc.Accept(writer);

    fclose(file);

    return level_data_file_location;
}


void Core::LevelParser::SetLevelDataFileLocation(std::filesystem::path _path)
{
    level_data_file_location = _path;
}

Core::Level* Core::LevelParser::LoadLevelData(std::filesystem::path _path)
{
    return new Core::Level("");
}


struct SavedEntity
{
    std::string name = "";
    std::uint32_t id = 0;


};