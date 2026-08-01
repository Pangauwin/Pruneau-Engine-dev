#include "level/level_parser.h"
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
#include <fstream>
#include <string>
#include <vector>


#include <entt/entt.hpp>

static std::filesystem::path level_data_file_location = "/home/jerome-gauwin/Documents/dev/Pruneau-Engine-dev/ressources/saves";

std::filesystem::path Core::LevelParser::SaveLevelData(Core::Level& data)
{
    Core::LogMessageInfo(std::string(level_data_file_location.c_str()) + "/" + data.name + ".plvl");
    std::filesystem::path _path = std::string(level_data_file_location.c_str()) + "/" + data.name + ".plvl";

    std::ofstream file(_path);
    cereal::JSONOutputArchive archive( file  );

    Core::Level* _lvl = Core::LevelManager::GetCurrentLevel();
    entt::registry& _reg = _lvl->GetRegistry();
    auto view = _reg.view<Entity_info>();

    for(auto& _ent : view)
    {
        archive.setNextName(std::to_string(entt::to_integral(_ent)).c_str());
        archive.startNode();

        Entity_info& info = _lvl->GetComponent<Entity_info>(_ent);

        archive(cereal::make_nvp("Name", info.name));

        archive.setNextName("Components");
        archive.startNode();

        for(const auto& [id, component]: ComponentRegistry::m_component_registry)
        {
            if(!component.Has(_reg, _ent)) continue;

            archive.startNode();
        
            archive(cereal::make_nvp("Type", component.name));

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

}

struct SerializedEntity
{
    std::uint32_t id;

    std::vector<std::string> components_names;

    
};

struct LevelData
{
    std::string name;

    std::vector<Core::Entity> _entities;

    template<class Archive>
    void save(Archive& ar) const
    {
        //ar(name, _entities);
    }

    template<class Archive>
    void load(Archive& ar)
    {

    }
};
