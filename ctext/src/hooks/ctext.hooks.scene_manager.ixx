module;

#include "helpers.hpp"

export module ctext.hooks:scene_manager;

import ctext.companion_export;
import ctext.config;


namespace {
	HOOK(SceneManager_create, __fastcall, int, mapId, int, arg2) {
		if (ctext::Config::Get().CompanionEnabled)
			ctext::companion::CompanionExport::Get().OnMapScene(mapId);
		return CALL_ORIG(SceneManager_create, mapId, arg2);
	}

	HOOK(SceneManager_pushScene, __fastcall, int, mapId, int, arg2) {
		if (ctext::Config::Get().CompanionEnabled)
			ctext::companion::CompanionExport::Get().OnMapScene(mapId);
		CALL_ORIG(SceneManager_pushScene, mapId, arg2);
	}

	HOOK(SceneManager_NextScene, __fastcall, int, mapId) {
		if (ctext::Config::Get().CompanionEnabled)
			ctext::companion::CompanionExport::Get().OnMapScene(mapId);
		CALL_ORIG(SceneManager_NextScene, mapId);
	}
}


export namespace ctext::hooks {
	void EnableSceneManagerHooks() {
		if (!ctext::Config::Get().CompanionEnabled)
			return;

		ENABLE_HOOK(SceneManager_create, 0x297860);
		ENABLE_HOOK(SceneManager_pushScene, 0x298410);
		ENABLE_HOOK(SceneManager_NextScene, 0x297B60);
	}
}
