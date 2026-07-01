module;

#include "helpers.hpp"

export module ctext.hooks:render;

import ctext.config;


namespace {
	HOOK(DrawExclamationMark, __fastcall, int x, int y/*, int spriteIdx*/ ) {
		// Do nothing
	}
}


export namespace ctext::hooks {
	void EnableRenderHooks() {
		if (ctext::Config::Get().MiscDisableFieldActionIndicator)
			ENABLE_HOOK(DrawExclamationMark, 0x0D8590);
	}
}