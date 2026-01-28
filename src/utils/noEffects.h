#pragma once

#include <Geode/modify/Modify.hpp>
#include <Geode/binding/PlayerObject.hpp>
#include <imgui.h>
#include <Geode/binding/CCCircleWave.hpp>
#include <Geode/binding/DashRingObject.hpp>
#include <Geode/binding/RingObject.hpp>
#include <Geode/cocos/actions/CCActionInstant.h>
#include <Geode/cocos/CCDirector.h>
#include <Geode/cocos/cocoa/CCGeometry.h>
#include <Geode/cocos/cocoa/CCObject.h>
#include <ccTypes.h>
#include <Geode/cocos/misc_nodes/CCMotionStreak.h>
#include <Geode/binding/HardStreak.hpp>
#include <Geode/cocos/particle_nodes/CCParticleSystem.h>
#include <Geode/cocos/particle_nodes/CCParticleSystemQuad.h>
#include <Geode/DefaultInclude.hpp>
#include <Geode/loader/Log.hpp>
#include <Geode/binding/GhostTrailEffect.hpp>
#include <Geode/binding/GJBaseGameLayer.hpp>

#define uniqueId(imguiItem) ImGui::PushID(__COUNTER__); imguiItem ImGui::PopID();

#define getSetting(value) (EffectSettings[EffectSetting::value])
#define getSettingRaw(value) (EffectSettings[EffectSetting::value])

#define checkbox(label, value) uniqueId(ImGui::TextUnformatted(label); ImGui::SameLine(); ImGui::Checkbox("##", &getSettingRaw(value)););

using namespace geode::prelude;

static enum EffectSetting {
    DisableDashEffect,
    DisableOrbHit,
    DisableSpawn,
    DisableDeath,
    DisableLightning,
    DisableGravity,
    DisableSpider,
    DisableShine,
    DisableScale,
    DisableCircleWave,
    DisableMotionStreak,
    DisableParticles,
    DisableHardStreak,
    DisableGhostStreak,
    DisableAnimation,

    EffectSetting_COUNT
};

static bool EffectSettings[EffectSetting::EffectSetting_COUNT] = {0};

namespace NoEffects {
    void drawImGui() {
        bool toggle = ImGui::Button("Toggle All");
        bool alloff = ImGui::Button("Disable All");

        if (toggle || alloff)
            for (int i = 0; i < EffectSetting_COUNT; ++i) {
                EffectSettings[i] = toggle ? !EffectSettings[i] : (alloff ? false : EffectSettings[i]);
            }

        if (ImGui::TreeNode("Level")) {
            checkbox("Lightning", DisableLightning);
            checkbox("Gravity effect", DisableGravity);
            checkbox("Object shine", DisableShine);
            checkbox("Object scale", DisableScale);
            checkbox("Circle wave", DisableCircleWave);
            checkbox("Particles", DisableParticles);
            checkbox("Animation", DisableAnimation);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Player")) {
            checkbox("Spawn effect", DisableSpawn);
            checkbox("Death effect", DisableDeath);
            checkbox("Wave trail", DisableHardStreak);
            checkbox("Player trail", DisableMotionStreak);
            checkbox("Ghost trail", DisableGhostStreak);
            checkbox("Dash fire", DisableDashEffect);
            checkbox("Orb jump", DisableOrbHit);
            checkbox("Spider teleport", DisableSpider);
            ImGui::TreePop();
        }

        ImGui::Separator();
    }
}

class $modify(MyCleanPlayerObject, PlayerObject) {
    void startDashing(DashRingObject* object) {
        if (getSetting(DisableDashEffect) == 1 && object != 0) {
            bool noEffect_old = object->m_hasNoEffects;

            object->m_hasNoEffects = true;
            PlayerObject::startDashing(object);

            object->m_hasNoEffects = noEffect_old;
        }
        else {
            PlayerObject::startDashing(object);
        }
    }

    void stopDashing() {
        if (getSetting(DisableDashEffect) == 1) {
            bool noEffect_old = this->m_playEffects;

            this->m_playEffects = false;
            PlayerObject::stopDashing();

            this->m_playEffects = noEffect_old;
        }
        else {
            PlayerObject::stopDashing();
        }
    }

    void ringJump(RingObject* object, bool skipCheck) {
        if (getSetting(DisableOrbHit) == 1 && object != 0) {
            bool noEffect_old = object->m_hasNoEffects;

            object->m_hasNoEffects = true;
            PlayerObject::ringJump(object, skipCheck);

            object->m_hasNoEffects = noEffect_old;
        }
        else {
            PlayerObject::ringJump(object, skipCheck);
        }
    }

    void playDeathEffect() {
        if (getSetting(DisableDeath))
            return;

        PlayerObject::playDeathEffect();
    }
    
    void playSpawnEffect() {
        if (getSetting(DisableSpawn))
            return;

        PlayerObject::playSpawnEffect();
    }

    void playSpiderDashEffect(CCPoint from, CCPoint to) {
        if (getSetting(DisableSpider))
            return;

        PlayerObject::playSpiderDashEffect(from, to);
    }
};

class $modify(MyCleanCCCircleWave, CCCircleWave) {
    bool init(float startRadius, float endRadius, float duration, bool fadeIn, bool easeOut) {
        if (getSetting(DisableCircleWave)) {
            CCCallFunc* callFunc = CCCallFunc::create((CCObject*)this, (SEL_CallFunc)(&CCCircleWave::removeMeAndCleanup));
            CCDirector::get()->getActionManager()->addAction(callFunc, this, false);

            return 1;
        }
        else {
            return CCCircleWave::init(startRadius, endRadius, duration, fadeIn, easeOut);
        }
    }

    void draw() {
        if (getSetting(DisableCircleWave)) {
            this->stopAllActions();
            this->removeMeAndCleanup();
            return;
        }
        CCCircleWave::draw();
    }
};

class $modify(MyCleanCCMotionStreak, CCMotionStreak) {
    void draw() {
        if (getSetting(DisableMotionStreak))
            return this->reset();

        CCMotionStreak::draw();
    }
};

class $modify(MyCleanCCParticleSystem, CCParticleSystem) {
    void draw() {
        if (getSetting(DisableParticles))
            return this->resetSystem();

        CCParticleSystem::draw();
    }
};

class $modify(MyCleanCCParticleSystemQuad, CCParticleSystemQuad) {
    void draw() {
        if (getSetting(DisableParticles)) {
            this->resetSystem();
            return;
        }

        CCParticleSystemQuad::draw();
    }
};

class $modify(MyCleanHardStreak, HardStreak) {
    static void onModify(auto& self) {
        if (!self.setHookPriorityPre("HardStreak::updateStroke", Priority::First)) {
            geode::log::warn("Failed to set hook priority.");
        }
    }

    void updateStroke(float dt) {
        if (getSetting(DisableHardStreak)) {
            this->clear();
            this->m_pointArray->removeAllObjects();
            this->stopAllActions();
            return;
        }

        HardStreak::updateStroke(dt);
    }
};

class $modify(MyCleanGJBaseGameLayer, GJBaseGameLayer) {
    /*void lightningFlash(CCPoint to, ccColor3B color) {
        if (getSetting(DisableLightning))
            return;

        GJBaseGameLayer::lightningFlash(to, color);
    };*/

    void lightningFlash(cocos2d::CCPoint from, cocos2d::CCPoint to, cocos2d::ccColor3B color, float lineWidth, float duration, int displacement, bool flash, float opacity) {
        if (getSetting(DisableLightning))
            return;

        GJBaseGameLayer::lightningFlash(from, to, color, lineWidth, duration, displacement, flash, opacity);
    };
};

class $modify(MyCleanPlayLayer, PlayLayer) {
    void playGravityEffect(bool flip) {
        if (getSetting(DisableGravity))
            return;

        PlayLayer::playGravityEffect(flip);
    }
};

class $modify(MyCleanGameObject, GameObject) {
    void playShineEffect() {
        if (getSetting(DisableShine))
            return;

        GameObject::playShineEffect();
    }

    void setRScale(float scale) {
        if (getSetting(DisableScale))
            return GameObject::setRScale(0.5);

        GameObject::setRScale(scale);
    }

    /*void animationTriggered() {
        if (getSetting(DisableAnimation))
            return;

        GameObject::animationTriggered();
    }*/

    void playDestroyObjectAnim(GJBaseGameLayer* layer) {
        if (getSetting(DisableAnimation))
            return;

        GameObject::playDestroyObjectAnim(layer);
    }

    void playPickupAnimation(cocos2d::CCSprite* target, float xOffset, float yOffset, float controlYOffset1, float controlYOffset2, float endYOffset, float duration, float fadeDelay, float fadeDuration, bool rotate, float randomValue1, float randomValue2) {
        if (getSetting(DisableAnimation))
            return;

        GameObject::playPickupAnimation(target, xOffset, yOffset, controlYOffset1, controlYOffset2, endYOffset, duration, fadeDelay, fadeDuration, rotate, randomValue1, randomValue2);
    }

};

class $modify(MyCleanEffectGameObject, EffectGameObject) {
    void playTriggerEffect() {
        if (getSetting(DisableOrbHit))
            return;

        EffectGameObject::playTriggerEffect();
    }
};

class $modify(MyCleanRingObject, RingObject) {
    void setRScale(float scale) {
        if (getSetting(DisableScale))
            return RingObject::setRScale(1);

        RingObject::setRScale(scale);
    }
};

class $modify(MyCleanGhostTrail, GhostTrailEffect) {
    void trailSnapshot(float dt) {
        if (getSetting(DisableGhostStreak))
            return;

        GhostTrailEffect::trailSnapshot(dt);
    }
};

class $modify(MyCleanEnhancedGameObject, EnhancedGameObject) {
    void updateRotateAction(float dt) {
        if (getSetting(DisableAnimation))
            return;

        EnhancedGameObject::updateRotateAction(dt);
    }

    void updateSyncedAnimation(float totalTime, int frameIndex) {
        if (getSetting(DisableAnimation))
            return;

        EnhancedGameObject::updateSyncedAnimation(totalTime, frameIndex);
    }

    void updateAnimateOnTrigger(bool animate) {
        if (getSetting(DisableAnimation))
            return;

        EnhancedGameObject::updateAnimateOnTrigger(animate);
    }
};

class $modify(MyCleanAnimatedGameObject, AnimatedGameObject) {
    void updateObjectAnimation() {
        if (getSetting(DisableAnimation))
            return;

        AnimatedGameObject::updateObjectAnimation();
    }

    void playAnimation(int type) {
        if (getSetting(DisableAnimation))
            return;

        AnimatedGameObject::playAnimation(type);
    }
};

#undef checkbox
#undef getSettingRaw
#undef getSetting
#undef uniqueId