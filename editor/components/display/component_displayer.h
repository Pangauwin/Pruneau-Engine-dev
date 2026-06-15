#pragma once

#include "core/entity.h"

namespace Editor
{

class ComponentDisplayer
{
private:
    ComponentDisplayer();
    ~ComponentDisplayer();

public:
    static void DrawEntityInterface(Core::Entity& _entity);

    //TODO: Implement registration API (RegisterInspector, UnregisterInspector, etc...) for the user to implement his own functions
};

}