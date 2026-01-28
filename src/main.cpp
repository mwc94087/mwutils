#include <imgui-cocos.hpp>
#include <imgui.h>

#include <geode.custom-keybinds/include/Keybinds.hpp>

#include <Geode/modify/AnimatedGameObject.hpp>
#include <Geode/modify/CCCircleWave.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/CCMotionStreak.hpp>
#include <Geode/modify/CCParticleSystem.hpp>
#include <Geode/modify/CCParticleSystemQuad.hpp>
#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/DashRingObject.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/EffectGameObject.hpp>
#include <Geode/modify/EnhancedGameObject.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/GhostTrailEffect.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/GravityEffectSprite.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/RingObject.hpp>

#include <Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h>
#include <Geode/loader/Event.hpp>
#include <Geode/loader/Log.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/ModEvent.hpp>

#include <mwDrawNode.h>

#include <domba.h>
#include <noEffects.h>
#include <fixCrashes.h>
#include <marker.h>
#include <snap.h>

using namespace keybinds;
using namespace geode::prelude;

namespace mwUtils {
    static bool showImGui = 0;

    // imgui and keybind
    static void drawImGui() {
        if (!showImGui)
            return;

        ImGui::Begin("mws utils");
        {
            // Visual
            NoEffects::drawImGui();

            DombaBuff::drawImGui();

            Marker::drawImGui();

            NoCrash::drawImGui();

            SnapPlayer::drawImGui();

            if (ImGui::Button("Write separator to logs"))
                geode::log::info("--------------------------------------------------------------------------------\n");
        }

        ImGui::End();
    }

    static ListenerResult keypressCallback(InvokeBindEvent* event) {
        if (event->isDown()) {
            showImGui = !showImGui;
        }

        return ListenerResult::Propagate;
    }
}

$on_mod(Loaded) {
    ImGuiCocos::get().setup([] {}).draw(mwUtils::drawImGui);

    BindManager::get()->registerBindable({
        "imgui"_spr, "Toggle ImGui", "Toggles the ImGui window",
        {Keybind::create(KEY_F1, Modifier::None)}, "mwmod/binds"
        });

    new EventListener(&mwUtils::keypressCallback, InvokeBindFilter(0, "imgui"_spr));
}