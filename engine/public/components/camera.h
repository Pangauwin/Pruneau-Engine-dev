#pragma once

#include "core/application.h"
#include "core/component/component.h"
#include "core/component/component_system.h"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace Renderer
{
class Renderer;
}

namespace Core
{

enum CAMERA_TYPE
{
	CAMERA_TYPE_PERSPECTIVE,
	CAMERA_TYPE_ORTHOGRAPHIC
};

struct CameraData {
	virtual ~CameraData() = default;
	virtual CameraData* Clone() const = 0;
};

struct CameraPerspectiveData : public CameraData
{
	float fov = 70.f;
	float aspect = 16.f / 9.f;
	float near_plane = 0.1f;
	float far_plane = 100.f;

	CameraPerspectiveData* Clone() const override { return new CameraPerspectiveData(*this); }
};

struct CameraOrthographicData : public CameraData
{
	float top = 50.f;
	float bottom = -50.f;
	float left = -50.f;
	float right = 50.f;

	CameraOrthographicData* Clone() const override { return new CameraOrthographicData(*this); }
};

struct CameraConfig {
	CAMERA_TYPE type;
	CameraData* data; //TODO : use a variant instead

	CameraConfig(CAMERA_TYPE _type, CameraData* _data) : type(_type), data(nullptr)
	{
		if (_data)
		{
			data = _data->Clone();
		}
		else
		{
			if (type == CAMERA_TYPE_PERSPECTIVE)
				data = new CameraPerspectiveData();
			else
				data = new CameraOrthographicData();
		}
	}

	// copy ctor
	CameraConfig(const CameraConfig& other) : type(other.type), data(nullptr)
	{
		if (other.data) data = other.data->Clone();
	}

	// copy assign
	CameraConfig& operator=(const CameraConfig& other)
	{
		if (this == &other) return *this;
		type = other.type;
		delete data;
		data = other.data ? other.data->Clone() : nullptr;
		return *this;
	}

	~CameraConfig()
	{
		delete data;
	}
};

struct Camera : public Component
{
	bool dirty = true;
	CameraConfig config = CameraConfig(CAMERA_TYPE_PERSPECTIVE, 
		new CameraPerspectiveData());
	glm::mat4 projection = glm::mat4(1.0f);

	unsigned int index = 0;
};

class CameraSystem : public Core::ComponentSystem
{
public:
	static unsigned int GetNextCameraIndex();

	void Register() override {
		ComponentRegistry::RegisterComponent<Camera>(
			"Camera",
			[] (rapidjson::Value& _val, rapidjson::Document::AllocatorType& _al, const Camera& t)
			{
				rapidjson::Value index;
				index.SetInt(t.index);

				_val.AddMember("index", index, _al);

				rapidjson::Value config(rapidjson::kObjectType);
				
				if(t.config.type == CAMERA_TYPE_PERSPECTIVE)
				{
					config.AddMember("type", "perspective", _al);
					CameraPerspectiveData* data = static_cast<CameraPerspectiveData*>(t.config.data);
					config.AddMember("fov", data->fov, _al);
					config.AddMember("far", data->far_plane, _al);
					config.AddMember("near", data->near_plane, _al);
				}

				else
				{
					config.AddMember("type", "orthographic", _al);
					CameraOrthographicData* data = static_cast<CameraOrthographicData*>(t.config.data);

					config.AddMember("bottom", data->bottom, _al);
					config.AddMember("top", data->top, _al);
					config.AddMember("left", data->left, _al);
					config.AddMember("right", data->right, _al);
				}

				_val.AddMember("config", config, _al);
			},
			[] (const rapidjson::Value& _val, Camera& t)
			{
				if(!_val.HasMember("config"))
				{
					Core::LogMessageError("Unable to load camera ! No Camera data !");
					return false;
				}
				else {
					if(std::string(_val["config"]["type"].GetString()) == "perspective")
					{
						t.config.type = CAMERA_TYPE_PERSPECTIVE;
						t.config.data = new CameraPerspectiveData();
						CameraPerspectiveData* data = static_cast<CameraPerspectiveData*>(t.config.data);
						data->fov = _val["config"]["fov"].GetFloat();
						data->near_plane = _val["config"]["near"].GetFloat();
						data->far_plane = _val["config"]["far"].GetFloat();
					}
					else {
						t.config.type = CAMERA_TYPE_ORTHOGRAPHIC;
						t.config.data = new CameraOrthographicData();
						CameraOrthographicData* data = static_cast<CameraOrthographicData*>(t.config.data);
						data->bottom = _val["config"]["bottom"].GetFloat();
						data->top = _val["config"]["top"].GetFloat();
						data->left = _val["config"]["left"].GetFloat();
						data->right = _val["config"]["right"].GetFloat();
					}

				}

				if(!_val.HasMember("index"))
				{
					Core::LogMessageWarning("No camera index affected to camera, defaulting to 1"); // TODO: default the value to the one given by the level itself
					t.index = 1;
				}
				else {
					t.index = _val["index"].GetInt();
				}

				t.dirty = true;

				return true;
			}
		);
	}

protected:
	void OnUpdate() override;
};
}