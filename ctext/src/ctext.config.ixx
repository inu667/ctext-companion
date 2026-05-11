module;

#include <nlohmann/json.hpp>

export module ctext.config;

import ctext.singleton;

import std;


export namespace ctext {
	class Config final : public Singleton<Config> {
	public:
		void Initialise() {
			std::ifstream file("ctext.json");
			auto cfg = nlohmann::json::parse(file, nullptr, true, true, true);

			FixesRevertDiagonalMovement = cfg["fixes"]["revert_diagonal_movement"];

			GraphicsForceNearestFilter = cfg["graphics"]["force_nearest_filter"];

			FontForceNearestFilter = cfg["font"]["force_nearest_filter"];
			FontUseCustomFont = cfg["font"]["use_custom_font"];
			FontCustomFont = cfg["font"]["custom_font"];
			FontUseFixedFontSize = cfg["font"]["use_fixed_font_size"];
			FontFixedFontSize = cfg["font"]["fixed_font_size"];

			ModsLoadOrder = cfg["mods"]["load_order"];

			file.close();
		}


		bool FixesRevertDiagonalMovement;

		bool GraphicsForceNearestFilter;

		bool FontForceNearestFilter;
		bool FontUseCustomFont;
		std::string FontCustomFont;
		bool FontUseFixedFontSize;
		int FontFixedFontSize;

		std::vector<std::string> ModsLoadOrder;
	};
}