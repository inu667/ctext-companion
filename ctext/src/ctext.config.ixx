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

			FixesRevertDiagonalMovement = cfg["fixes"].value("revert_diagonal_movement", false);

			GraphicsForceNearestFilter = cfg["graphics"].value("force_nearest_filter", false);

			FontForceNearestFilter = cfg["font"].value("force_nearest_filter", false);
			FontUseCustomFont = cfg["font"].value("use_custom_font", false);
			FontCustomFont = cfg["font"].value("custom_font", "");
			FontUseFixedFontSize = cfg["font"].value("use_fixed_font_size", false);
			FontFixedFontSize = cfg["font"].value("fixed_font_size", 16);

			MiscDisableFieldActionIndicator = cfg["misc"].value("disable_field_action_indicator", false);

			ModsLoadOrder = cfg["mods"].value("load_order", std::vector<std::string>{});

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