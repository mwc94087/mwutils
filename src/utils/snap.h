#pragma once

#include <Geode/modify/Modify.hpp>
#include <Geode/binding/PlayerObject.hpp>
#include <imgui.h>
#include <Geode/cocos/cocoa/CCGeometry.h>
#include <Geode/loader/Log.hpp>
#include <vector>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <ccTypes.h>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <mwDrawNode.h>

#define uniqueId(imguiItem) ImGui::PushID(__COUNTER__); imguiItem ImGui::PopID();

#define getSetting(value) (value)

#define checkbox(label, value) uniqueId(ImGui::TextUnformatted(label); ImGui::SameLine(); ImGui::Checkbox("##", &getSetting(value)););

using namespace geode::prelude;

namespace SnapPlayer {
    // Snap info
    static char xposCalls = 0;

    // What we jumped from
    static bool hasSnapObject = 0;
    static CCRect snapObject;
    static CCPoint snapObjectPos;

    // How far we are from the center of above
    // This value is used to predict snaps
    static float snapDistance = 0;

    // Snap results
    static bool snapIsHappening = 0;
    static CCRect snapToObject;
    static CCPoint snapToObjectPos;
    static float snapDT = 0;
    static float playerXposBeforeSnap = 0;

    // Settings
    static bool showSnapData = 0;

    static void drawSnapInfo(MwDrawNode* node) {
        if (!showSnapData)
            return;

        // Highlight block we jumped from
        if (hasSnapObject) { // TODO impl userdata into mwdrawnode
            node->drawRect(snapObject, ccColor4F(0, 0, 1, 0.1), 0.2, ccColor4F(0, 0, 1, 1));

            CCPoint* xposLine = new CCPoint[2];

            xposLine[0] = snapObjectPos - CCPoint(0, 1000);
            xposLine[1] = snapObjectPos + CCPoint(0, 1000);

            node->drawLines(xposLine, 1, 0.5, ccColor4F(0, 0, 1, 1));

            xposLine[0] = CCPoint(snapObjectPos.x + snapDistance, snapObjectPos.y + 1000);
            xposLine[1] = CCPoint(snapObjectPos.x + snapDistance, snapObjectPos.y - 1000);

            node->drawLines(xposLine, 1, 0.5, ccColor4F(0, 0.75, 1, 1));

            delete xposLine;
        }

        if (snapIsHappening) {
            node->drawRect(snapToObject, ccColor4F(1, 0, 0, 0.1), 0.2, ccColor4F(1, 0, 0, 1));

            CCPoint* xposLine = new CCPoint[2];


            // Landing block X
            xposLine[0] = snapToObjectPos - CCPoint(0, 1000);
            xposLine[1] = snapToObjectPos + CCPoint(0, 1000);

            node->drawLines(xposLine, 1, 0.5, ccColor4F(1, 0, 0, 1)); 

            // Actual Landing X
            xposLine[0] = CCPoint(playerXposBeforeSnap, snapToObjectPos.y + 1000);
            xposLine[1] = CCPoint(playerXposBeforeSnap, snapToObjectPos.y - 1000);

            node->drawLines(xposLine, 1, 0.5, ccColor4F(0, 0.5, 0, 1)); 

            // Actual Landing X + snap
            xposLine[0] = CCPoint(playerXposBeforeSnap + snapDT, snapToObjectPos.y + 1000);
            xposLine[1] = CCPoint(playerXposBeforeSnap + snapDT, snapToObjectPos.y - 1000);

            node->drawLines(xposLine, 1, 0.5, ccColor4F(0, 0.8, 0, 1)); 

            // Predicted Landing X
            xposLine[0] = CCPoint(snapToObjectPos.x + snapDistance, snapToObjectPos.y + 1000);
            xposLine[1] = CCPoint(snapToObjectPos.x + snapDistance, snapToObjectPos.y - 1000);

            node->drawLines(xposLine, 1, 0.5, ccColor4F(0, 1, 1, 1));

            delete xposLine;
        }
    }

    // ImGui
    void drawImGui() {
        ImGui::Text("Snap DT: %f", snapDT);

        checkbox("Show player snap data", showSnapData);

        ImGui::Separator();
    }
}

class $modify(SnappingPlayer, PlayerObject) {
    void setPosition(cocos2d::CCPoint const& position) {
        SnapPlayer::xposCalls += 1;

        PlayerObject::setPosition(position);
    }

    void checkSnapJumpToObject(GameObject* object) {
        float prevXpos = this->getPositionX();
        char prevXposCalls = SnapPlayer::xposCalls;

        PlayerObject::checkSnapJumpToObject(object);

        if (prevXposCalls != SnapPlayer::xposCalls) {
            SnapPlayer::snapDT = this->getPositionX() - prevXpos;
            SnapPlayer::snapIsHappening = true;
            SnapPlayer::snapToObject = object->getObjectRect();
            SnapPlayer::snapToObjectPos = object->getPosition();
            SnapPlayer::playerXposBeforeSnap = prevXpos;
        }

    }

    void update(float dt) {
        if (m_objectSnappedTo) {
            SnapPlayer::hasSnapObject = 1;
            SnapPlayer::snapObject = m_objectSnappedTo->getObjectRect();
            SnapPlayer::snapObjectPos = m_objectSnappedTo->getPosition();
        }
        else {
            SnapPlayer::hasSnapObject = 0;
        }

        SnapPlayer::snapIsHappening = 0;

        SnapPlayer::snapDistance = m_snapDistance;

        PlayerObject::update(dt); // most useless hook oat
    }
};

class $modify(SnappedLevelEditor, LevelEditorLayer) {
    bool init(GJGameLevel * level, bool noUI) {
        SnapPlayer::hasSnapObject = 0;

        if (!LevelEditorLayer::init(level, noUI)) {
            return false;
        }

        MwDrawNode* dombaNode = MwDrawNode::create(&SnapPlayer::drawSnapInfo);

        CCNode* robtopHitboxNode = this->m_debugDrawNode;

        robtopHitboxNode->getParent()->addChild(dombaNode);

        return true;
    }
};

class $modify(SnappedPlayLayer, PlayLayer) {
    bool init(GJGameLevel * level, bool useReplay, bool dontCreateObjects) {
        SnapPlayer::hasSnapObject = 0;

        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
            return false;
        }

        MwDrawNode* dombaNode = MwDrawNode::create(&SnapPlayer::drawSnapInfo);

        CCNode* robtopHitboxNode = this->m_debugDrawNode;

        robtopHitboxNode->getParent()->addChild(dombaNode);

        return true;
    }
};

#undef checkbox
#undef getSetting
#undef uniqueId