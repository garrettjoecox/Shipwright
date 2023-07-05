#ifdef ENABLE_REMOTE_CONTROL

#include "GameInteractor_Sail.h"
#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>
#include <nlohmann/json.hpp>

template <class DstType, class SrcType>
bool IsType(const SrcType* src) {
  return dynamic_cast<const DstType*>(src) != nullptr;
}

void GameInteractorSail::Enable() {
    if (isEnabled) {
        return;
    }

    isEnabled = true;
    GameInteractor::Instance->EnableRemoteInteractor();
    GameInteractor::Instance->RegisterRemoteJsonHandler([&](nlohmann::json payload) {
        HandleRemoteJson(payload);
    });
}

void GameInteractorSail::Disable() {
    if (!isEnabled) {
        return;
    }

    isEnabled = false;
    GameInteractor::Instance->DisableRemoteInteractor();
}

void GameInteractorSail::HandleRemoteJson(nlohmann::json payload) {
    SPDLOG_INFO("[GameInteractorSail] Received payload: \n{}\n", payload.dump());
    try {
        if (!payload.contains("id") || !payload.contains("effect") || !payload["effect"].contains("type")) {
            return;
        }

        std::string type = payload["effect"]["type"].get<std::string>();
        nlohmann::json responsePayload;
        responsePayload["id"] = payload["id"];
        responsePayload["status"] = "success";

        if (type == "console") {
            std::string command = payload["effect"]["command"].get<std::string>();
            std::reinterpret_pointer_cast<LUS::ConsoleWindow>(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch(command);
            GameInteractor::Instance->TransmitJsonToRemote(responsePayload);
        } else if (type == "apply" || type == "remove") {
            if (!GameInteractor::IsSaveLoaded()) {
                responsePayload["status"] = "try_again";
                GameInteractor::Instance->TransmitJsonToRemote(responsePayload);
                return;
            }

            GameInteractionEffectBase* giEffect = EffectFromJson(payload["effect"]);
            if (giEffect) {
                GameInteractionEffectQueryResult result;
                if (type == "remove") {
                    if (IsType<RemovableGameInteractionEffect>(giEffect)) {
                        result = dynamic_cast<RemovableGameInteractionEffect*>(giEffect)->Remove();
                    } else {
                        result = GameInteractionEffectQueryResult::NotPossible;
                    }
                } else {
                    result = giEffect->Apply();
                }

                if (result == GameInteractionEffectQueryResult::NotPossible) {
                    responsePayload["status"] = "failure";
                } else if (result == GameInteractionEffectQueryResult::TemporarilyNotPossible) {
                    responsePayload["status"] = "try_again";
                }
                GameInteractor::Instance->TransmitJsonToRemote(responsePayload);
            }
        } else {
            SPDLOG_INFO("[GameInteractorSail] Unknown effect type: {}", type);
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("[GameInteractorSail] Exception handling remote JSON: {}", e.what());
    } catch (...) {
        SPDLOG_ERROR("[GameInteractorSail] Unknown exception handling remote JSON");
    }
}

GameInteractionEffectBase* GameInteractorSail::EffectFromJson(nlohmann::json payload) {
    if (!payload.contains("type") || !payload.contains("name")) {
        return nullptr;
    }

    std::string name = payload["name"].get<std::string>();

    if (name == "SetSceneFlag") {
        auto effect = new GameInteractionEffect::SetSceneFlag();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
            effect->parameters[2] = payload["parameters"][2].get<int32_t>();
        }
        return effect;
    } else if (name == "UnsetSceneFlag") {
        auto effect = new GameInteractionEffect::UnsetSceneFlag();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
            effect->parameters[2] = payload["parameters"][2].get<int32_t>();
        }
        return effect;
    } else if (name == "SetFlag") {
        auto effect = new GameInteractionEffect::SetFlag();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
        }
        return effect;
    } else if (name == "UnsetFlag") {
        auto effect = new GameInteractionEffect::UnsetFlag();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
        }
        return effect;
    } else if (name == "ModifyHeartContainers") {
        auto effect = new GameInteractionEffect::ModifyHeartContainers();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "FillMagic") {
        return new GameInteractionEffect::FillMagic();
    } else if (name == "EmptyMagic") {
        return new GameInteractionEffect::EmptyMagic();
    } else if (name == "ModifyRupees") {
        auto effect = new GameInteractionEffect::ModifyRupees();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "NoUI") {
        return new GameInteractionEffect::NoUI();
    } else if (name == "ModifyGravity") {
        auto effect = new GameInteractionEffect::ModifyGravity();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "ModifyHealth") {
        auto effect = new GameInteractionEffect::ModifyHealth();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "SetPlayerHealth") {
        auto effect = new GameInteractionEffect::SetPlayerHealth();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "FreezePlayer") {
        return new GameInteractionEffect::FreezePlayer();
    } else if (name == "BurnPlayer") {
        return new GameInteractionEffect::BurnPlayer();
    } else if (name == "ElectrocutePlayer") {
        return new GameInteractionEffect::ElectrocutePlayer();
    } else if (name == "KnockbackPlayer") {
        auto effect = new GameInteractionEffect::KnockbackPlayer();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "ModifyLinkSize") {
        auto effect = new GameInteractionEffect::ModifyLinkSize();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "InvisibleLink") {
        return new GameInteractionEffect::InvisibleLink();
    } else if (name == "PacifistMode") {
        return new GameInteractionEffect::PacifistMode();
    } else if (name == "DisableZTargeting") {
        return new GameInteractionEffect::DisableZTargeting();
    } else if (name == "WeatherRainstorm") {
        return new GameInteractionEffect::WeatherRainstorm();
    } else if (name == "ReverseControls") {
        return new GameInteractionEffect::ReverseControls();
    } else if (name == "ForceEquipBoots") {
        auto effect = new GameInteractionEffect::ForceEquipBoots();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "ModifyRunSpeedModifier") {
        auto effect = new GameInteractionEffect::ModifyRunSpeedModifier();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "OneHitKO") {
        return new GameInteractionEffect::OneHitKO();
    } else if (name == "ModifyDefenseModifier") {
        auto effect = new GameInteractionEffect::ModifyDefenseModifier();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "GiveOrTakeShield") {
        auto effect = new GameInteractionEffect::GiveOrTakeShield();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "TeleportPlayer") {
        auto effect = new GameInteractionEffect::TeleportPlayer();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "ClearAssignedButtons") {
        auto effect = new GameInteractionEffect::ClearAssignedButtons();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "SetTimeOfDay") {
        auto effect = new GameInteractionEffect::SetTimeOfDay();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "SetCollisionViewer") {
        return new GameInteractionEffect::SetCollisionViewer();
    } else if (name == "SetCosmeticsColor") {
        auto effect = new GameInteractionEffect::SetCosmeticsColor();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
        }
        return effect;
    } else if (name == "RandomizeCosmetics") {
        return new GameInteractionEffect::RandomizeCosmetics();
    } else if (name == "PressButton") {
        auto effect = new GameInteractionEffect::PressButton();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "PressRandomButton") {
        auto effect = new GameInteractionEffect::PressRandomButton();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
        }
        return effect;
    } else if (name == "AddOrTakeAmmo") {
        auto effect = new GameInteractionEffect::AddOrTakeAmmo();
        if (payload.contains("parameters")) {
            effect->parameters[0] = payload["parameters"][0].get<int32_t>();
            effect->parameters[1] = payload["parameters"][1].get<int32_t>();
        }
        return effect;
    } else if (name == "RandomBombFuseTimer") {
        return new GameInteractionEffect::RandomBombFuseTimer();
    } else if (name == "DisableLedgeGrabs") {
        return new GameInteractionEffect::DisableLedgeGrabs();
    } else if (name == "RandomWind") {
        return new GameInteractionEffect::RandomWind();
    } else if (name == "RandomBonks") {
        return new GameInteractionEffect::RandomBonks();
    } else if (name == "PlayerInvincibility") {
        return new GameInteractionEffect::PlayerInvincibility();
    } else if (name == "SlipperyFloor") {
        return new GameInteractionEffect::SlipperyFloor();
    } else {
        SPDLOG_INFO("[GameInteractorSail] Unknown effect name: {}", name);
        return nullptr;
    }
}

#endif
