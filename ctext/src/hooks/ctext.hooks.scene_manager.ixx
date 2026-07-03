module;

#include "helpers.hpp"

#include <cocos/2d/CCScene.h>

export module ctext.hooks:scene_manager;

import ctext.companion_export;
import ctext.config;


namespace {
	HOOK_RET(SceneManager_create, __fastcall, cocos2d::Scene*, int sceneIndex, int arg2) {
		if (ctext::Config::Get().CompanionEnabled)
			ctext::companion::CompanionExport::Get().OnMapScene(sceneIndex);
		return CALL_ORIG(SceneManager_create, sceneIndex, arg2);
	}

	HOOK(SceneManager_pushScene, __fastcall, int sceneIndex, int arg2) {
		if (ctext::Config::Get().CompanionEnabled)
			ctext::companion::CompanionExport::Get().OnMapScene(sceneIndex);
		CALL_ORIG(SceneManager_pushScene, sceneIndex, arg2);
	}

	HOOK(SceneManager_NextScene, __fastcall, int sceneIndex) {
		if (ctext::Config::Get().CompanionEnabled)
			ctext::companion::CompanionExport::Get().OnMapScene(sceneIndex);
		CALL_ORIG(SceneManager_NextScene, sceneIndex);
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
