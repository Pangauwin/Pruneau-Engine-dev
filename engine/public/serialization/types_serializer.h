#pragma once

#include <rapidjson/document.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#pragma region GLM

inline void save(rapidjson::Value& out, rapidjson::Document::AllocatorType& alloc, const glm::vec3& v)
{
    out.SetObject();
    out.AddMember("x", v.x, alloc);
    out.AddMember("y", v.y, alloc);
    out.AddMember("z", v.z, alloc);
}

inline bool load(const rapidjson::Value& in, glm::vec3& out)
{
    if(!in.IsObject()) return false;
    if(!(in.HasMember("x") && in.HasMember("y") && in.HasMember("z"))) return false;
    if(!(in["x"].IsNumber() && in["y"].IsNumber() && in["z"].IsNumber())) return false;

    out.x = in["x"].GetFloat();
    out.y = in["y"].GetFloat();
    out.z = in["z"].GetFloat();

    return true;
}

inline void save(rapidjson::Value& out, rapidjson::Document::AllocatorType& alloc, const glm::quat& q)
{
    out.SetObject();
    out.AddMember("x", q.x, alloc);
    out.AddMember("y", q.y, alloc);
    out.AddMember("z", q.z, alloc);
    out.AddMember("w", q.w, alloc);
}

inline bool load(const rapidjson::Value& in, glm::quat& out)
{
    if(!in.IsObject()) return false;
    if(!(in.HasMember("x") && in.HasMember("y") && in.HasMember("z") && in.HasMember("w"))) return false;
    if(!(in["x"].IsNumber() && in["y"].IsNumber() && in["z"].IsNumber() && in["w"].IsNumber())) return false;

    out.x = in["x"].GetFloat();
    out.y = in["y"].GetFloat();
    out.z = in["z"].GetFloat();
    out.w = in["w"].GetFloat();

    return true;
}

#pragma endregion

