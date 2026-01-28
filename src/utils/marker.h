#pragma once

#include <Geode/modify/Modify.hpp>
#include <Geode/binding/PlayerObject.hpp>
#include <imgui.h>
#include <Geode/cocos/cocoa/CCGeometry.h>
#include <Geode/loader/Log.hpp>
#include <cmath>
#include <vector>

using namespace geode::prelude;

// Statics
////////////////////////////////////////////////////////////////////////////////

static std::vector<CCRect> markers;

static void placeMarker() {
    PlayLayer* pl = PlayLayer::get();

    if (pl) {
        if (pl->m_player1) {
            markers.push_back(pl->m_player1->getObjectRect());
        }

        if (pl->m_player2) {
            markers.push_back(pl->m_player1->getObjectRect());
        }
    }
}

static void drawMarkers(MwDrawNode* drawnode) {
    for (CCRect rect : markers) {
        drawnode->drawRect(rect, ccColor4F(0, 0, 0, 0), 0.2, ccColor4F(0, 0.7, 0, 1));
    }
}

// Outer facing namespace
////////////////////////////////////////////////////////////////////////////////

namespace Marker {
    void drawImGui() {
        if (ImGui::Button("Add Marker")) {
            placeMarker();
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear markers")) {
            markers.clear();
        }

        ImGui::Text("%d markers", markers.size());

        ImGui::Separator();
    }
}

// Hooks
////////////////////////////////////////////////////////////////////////////////

// TODO refactor to be dry code

// Put a MarkerNode into the LevelEditorLayer
class $modify(MarkedLevelEditorLayer, LevelEditorLayer) {
    bool init(GJGameLevel * level, bool noUI) {
        if (!LevelEditorLayer::init(level, noUI)) {
            return false;
        }

        MwDrawNode* drawNode = MwDrawNode::create(&drawMarkers);

        CCNode* robtopHitboxNode = this->m_debugDrawNode;

        robtopHitboxNode->getParent()->addChild(drawNode);

        return true;
    }
};

// put a MarkerNode into the PlayLayer
class $modify(MarkedPlayLayer, PlayLayer) {
    bool init(GJGameLevel * level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
            return false;
        }

        MwDrawNode* drawNode = MwDrawNode::create(&drawMarkers);

        CCNode* robtopHitboxNode = this->m_debugDrawNode;

        robtopHitboxNode->getParent()->addChild(drawNode);

        return true;
    }
};