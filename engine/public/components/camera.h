#pragma once

#include "core/component/component.h"
#include "core/component/component_system.h"

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
			[] (SaveArchive& ar, const Camera& t)
			{
				// TODO: Implement
			},
			[] (LoadArchive& ar, Camera& t)
			{
				// TODO: Implement
			}
		);
	}

protected:
	void OnUpdate() override;
};
}