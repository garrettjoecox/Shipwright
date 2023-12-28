#include <libultraship/bridge.h>
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

#define RAND_GET_OPTION(option) Rando::Context::GetInstance()->GetOption(option).GetSelectedOptionIndex()

RandomizerCheck GetRandomizerCheckFromFlag(int16_t flagType, int16_t flag) {
    for (auto& loc : Rando::StaticData::GetLocationTable()) {
        if (loc.GetCollectionCheck().flag == flag && (
            (flagType == FLAG_EVENT_CHECK_INF && loc.GetCollectionCheck().type == SPOILER_CHK_EVENT_CHK_INF) ||
            (flagType == FLAG_RANDOMIZER_INF && loc.GetCollectionCheck().type == SPOILER_CHK_RANDOMIZER_INF)
        )) {
            return loc.GetRandomizerCheck();
        }
    }

    return RC_UNKNOWN_CHECK;
}

RandomizerCheck GetRandomizerCheckFromSceneFlag(int16_t sceneNum, int16_t flagType, int16_t flag) {
    for (auto& loc : Rando::StaticData::GetLocationTable()) {
        if (loc.GetCollectionCheck().scene == sceneNum && loc.GetCollectionCheck().flag == flag && (
            (flagType == FLAG_SCENE_TREASURE && loc.GetCollectionCheck().type == SPOILER_CHK_CHEST) ||
            (flagType == FLAG_GS_TOKEN && loc.GetCollectionCheck().type == SPOILER_CHK_GOLD_SKULLTULA)
        )) {
            return loc.GetRandomizerCheck();
        }
    }

    return RC_UNKNOWN_CHECK;
}

bool MeetsLACSRequirements() {
    switch (RAND_GET_OPTION(RSK_GANONS_BOSS_KEY)) {
        case RO_GANON_BOSS_KEY_LACS_STONES:
            if ((CheckStoneCount() + CheckLACSRewardCount()) >= RAND_GET_OPTION(RSK_LACS_STONE_COUNT)) {
                return true;
            }
            break;
        case RO_GANON_BOSS_KEY_LACS_MEDALLIONS:
            if ((CheckMedallionCount() + CheckLACSRewardCount()) >= RAND_GET_OPTION(RSK_LACS_MEDALLION_COUNT)) {
                return true;
            }
            break;
        case RO_GANON_BOSS_KEY_LACS_REWARDS:
            if ((CheckMedallionCount() + CheckStoneCount() + CheckLACSRewardCount()) >= RAND_GET_OPTION(RSK_LACS_REWARD_COUNT)) {
                return true;
            }
            break;
        case RO_GANON_BOSS_KEY_LACS_DUNGEONS:
            if ((CheckDungeonCount() + CheckLACSRewardCount()) >= RAND_GET_OPTION(RSK_LACS_DUNGEON_COUNT)) {
                return true;
            }
            break;
        case RO_GANON_BOSS_KEY_LACS_TOKENS:
            if (gSaveContext.inventory.gsTokens >= RAND_GET_OPTION(RSK_LACS_TOKEN_COUNT)) {
                return true;
            }
            break;
        default:
            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) && CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW)) {
                return true;
            }
            break;
    }

    return false;
}

// Todo Move this to randomizer context, clear it out on save load etc
static std::queue<RandomizerCheck> randomizerQueuedChecks;
static RandomizerCheck randomizerQueuedCheck = RC_UNKNOWN_CHECK;
static GetItemEntry randomizerQueuedItemEntry = GET_ITEM_NONE;

void RandomizerOnFlagSetHandler(int16_t flagType, int16_t flag) {
    RandomizerCheck rc = GetRandomizerCheckFromFlag(flagType, flag);
    if (rc == RC_UNKNOWN_CHECK) return;

    auto loc = Rando::Context::GetInstance()->GetItemLocation(rc);
    if (loc == nullptr || loc->HasObtained()) return;

    SPDLOG_INFO("Queuing RC: {}", rc);
    randomizerQueuedChecks.push(rc);
}

void RandomizerOnSceneFlagSetHandler(int16_t sceneNum, int16_t flagType, int16_t flag) {
    RandomizerCheck rc = GetRandomizerCheckFromSceneFlag(sceneNum, flagType, flag);
    if (rc == RC_UNKNOWN_CHECK) return;

    auto loc = Rando::Context::GetInstance()->GetItemLocation(rc);
    if (loc == nullptr || loc->HasObtained()) return;

    SPDLOG_INFO("Queuing RC: {}", rc);
    randomizerQueuedChecks.push(rc);
}

void RandomizerOnPlayerUpdateForRCQueueHandler() {
    if (randomizerQueuedChecks.size() < 1 || randomizerQueuedCheck != RC_UNKNOWN_CHECK) return;

    RandomizerCheck rc = randomizerQueuedChecks.front();
    auto loc = Rando::Context::GetInstance()->GetItemLocation(rc);
    GetItemEntry getItemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(rc, true, (GetItemID)Rando::StaticData::GetLocation(rc)->GetVanillaItem());

    if (loc->HasObtained()) {
        SPDLOG_INFO("RC {} already obtained, skipping", rc);
    } else {
        randomizerQueuedCheck = rc;
        randomizerQueuedItemEntry = getItemEntry;
        SPDLOG_INFO("Queueing Item mod {} item {} from RC {}", getItemEntry.modIndex, getItemEntry.itemId, rc);
    }

    randomizerQueuedChecks.pop();
}

void RandomizerOnPlayerUpdateForItemQueueHandler() {
    if (randomizerQueuedCheck == RC_UNKNOWN_CHECK) return;

    Player* player = GET_PLAYER(gPlayState);
    if (player == NULL || Player_InBlockingCsMode(gPlayState, player) || player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS || player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM || player->stateFlags1 & PLAYER_STATE1_ITEM_OVER_HEAD) {
        return;
    }

    SPDLOG_INFO("Attempting to give Item mod {} item {} from RC {}", randomizerQueuedItemEntry.modIndex, randomizerQueuedItemEntry.itemId, randomizerQueuedCheck);
    GiveItemEntryWithoutActor(gPlayState, randomizerQueuedItemEntry);
    if (player->stateFlags1 & PLAYER_STATE1_IN_WATER) {
        // Allow the player to receive the item while swimming
        player->stateFlags2 |= PLAYER_STATE2_UNDERWATER;
        func_8083E5A8(player, gPlayState);
    }
}

void RandomizerOnItemReceiveHandler(GetItemEntry receivedItemEntry) {
    if (randomizerQueuedCheck == RC_UNKNOWN_CHECK) return;

    auto loc = Rando::Context::GetInstance()->GetItemLocation(randomizerQueuedCheck);
    if (randomizerQueuedItemEntry.modIndex == receivedItemEntry.modIndex && randomizerQueuedItemEntry.itemId == receivedItemEntry.itemId) {
        SPDLOG_INFO("Item received mod {} item {} from RC {}", receivedItemEntry.modIndex, receivedItemEntry.itemId, randomizerQueuedCheck);
        loc->MarkAsObtained();
        randomizerQueuedCheck = RC_UNKNOWN_CHECK;
        randomizerQueuedItemEntry = GET_ITEM_NONE;
    }

    if (loc->GetRandomizerCheck() == RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST) {
        static uint32_t updateHook;
        updateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
            Player* player = GET_PLAYER(gPlayState);
            if (player == NULL || Player_InBlockingCsMode(gPlayState, player) || player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS || player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM || player->stateFlags1 & PLAYER_STATE1_ITEM_OVER_HEAD) {
                return;
            }

            gPlayState->nextEntranceIndex = ENTR_DESERT_COLOSSUS_0;
            gPlayState->transitionTrigger = TRANS_TRIGGER_START;
            gSaveContext.nextCutsceneIndex = 0xFFF1;
            gPlayState->transitionType = TRANS_TYPE_SANDSTORM_END;
            GET_PLAYER(gPlayState)->stateFlags1 &= ~PLAYER_STATE1_IN_CUTSCENE;
            Player_TryCsAction(gPlayState, NULL, 8);
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(updateHook);
        });
    }
}

void RandomizerOnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, void* optionalArg) {
    switch (id) {
        case GI_VB_GIVE_ITEM_FROM_CHEST: {
            Player* player = GET_PLAYER(gPlayState);
            player->unk_850 = 1;
            player->getItemId = GI_NONE;
            player->getItemEntry = (GetItemEntry) GET_ITEM_NONE;
            *should = false;
            break;
        }
        case GI_VB_PLAY_NABOORU_CAPTURED_CS:
            // This behavior is replicated for randomizer in RandomizerOnItemReceiveHandler
            *should = false;
            break;
        case GI_VB_BE_ELIGIBLE_FOR_SERENADE_OF_WATER:
            *should = !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_SERENADE_OF_WATER) && Flags_GetTreasure(gPlayState, 0x2);
            break;
        case GI_VB_BE_ELIGIBLE_FOR_PRELUDE_OF_LIGHT:
            *should = !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT) && CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST);
            break;
        case GI_VB_MOVE_MIDO_IN_KOKIRI_FOREST:
            if (RAND_GET_OPTION(RSK_FOREST) == RO_FOREST_OPEN) {
                *should = true;
            }
            break;
        case GI_VB_MIDO_CONSIDER_DEKU_TREE_DEAD:
            *should = Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_KOKIRI_EMERALD_DEKU_TREE_DEAD);
            break;
        case GI_VB_OPEN_KOKIRI_FOREST:
            *should = Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_KOKIRI_EMERALD_DEKU_TREE_DEAD) || RAND_GET_OPTION(RSK_FOREST) != RO_FOREST_CLOSED;
            break;
        case GI_VB_BE_ELIGIBLE_FOR_DARUNIAS_JOY_REWARD:
            *should = !Flags_GetRandomizerInf(RAND_INF_DARUNIAS_JOY);
            break;
        case GI_VB_BE_ELIGIBLE_FOR_LIGHT_ARROWS:
            *should = !Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS) && MeetsLACSRequirements();
            break;
        case GI_VB_BE_ELIGIBLE_FOR_NOCTURNE_OF_SHADOW:
            *should =
                    !Flags_GetEventChkInf(EVENTCHKINF_BONGO_BONGO_ESCAPED_FROM_WELL) &&
                    LINK_IS_ADULT &&
                    gEntranceTable[((void)0, gSaveContext.entranceIndex)].scene == SCENE_KAKARIKO_VILLAGE &&
                    CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST) &&
                    CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) &&
                    CHECK_QUEST_ITEM(QUEST_MEDALLION_WATER);
            break;
        case GI_VB_GIVE_ITEM_MASTER_SWORD:
            if (RAND_GET_OPTION(RSK_SHUFFLE_MASTER_SWORD)) {
                *should = false;
            } else {
                *should = true;
                Rando::Context::GetInstance()->GetItemLocation(RC_TOT_MASTER_SWORD)->MarkAsObtained();
            }
            break;
        case GI_VB_GIVE_ITEM_STRENGTH_1:
        case GI_VB_GIVE_ITEM_LIGHT_MEDALLION:
        case GI_VB_GIVE_ITEM_OCARINA_OF_TIME:
            *should = false;
            break;
    }
}

void RandomizerOnSceneInitHandler(int16_t sceneNum) {
    // LACs & Prelude checks
    static uint32_t updateHook = 0;

    if (updateHook) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(updateHook);
        updateHook = 0;
    }

    if (
        sceneNum != SCENE_TEMPLE_OF_TIME || 
        (
            Flags_GetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT) &&
            Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS)
        )
    ) return;

    updateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        if (!Flags_GetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT) && LINK_IS_ADULT && CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST) && gPlayState->roomCtx.curRoom.num == 0) {
            Flags_SetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT);
        }

        if (!Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS) && MeetsLACSRequirements()) {
            Flags_SetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS);
        }

        if (
            Flags_GetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT) &&
            Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS)
        ) {
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(updateHook);
        }
    });
}

void RandomizerRegisterHooks() {
    static uint32_t onFlagSetHook = 0;
    static uint32_t onSceneFlagSetHook = 0;
    static uint32_t onPlayerUpdateForRCQueueHook = 0;
    static uint32_t onPlayerUpdateForItemQueueHook = 0;
    static uint32_t onItemReceiveHook = 0;
    static uint32_t onVanillaBehaviorHook = 0;
    static uint32_t onSceneInitHook = 0;

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([](int32_t fileNum) {
        randomizerQueuedChecks = std::queue<RandomizerCheck>();
        randomizerQueuedCheck = RC_UNKNOWN_CHECK;
        randomizerQueuedItemEntry = GET_ITEM_NONE;

        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagSet>(onFlagSetHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneFlagSet>(onSceneFlagSetHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(onPlayerUpdateForRCQueueHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(onPlayerUpdateForItemQueueHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnItemReceive>(onItemReceiveHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnVanillaBehavior>(onVanillaBehaviorHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(onSceneInitHook);

        onFlagSetHook = 0;
        onSceneFlagSetHook = 0;
        onPlayerUpdateForRCQueueHook = 0;
        onPlayerUpdateForItemQueueHook = 0;
        onItemReceiveHook = 0;
        onVanillaBehaviorHook = 0;
        onSceneInitHook = 0;

        if (!IS_RANDO) return;

        onFlagSetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>(RandomizerOnFlagSetHandler);
        onSceneFlagSetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>(RandomizerOnSceneFlagSetHandler);
        onPlayerUpdateForRCQueueHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>(RandomizerOnPlayerUpdateForRCQueueHandler);
        onPlayerUpdateForItemQueueHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>(RandomizerOnPlayerUpdateForItemQueueHandler);
        onItemReceiveHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>(RandomizerOnItemReceiveHandler);
        onVanillaBehaviorHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnVanillaBehavior>(RandomizerOnVanillaBehaviorHandler);
        onSceneInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>(RandomizerOnSceneInitHandler);
    });
}
