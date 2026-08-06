#include "level/level_parser.h"
#include "entt/core/fwd.hpp"
#include "entt/entity/entity.hpp"
#include <cstdio>
#include <fstream>
#include <iterator>

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
    std::ifstream file(_path);

    std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    rapidjson::Document doc;

    doc.Parse(json.c_str());

    if(doc.HasParseError())
    {
        Core::LogMessageError("Unable to load level: save file has parse errors!");
        return nullptr;
    }

    if(!doc.HasMember("level"))
    {
        Core::LogMessageError("Unable to load level: save file does not contain any level data!");
        return nullptr;
    }

    std::string level_name = "Anonyme Level";
    unsigned int camera_index = 0;

    rapidjson::Value& j_lvl = doc["level"];

    if(!j_lvl.HasMember("Name"))
    {
        Core::LogMessageWarning("Saved Level had no name! Naming this level as \"Anonyme Level\"");
    }
    else {
        level_name = j_lvl["Name"].GetString();
    }

    if(!j_lvl.HasMember("camera_index"))
    {
        Core::LogMessageWarning("Saved Level had no camera_index! Defaulting camera_index to 0");
    }
    else {
        camera_index = j_lvl["camera_index"].GetUint();
    }

    Core::Level* _new_lvl = new Level(level_name.c_str());
    _new_lvl->camera_index = camera_index;

    if(!j_lvl.HasMember("Entities"))
    {
        Core::LogMessageError("Unable to load level! No entities found!");
    }
    else 
    {
        rapidjson::Value& entities = j_lvl["Entities"];
        
        for(rapidjson::Value::ConstMemberIterator itr = entities.MemberBegin();
            itr != entities.MemberEnd(); ++itr)
        {
            Entity new_entity = _new_lvl->CreateEntity(itr->value["name"].GetString());
            _new_lvl->SetParent(new_entity, itr->value["parent"].GetUint());

            for(rapidjson::Value::ConstMemberIterator j_comp = itr->value["Components"].MemberBegin();
                j_comp != itr->value["Components"].MemberEnd(); ++j_comp)
            {
                if(!ComponentRegistry::HasComponentName(j_comp->name.GetString()))
                {
                    Core::LogMessageError("Failed adding component " + std::string(j_comp->name.GetString()) + ": No such component found in component registry!");
                    continue;
                }

                entt::id_type comp = ComponentRegistry::m_name_to_id[j_comp->name.GetString()];
                if(!ComponentRegistry::m_component_registry[comp].Load(j_comp->value, _new_lvl->GetRegistry(), static_cast<entt::entity>(new_entity)))
                {
                    Core::LogMessageError("Failed to load component " + std::string(j_comp->name.GetString()) + "!");
                } 
            }
        }
    }

    return _new_lvl;
}