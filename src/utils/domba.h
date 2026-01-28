#pragma once

#include <vector>

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/cocoa/CCGeometry.h>
#include <Geode/modify/Modify.hpp>
#include <Geode/binding/GJBaseGameLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/PlayerObject.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/Enums.hpp>

using namespace geode::prelude;

namespace DombaBuff {
    static bool recording = 0;
    static bool visible = 0;
    static std::vector<CCRect> frames;

    static void clearHitboxes() {
        frames.clear();
    }

    static void addHitbox(CCRect hitbox) {
        frames.push_back(hitbox);
    }

    static int countHitboxes() {
        return frames.size();
    }

    static void drawDombas(MwDrawNode* node) {
        if (!visible)
            return;

        for (CCRect rect : frames) {
            node->drawRect(rect, ccColor4F(0, 0, 0, 0), 0.2, ccColor4F(0.7, 0, 0.7, 1));
        }
    }

    void drawImGui() {
        ImGui::TextUnformatted("Record held hitboxes"); ImGui::SameLine();  ImGui::Checkbox("##dombabuff", &recording);

        ImGui::TextUnformatted("Show held hitboxes"); ImGui::SameLine(); ImGui::Checkbox("##dombaview", &visible);

        ImGui::Text("%d recorded hitboxes", countHitboxes());

        if (ImGui::Button("Clear frames"))
            clearHitboxes();

        ImGui::Separator();
    }
}

// Put MwDrawNode into the editor
class $modify(DombaLEL, LevelEditorLayer) {
    bool init(GJGameLevel * level, bool noUI) {
        if (!LevelEditorLayer::init(level, noUI)) {
            return false;
        }

        MwDrawNode* dombaNode = MwDrawNode::create(&DombaBuff::drawDombas);

        CCNode* robtopHitboxNode = this->m_debugDrawNode;

        robtopHitboxNode->getParent()->addChild(dombaNode);

        return true;
    }
};

// Put MwDrawNode into PlayLayer + reset frames on spawn
class $modify(DombaPL, PlayLayer) {
    bool init(GJGameLevel * level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
            return false;
        }

        MwDrawNode* dombaNode = MwDrawNode::create(&DombaBuff::drawDombas);

        CCNode* robtopHitboxNode = this->m_debugDrawNode;

        robtopHitboxNode->getParent()->addChild(dombaNode);

        return true;
    }

    void resetLevel() {
        DombaBuff::clearHitboxes();
        PlayLayer::resetLevel();
    }
};

// Record frames
class $modify(DombaPO, PlayerObject) {
    void update(float dt) {
        if (DombaBuff::recording) {
            auto gjbgl = GJBaseGameLayer::get();
            if (gjbgl != nullptr && (this == gjbgl->m_player1 || this == gjbgl->m_player2)) {
                bool isHolding = false;

                if (this->m_holdingButtons.size() > 1 && this->m_holdingButtons.at(1) == true) {
                    isHolding = true;
                }

                if (isHolding) {
                    DombaBuff::addHitbox(this->getObjectRect());
                }
            }
        }

        PlayerObject::update(dt);
    }

    bool pushButton(PlayerButton button) {
        if (DombaBuff::recording && button == PlayerButton::Jump) {
            auto gjbgl = GJBaseGameLayer::get();
            if (gjbgl != nullptr && (this == gjbgl->m_player1 || this == gjbgl->m_player2)) {
                DombaBuff::addHitbox(this->getObjectRect());
            }
        }

        return PlayerObject::pushButton(button);
    }
};