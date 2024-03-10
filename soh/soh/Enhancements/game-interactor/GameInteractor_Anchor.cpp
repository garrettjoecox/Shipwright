#ifdef ENABLE_REMOTE_CONTROL

#include "GameInteractor_Anchor.h"
#include <libultraship/libultraship.h>
#include <soh/OTRGlobals.h>
#include <soh/Enhancements/item-tables/ItemTableManager.h>
#include <soh/Enhancements/randomizer/randomizerTypes.h>
#include <soh/Enhancements/randomizer/adult_trade_shuffle.h>
#include <soh/Enhancements/nametag.h>
#include <soh/Enhancements/presets.h>
#include <soh/Enhancements/randomizer/randomizer_check_tracker.h>
#include <soh/util.h>
#include <nlohmann/json.hpp>
#include <sstream>

extern "C" {
#include <variables.h>
#include "macros.h"
#include "z64scene.h"
#include "z64actor.h"
#include "functions.h"
extern "C" s16 gEnPartnerId;
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}

using json = nlohmann::json;

void from_json(const json& j, Color_RGB8& color) {
    j.at("r").get_to(color.r);
    j.at("g").get_to(color.g);
    j.at("b").get_to(color.b);
}

void to_json(json& j, const Color_RGB8& color) {
    j = json{
        {"r", color.r},
        {"g", color.g},
        {"b", color.b}
    };
}

void to_json(json& j, const Vec3f& vec) {
    j = json{
        {"x", vec.x},
        {"y", vec.y},
        {"z", vec.z}
    };
}

void to_json(json& j, const Vec3s& vec) {
    j = json{
        {"x", vec.x},
        {"y", vec.y},
        {"z", vec.z}
    };
}

void from_json(const json& j, Vec3f& vec) {
    j.at("x").get_to(vec.x);
    j.at("y").get_to(vec.y);
    j.at("z").get_to(vec.z);
}

void from_json(const json& j, Vec3s& vec) {
    j.at("x").get_to(vec.x);
    j.at("y").get_to(vec.y);
    j.at("z").get_to(vec.z);
}

void to_json(json& j, const PosRot& posRot) {
    j = json{
        {"pos", posRot.pos},
        {"rot", posRot.rot}
    };
}

void from_json(const json& j, PosRot& posRot) {
    j.at("pos").get_to(posRot.pos);
    j.at("rot").get_to(posRot.rot);
}

void from_json(const json& j, AnchorClient& client) {
    j.contains("clientId") ? j.at("clientId").get_to(client.clientId) : client.clientId = 0;
    j.contains("clientVersion") ? j.at("clientVersion").get_to(client.clientVersion) : client.clientVersion = "???";
    j.contains("name") ? j.at("name").get_to(client.name) : client.name = "???";
    j.contains("color") ? j.at("color").get_to(client.color) : client.color = {255, 255, 255};
    j.contains("seed") ? j.at("seed").get_to(client.seed) : client.seed = 0;
    j.contains("fileNum") ? j.at("fileNum").get_to(client.fileNum) : client.fileNum = 0xFF;
    j.contains("gameComplete") ? j.at("gameComplete").get_to(client.gameComplete) : client.gameComplete = false;
    j.contains("sceneNum") ? j.at("sceneNum").get_to(client.sceneNum) : client.sceneNum = SCENE_ID_MAX;
    j.contains("roomIndex") ? j.at("roomIndex").get_to(client.roomIndex) : client.roomIndex = 0;
    j.contains("entranceIndex") ? j.at("entranceIndex").get_to(client.entranceIndex) : client.entranceIndex = 0;
    j.contains("posRot") ? j.at("posRot").get_to(client.posRot) : client.posRot = { -9999, -9999, -9999, 0, 0, 0 };
}

void to_json(json& j, const SavedSceneFlags& flags) {
    j = json{
        {"chest", flags.chest},
        {"swch", flags.swch},
        {"clear", flags.clear},
        {"collect", flags.collect},
    };
}

void from_json(const json& j, SavedSceneFlags& flags) {
    j.at("chest").get_to(flags.chest);
    j.at("swch").get_to(flags.swch);
    j.at("clear").get_to(flags.clear);
    j.at("collect").get_to(flags.collect);
}

void to_json(json& j, const Inventory& inventory) {
    j = json{
        {"items", inventory.items},
        {"ammo", inventory.ammo},
        {"equipment", inventory.equipment},
        {"upgrades", inventory.upgrades},
        {"questItems", inventory.questItems},
        {"dungeonItems", inventory.dungeonItems},
        {"dungeonKeys", inventory.dungeonKeys},
        {"defenseHearts", inventory.defenseHearts},
        {"gsTokens", inventory.gsTokens}
    };
}

void from_json(const json& j, Inventory& inventory) {
    j.at("items").get_to(inventory.items);
    j.at("ammo").get_to(inventory.ammo);
    j.at("equipment").get_to(inventory.equipment);
    j.at("upgrades").get_to(inventory.upgrades);
    j.at("questItems").get_to(inventory.questItems);
    j.at("dungeonItems").get_to(inventory.dungeonItems);
    j.at("dungeonKeys").get_to(inventory.dungeonKeys);
    j.at("defenseHearts").get_to(inventory.defenseHearts);
    j.at("gsTokens").get_to(inventory.gsTokens);
}

void to_json(json& j, const SohStats& sohStats) {
    j = json{
        {"entrancesDiscovered", sohStats.entrancesDiscovered},
        {"fileCreatedAt", sohStats.fileCreatedAt},
    };
}

void from_json(const json& j, SohStats& sohStats) {
    j.at("entrancesDiscovered").get_to(sohStats.entrancesDiscovered);
    j.at("fileCreatedAt").get_to(sohStats.fileCreatedAt);
}

void to_json(json& j, const SaveContext& saveContext) {
    j = json{
        {"healthCapacity", saveContext.healthCapacity},
        {"magicLevel", saveContext.magicLevel},
        {"magicCapacity", saveContext.magicCapacity},
        {"isMagicAcquired", saveContext.isMagicAcquired},
        {"isDoubleMagicAcquired", saveContext.isDoubleMagicAcquired},
        {"isDoubleDefenseAcquired", saveContext.isDoubleDefenseAcquired},
        {"bgsFlag", saveContext.bgsFlag},
        {"swordHealth", saveContext.swordHealth},
        {"sceneFlags", saveContext.sceneFlags},
        {"eventChkInf", saveContext.eventChkInf},
        {"itemGetInf", saveContext.itemGetInf},
        {"infTable", saveContext.infTable},
        {"randomizerInf", saveContext.randomizerInf},
        {"gsFlags", saveContext.gsFlags},
        {"inventory", saveContext.inventory},
        {"sohStats", saveContext.sohStats},
        {"adultTradeItems", saveContext.adultTradeItems},
        {"checkTrackerData", saveContext.checkTrackerData},
        {"triforcePiecesCollected", saveContext.triforcePiecesCollected},
    };
}

void from_json(const json& j, SaveContext& saveContext) {
    j.at("healthCapacity").get_to(saveContext.healthCapacity);
    j.at("magicLevel").get_to(saveContext.magicLevel);
    j.at("magicCapacity").get_to(saveContext.magicCapacity);
    j.at("isMagicAcquired").get_to(saveContext.isMagicAcquired);
    j.at("isDoubleMagicAcquired").get_to(saveContext.isDoubleMagicAcquired);
    j.at("isDoubleDefenseAcquired").get_to(saveContext.isDoubleDefenseAcquired);
    j.at("bgsFlag").get_to(saveContext.bgsFlag);
    j.at("swordHealth").get_to(saveContext.swordHealth);
    j.at("sceneFlags").get_to(saveContext.sceneFlags);
    j.at("eventChkInf").get_to(saveContext.eventChkInf);
    j.at("itemGetInf").get_to(saveContext.itemGetInf);
    j.at("infTable").get_to(saveContext.infTable);
    j.at("randomizerInf").get_to(saveContext.randomizerInf);
    j.at("gsFlags").get_to(saveContext.gsFlags);
    j.at("inventory").get_to(saveContext.inventory);
    j.at("sohStats").get_to(saveContext.sohStats);
    j.at("adultTradeItems").get_to(saveContext.adultTradeItems);
    j.at("checkTrackerData").get_to(saveContext.checkTrackerData);
    j.at("triforcePiecesCollected").get_to(saveContext.triforcePiecesCollected);
}

std::map<uint32_t, AnchorClient> GameInteractorAnchor::AnchorClients = {};
std::vector<uint32_t> GameInteractorAnchor::ActorIndexToClientId = {};
std::string GameInteractorAnchor::clientVersion = (char*)gBuildVersion;
std::vector<std::pair<uint16_t, int16_t>> receivedItems = {};
std::vector<uint16_t> discoveredEntrances = {};
std::vector<AnchorMessage> anchorMessages = {};
uint32_t notificationId = 0;
bool settingsCopied = false;

void Anchor_DisplayMessage(AnchorMessage message = {}) {
    message.id = notificationId++;
    anchorMessages.push_back(message);
    Audio_PlaySoundGeneral(NA_SE_SY_METRONOME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
}

void Anchor_SendClientData() {
    nlohmann::json payload;
    payload["data"]["name"] = CVarGetString("gRemote.AnchorName", "");
    payload["data"]["color"] = CVarGetColor24("gRemote.AnchorColor", { 100, 255, 100 });
    payload["data"]["clientVersion"] = GameInteractorAnchor::clientVersion;
    payload["type"] = "UPDATE_CLIENT_DATA";

    SPDLOG_INFO("[Anchor] A IsSaveLoaded() {}", GameInteractor::IsSaveLoaded());

    if (GameInteractor::Instance->IsSaveLoaded()) {
        payload["data"]["seed"] = gSaveContext.finalSeed;
        payload["data"]["fileNum"] = gSaveContext.fileNum;
        payload["data"]["sceneNum"] = gPlayState->sceneNum;
        payload["data"]["entranceIndex"] = gSaveContext.entranceIndex;
        payload["data"]["gameComplete"] = gSaveContext.sohStats.gameComplete;
    } else {
        payload["data"]["seed"] = 0;
        payload["data"]["fileNum"] = 0xFF;
        payload["data"]["sceneNum"] = SCENE_ID_MAX;
        payload["data"]["entranceIndex"] = 0x00;
        payload["data"]["gameComplete"] = false;
    }

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_PushSettingsToRemote() {
    // If we're asked to push, set settingsCopied to true since we were either the
    // first in the room, or we've already had our settings copied.
    settingsCopied = true;

    nlohmann::json payload;
    payload["type"] = "PUSH_ANCHOR_SETTINGS";
    payload["teleportRupeeCost"] = CVarGetInteger("gTeleportRupeeCost", 0);

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_RequestSettingsFromRemote() {
    nlohmann::json payload;
    payload["type"] = "REQUEST_ANCHOR_SETTINGS";

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_CopySettingsFromRemote(nlohmann::json payload) {
    if (settingsCopied) {
        return;
    }

    CVarSetInteger("gTeleportRupeeCost", payload["teleportRupeeCost"].get<int32_t>());

    settingsCopied = true;
    Anchor_DisplayMessage({ .message = "Settings copied from remote." });
}

void GameInteractorAnchor::Enable() {
    if (isEnabled) {
        return;
    }

    isEnabled = true;
    GameInteractor::Instance->EnableRemoteInteractor();
    GameInteractor::Instance->RegisterRemoteJsonHandler([&](nlohmann::json payload) {
        try {
            HandleRemoteJson(payload);
        } catch (const std::exception& e) {
            SPDLOG_ERROR("[Anchor] Error handling remote JSON: {}", e.what());
        } catch (...) {
            SPDLOG_ERROR("[Anchor] Unknown Error handling remote JSON");
        }
    });
    GameInteractor::Instance->RegisterRemoteConnectedHandler([&]() {
        Anchor_DisplayMessage({ .message = "Connected to Anchor" });
        Anchor_SendClientData();
        Anchor_RequestSettingsFromRemote();

        if (GameInteractor::IsSaveLoaded()) {
            Anchor_RequestSaveStateFromRemote();
        }
    });
    GameInteractor::Instance->RegisterRemoteDisconnectedHandler([&]() {
        Anchor_DisplayMessage({ .message = "Disconnected from Anchor" });
    });
}

void GameInteractorAnchor::Disable() {
    if (!isEnabled) {
        return;
    }

    isEnabled = false;
    settingsCopied = false;
    GameInteractor::Instance->DisableRemoteInteractor();

    GameInteractorAnchor::AnchorClients.clear();
    Anchor_RefreshClientActors();
}

void GameInteractorAnchor::TransmitJsonToRemote(nlohmann::json payload) {
    payload["roomId"] = CVarGetString("gRemote.AnchorRoomId", "");
    if (!payload.contains("quiet")) {
        SPDLOG_INFO("[Anchor] Sending payload:\n{}", payload.dump());
    }
    GameInteractor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_ParseSaveStateFromRemote(nlohmann::json payload);
void Anchor_PushSaveStateToRemote();

void GameInteractorAnchor::HandleRemoteJson(nlohmann::json payload) {
    if (!payload.contains("type")) {
        return;
    }

    if (!payload.contains("quiet")) {
        SPDLOG_INFO("[Anchor] Received payload:\n{}", payload.dump());
    }

    if ((payload["type"] != "ALL_CLIENT_DATA" && payload["type"] != "UPDATE_CLIENT_DATA")) {
        if (payload.contains("clientId")) {
            uint32_t clientId = payload["clientId"].get<uint32_t>();
            if (GameInteractorAnchor::AnchorClients.contains(clientId) && GameInteractorAnchor::AnchorClients[clientId].clientVersion != GameInteractorAnchor::clientVersion) {
                return;
            }
        }
    }

    if (payload["type"] == "GIVE_ITEM") {
        auto effect = new GameInteractionEffect::GiveItem();
        effect->parameters[0] = payload["modId"].get<uint16_t>();
        effect->parameters[1] = payload["getItemId"].get<int16_t>();
        CVarSetInteger("gFromGI", 1);
        receivedItems.push_back({ payload["modId"].get<uint16_t>(), payload["getItemId"].get<int16_t>() });
        if (effect->Apply() == Possible) {
            GetItemEntry getItemEntry = ItemTableManager::Instance->RetrieveItemEntry(effect->parameters[0], effect->parameters[1]);

            AnchorClient anchorClient = GameInteractorAnchor::AnchorClients[payload["clientId"].get<uint32_t>()];
            if (getItemEntry.getItemCategory != ITEM_CATEGORY_JUNK) {
                if (getItemEntry.modIndex == MOD_NONE) {
                    Anchor_DisplayMessage({
                        .itemIcon = SohUtils::GetIconNameFromItemID(getItemEntry.itemId),
                        .prefix = SohUtils::GetItemName(getItemEntry.itemId),
                        .message = "from",
                        .suffix = anchorClient.name
                    });
                } else if (getItemEntry.modIndex == MOD_RANDOMIZER) {
                    Anchor_DisplayMessage({
                        .itemIcon = SohUtils::GetIconNameFromItemID(SohUtils::GetItemIdIconFromRandomizerGet(getItemEntry.getItemId)),
                        .prefix = OTRGlobals::Instance->gRandomizer->EnumToSpoilerfileGetName[(RandomizerGet)getItemEntry.getItemId][gSaveContext.language],
                        .message = "from",
                        .suffix = anchorClient.name
                    });
                }
            }
        }
        CVarClear("gFromGI");
    }
    if (payload["type"] == "SET_SCENE_FLAG") {
        auto effect = new GameInteractionEffect::SetSceneFlag();
        effect->parameters[0] = payload["sceneNum"].get<int16_t>();
        effect->parameters[1] = payload["flagType"].get<int16_t>();
        effect->parameters[2] = payload["flag"].get<int16_t>();
        effect->Apply();
    }
    if (payload["type"] == "SET_FLAG") {
        auto effect = new GameInteractionEffect::SetFlag();
        effect->parameters[0] = payload["flagType"].get<int16_t>();
        effect->parameters[1] = payload["flag"].get<int16_t>();
        effect->Apply();

        // If mweep flag replace ruto's letter
        if (
            payload["flagType"].get<int16_t>() == FLAG_EVENT_CHECK_INF && 
            payload["flag"].get<int16_t>() == EVENTCHKINF_KING_ZORA_MOVED &&
            Inventory_HasSpecificBottle(ITEM_LETTER_RUTO)
        ) {
            Inventory_ReplaceItem(gPlayState, ITEM_LETTER_RUTO, ITEM_BOTTLE);
        }
    }
    if (payload["type"] == "UNSET_SCENE_FLAG") {
        auto effect = new GameInteractionEffect::UnsetSceneFlag();
        effect->parameters[0] = payload["sceneNum"].get<int16_t>();
        effect->parameters[1] = payload["flagType"].get<int16_t>();
        effect->parameters[2] = payload["flag"].get<int16_t>();
        effect->Apply();
    }
    if (payload["type"] == "UNSET_FLAG") {
        auto effect = new GameInteractionEffect::UnsetFlag();
        effect->parameters[0] = payload["flagType"].get<int16_t>();
        effect->parameters[1] = payload["flag"].get<int16_t>();
        effect->Apply();
    }
    if (payload["type"] == "CLIENT_UPDATE") {
        uint32_t clientId = payload["clientId"].get<uint32_t>();

        if (GameInteractorAnchor::AnchorClients.contains(clientId)) {
            GameInteractorAnchor::AnchorClients[clientId].sceneNum = payload["sceneNum"].get<int16_t>();
            GameInteractorAnchor::AnchorClients[clientId].roomIndex = payload.contains("roomIndex") ? payload.at("roomIndex").get<int16_t>() : 0;
            GameInteractorAnchor::AnchorClients[clientId].entranceIndex = payload.contains("entranceIndex") ? payload.at("entranceIndex").get<int16_t>() : 0;
            GameInteractorAnchor::AnchorClients[clientId].posRot = payload["posRot"].get<PosRot>();
        }
    }
    if (payload["type"] == "PUSH_SAVE_STATE" && GameInteractor::IsSaveLoaded()) {
        Anchor_ParseSaveStateFromRemote(payload);
    }
    if (payload["type"] == "REQUEST_SAVE_STATE" && GameInteractor::IsSaveLoaded()) {
        Anchor_PushSaveStateToRemote();
    }
    if (payload["type"] == "PUSH_ANCHOR_SETTINGS") {
        Anchor_CopySettingsFromRemote(payload);
    }
    if (payload["type"] == "REQUEST_ANCHOR_SETTINGS") {
        Anchor_PushSettingsToRemote();
    }
    if (payload["type"] == "ALL_CLIENT_DATA") {
        std::vector<AnchorClient> newClients = payload["clients"].get<std::vector<AnchorClient>>();

        // add new clients
        for (auto& client : newClients) {
            if (!GameInteractorAnchor::AnchorClients.contains(client.clientId)) {
                GameInteractorAnchor::AnchorClients[client.clientId] = {
                    client.clientId,
                    client.clientVersion,
                    client.name,
                    client.color,
                    client.seed,
                    client.fileNum,
                    client.gameComplete,
                    client.sceneNum,
                    0,
                    client.entranceIndex,
                    { -9999, -9999, -9999, 0, 0, 0 }
                };
                Anchor_DisplayMessage({
                    .prefix = client.name,
                    .prefixColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
                    .message = "connected"
                });
            }
        }

        // remove clients that are no longer in the list
        std::vector<uint32_t> clientsToRemove;
        for (auto& [clientId, client] : GameInteractorAnchor::AnchorClients) {
            if (std::find_if(newClients.begin(), newClients.end(), [clientId = clientId](AnchorClient& c) { return c.clientId == clientId; }) == newClients.end()) {
                clientsToRemove.push_back(clientId);
            }
        }
        for (auto& clientId : clientsToRemove) {
            Anchor_DisplayMessage({
                .prefix = GameInteractorAnchor::AnchorClients[clientId].name,
                .prefixColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
                .message = "disconnected"
            });
            GameInteractorAnchor::AnchorClients.erase(clientId);
        }

        Anchor_RefreshClientActors();
    }
    if (payload["type"] == "UPDATE_CLIENT_DATA") {
        uint32_t clientId = payload["clientId"].get<uint32_t>();
        if (GameInteractorAnchor::AnchorClients.contains(clientId)) {
            AnchorClient client = payload["data"].get<AnchorClient>();
            GameInteractorAnchor::AnchorClients[clientId].clientVersion = client.clientVersion;
            GameInteractorAnchor::AnchorClients[clientId].name = client.name;
            GameInteractorAnchor::AnchorClients[clientId].color = client.color;
            GameInteractorAnchor::AnchorClients[clientId].seed = client.seed;
            GameInteractorAnchor::AnchorClients[clientId].fileNum = client.fileNum;
            GameInteractorAnchor::AnchorClients[clientId].gameComplete = client.gameComplete;
            GameInteractorAnchor::AnchorClients[clientId].sceneNum = client.sceneNum;
            GameInteractorAnchor::AnchorClients[clientId].entranceIndex = client.entranceIndex;
        }
    }
    if (payload["type"] == "UPDATE_CHECK_DATA" && GameInteractor::IsSaveLoaded()) {
        auto check = payload["locationIndex"].get<uint32_t>();
        auto data = payload["checkData"].get<RandomizerCheckTrackerData>();
        CheckTracker::UpdateCheck(check, data);
    }
    if (payload["type"] == "ENTRANCE_DISCOVERED") {
        auto entranceIndex = payload["entranceIndex"].get<uint16_t>();
        discoveredEntrances.push_back(entranceIndex);
        Entrance_SetEntranceDiscovered(entranceIndex, 1);
    }
    if (payload["type"] == "UPDATE_BEANS_BOUGHT" && GameInteractor::IsSaveLoaded()) {
        BEANS_BOUGHT = payload["amount"].get<uint8_t>();
    }
    if (payload["type"] == "UPDATE_BEANS_COUNT" && GameInteractor::IsSaveLoaded()) {
        AMMO(ITEM_BEAN) = payload["amount"].get<uint8_t>();
    }
    if (payload["type"] == "CONSUME_ADULT_TRADE_ITEM" && GameInteractor::IsSaveLoaded()) {
        uint8_t itemId = payload["itemId"].get<uint8_t>();
        gSaveContext.adultTradeItems &= ~ADULT_TRADE_FLAG(itemId);
	    Inventory_ReplaceItem(gPlayState, itemId, Randomizer_GetNextAdultTradeItem());
    }
    if (payload["type"] == "UPDATE_KEY_COUNT" && GameInteractor::IsSaveLoaded()) {
        gSaveContext.inventory.dungeonKeys[payload["sceneNum"].get<int16_t>()] = payload["amount"].get<int8_t>();
    }
    if (payload["type"] == "GIVE_DUNGEON_ITEM" && GameInteractor::IsSaveLoaded()) {
        gSaveContext.inventory.dungeonItems[payload["sceneNum"].get<int16_t>()] |= gBitFlags[payload["itemId"].get<uint16_t>() - ITEM_KEY_BOSS];
    }
    if (payload["type"] == "GAME_COMPLETE") {
        AnchorClient anchorClient = GameInteractorAnchor::AnchorClients[payload["clientId"].get<uint32_t>()];
        Anchor_DisplayMessage({
            .prefix = anchorClient.name,
            .message = "has killed Ganon.",
        });
    }
    if (payload["type"] == "REQUEST_TELEPORT") {
        Anchor_TeleportToPlayer(payload["clientId"].get<uint32_t>());
    }
    if (payload["type"] == "TELEPORT_TO") {
        // Check if we have enough rupees.
        int32_t teleportCost = CVarGetInteger("gTeleportRupeeCost", 0);
        if (gSaveContext.rupees < teleportCost) {
            // Can't teleport.
            std::stringstream msg;
            msg << "Need " << teleportCost << " rupees to teleport.";
            Anchor_DisplayMessage({ .message = msg.str() });
            return;
        }
        Rupees_ChangeBy(-1 * teleportCost);

        uint32_t entranceIndex = payload["entranceIndex"].get<uint32_t>();
        uint32_t roomIndex = payload["roomIndex"].get<uint32_t>();
        PosRot posRot = payload["posRot"].get<PosRot>();

        Play_SetRespawnData(gPlayState, RESPAWN_MODE_DOWN, entranceIndex, roomIndex, 0xDFF, &posRot.pos, posRot.rot.y);
        Play_TriggerVoidOut(gPlayState);
    }
    if (payload["type"] == "SERVER_MESSAGE") {
        Anchor_DisplayMessage({
            .prefix = "Server:",
            .prefixColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
            .message = payload["message"].get<std::string>(),
        });
    }
    if (payload["type"] == "DISABLE_ANCHOR") {
        GameInteractor::Instance->isRemoteInteractorEnabled = false;
        GameInteractorAnchor::Instance->isEnabled = false;
    }
    if (payload["type"] == "RESET") {
        std::reinterpret_pointer_cast<LUS::ConsoleWindow>(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch("reset");
    }
}

void Anchor_PushSaveStateToRemote() {
    json payload = gSaveContext;
    payload["type"] = "PUSH_SAVE_STATE";
    // manually update current scene flags
    payload["sceneFlags"][gPlayState->sceneNum]["chest"] = gPlayState->actorCtx.flags.chest;
    payload["sceneFlags"][gPlayState->sceneNum]["swch"] = gPlayState->actorCtx.flags.swch;
    payload["sceneFlags"][gPlayState->sceneNum]["clear"] = gPlayState->actorCtx.flags.clear;
    payload["sceneFlags"][gPlayState->sceneNum]["collect"] = gPlayState->actorCtx.flags.collect;

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_RequestSaveStateFromRemote() {
    nlohmann::json payload;
    payload["type"] = "REQUEST_SAVE_STATE";

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_ParseSaveStateFromRemote(nlohmann::json payload) {
    SaveContext loadedData = payload.get<SaveContext>();

    gSaveContext.healthCapacity = loadedData.healthCapacity;
    gSaveContext.magicLevel = loadedData.magicLevel;
    gSaveContext.magicCapacity = gSaveContext.magic = loadedData.magicCapacity;
    gSaveContext.isMagicAcquired = loadedData.isMagicAcquired;
    gSaveContext.isDoubleMagicAcquired = loadedData.isDoubleMagicAcquired;
    gSaveContext.isDoubleDefenseAcquired = loadedData.isDoubleDefenseAcquired;
    gSaveContext.bgsFlag = loadedData.bgsFlag;
    gSaveContext.swordHealth = loadedData.swordHealth;
    gSaveContext.adultTradeItems = loadedData.adultTradeItems;
    gSaveContext.triforcePiecesCollected = loadedData.triforcePiecesCollected;

    for (int i = 0; i < 124; i++) {
        gSaveContext.sceneFlags[i] = loadedData.sceneFlags[i];
        if (gPlayState->sceneNum == i) {
            gPlayState->actorCtx.flags.chest = loadedData.sceneFlags[i].chest;
            gPlayState->actorCtx.flags.swch = loadedData.sceneFlags[i].swch;
            gPlayState->actorCtx.flags.clear = loadedData.sceneFlags[i].clear;
            gPlayState->actorCtx.flags.collect = loadedData.sceneFlags[i].collect;
        }
    }

    for (int i = 0; i < 14; i++) {
        gSaveContext.eventChkInf[i] = loadedData.eventChkInf[i];
    }

    for (int i = 0; i < 4; i++) {
        gSaveContext.itemGetInf[i] = loadedData.itemGetInf[i];
    }

    // Skip last row of infTable, don't want to sync swordless flag
    for (int i = 0; i < 29; i++) {
        gSaveContext.infTable[i] = loadedData.infTable[i];
    }

    for (int i = 0; i < 9; i++) {
        gSaveContext.randomizerInf[i] = loadedData.randomizerInf[i];
    }

    for (int i = 0; i < 6; i++) {
        gSaveContext.gsFlags[i] = loadedData.gsFlags[i];
    }

    for (int i = 0; i < SAVEFILE_ENTRANCES_DISCOVERED_IDX_COUNT; i++) {
        gSaveContext.sohStats.entrancesDiscovered[i] = loadedData.sohStats.entrancesDiscovered[i];
    }

    CheckTracker::Teardown();
    CheckTracker::CheckTrackerLoadGame(gSaveContext.fileNum);
    for (int i = 2; i < RC_MAX; i++) {
        gSaveContext.checkTrackerData[i].status = loadedData.checkTrackerData[i].status;
        gSaveContext.checkTrackerData[i].skipped = loadedData.checkTrackerData[i].skipped;
        gSaveContext.checkTrackerData[i].price = loadedData.checkTrackerData[i].price;
        gSaveContext.checkTrackerData[i].hintItem = loadedData.checkTrackerData[i].hintItem;
    }

    gSaveContext.sohStats.fileCreatedAt = loadedData.sohStats.fileCreatedAt;

    // Restore master sword state
    u8 hasMasterSword = CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, 1);
    if (hasMasterSword) {
        loadedData.inventory.equipment |= 0x2;
    } else {
        loadedData.inventory.equipment &= ~0x2;
    }

    // Restore bottle contents (unless it's ruto's letter)
    for (int i = 0; i < 4; i++) {
        if (gSaveContext.inventory.items[SLOT_BOTTLE_1 + i] != ITEM_NONE && gSaveContext.inventory.items[SLOT_BOTTLE_1 + i] != ITEM_LETTER_RUTO) {
            loadedData.inventory.items[SLOT_BOTTLE_1 + i] = gSaveContext.inventory.items[SLOT_BOTTLE_1 + i];
        }
    }

    // Restore ammo if it's non-zero, unless it's beans
    for (int i = 0; i < ARRAY_COUNT(gSaveContext.inventory.ammo); i++) {
        if (gSaveContext.inventory.ammo[i] != 0 && i != SLOT(ITEM_BEAN) && i != SLOT(ITEM_BEAN + 1)) {
            loadedData.inventory.ammo[i] = gSaveContext.inventory.ammo[i];
        }
    }

    gSaveContext.inventory = loadedData.inventory;
    Anchor_DisplayMessage({ .message = "State loaded from remote!" });
};

AnchorClient* Anchor_GetClientByActorIndex(uint32_t actorIndex) {
    if (actorIndex < GameInteractorAnchor::ActorIndexToClientId.size()) {
        uint32_t clientId = GameInteractorAnchor::ActorIndexToClientId[actorIndex];
        if (GameInteractorAnchor::AnchorClients.find(clientId) != GameInteractorAnchor::AnchorClients.end()) {
            return &GameInteractorAnchor::AnchorClients[clientId];
        }
    }

    return nullptr;
}

uint8_t Anchor_GetClientScene(uint32_t actorIndex) {
    AnchorClient* client = Anchor_GetClientByActorIndex(actorIndex);
    if (client == nullptr) return SCENE_ID_MAX;

    return client->sceneNum;
}

PosRot Anchor_GetClientPosition(uint32_t actorIndex) {
    AnchorClient* client = Anchor_GetClientByActorIndex(actorIndex);
    if (client == nullptr) return { -9999.0, -9999.0, -9999.0, 0, 0, 0 };

    return client->posRot;
}

uint8_t Anchor_GetClientRoomIndex(uint32_t actorIndex) {
    AnchorClient* client = Anchor_GetClientByActorIndex(actorIndex);
    if (client == nullptr) return 0xFF;

    return client->roomIndex;
}

Color_RGB8 Anchor_GetClientColor(uint32_t actorIndex) {
    AnchorClient* client = Anchor_GetClientByActorIndex(actorIndex);
    if (client == nullptr) return { 100, 255, 100 };

    return client->color;
}

void Anchor_RefreshClientActors() {
    if (!GameInteractor::IsSaveLoaded()) return;
    Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;
    while (actor != NULL) {
        if (gEnPartnerId == actor->id) {
            Actor_Kill(actor);
        }
        actor = actor->next;
    }

    GameInteractorAnchor::ActorIndexToClientId.clear();

    uint32_t i = 0;
    for (auto [clientId, client] : GameInteractorAnchor::AnchorClients) {
        GameInteractorAnchor::ActorIndexToClientId.push_back(clientId);
        auto fairy = Actor_Spawn(
            &gPlayState->actorCtx, gPlayState, gEnPartnerId,
            client.posRot.pos.x, client.posRot.pos.y, client.posRot.pos.z, 
            client.posRot.rot.x, client.posRot.rot.y, client.posRot.rot.z,
            3 + i, false
        );
        NameTag_RegisterForActor(fairy, client.name.c_str());
        i++;
    }
}

static uint32_t lastSceneNum = SCENE_ID_MAX;

void Anchor_RegisterHooks() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneSpawnActors>([]() {
        if (gPlayState == NULL || !GameInteractor::Instance->isRemoteInteractorConnected) return;

        // Moved to a new scene
        if (lastSceneNum != gPlayState->sceneNum) {
            Anchor_SendClientData();
        }
        
        if (GameInteractor::Instance->IsSaveLoaded()) {
            // Player loaded into file
            if (lastSceneNum == SCENE_ID_MAX) {
                Anchor_RequestSaveStateFromRemote();
            }

            Anchor_RefreshClientActors();
        }

        lastSceneNum = gPlayState->sceneNum;
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPresentFileSelect>([]() {
        lastSceneNum = SCENE_ID_MAX;
        if (!GameInteractor::Instance->isRemoteInteractorConnected) return;

        Anchor_SendClientData();
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnExitGame>([](int32_t fileNum) {
        lastSceneNum = SCENE_ID_MAX;
        if (!GameInteractor::Instance->isRemoteInteractorConnected) return;

        Anchor_SendClientData();
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>([](GetItemEntry itemEntry) {
        if (itemEntry.modIndex == MOD_NONE && ((itemEntry.itemId >= ITEM_KEY_BOSS && itemEntry.itemId <= ITEM_KEY_SMALL) || itemEntry.itemId == ITEM_SWORD_MASTER)) {
            return;
        }

        // If the item exists in receivedItems, remove it from the list and don't emit the packet
        auto it = std::find_if(receivedItems.begin(), receivedItems.end(), [itemEntry](std::pair<uint16_t, int16_t> pair) {
            return pair.first == itemEntry.tableId && pair.second == itemEntry.getItemId;
        });
        if (it != receivedItems.end()) {
            receivedItems.erase(it);
            return;
        }

        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

        nlohmann::json payload;

        payload["type"] = "GIVE_ITEM";
        payload["modId"] = itemEntry.tableId;
        payload["getItemId"] = itemEntry.getItemId;

        GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>([](int16_t sceneNum, int16_t flagType, int16_t flag) {
        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;
        nlohmann::json payload;

        payload["type"] = "SET_SCENE_FLAG";
        payload["sceneNum"] = sceneNum;
        payload["flagType"] = flagType;
        payload["flag"] = flag;
        payload["quiet"] = true;

        GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>([](int16_t flagType, int16_t flag) {
        if (flagType == FLAG_INF_TABLE && flag == INFTABLE_SWORDLESS) {
            return;
        }

        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;
        nlohmann::json payload;

        payload["type"] = "SET_FLAG";
        payload["flagType"] = flagType;
        payload["flag"] = flag;
        payload["quiet"] = true;

        GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagUnset>([](int16_t sceneNum, int16_t flagType, int16_t flag) {
        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;
        nlohmann::json payload;

        payload["type"] = "UNSET_SCENE_FLAG";
        payload["sceneNum"] = sceneNum;
        payload["flagType"] = flagType;
        payload["flag"] = flag;
        payload["quiet"] = true;

        GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagUnset>([](int16_t flagType, int16_t flag) {
        if (flagType == FLAG_INF_TABLE && flag == INFTABLE_SWORDLESS) {
            return;
        }

        if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;
        nlohmann::json payload;

        payload["type"] = "UNSET_FLAG";
        payload["flagType"] = flagType;
        payload["flag"] = flag;
        payload["quiet"] = true;

        GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        static uint32_t lastPlayerCount = 0;
        uint32_t currentPlayerCount = 0;
        for (auto& [clientId, client] : GameInteractorAnchor::AnchorClients) {
            if (client.sceneNum == gPlayState->sceneNum) {
                currentPlayerCount++;
            }
        }
        if (!GameInteractor::Instance->isRemoteInteractorConnected || gPlayState == NULL || !GameInteractor::Instance->IsSaveLoaded()) {
            lastPlayerCount = currentPlayerCount;
            return;
        }
        Player* player = GET_PLAYER(gPlayState);
        nlohmann::json payload;
        float currentPosition = player->actor.world.pos.x + player->actor.world.pos.y + player->actor.world.pos.z + player->actor.world.rot.y;
        static float lastPosition = 0.0f;

        if (currentPosition == lastPosition && currentPlayerCount == lastPlayerCount) return;

        payload["type"] = "CLIENT_UPDATE";
        payload["sceneNum"] = gPlayState->sceneNum;
        payload["roomIndex"] = gPlayState->roomCtx.curRoom.num;
        payload["entranceIndex"] = gSaveContext.entranceIndex;
        payload["posRot"] = player->actor.world;
        payload["quiet"] = true;

        lastPosition = currentPosition;
        lastPlayerCount = currentPlayerCount;

        for (auto& [clientId, client] : GameInteractorAnchor::AnchorClients) {
            if (client.sceneNum == gPlayState->sceneNum) {
                payload["targetClientId"] = clientId;
                GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
            }
        }
    });
}

void Anchor_EntranceDiscovered(uint16_t entranceIndex) {
    if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

    // If the entrance exists in discoveredEntrances, remove it from the list and don't emit the packet
    auto it = std::find(discoveredEntrances.begin(), discoveredEntrances.end(), entranceIndex);
    if (it != discoveredEntrances.end()) {
        discoveredEntrances.erase(it);
        return;
    }

    nlohmann::json payload;

    payload["type"] = "ENTRANCE_DISCOVERED";
    payload["entranceIndex"] = entranceIndex;

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_UpdateCheckData(uint32_t locationIndex) {
    if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

    nlohmann::json payload;

    payload["type"] = "UPDATE_CHECK_DATA";
    payload["locationIndex"] = locationIndex;
    payload["checkData"] = gSaveContext.checkTrackerData[locationIndex];
    if (gSaveContext.checkTrackerData[locationIndex].status == RCSHOW_COLLECTED) {
        payload["checkData"]["status"] = RCSHOW_SAVED;
    }

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_UpdateBeansBought(uint8_t amount) {
    if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

    nlohmann::json payload;

    payload["type"] = "UPDATE_BEANS_BOUGHT";
    payload["amount"] = amount;

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_UpdateBeansCount(uint8_t amount) {
    if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

    nlohmann::json payload;

    payload["type"] = "UPDATE_BEANS_COUNT";
    payload["amount"] = amount;

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_ConsumeAdultTradeItem(uint8_t itemId) {
    if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

    nlohmann::json payload;

    payload["type"] = "CONSUME_ADULT_TRADE_ITEM";
    payload["itemId"] = itemId;

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_UpdateKeyCount(int16_t sceneNum, int8_t amount) {
    if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

    nlohmann::json payload;

    payload["type"] = "UPDATE_KEY_COUNT";
    payload["sceneNum"] = sceneNum;
    payload["amount"] = amount;

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_GiveDungeonItem(int16_t sceneNum, uint16_t itemId) {
    if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

    nlohmann::json payload;

    payload["type"] = "GIVE_DUNGEON_ITEM";
    payload["sceneNum"] = sceneNum;
    payload["itemId"] = itemId;

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_GameComplete() {
    if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

    nlohmann::json payload;

    payload["type"] = "GAME_COMPLETE";

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
    Anchor_SendClientData();
}

void Anchor_RequestTeleport(uint32_t clientId) {
    if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;

    nlohmann::json payload;

    payload["type"] = "REQUEST_TELEPORT";
    payload["targetClientId"] = clientId;

    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

void Anchor_TeleportToPlayer(uint32_t clientId) {
    if (!GameInteractor::Instance->isRemoteInteractorConnected || !GameInteractor::Instance->IsSaveLoaded()) return;
    Player* player = GET_PLAYER(gPlayState);

    nlohmann::json payload;

    payload["type"] = "TELEPORT_TO";
    payload["targetClientId"] = clientId;
    payload["entranceIndex"] = gSaveContext.entranceIndex;
    payload["roomIndex"] = gPlayState->roomCtx.curRoom.num;
    payload["posRot"] = player->actor.world;
    
    GameInteractorAnchor::Instance->TransmitJsonToRemote(payload);
}

const ImVec4 GRAY = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
const ImVec4 WHITE = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
const ImVec4 GREEN = ImVec4(0.5f, 1.0f, 0.5f, 1.0f);

void AnchorPlayerLocationWindow::DrawElement() {
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
    ImGui::Begin("AnchorPlayerLocationWindow", &mIsVisible, 
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoScrollbar
    );

    ImGui::TextColored(gSaveContext.sohStats.gameComplete ? GREEN : WHITE, "%s", CVarGetString("gRemote.AnchorName", ""));
    if (GameInteractor::Instance->IsSaveLoaded()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), "%s", SohUtils::GetSceneName(gPlayState->sceneNum).c_str());
    }
    for (auto& [clientId, client] : GameInteractorAnchor::AnchorClients) {
        ImGui::PushID(clientId);
        ImGui::TextColored(client.gameComplete ? GREEN : WHITE, "%s", client.name.c_str());
        if (client.clientVersion != GameInteractorAnchor::clientVersion) {
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), ICON_FA_EXCLAMATION_TRIANGLE);
                    if (ImGui::IsItemHovered()) {
                        ImGui::BeginTooltip();
                        ImGui::Text("Incompatible version! Will not work together!");
                        ImGui::Text("Yours: %s", GameInteractorAnchor::clientVersion.c_str());
                        ImGui::Text("Theirs: %s", client.clientVersion.c_str());
                        ImGui::EndTooltip();
                    }
                }
        if (client.seed != gSaveContext.finalSeed && client.fileNum != 0xFF && GameInteractor::Instance->IsSaveLoaded()) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 0, 0, 1), ICON_FA_EXCLAMATION_TRIANGLE);
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("Seed mismatch! Continuing will break things!");
                ImGui::Text("Yours: %u", gSaveContext.finalSeed);
                ImGui::Text("Theirs: %u", client.seed);
                ImGui::EndTooltip();
            }
        }
        if (client.sceneNum < SCENE_ID_MAX && client.fileNum != 0xFF) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.5, 0.5, 0.5, 1), "%s", SohUtils::GetSceneName(client.sceneNum).c_str());
            if (GameInteractor::Instance->IsSaveLoaded() && client.sceneNum != SCENE_GROTTOS && client.sceneNum != SCENE_ID_MAX) {
                ImGui::SameLine();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                if (ImGui::Button(ICON_FA_CHEVRON_RIGHT, ImVec2(ImGui::GetFontSize() * 1.0f, ImGui::GetFontSize() * 1.0f))) {
                    Anchor_RequestTeleport(clientId);
                }
                ImGui::PopStyleVar();
            }
        }
        ImGui::PopID();
    }

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
}

void AnchorLogWindow::DrawElement() {
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
    ImGui::Begin("AnchorLogWindow", &mIsVisible,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoScrollbar
    );

    // Options to stack notifications on top or bottom, and left or right
    if (ImGui::Button(CVarGetInteger("gRemote.AnchorLogWindowX", 1) ? ICON_FA_CHEVRON_RIGHT : ICON_FA_CHEVRON_LEFT, ImVec2(20, 20))) {
        CVarSetInteger("gRemote.AnchorLogWindowX", !CVarGetInteger("gRemote.AnchorLogWindowX", 1));
    }
    ImGui::SameLine();
    if (ImGui::Button(CVarGetInteger("gRemote.AnchorLogWindowY", 1) ? ICON_FA_CHEVRON_DOWN : ICON_FA_CHEVRON_UP, ImVec2(20, 20))) {
        CVarSetInteger("gRemote.AnchorLogWindowY", !CVarGetInteger("gRemote.AnchorLogWindowY", 1));
    }

    // Store x/y position of window
    ImVec2 anchorPos = ImGui::GetWindowPos();
    ImVec2 anchorSize = ImGui::GetWindowSize();

    for (int index = 0; index < anchorMessages.size(); ++index) {
        auto& message = anchorMessages[index];
        int inverseIndex = -ABS(index - (anchorMessages.size() - 1));
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        if (message.remainingTime < 4.0f) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, (message.remainingTime - 1) / 3.0f);
        } else {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
        }
        ImGui::Begin(("anchorLog" + std::to_string(message.id)).c_str(), nullptr,
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar
        );
        ImGui::SetWindowPos(ImVec2(
            // X position should take into account both the alignment and the width of the message window
            anchorPos.x + (CVarGetInteger("gRemote.AnchorLogWindowX", 1) ? 0 : -(ImGui::GetWindowSize().x - anchorSize.x)),
            // Y Position should take into account the stack direction and index of the message
            anchorPos.y + (CVarGetInteger("gRemote.AnchorLogWindowY", 1) ? (anchorSize.y + (ImGui::GetWindowSize().y * inverseIndex)) : -(ImGui::GetWindowSize().y * (inverseIndex + 1)))
        ));
        ImGui::SetWindowFontScale(1.8f);

        if (message.itemIcon != nullptr) {
            ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(message.itemIcon), ImVec2(24, 24));
            ImGui::SameLine();
        }
        if (!message.prefix.empty()) {
            ImGui::TextColored(message.prefixColor, "%s", message.prefix.c_str());
            ImGui::SameLine();
        }
        ImGui::TextColored(message.messageColor, "%s", message.message.c_str());
        if (!message.suffix.empty()) {
            ImGui::SameLine();
            ImGui::TextColored(message.suffixColor, "%s", message.suffix.c_str());
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
}

void AnchorLogWindow::UpdateElement() {
    // Remove expired messages
    for (int index = 0; index < anchorMessages.size(); ++index) {
        auto& message = anchorMessages[index];

        // decrement remainingTime
        message.remainingTime -= ImGui::GetIO().DeltaTime;

        // remove message if it has expired
        if (message.remainingTime <= 0) {
            anchorMessages.erase(anchorMessages.begin() + index);
            --index;
        }
    }
}

#endif
