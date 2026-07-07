#pragma once

#include "asset/asset.h"

namespace Editor
{

class AssetDisplayer
{
private:
    AssetDisplayer();
    ~AssetDisplayer();

public:
    static void DrawComponentGui(Core::Asset& asset);
    static void DrawComponentContextMenu(Core::Asset& asset);
};

}