module;

#include "helpers.hpp"

#include <Windows.h>
#include <ShlObj.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

#include <nlohmann/json.hpp>

export module ctext.companion_export;

import ctext.config;
import ctext.singleton;
import ct;
import ct.scene;


export namespace ctext::companion {
	class CompanionExport final : public Singleton<CompanionExport> {
	public:
		void Start() {
			auto& cfg = Config::Get();
			if (!cfg.CompanionEnabled || running_)
				return;

			exportPath_ = ResolveExportPath(cfg.CompanionExportPath);
			running_ = true;

			try {
				std::ofstream probe("ctext_companion.ok", std::ios::trunc);
				probe << "companion_export_start\n";
			}
			catch (...) {
			}

			worker_ = std::thread([this] { WorkerLoop(); });
		}

		void Stop() {
			running_ = false;
			if (worker_.joinable())
				worker_.join();
		}

		void RequestExport() {
			exportRequested_ = true;
		}

		/** CTViewer / pushScene map index — unique per field map on PC. */
		void OnMapScene(int id) {
			if (id >= 0)
				mapSceneId_.store(id, std::memory_order_relaxed);
			exportRequested_ = true;
		}

		void OnPlayerPosition(int x, int y) {
			playerX_.store(x, std::memory_order_relaxed);
			playerY_.store(y, std::memory_order_relaxed);
		}

	private:
		std::filesystem::path ResolveExportPath(const std::string& configured) {
			if (!configured.empty())
				return std::filesystem::path(configured);

			PWSTR docs = nullptr;
			if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &docs))) {
				std::filesystem::path path(docs);
				CoTaskMemFree(docs);
				path /= "My Games";
				path /= "Chrono Trigger";
				path /= "companion_state.json";
				return path;
			}

			return "companion_state.json";
		}

		void WorkerLoop() {
			while (running_) {
				ExportOnce();

				const int intervalMs = Config::Get().CompanionPollIntervalMs < 100
					? 100
					: Config::Get().CompanionPollIntervalMs;
				for (int waited = 0; running_ && waited < intervalMs; waited += 50) {
					if (exportRequested_.exchange(false)) {
						ExportOnce();
						break;
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
				}
			}
		}

		void ExportOnce() {
			auto* canvas = ct::ChronoCanvas::getInstance();

			nlohmann::json state;
			state["version"] = 2;
			state["source"] = "ctext";

			if (canvas) {
				// currentFieldId is NOT a unique location on PC (repeats across maps).
				state["fieldId"] = canvas->currentFieldId;
				state["sceneId"] = ct::scene::SceneManager::nowScene();
				state["party"] = nlohmann::json::array();
				for (int i = 0; i < 3; ++i)
					state["party"].push_back(canvas->activePartyMemberIds[i]);
			}
			else {
				state["fieldId"] = nullptr;
				state["sceneId"] = nullptr;
				state["party"] = nlohmann::json::array();
				state["exportStatus"] = "no_canvas";
			}

			const int mapSceneId = mapSceneId_.load(std::memory_order_relaxed);
			state["mapSceneId"] = mapSceneId >= 0 ? mapSceneId : nullptr;
			state["posX"] = playerX_.load(std::memory_order_relaxed);
			state["posY"] = playerY_.load(std::memory_order_relaxed);

			if (Config::Get().CompanionStorylineRva != 0) {
				auto* storyline = ADDR_AS(uint8_t*, Config::Get().CompanionStorylineRva);
				state["storyline"] = *storyline;
			}
			else {
				state["storyline"] = nullptr;
			}

			state["inBattle"] = false;
			state["eventFlags"] = nlohmann::json::object();

			const auto now = std::chrono::system_clock::now();
			const std::time_t tt = std::chrono::system_clock::to_time_t(now);
			std::tm utc{};
			gmtime_s(&utc, &tt);
			char buf[32];
			std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &utc);
			state["updatedAt"] = buf;

			try {
				if (!exportPath_.parent_path().empty())
					std::filesystem::create_directories(exportPath_.parent_path());

				std::ofstream out(exportPath_, std::ios::binary | std::ios::trunc);
				out << state.dump(2) << "\n";
			}
			catch (...) {
			}
		}

		std::filesystem::path exportPath_;
		std::thread worker_;
		std::atomic<bool> running_{ false };
		std::atomic<bool> exportRequested_{ false };
		std::atomic<int> mapSceneId_{ -1 };
		std::atomic<int> playerX_{ 0 };
		std::atomic<int> playerY_{ 0 };
	};
}
