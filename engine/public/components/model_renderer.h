#pragma once

#include "asset/asset.h"
#include "core/component/component.h"

namespace Core
{

struct ModelRenderer : public Component
{
	AssetID model_id = 0;
};
}