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

		/** Parsed from mapinfo_N.dat header — unique per room/map on PC. */
		void OnLocationLoaded(int mapinfoId, int mapTableId, int scriptId) {
			if (mapinfoId > 0)
				mapinfoId_.store(mapinfoId, std::memory_order_relaxed);
			if (mapTableId > 0)
				mapTableId_.store(mapTableId, std::memory_order_relaxed);
			if (scriptId > 0)
				scriptId_.store(scriptId, std::memory_order_relaxed);
			exportRequested_ = true;
		}

		/** Called on the game thread from FieldImpl hook — position only, never map id. */
		void OnFieldImpl(ct::FieldImpl* impl) {
			if (!impl)
				return;

			if (impl->dword854) {
				playerX_.store(impl->dword854[38], std::memory_order_relaxed);
				playerY_.store(impl->dword854[41], std::memory_order_relaxed);
			}

			exportRequested_ = true;
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
			state["version"] = 4;
			state["source"] = "ctext";

			if (canvas) {
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

			const int mapinfoId = mapinfoId_.load(std::memory_order_relaxed);
			const int mapTableId = mapTableId_.load(std::memory_order_relaxed);
			const int scriptId = scriptId_.load(std::memory_order_relaxed);

			if (mapinfoId > 0) {
				state["mapinfoId"] = mapinfoId;
				state["mapSceneId"] = mapinfoId;
			}
			else {
				state["mapinfoId"] = nullptr;
				state["mapSceneId"] = nullptr;
			}

			if (mapTableId > 0)
				state["mapTableId"] = mapTableId;
			else
				state["mapTableId"] = nullptr;

			if (scriptId > 0)
				state["scriptId"] = scriptId;
			else
				state["scriptId"] = nullptr;

			if (mapinfoId > 0 && mapTableId > 0 && scriptId > 0) {
				state["locationKey"] = std::to_string(mapinfoId) + ":"
					+ std::to_string(mapTableId) + ":"
					+ std::to_string(scriptId);
			}

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
		std::atomic<int> mapinfoId_{ -1 };
		std::atomic<int> mapTableId_{ -1 };
		std::atomic<int> scriptId_{ -1 };
		std::atomic<int> playerX_{ 0 };
		std::atomic<int> playerY_{ 0 };
	};
}
