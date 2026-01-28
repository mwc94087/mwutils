#pragma once

#include <Geode/cocos/cocoa/CCGeometry.h>
#include <Geode/cocos/draw_nodes/CCDrawNode.h>
#include <ccTypes.h>
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

class MwDrawNode;

typedef void mwDrawCallback(MwDrawNode*);

class MwDrawNode : public CCDrawNode {
public:
    static MwDrawNode* create(mwDrawCallback * callback, int zOrder = 10101) {
        MwDrawNode* node = new MwDrawNode(callback);

        if (node->init()) {
            node->autorelease();
            node->m_bUseArea = false;

            node->setID("MwDrawNode"_spr);
            node->setZOrder(zOrder);

            return node;
        }

        delete node;
        return nullptr;
    }

    MwDrawNode(mwDrawCallback * drawCallback) {
        this->drawCallback = drawCallback;
    }

    bool enableDrawing = true;

    void draw() {
        if (!isCleared) {
            this->clear();
        }

        if (enableDrawing) {
            drawCallback(this);

            CCDrawNode::draw();
            isCleared = false;
        }
        else {
            isCleared = true;
        }
    }
private:
    bool isCleared = false;
    mwDrawCallback * drawCallback;
};