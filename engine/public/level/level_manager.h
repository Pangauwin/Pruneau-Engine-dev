#pragma once

#include "level.h"

namespace Core
{
class LevelManager
{
public:
	static void Init();

	static void LoadLevel(Level* _level);
	static void SwitchLevel(Level* _level);

	static void OnUpdate(float dt);
	static void OnLateUpdate(float dt);
	static void OnRender();
	static void OnGUIRender();

	static Level* GetCurrentLevel();

	static bool HasCurrentLevelAlreadyAwaked();

private:
	LevelManager(Level* _default_level = nullptr);
	~LevelManager();

	Level* m_current_level;
	static bool has_level_already_awaked;
};
}