module;

#include "helpers.hpp"

#include <cocos/2d/CCScene.h>

export module ctext.hooks:scene_manager;

import ctext.config;


namespace {
	HOOK_RET(SceneManager_create, __fastcall, cocos2d::Scene*, int sceneIndex, int arg2) {
		return CALL_ORIG(SceneManager_create, sceneIndex, arg2);
	}

	HOOK(SceneManager_pushScene, __fastcall, int sceneIndex, int arg2) {
		CALL_ORIG(SceneManager_pushScene, sceneIndex, arg2);
	}

	HOOK(SceneManager_NextScene, __fastcall, int sceneIndex) {
		CALL_ORIG(SceneManager_NextScene, sceneIndex);
	}
}


export namespace ctext::hooks {
	void EnableSceneManagerHooks() {
		if (!ctext::Config::Get().CompanionEnabled)
			return;

		// Scene hooks disabled — pushScene id is not the map index; use mapinfo load instead.
	}
}
