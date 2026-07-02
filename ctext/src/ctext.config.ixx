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

			MiscDisableFieldActionIndicator = cfg["misc"]["disable_field_action_indicator"];

			ModsLoadOrder = cfg["mods"]["load_order"];

			CompanionEnabled = false;
			CompanionPollIntervalMs = 1000;
			CompanionExportPath = "";
			CompanionStorylineRva = 0;

			if (cfg.contains("companion") && cfg["companion"].is_object()) {
				const auto& companion = cfg["companion"];
				CompanionEnabled = companion.value("enabled", false);
				CompanionPollIntervalMs = companion.value("poll_interval_ms", 1000);
				CompanionExportPath = companion.value("export_path", "");
				CompanionStorylineRva = companion.value("storyline_rva", 0);
			}
		}


		bool FixesRevertDiagonalMovement;

		bool GraphicsForceNearestFilter;

		bool FontForceNearestFilter;
		bool FontUseCustomFont;
		std::string FontCustomFont;
		bool FontUseFixedFontSize;
		int FontFixedFontSize;

		bool MiscDisableFieldActionIndicator;

		std::vector<std::string> ModsLoadOrder;

		bool CompanionEnabled;
		int CompanionPollIntervalMs;
		std::string CompanionExportPath;
		int CompanionStorylineRva;
	};
}