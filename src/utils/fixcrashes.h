#pragma once

#include <Geode/modify/Modify.hpp>
#include <imgui.h>
#include <cmath>

#define uniqueId(imguiItem) ImGui::PushID(__COUNTER__); imguiItem ImGui::PopID();

#define getSetting(value) (AntiCrashSettingg[AntiCrashSetting::value])

#define checkbox(label, value) uniqueId(ImGui::TextUnformatted(label); ImGui::SameLine(); ImGui::Checkbox("##", &getSetting(value)););

using namespace geode::prelude;

static enum AntiCrashSetting {
    FixNanPos = 0,
    AntiCrashSetting_COUNT
};

static bool AntiCrashSettingg[AntiCrashSetting::AntiCrashSetting_COUNT] = { 1 };

namespace NoCrash {
    void drawImGui() {
        checkbox("Fix NaN Position crash", FixNanPos);

        ImGui::Separator();
    }
}

class $modify(HealthyBGL, GJBaseGameLayer) {
    void addToSection(GameObject * object) {
        if (getSetting(FixNanPos)) {
            double xpos = object->m_positionX;
            double ypos = object->m_positionY;

            if (std::isnan(xpos)) {
                object->m_positionX = 0;
            }

            if (std::isnan(ypos)) {
                object->m_positionY = 0;
            }
        }

        GJBaseGameLayer::addToSection(object);
    }
};

#undef checkbox
#undef getSettingRaw
#undef getSetting
#undef uniqueId