module;

#include "helpers.hpp"

export module ctext.hooks:scene_manager;

import ct.scene;
import ctext.companion_export;
import ctext.config;


namespace {
	HOOK(SceneManager_create, __fastcall, int, id, int, a2) {
		if (ctext::Config::Get().CompanionEnabled)
			ctext::companion::CompanionExport::Get().OnMapScene(id);
		return CALL_ORIG(SceneManager_create, id, a2);
	}

	HOOK(SceneManager_pushScene, __fastcall, int, id, int, a2) {
		if (ctext::Config::Get().CompanionEnabled)
			ctext::companion::CompanionExport::Get().OnMapScene(id);
		CALL_ORIG(SceneManager_pushScene, id, a2);
	}

	HOOK(SceneManager_NextScene, __fastcall, int, a1) {
		if (ctext::Config::Get().CompanionEnabled)
			ctext::companion::CompanionExport::Get().OnMapScene(a1);
		CALL_ORIG(SceneManager_NextScene, a1);
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
