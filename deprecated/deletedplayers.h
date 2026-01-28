#include <unordered_map>
#include <Geode/cocos/cocoa/CCGeometry.h>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <Geode/loader/Log.hpp>
#include <Geode/modify/Modify.hpp>
#include <Geode/binding/GJBaseGameLayer.hpp>
#include <Geode/binding/PlayerObject.hpp>
#include <Geode/binding/PlayLayer.hpp>

// ImGui::TextUnformatted("Disable unsafe player actions");  ImGui::SameLine(); ImGui::Checkbox("##crashfix", &PlayerTracker::guardAgainstCrashes);


using namespace geode::prelude;

class PlayerTracker {
public:
	static void markPlayerAlive(PlayerObject* player, const char* reason) {
		if (!LivingPlayers.contains(player)) {
			LivingPlayers.insert({ player, 1 });
			geode::log::info("Marked player {} Living for {}", (void*)player, reason);
		}
	}

	static void markPlayerDead(PlayerObject* player, const char* reason) {
		if (LivingPlayers.contains(player)) {
			LivingPlayers.erase(player);
			geode::log::info("Marked player {} Dead for {}", (void*)player, reason);
		}
	}

	static void markAllDead(const char* reason) {
		LivingPlayers.clear();
		geode::log::info("Marked all Dead for {}", reason);
	}

	static bool isPlayerAlive(PlayerObject* player) {
		return LivingPlayers.contains(player);
	}

	static std::unordered_map<PlayerObject*, bool> saveLivingPlayers() {
		std::unordered_map<PlayerObject*, bool> LivingPlayersCopy;
		for (std::pair<PlayerObject*, bool> a : LivingPlayers) {
			LivingPlayersCopy.insert({ a.first, 1 });
		}

		return LivingPlayersCopy;
	}

	static void loadLivingPlayers(std::unordered_map<PlayerObject*, bool>& LivingPlayersCopy) {
		LivingPlayers.clear();
		for (std::pair<PlayerObject*, bool> a : LivingPlayersCopy) {
			LivingPlayers.insert({ a.first, 1 });
		}
	}

	static bool guardAgainstCrashes;
private:
	static std::unordered_map<PlayerObject*, bool> LivingPlayers;
};

std::unordered_map<PlayerObject*, bool> PlayerTracker::LivingPlayers;
bool PlayerTracker::guardAgainstCrashes = true;

// Tracking + guards
class $modify(TrackedPlayerObject, PlayerObject) {
	// Mark alive
	bool init(int player, int ship, GJBaseGameLayer * gameLayer, cocos2d::CCLayer * layer, bool playLayer) {
		if (PlayerObject::init(player, ship, gameLayer, layer, playLayer)) {
			PlayerTracker::markPlayerAlive(this, "player init");

			return 1;
		}

		return 0;
	}

	// Mark dead
	void resetObject() {
		// PlayerTracker::markPlayerDead(this, "player reset");

		PlayerObject::resetObject();
	}

	// Guards
	void setPosition(cocos2d::CCPoint const& position) {
		if (PlayerTracker::isPlayerAlive(this) || !PlayerTracker::guardAgainstCrashes) {
			PlayerObject::setPosition(position);
		}
	}

	void update(float dt) {
		if (PlayerTracker::isPlayerAlive(this) || !PlayerTracker::guardAgainstCrashes) {
			PlayerObject::update(dt);
		}
	}
};

// Tracking
class $modify(TrackedPlayLayer, PlayLayer) {
	// Mark dead
	void resetLevel() {
		// auto a = PlayerTracker::saveLivingPlayers(); i am actually going to flip my table because what the fuck is silicate doing

		PlayerTracker::markAllDead("playlayer reset");

		PlayerTracker::markPlayerAlive(m_player1, "playlayer reset");
		PlayerTracker::markPlayerAlive(m_player2, "playlayer reset");

		PlayLayer::resetLevel();

		// PlayerTracker::loadLivingPlayers(a);
	}

	void onQuit() {
		PlayerTracker::markAllDead("playlayer quit");
		PlayLayer::onQuit();
	}

	// Mark alive
	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
			return false;
		}

		if (m_player1) {
			PlayerTracker::markPlayerAlive(m_player1, "playlayer init");
		}

		if (m_player2) {
			PlayerTracker::markPlayerAlive(m_player2, "playlayer init");
		}

		return true;
	};
};

// Tracking
class $modify(TrackedLevelEditorLayer, LevelEditorLayer) {
	void onPlaytest() {
		LevelEditorLayer::onPlaytest();

		PlayerTracker::markPlayerAlive(this->m_player1, "editor playtest");
		PlayerTracker::markPlayerAlive(this->m_player2, "editor playtest");
	}
};
















































































// Previous fix (still w dank_meme01 btw)
namespace killyourself {
	////////////////////////////////////////////////////////////////////////////////
// Fix Silicate Crash
//class $modify(PlayerObject) {
	void setPosition(cocos2d::CCPoint const& position) { // w dank_meme01

		//if (fixSilicateCrash && this->getParent() == 0) {
		geode::log::info("Attempted to stop the Silicate crash");
		return;
		//}

		/*
		geode::log::info("Parent -> {}", this->getParent());

		if ((void*)(this->m_waveTrail->m_pointArray->data) == 0x0) {
			geode::log::info("Null data array!");
		}

		geode::log::info("Point array address -> {}",
			(void*)&this->m_waveTrail->m_pointArray);


		geode::log::info("Point array data address -> {}",
			(void*)(this->m_waveTrail->m_pointArray->data));


		geode::log::info("Point array data num -> {}",
			(void*)(this->m_waveTrail->m_pointArray->data->num));


		geode::log::info("Point array data max -> {}",
			(void*)(this->m_waveTrail->m_pointArray->data->max));


		geode::log::info("Point array data unknown -> {}",
			(void*)(this->m_waveTrail->m_pointArray->data->unknown));

		geode::log::info("Point array -> {}",
			this->m_waveTrail->m_pointArray);

		geode::log::info("Successfully printed array\n\n");*/

		//PlayerObject::setPosition(position);



 /*
		geode::log::info("Point array addr -> {}",
			(void*)&this->m_waveTrail->m_pointArray);


		geode::log::info("Point array -> {}",
			this->m_waveTrail->m_pointArray);


		geode::log::info("Current point -> {}",
			this->m_waveTrail->m_currentPoint);

		geode::log::info("");*/

	}
	//};
}