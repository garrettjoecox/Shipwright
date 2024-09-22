#include "ChaosWindow.h"
#include "../../UIWidgets.hpp"
#include "../../util.h"
#include "../../OTRGlobals.h"

#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include <cmath>

extern "C" {
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
#include "src/overlays/actors/ovl_En_Md/z_en_md.h"
#include "src/overlays/actors/ovl_En_Bubble/z_en_bubble.h"
void func_809CC774(EnBubble* thisx);
}

float chaosWindowSize = 1.0f;

uint32_t frameCounter = 0;
uint32_t invisibilityTimer = 0;
uint32_t stormTimer = 0;
uint32_t ironBootTimer = 0;
uint32_t hoverBootTimer = 0;
uint32_t bombermanTimer = 0;
uint32_t lavaTimer = 0;
uint32_t fakeTimer = 0;
uint32_t fakeTeleportTimer = 0;
uint32_t magnetTimer = 0;
uint32_t deathSwitchTimer = 0;
uint32_t knuckleTimer = 0;
uint32_t midoTimer = 0;
uint32_t guardTimer = 0;
uint32_t erasureTimer = 0;

uint32_t chaosInterval = 5;
uint32_t votingInterval = 5;
uint32_t activeInterval = 1;

uint32_t prevExplosiveLimit = 0;
int16_t prevYaw;
Vec3f prevPos;
RoomContext prevRoomCtx;
uint32_t prevRoomNum;
uint32_t prevScene = 0;

uint8_t prevEquip;
uint32_t prevRoll = 10;

uint32_t eventInvisibleTimer;
uint32_t eventStormTimer;
uint32_t eventIronTimer;
uint32_t eventHoverTimer;
uint32_t eventBomberTimer;
uint32_t eventLavaTimer;
uint32_t eventFakeTimer;
uint32_t eventRealTimer;
uint32_t eventMagnetTimer;
uint32_t eventDeathSwitchTimer;
uint32_t eventKnuckleTimer;
uint32_t eventMidoTimer;
uint32_t eventGuardTimer;
uint32_t eventErasureTimer;
uint32_t moblinTimer;
uint32_t backwardsTimer;
uint32_t limbChaosTimer;
uint32_t cardinalsRevengeTimer;
uint32_t acidTimer;
uint32_t fireMazeTimer;
uint32_t floatingStuffTimer;
static std::vector<Actor*> floatingStuffActors;

ActorListEntry guardActors;
Actor* currGuard = guardActors.head;

static uint32_t actorMagnetHook = 0;
static uint32_t votingHook = 0;

uint32_t votingOptionA;
uint32_t votingOptionB;
uint32_t votingOptionC;
uint32_t voteCountA = 0;
uint32_t voteCountCRight = 0;
uint32_t voteCountCLeft = 0;

bool isVotingActive = false;
bool rollOptions = false;
bool shouldTeleport = false;
bool openChaosSettings = false;

const char* fakeText = "Random Teleport";

std::vector<voteObject> votingObjectList = {
    { votingOptionA, voteCountA },
    { votingOptionB, voteCountCLeft },
    { votingOptionC, voteCountCRight },
};

std::vector<eventObject> votingList = {};

std::vector<eventObject> eventList = {
    { EVENT_INVISIBILITY, "Invisibility", "gEnhancements.Invisibility", eventInvisibleTimer,
      "Link is invisible for the length of the timer. Try not to stub your toe on a pot!" },
    { EVENT_STORMY_WEATHER, "Stormy Weather", "gEnhancements.StormyWeather", eventStormTimer,
      "You aren't sure where this storm came from or why it also appears to be raining indoors..." },
    { EVENT_FORCE_IRON_BOOTS, "Force Iron Boots", "gEnhancements.IronBoots", eventIronTimer,
      "Perfect for that trip to the Water Temple you have been meaning to take." },
    { EVENT_FORCE_HOVER_BOOTS, "Force Hover Boots", "gEnhancements.HoverBoots", eventHoverTimer,
      "Not so perfect for that trip to the Ice Cavern you have been dreading." },
    { EVENT_BOMBERMAN_MODE, "Bomberman Mode", "gEnhancements.Bomberman", eventBomberTimer,
      "Exactly like you remember, except worse!" },
    { EVENT_FLOOR_IS_LAVA, "The Floor is Lava!", "gEnhancements.LavaFloor", eventLavaTimer,
      "The floor is lava, you know what that means." },
    { EVENT_FAKE_TELEPORT, "Fake Teleport", "gEnhancements.FakeTeleport", eventFakeTimer,
      "You totally wanted to come here..." },
    { EVENT_REAL_TELEPORT, "Random Teleport", "gEnhancements.RealTeleport", eventRealTimer,
      "You totally wanted to come here..." },
    { EVENT_ACTOR_MAGNET, "Actor Magnet", "gEnhancements.ActorMagnet", eventMagnetTimer,
      "Opposites attract, or something to that extent." },
    { EVENT_DEATH_SWITCH, "Death Switch", "gEnhancements.DeathSwitch", eventDeathSwitchTimer,
      "A random input will kill Link, which one that is changes over time." },
    { EVENT_KNUCKLE_RING, "Iron Knuckle Ring", "gEnhancements.KnuckleRing", eventKnuckleTimer,
      "A ring of Iron Knuckles has spawned!" },
    { EVENT_MIDO_SUCKS, "Mido Sucks", "gEnhancements.MidoSucks", eventMidoTimer,
      "Mido abruptly reminds you why they suck so much." },
    { EVENT_THROWN_IN_THE_PAST, "Spawn Royal Guard", "gEnhancements.RoyalGuard", eventGuardTimer,
       "Hyrules finest Royal Guard comes to show you the business." },
    { EVENT_ERASURE, "Erase Dungeon Reward", "gEnhancements.Erasure", eventErasureTimer,
        "Erases a Dungeon Reward and respective Dungeon Boss." },
    { EVENT_MOBLIN_CHARGE, "Moblin Charge", "gEnhancements.Moblin", moblinTimer,
        "HERE'S JOHNNY" },
    { EVENT_BACKWARDS, "Backwards Movement", "gEnhancements.Backwards", backwardsTimer,
        "What are you running from?" },
    { EVENT_LIMB_CHAOS, "Limb Chaos", "gEnhancements.LimbChaos", limbChaosTimer,
        "My Leg!!!!!" },
    { EVENT_CARDINALS_REVENGE, "Cardinals Revenge", "gEnhancements.CardinalsRevenge", cardinalsRevengeTimer,
        "The pots are mad" },
    { EVENT_ACID, "Acid", "gEnhancements.Acid", acidTimer,
        "Trippy stuff man" },
    { EVENT_FIRE_MAZE, "Fire Maze", "gEnhancements.FireMaze", fireMazeTimer,
        "It's getting hot in here" },
    { EVENT_FLOATING_STUFF, "Floating Stuff", "gEnhancements.FloatingStuff", floatingStuffTimer,
        "What is this stuff? Why is it floating?" },
};

std::vector<uint32_t> teleportList = {
    GI_TP_DEST_SERENADE,
    GI_TP_DEST_REQUIEM,
    GI_TP_DEST_BOLERO,
    GI_TP_DEST_MINUET,
    GI_TP_DEST_NOCTURNE,
    GI_TP_DEST_PRELUDE,
    GI_TP_DEST_LINKSHOUSE,
};

std::vector<uint32_t> actorCatList = {
    ACTORCAT_SWITCH,
    ACTORCAT_BG,
    ACTORCAT_NPC,
    ACTORCAT_PROP,
    ACTORCAT_MISC,
};

std::vector<uint32_t> rcBosses = {
    RC_QUEEN_GOHMA,
    RC_KING_DODONGO,
    RC_BARINADE,
    RC_PHANTOM_GANON,
    RC_VOLVAGIA,
    RC_MORPHA,
    RC_BONGO_BONGO,
    RC_TWINROVA,
};

std::unordered_map<uint32_t, uint32_t> bossList = {
    { RC_QUEEN_GOHMA, 		SCENE_DEKU_TREE_BOSS, },
    { RC_KING_DODONGO, 		SCENE_DODONGOS_CAVERN_BOSS, },
    { RC_BARINADE, 			SCENE_JABU_JABU_BOSS, },
    { RC_PHANTOM_GANON, 	SCENE_FOREST_TEMPLE_BOSS, },
    { RC_VOLVAGIA, 			SCENE_FIRE_TEMPLE_BOSS, },
    { RC_MORPHA, 			SCENE_WATER_TEMPLE_BOSS, },
    { RC_BONGO_BONGO, 		SCENE_SHADOW_TEMPLE_BOSS, },
    { RC_TWINROVA, 			SCENE_SPIRIT_TEMPLE_BOSS, },
};

std::unordered_map<uint32_t, uint32_t> itemToQuestList = {
    { ITEM_MEDALLION_FOREST, QUEST_MEDALLION_FOREST, },
    { ITEM_MEDALLION_FIRE,   QUEST_MEDALLION_FIRE, },
    { ITEM_MEDALLION_WATER,  QUEST_MEDALLION_WATER, },
    { ITEM_MEDALLION_SPIRIT, QUEST_MEDALLION_SPIRIT, },
    { ITEM_MEDALLION_SHADOW, QUEST_MEDALLION_SHADOW, },
    { ITEM_MEDALLION_LIGHT,  QUEST_MEDALLION_LIGHT, },
    { ITEM_KOKIRI_EMERALD,   QUEST_KOKIRI_EMERALD, },
    { ITEM_GORON_RUBY,       QUEST_GORON_RUBY, },
    { ITEM_ZORA_SAPPHIRE,    QUEST_ZORA_SAPPHIRE, },
};

std::vector<Actor*> actorData = {};
std::vector<std::pair<Vec3f, Vec3s>> actorDefaultData = {};
std::vector<eventObject> activeEvents = {};

std::vector<colorObject> colorOptions = {
    { COLOR_WHITE, ImVec4(1.0f, 1.0f, 1.0f, 1.0f) },     { COLOR_GRAY, ImVec4(0.4f, 0.4f, 0.4f, 1.0f) },
    { COLOR_DARK_GRAY, ImVec4(0.1f, 0.1f, 0.1f, 1.0f) }, { COLOR_INDIGO, ImVec4(0.24f, 0.31f, 0.71f, 1.0f) },
    { COLOR_LIGHT_RED, ImVec4(1.0f, 0.5f, 0.5f, 1.0f) }, { COLOR_RED, ImVec4(0.5f, 0.0f, 0.0f, 1.0f) },
    { COLOR_DARK_RED, ImVec4(0.3f, 0.0f, 0.0f, 1.0f) },  { COLOR_LIGHT_GREEN, ImVec4(0.0f, 0.7f, 0.0f, 1.0f) },
    { COLOR_GREEN, ImVec4(0.0f, 0.5f, 0.0f, 1.0f) },     { COLOR_DARK_GREEN, ImVec4(0.0f, 0.3f, 0.0f, 1.0f) },
    { COLOR_YELLOW, ImVec4(1.0f, 0.627f, 0.0f, 1.0f) },
};

ImVec4 voteColor = colorOptions[COLOR_WHITE].colorCode;

std::string formatChaosTimers(uint32_t value) {
    uint32_t sec = value / 20;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = value % 10;
    return fmt::format("{:0>2}:{:0>2}.{}", mm, ss, ds);
}

void ChaosUpdateWindowSize() {
    chaosWindowSize = CVarGetFloat(CVAR_ENHANCEMENT("ChaosWindowSize"), 1.0f);
}

void ChaosUpdateInterval() {
    chaosInterval = (CVarGetInteger(CVAR_ENHANCEMENT("ChaosInterval"), 5) * 1200);
}

void ChaosUpdateVotingInterval() {
    //votingInterval = (CVarGetInteger(CVAR_ENHANCEMENT("VotingInterval"), 60) * 20);
    votingInterval = 60;
}

void ChaosUpdateEventTimers() {
    uint32_t loop = 0;
    for (auto& eventUpdate : eventList) {
        int32_t setTimer = CVarGetInteger(eventUpdate.eventVariable, 5) * 1200;
        eventList[loop].eventTimer = setTimer;
        loop++;
    }
}

void ChaosEventActorMagnet() {
    if (!gPlayState) {
        return;
    }
    actorData.clear();
    actorDefaultData.clear();
    for (int i = 0; i < actorCatList.size(); i++) {
        ActorListEntry currList = gPlayState->actorCtx.actorLists[actorCatList[i]];
        Actor* currAct = currList.head;
        if (currAct != nullptr) {
            while (currAct != nullptr) {
                actorData.push_back(currAct);
                actorDefaultData.push_back({ currAct->world.pos, currAct->world.rot });
                currAct = currAct->next;
            }
        }
    }
}

void ChaosEventDeathSwitch(uint8_t buttonId) {
    prevEquip = gSaveContext.equips.buttonItems[buttonId];
    prevRoll = buttonId;
}

void ChaosEventsManager(uint32_t eventAction, uint32_t eventId) {
    switch (eventAction) {
        case EVENT_ACTION_REMOVE:
            for (int i = 0; i < activeEvents.size(); i++) {
                if (activeEvents[i].eventId == eventId) {
                    activeEvents.erase(activeEvents.begin() + i);
                    break;
                }
            }
            break;
        case EVENT_ACTION_ADD:
            for (auto& listCheck : eventList) {
                if (listCheck.eventId == eventId) {
                    activeEvents.push_back(listCheck);
                    break;
                }
            }
            break;
        default:
            break;
    }
}

bool isEventIdPresent(uint32_t eventId) {
    auto it = std::find_if(activeEvents.begin(), activeEvents.end(), [eventId](const eventObject& event) {
        return event.eventId == eventId;
    });

    return it != activeEvents.end();
}

void EnBubble_DrawAlt(Actor* thisx, PlayState* play) {
    EnBubble* enBubble = (EnBubble*)thisx;
    u32 pad;

    thisx->shape.rot.y += 960;

    GetItem_Draw(play, (GetItemDrawID)enBubble->unk_218);

    enBubble->actor.shape.shadowScale = (f32)((enBubble->expansionWidth + 1.0f) * 0.2f);
    func_809CC774(enBubble);
}

void ChaosEventsRepeater() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!gPlayState || GameInteractor::IsGameplayPaused()) {
            return;
        }
        Player* player = GET_PLAYER(gPlayState);

        if (isEventIdPresent(EVENT_INVISIBILITY) == true) {
            if (invisibilityTimer > 0) {
                invisibilityTimer--;
                GameInteractor::RawAction::SetLinkInvisibility(true);
            } else {
                GameInteractor::RawAction::SetLinkInvisibility(false);
            }
        }

        if (isEventIdPresent(EVENT_STORMY_WEATHER) == true) {
            if (stormTimer > 0) {
                stormTimer--;
                GameInteractor::RawAction::SetWeatherStorm(true);
            } else {
                GameInteractor::RawAction::SetWeatherStorm(false);
            }
        }

        if (isEventIdPresent(EVENT_FORCE_IRON_BOOTS) == true) {
            if (ironBootTimer > 0) {
                ironBootTimer--;
            } else {
                GameInteractor::RawAction::ForceEquipBoots(EQUIP_VALUE_BOOTS_KOKIRI);
            }
            if (player->currentBoots != PLAYER_BOOTS_IRON) {
                GameInteractor::RawAction::ForceEquipBoots(EQUIP_VALUE_BOOTS_IRON);
            }
        }

        if (isEventIdPresent(EVENT_FORCE_HOVER_BOOTS) == true) {
            if (hoverBootTimer > 0) {
                hoverBootTimer--;
            } else {
                GameInteractor::RawAction::ForceEquipBoots(EQUIP_VALUE_BOOTS_KOKIRI);
            }
            if (player->currentBoots != PLAYER_BOOTS_HOVER) {
                GameInteractor::RawAction::ForceEquipBoots(EQUIP_VALUE_BOOTS_HOVER);
            }
        }

        if (isEventIdPresent(EVENT_BOMBERMAN_MODE) == true) {
            if (bombermanTimer > 0) {
                if (bombermanTimer % 5 == 0) {
                    GameInteractor::RawAction::SpawnActor(ACTOR_EN_BOM, 0);
                }
                bombermanTimer--;
            } else {
                CVarSetInteger(CVAR_ENHANCEMENT("RemoveExplosiveLimit"), prevExplosiveLimit);
            }
        }

        if (isEventIdPresent(EVENT_FLOOR_IS_LAVA) == true) {
            if (lavaTimer > 0) {
                if (lavaTimer % 20 == 0) {
                    func_80837C0C(gPlayState, player, 0, 4, 5, 21372, 20);
                }
                lavaTimer--;
            }
        }
        
        if (isEventIdPresent(EVENT_FAKE_TELEPORT) == true) {
            if (fakeTimer > 0) {
                fakeTimer--;
            } else if (fakeTimer == 0 && shouldTeleport == false) {
                fakeText = "Fake Teleport";
                gPlayState->nextEntranceIndex = prevScene;
                gPlayState->transitionTrigger = TRANS_TRIGGER_START;
                gPlayState->transitionType = TRANS_TYPE_CIRCLE(TCA_WAVE, TCC_WHITE, TCS_FAST);
                gSaveContext.nextTransitionType = TRANS_TYPE_CIRCLE(TCA_WAVE, TCC_WHITE, TCS_SLOW);
                fakeTeleportTimer = 1;
                shouldTeleport = true;
            }
            if (fakeTeleportTimer == 0 && shouldTeleport == true) {
                if (gPlayState->transitionTrigger != TRANS_TRIGGER_START &&
                    gSaveContext.nextTransitionType == TRANS_NEXT_TYPE_DEFAULT) {
                    GET_PLAYER(gPlayState)->actor.shape.rot.y = prevYaw;
                    GET_PLAYER(gPlayState)->actor.world.pos = prevPos;
                    if (prevRoomNum != gPlayState->roomCtx.curRoom.num) {
                        func_8009728C(gPlayState, &prevRoomCtx, prevRoomNum);
                        func_80097534(gPlayState, &prevRoomCtx);
                    }
                }
                shouldTeleport = false;
            } else if (fakeTeleportTimer > 0) {
                fakeTeleportTimer--;
                fakeTimer--;
            }
            if (shouldTeleport == true && fakeTeleportTimer == 0 && fakeTimer > 0) {
                fakeTimer--;
            }
        }

        if (isEventIdPresent(EVENT_ACTOR_MAGNET) == true) {
            if (magnetTimer > 0) {
                for (auto& actorUpdate : actorData) {
                    Math_SmoothStepToF(&actorUpdate->world.pos.x, player->actor.world.pos.x + 30, 0.4f, 5.0f, 0.0f);
                    Math_SmoothStepToF(&actorUpdate->world.pos.y, player->actor.world.pos.y, 0.4f, 5.0f, 0.0f);
                    Math_SmoothStepToF(&actorUpdate->world.pos.z, player->actor.world.pos.z + 30, 0.4f, 5.0f, 0.0f);
                    actorUpdate->world.rot = player->actor.world.rot;
                }
                magnetTimer--;
            }
        }

        if (isEventIdPresent(EVENT_DEATH_SWITCH) == true) {
            if (deathSwitchTimer > 0) {
                deathSwitchTimer--;               
            }
            if (deathSwitchTimer % 120 == 0 || deathSwitchTimer == eventDeathSwitchTimer - 1) {
                uint32_t roll = (rand() % 7) + 1;
                if (prevRoll == 10) {
                    ChaosEventDeathSwitch(roll);
                    gSaveContext.equips.buttonItems[roll] = ITEM_SOLD_OUT;
                } else {
                    gSaveContext.equips.buttonItems[prevRoll] = prevEquip;
                    ChaosEventDeathSwitch(roll);
                    gSaveContext.equips.buttonItems[roll] = ITEM_SOLD_OUT;
                }
                GameInteractor::RawAction::ForceInterfaceUpdate();
            }
            if (prevRoll != 10) {
                bool deathTrigger = false;
                switch (prevRoll) {
                    case BUTTON_CLEFT:
                        if (CHECK_BTN_ALL(gPlayState->state.input->press.button, BTN_CLEFT)) {
                            deathTrigger = true;
                        }
                        break;
                    case BUTTON_CDOWN:
                        if (CHECK_BTN_ALL(gPlayState->state.input->press.button, BTN_CDOWN)) {
                            deathTrigger = true;
                        }
                        break;
                    case BUTTON_CRIGHT:
                        if (CHECK_BTN_ALL(gPlayState->state.input->press.button, BTN_CRIGHT)) {
                            deathTrigger = true;
                        }
                        break;
                    case BUTTON_DUP:
                        if (CHECK_BTN_ALL(gPlayState->state.input->press.button, BTN_DUP)) {
                            deathTrigger = true;
                        }
                        break;
                    case BUTTON_DDOWN:
                        if (CHECK_BTN_ALL(gPlayState->state.input->press.button, BTN_DDOWN)) {
                            deathTrigger = true;
                        }
                        break;
                    case BUTTON_DLEFT:
                        if (CHECK_BTN_ALL(gPlayState->state.input->press.button, BTN_DLEFT)) {
                            deathTrigger = true;
                        }
                        break;
                    case BUTTON_DRIGHT:
                        if (CHECK_BTN_ALL(gPlayState->state.input->press.button, BTN_DRIGHT)) {
                            deathTrigger = true;
                        }
                        break;
                    default:
                        deathTrigger = false;
                        break;
                }
                if (deathTrigger == true) {
                    gSaveContext.health = 0;
                }
            }
        }
        if (isEventIdPresent(EVENT_KNUCKLE_RING) == true) {
            if (knuckleTimer > 0) {
                knuckleTimer--;
            }
        }

        if (isEventIdPresent(EVENT_MIDO_SUCKS) == true) {
            if (midoTimer > 0) {
                midoTimer--;
                Vec3f_ actorPosition;

                ActorListEntry midoActors = gPlayState->actorCtx.actorLists[ACTORCAT_NPC];
                Actor* currAct = midoActors.head;
                Actor* actorToBlock = &GET_PLAYER(gPlayState)->actor;
                if (currAct != nullptr) {
                    while (currAct != nullptr) {
                        if (currAct->id == ACTOR_EN_MD) {
                            EnMd* midoActor = (EnMd*)currAct;
                            currAct->world.rot.y = currAct->yawTowardsPlayer;
                            currAct->shape.rot.y = currAct->yawTowardsPlayer;

                            s16 yaw = Math_Vec3f_Yaw(&currAct->home.pos, &actorToBlock->world.pos);

                            currAct->world.pos.x = currAct->home.pos.x;
                            currAct->world.pos.x += 60.0f * Math_SinS(yaw);

                            currAct->world.pos.z = currAct->home.pos.z;
                            currAct->world.pos.z += 60.0f * Math_CosS(yaw);

                            f32 temp = fabsf((f32)currAct->yawTowardsPlayer - yaw) * 0.001f * 3.0f;
                            midoActor->skelAnime.playSpeed = CLAMP(temp, 1.0f, 3.0f);
                        }
                        currAct = currAct->next;
                    }
                }
            }
        }

        if (isEventIdPresent(EVENT_THROWN_IN_THE_PAST) == true) {
            if (guardTimer > 0) {
                guardTimer--;
                guardActors = gPlayState->actorCtx.actorLists[ACTORCAT_NPC];
                currGuard = guardActors.head;
                if (currGuard != nullptr) {
                    while (currGuard != nullptr) {
                        if (currGuard->id == ACTOR_EN_HEISHI3) {
                            currGuard->world.rot.y += 300.0f;
                        }
                        currGuard = currGuard->next;
                    }
                }
                if (guardTimer == eventList[EVENT_THROWN_IN_THE_PAST].eventTimer / 2) {
                    Player* player = GET_PLAYER(gPlayState);
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_HEISHI3, player->actor.world.pos.x + 250.0f,
                                player->actor.world.pos.y, player->actor.world.pos.z, 0,
                                player->actor.world.rot.y + 16384, 0, 0, false);
                }
            }
        }

        if (isEventIdPresent(EVENT_ERASURE) == true) {
            if (erasureTimer > 0) {
                erasureTimer--;

            }
        }

        if (isEventIdPresent(EVENT_BACKWARDS) == true) {
            if (backwardsTimer > 0) {
                Player* player = GET_PLAYER(gPlayState);
                player->actor.shape.rot.x = 32767;
                player->actor.shape.rot.z = 32767;
                backwardsTimer--;
            }
        }

        if (isEventIdPresent(EVENT_LIMB_CHAOS) == true) {
            if (limbChaosTimer > 0) {
                limbChaosTimer--;
                // Direction can be 0 or 1
                static int xDirection = 0;
                static int yDirection = 0;
                static int zDirection = 0;
                // Every 60 frames (3 seconds) randomize the target limb, can be 0-4
                if (limbChaosTimer % 60 == 0) {
                    CVarSetInteger(CVAR_COSMETIC("LimbChaos.TargetLimb"), (rand() % 5));
                    CVarClear(CVAR_COSMETIC("LimbChaos.X"));
                    CVarClear(CVAR_COSMETIC("LimbChaos.Y"));
                    CVarClear(CVAR_COSMETIC("LimbChaos.Z"));
                    xDirection = rand() % 2;
                    yDirection = rand() % 2;
                    zDirection = rand() % 2;
                }

                // Over the course of 60 frames, move the limb in the direction in a full 360 degree rotation
                uint16_t rotation = ((float)(limbChaosTimer % 60) / 60) * UINT16_MAX;
                uint16_t inverseRotation = UINT16_MAX - rotation;
                // Apply the direction to the rotation
                CVarSetInteger(CVAR_COSMETIC("LimbChaos.X"), xDirection == 0 ? rotation : inverseRotation);
                CVarSetInteger(CVAR_COSMETIC("LimbChaos.Y"), yDirection == 0 ? rotation : inverseRotation);
                CVarSetInteger(CVAR_COSMETIC("LimbChaos.Z"), zDirection == 0 ? rotation : inverseRotation);
            }
        }

        if (isEventIdPresent(EVENT_CARDINALS_REVENGE) == true) {
            if (cardinalsRevengeTimer > 0) {
                cardinalsRevengeTimer--;
                static int numOfPots = 8; // Every 45 degrees
                static int spawnSpeed = 10; // 1 pot every 10 frames
                static int lastAgroPotIndex = 0;
                static float circleRadius = 100.0f; // Set the radius of the circle
                static float angleIncrement = 2 * M_PI / numOfPots; // Calculate angle increment

                if (cardinalsRevengeTimer % spawnSpeed == 0) {
                    if (lastAgroPotIndex < numOfPots) {
                        lastAgroPotIndex++;
                    } else {
                        lastAgroPotIndex = 0;
                    }

                    Player* player = GET_PLAYER(gPlayState);
                    float x = player->actor.world.pos.x + circleRadius * cos(lastAgroPotIndex * angleIncrement);
                    float z = player->actor.world.pos.z + circleRadius * sin(lastAgroPotIndex * angleIncrement);

                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_TUBO_TRAP, x, player->actor.world.pos.y, z, 0, 0, 0, 0x1A, false);
                }
            }
        }

        if (isEventIdPresent(EVENT_ACID) == true) {
            if (acidTimer > 0) {
                acidTimer--;
                CVarSetFloat("ChaosUVOffset", CVarGetFloat("ChaosUVOffset", 0.0f) + (0.05f));
                if (CVarGetFloat("ChaosUVOffset", 0.0f) > 1.0f) {
                    CVarSetFloat("ChaosUVOffset", -1.0f);
                }
            }
        }

        if (isEventIdPresent(EVENT_FLOATING_STUFF) == true) {
            if (floatingStuffTimer > 0) {
                floatingStuffTimer--;

                if (floatingStuffActors.size() < 100 && floatingStuffTimer % 80 == 0) {
                    float x = player->actor.world.pos.x + (rand() % 1000) - 500;
                    float y = player->actor.world.pos.y + (rand() % 1000);
                    float z = player->actor.world.pos.z + (rand() % 1000) - 500;
                    Actor* actor = Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_BUBBLE, x, y, z, 0, 0, 0, 0, false);
                    floatingStuffActors.push_back(actor);
                    GetItemDrawID randomItem = (GetItemDrawID)(rand() % GID_MAXIMUM);
                    ((EnBubble*)actor)->unk_218 = randomItem;

                    actor->draw = EnBubble_DrawAlt;
                }
            }
        }
    });
}

// Function to spawn a wall
Actor* SpawnWall(float x, float y, float z, int16_t rotationY) {
    return Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_BG_HIDAN_FIREWALL, x, y, z, 0, rotationY, 0, 0, false);
}

void BuildFireWallRoom(std::vector<Actor*> *walls, float centerX, float centerY, float centerZ, int16_t wallsPerSide) {
    const float wallWidth = 150.0f;
    const float halfWidth = (wallsPerSide * wallWidth) / 2.0f;

    // Loop through each side
    for (int i = 0; i < wallsPerSide; i++) {
        float offset = i * wallWidth - halfWidth + (wallWidth / 2.0f);

        // Front wall (facing +Z)
        walls->push_back(SpawnWall(centerX + offset, centerY, centerZ + halfWidth, 0));

        // Back wall (facing -Z)
        walls->push_back(SpawnWall(centerX + offset, centerY, centerZ - halfWidth, 0));

        // Left wall (facing -X)
        walls->push_back(SpawnWall(centerX - halfWidth, centerY, centerZ + offset, 0x4000));

        // Right wall (facing +X)
        walls->push_back(SpawnWall(centerX + halfWidth, centerY, centerZ + offset, 0x4000));
    }
}

void ChaosEventsActivator(uint32_t eventId, bool isActive) {
    switch (eventId) {
        case EVENT_INVISIBILITY:
            if (isActive) {
                invisibilityTimer = eventList[EVENT_INVISIBILITY].eventTimer;
            } else {
                GameInteractor::RawAction::SetLinkInvisibility(false);
            }
            break;
        case EVENT_STORMY_WEATHER:
            if (isActive) {
                stormTimer = eventList[EVENT_STORMY_WEATHER].eventTimer;
            } else {
                GameInteractor::RawAction::SetWeatherStorm(false);
            }
            break;
        case EVENT_FORCE_IRON_BOOTS:
            if (isActive) {
                ironBootTimer = eventList[EVENT_FORCE_IRON_BOOTS].eventTimer;
            } else {
                GameInteractor::RawAction::ForceEquipBoots(EQUIP_VALUE_BOOTS_KOKIRI);
            }
            break;
        case EVENT_FORCE_HOVER_BOOTS:
            if (isActive) {
                hoverBootTimer = eventList[EVENT_FORCE_HOVER_BOOTS].eventTimer;
            } else {
                GameInteractor::RawAction::ForceEquipBoots(EQUIP_VALUE_BOOTS_KOKIRI);
            }
            break;
        case EVENT_BOMBERMAN_MODE:
            if (isActive) {
                bombermanTimer = eventList[EVENT_BOMBERMAN_MODE].eventTimer;
                if (!CVarGetInteger(CVAR_ENHANCEMENT("RemoveExplosiveLimit"), 0)) {
                    prevExplosiveLimit = 1;
                } else {
                    prevExplosiveLimit = 0;
                }
                CVarSetInteger(CVAR_ENHANCEMENT("RemoveExplosiveLimit"), 1);
            } else {
                bombermanTimer = 0;
            }
            break;
        case EVENT_FLOOR_IS_LAVA:
            if (isActive) {
                lavaTimer = eventList[EVENT_FLOOR_IS_LAVA].eventTimer;
            } else {
                Player* player = GET_PLAYER(gPlayState);
                func_80837C0C(gPlayState, player, 0, 0, 0, 0, 0);
                lavaTimer = 0;
            }
            break;
        case EVENT_FAKE_TELEPORT: // setup fake to be real then fake text/timer
        case EVENT_REAL_TELEPORT:
            if (isActive) {
                if (eventId == EVENT_FAKE_TELEPORT) {
                    prevPos = GET_PLAYER(gPlayState)->actor.world.pos;
                    prevScene = gSaveContext.entranceIndex;
                    prevRoomCtx = gPlayState->roomCtx;
                    prevRoomNum = gPlayState->roomCtx.curRoom.num;
                    fakeTimer = (eventList[EVENT_FAKE_TELEPORT].eventTimer / 3);
                }
                uint32_t teleportLoc = rand() % 6;
                GameInteractor::RawAction::TeleportPlayer(teleportList[teleportLoc]);
            }
            break;
        case EVENT_ACTOR_MAGNET:
            if (isActive) {
                actorMagnetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneSpawnActors>([]() {
                    ChaosEventActorMagnet();    
                });
                ChaosEventActorMagnet();
                magnetTimer = eventList[EVENT_ACTOR_MAGNET].eventTimer;
            } else {
                if (actorMagnetHook != 0) {
                    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneSpawnActors>(actorMagnetHook);
                    actorMagnetHook = 0;
                }
                for (int i = 0; i < actorData.size(); i++) {
                    actorData[i]->world.pos = actorDefaultData[i].first;
                    actorData[i]->world.rot = actorDefaultData[i].second;
                }
                actorData.clear();
                actorDefaultData.clear();
                magnetTimer = 0;
            }
            break;
        case EVENT_DEATH_SWITCH:
            if (isActive) {
                deathSwitchTimer = eventList[EVENT_DEATH_SWITCH].eventTimer;
            } else {
                gSaveContext.equips.buttonItems[prevRoll] = prevEquip;
                GameInteractor::RawAction::ForceInterfaceUpdate();
            }
            break;
        case EVENT_KNUCKLE_RING:
            if (isActive) {
                knuckleTimer = eventList[EVENT_KNUCKLE_RING].eventTimer;
                Player* player = GET_PLAYER(gPlayState);
                float radius = 45.0f;
                std::vector<std::tuple<float, float, float>> spawnPoints;

                for (int i = 0; i < 8; i++) {
                    float angle = i * (2.0f * M_PI / 8);

                    float x = player->actor.world.pos.x + radius * cos(angle);
                    float z = player->actor.world.pos.z + radius * sin(angle);
                    float y = player->actor.world.pos.y;

                    spawnPoints.push_back(std::make_tuple(x, y, z));
                }
                for (auto spawnKnuckle : spawnPoints) {
                    float deltaX = player->actor.world.pos.x - std::get<0>(spawnKnuckle);
                    float deltaZ = player->actor.world.pos.z - std::get<2>(spawnKnuckle);
                    float angleToPlayer = atan2(deltaZ, deltaX);
                    int16_t rotY = static_cast<int16_t>(angleToPlayer * (180.0f / M_PI));
                    rotY -= 90.0f;
                    rotY = static_cast<int16_t>(((rotY / 360.0f) * 65536.0f) * -1);
                    uint32_t knuckleColor = rand() % 3;

                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_IK, std::get<0>(spawnKnuckle),
                                std::get<1>(spawnKnuckle), std::get<2>(spawnKnuckle), 0, rotY, 0, knuckleColor, false);
                }
                
            } else {
                if (!gPlayState) {
                    return;
                }
                ActorListEntry nqKnuckles = gPlayState->actorCtx.actorLists[ACTORCAT_ENEMY];
                ActorListEntry hqKnuckles = gPlayState->actorCtx.actorLists[ACTORCAT_BOSS];
                Actor* currAct = nqKnuckles.head;
                if (currAct != nullptr) {
                    while (currAct != nullptr) {
                        if (currAct->id == ACTOR_EN_IK) {
                            Actor_Kill(currAct);
                        }
                        currAct = currAct->next;
                    }
                }
                currAct = hqKnuckles.head;
                if (currAct != nullptr) {
                    while (currAct != nullptr) {
                        if (currAct->id == ACTOR_EN_IK) {
                            Actor_Kill(currAct);
                        }
                        currAct = currAct->next;
                    }
                }
            }
            break;
        case EVENT_MIDO_SUCKS:
            if (isActive) {
                Player* player = GET_PLAYER(gPlayState);
                Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_MD, player->actor.world.pos.x,
                            player->actor.world.pos.y, player->actor.world.pos.z, 0, player->actor.world.rot.y + 16384, 0, 256, false);
                midoTimer = eventList[EVENT_MIDO_SUCKS].eventTimer;
            } else {
                ActorListEntry midoActors = gPlayState->actorCtx.actorLists[ACTORCAT_NPC];
                Actor* currAct = midoActors.head;
                if (currAct != nullptr) {
                    while (currAct != nullptr) {
                        if (currAct->id == ACTOR_EN_MD) {
                            Actor_Kill(currAct);
                        }
                        currAct = currAct->next;
                    }
                }
            }
            break;
        case EVENT_THROWN_IN_THE_PAST:
            if (isActive) {
                guardTimer = eventList[EVENT_THROWN_IN_THE_PAST].eventTimer;
                Player* player = GET_PLAYER(gPlayState);
                Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_HEISHI3, player->actor.world.pos.x + 250.0f,
                            player->actor.world.pos.y, player->actor.world.pos.z, 0, player->actor.world.rot.y + 16384,
                            0, 0, false);
            } else {
                guardActors = gPlayState->actorCtx.actorLists[ACTORCAT_NPC];
                currGuard = guardActors.head;
                if (currGuard != nullptr) {
                    while (currGuard != nullptr) {
                        if (currGuard->id == ACTOR_EN_HEISHI3) {
                            Actor_Kill(currGuard);
                        }
                        currGuard = currGuard->next;
                    }
                }
            }
            break;
        case EVENT_ERASURE:
            if (isActive) {
                std::vector<uint32_t> questItems = {};
                for (int i = ITEM_MEDALLION_FOREST; i <= ITEM_ZORA_SAPPHIRE; i++) {
                    if (CHECK_QUEST_ITEM(itemToQuestList[i])) {
                        questItems.push_back(i);
                    }
                }
                uint32_t roll = rand() % questItems.size();
                for (int i = 0; i < rcBosses.size(); i++) {
                    auto itemEntry = OTRGlobals::Instance->gRandomizer->GetItemFromKnownCheck((RandomizerCheck)rcBosses[i], GI_NONE, false);
                    if (itemEntry.itemId == questItems[roll]) {
                        uint32_t flagToClear = bossList[rcBosses[i]];
                        uint32_t bitMask = 1 << itemToQuestList[questItems[roll]];
                        gSaveContext.sceneFlags[flagToClear].clear = 0;
                        gSaveContext.inventory.questItems &= ~bitMask;
                        //need to unset item get flag as well.
                        break;
                    }
                }
            }
            break;
        case EVENT_MOBLIN_CHARGE: {
            static Actor* moblin = nullptr;
            if (isActive) {
                Player* player = GET_PLAYER(gPlayState);
                float spawnDistance = 45.0f; // Distance in front of the player to spawn the Moblin

                // Get the player's current rotation (angle they are facing)
                float playerRotY = player->actor.shape.rot.y * (M_PI / 32768.0f); // Convert fixed-point rotation to radians

                // Calculate spawn coordinates in front of the player
                float spawnX = player->actor.world.pos.x + spawnDistance * sin(playerRotY);
                float spawnZ = player->actor.world.pos.z + spawnDistance * cos(playerRotY);
                float spawnY = player->actor.world.pos.y;

                // Calculate the Moblin's rotation so it faces the player
                float deltaX = player->actor.world.pos.x - spawnX;
                float deltaZ = player->actor.world.pos.z - spawnZ;
                float angleToPlayer = atan2(deltaZ, deltaX);
                int16_t moblinRotY = static_cast<int16_t>(angleToPlayer * (180.0f / M_PI));
                moblinRotY -= 90.0f; // Adjust to align correctly
                moblinRotY = static_cast<int16_t>(((moblinRotY / 360.0f) * 65536.0f) * -1);

                // Spawn a single Moblin
                moblin = Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_MB, spawnX, spawnY, spawnZ, 0, moblinRotY, 0, -1, false);
            } else {
                if (moblin != nullptr) {
                    Player* player = GET_PLAYER(gPlayState);
                    if (player->stateFlags2 & PLAYER_STATE2_GRABBED_BY_ENEMY) {
                        player->stateFlags2 &= ~PLAYER_STATE2_GRABBED_BY_ENEMY;
                        player->av2.actionVar2 = 200;
                        func_8002F71C(gPlayState, &player->actor, 4.0f, player->actor.world.rot.y + 0x8000, 4.0f);
                    }
                    Actor_Kill(moblin);
                    moblin = nullptr;
                }
            }
            break;
        }
        case EVENT_BACKWARDS: {
            Player* player = GET_PLAYER(gPlayState);
            if (isActive) {
                backwardsTimer = eventList[EVENT_BACKWARDS].eventTimer;
                player->actor.shape.rot.x = 32767;
                player->actor.shape.rot.z = 32767;
            } else {
                player->actor.shape.rot.x = 0;
                player->actor.shape.rot.z = 0;
            }
            break;
        }
        case EVENT_LIMB_CHAOS: {
            if (isActive) {
                CVarSetInteger(CVAR_COSMETIC("LimbChaos.Enabled"), 1);
                limbChaosTimer = eventList[EVENT_LIMB_CHAOS].eventTimer;
            } else {
                CVarClear(CVAR_COSMETIC("LimbChaos.Enabled"));
            }
            break;
        }
        case EVENT_CARDINALS_REVENGE: {
            if (isActive) {
                cardinalsRevengeTimer = eventList[EVENT_CARDINALS_REVENGE].eventTimer;
            }
            break;
        }
        case EVENT_ACID: {
            if (isActive) {
                CVarClear("ChaosUVOffset");
                acidTimer = eventList[EVENT_ACID].eventTimer;
            } else {
                CVarClear("ChaosUVOffset");
            }
            break;
        }
        case EVENT_FIRE_MAZE: {
            static std::vector<Actor*> fireMazeActors;
            if (isActive) {
                Player* player = GET_PLAYER(gPlayState);
                // Builds a maze using fire wall actors. It should just build 3 layers of fire walls, and then remove a random one from each layer.
                BuildFireWallRoom(&fireMazeActors, player->actor.world.pos.x, player->actor.world.pos.y, player->actor.world.pos.z, 1);
                BuildFireWallRoom(&fireMazeActors, player->actor.world.pos.x, player->actor.world.pos.y, player->actor.world.pos.z, 3);
                BuildFireWallRoom(&fireMazeActors, player->actor.world.pos.x, player->actor.world.pos.y, player->actor.world.pos.z, 5);

                // Remove a random wall from each layer
                int firstLayer = rand() % 4;
                int secondLayer = rand() % 12;
                int thirdLayer = rand() % 20;

                Actor_Kill(fireMazeActors[firstLayer]);
                Actor_Kill(fireMazeActors[4 + secondLayer]);
                Actor_Kill(fireMazeActors[16 + thirdLayer]);
            } else {
                for (auto actor : fireMazeActors) {
                    Actor_Kill(actor);
                }
                fireMazeActors.clear();
            }
        }
        case EVENT_FLOATING_STUFF: {
            if (isActive) {
                Player* player = GET_PLAYER(gPlayState);

                floatingStuffTimer = eventList[EVENT_FLOATING_STUFF].eventTimer;
            } else {
                for (auto actor : floatingStuffActors) {
                    Actor_Kill(actor);
                }
                floatingStuffActors.clear();
            }
        }
        default:
            break;
    }
}

void ChaosVoteSelector(uint32_t option) {
    uint32_t roll = EVENT_ERASURE;
    //uint32_t roll = rand() % votingList.size();
    votingObjectList[option].votingOption = votingList[roll].eventId;
    //votingList.erase(votingList.begin() + roll);
}

void ChaosVotingStarted() {
    if (rollOptions) {
        votingList.clear();
        votingList = eventList;
        ChaosVoteSelector(VOTE_OPTION_A);
        ChaosVoteSelector(VOTE_OPTION_B);
        ChaosVoteSelector(VOTE_OPTION_C);
        rollOptions = false;
    }
}

uint32_t GetVotesForOption(uint32_t option) {
    uint32_t currentOption = -1;
    if (option == votingObjectList[0].votingOption) currentOption = 1;
    if (option == votingObjectList[1].votingOption) currentOption = 2;
    if (option == votingObjectList[2].votingOption) currentOption = 3;

    // Loop over map and return amount of votes that equal the option
    uint32_t votes = 0;
    for (auto& voting : GameInteractor::State::ChaosVotes) {
        if (voting.second == currentOption) {
            votes++;
        }
    }
    return votes;
}

void ChaosVotingFinished() {
    uint32_t voteWinner = 
        std::max({ GetVotesForOption(votingObjectList[VOTE_OPTION_A].votingOption), GetVotesForOption(votingObjectList[VOTE_OPTION_B].votingOption),
                   GetVotesForOption(votingObjectList[VOTE_OPTION_C].votingOption) });
    if (GetVotesForOption(votingObjectList[VOTE_OPTION_A].votingOption) == voteWinner) {
        ChaosEventsManager(EVENT_ACTION_ADD, eventList[votingObjectList[VOTE_OPTION_A].votingOption].eventId);
        ChaosEventsActivator(eventList[votingObjectList[VOTE_OPTION_A].votingOption].eventId, true);
    } else if (GetVotesForOption(votingObjectList[VOTE_OPTION_B].votingOption) == voteWinner) {
        ChaosEventsManager(EVENT_ACTION_ADD, eventList[votingObjectList[VOTE_OPTION_B].votingOption].eventId);
        ChaosEventsActivator(eventList[votingObjectList[VOTE_OPTION_B].votingOption].eventId, true);
    } else if (GetVotesForOption(votingObjectList[VOTE_OPTION_C].votingOption) == voteWinner) {
        ChaosEventsManager(EVENT_ACTION_ADD, eventList[votingObjectList[VOTE_OPTION_C].votingOption].eventId);
        ChaosEventsActivator(eventList[votingObjectList[VOTE_OPTION_C].votingOption].eventId, true);
    }
    isVotingActive = false;
    if (votingHook != 0) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnGameFrameUpdate>(votingHook);
        votingHook = 0;
    }
}

void ChaosTrackerTimer() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
        if (!gPlayState || GameInteractor::IsGameplayPaused() || !CVarGetInteger(CVAR_ENHANCEMENT("EnableChaosMode"), 0)) {
            CVarClear("ChaosUVOffset");
            return;
        }

        if (frameCounter % activeInterval == 0) {
            frameCounter = 0;
            if (isVotingActive) {
                ChaosVotingFinished();
                activeInterval = chaosInterval;
            } else {
                GameInteractor::State::ChaosVotes.clear();
                votingObjectList[VOTE_OPTION_A].votingCount = 0;
                votingObjectList[VOTE_OPTION_B].votingCount = 0;
                votingObjectList[VOTE_OPTION_C].votingCount = 0;
                isVotingActive = true;
                rollOptions = true;
                activeInterval = votingInterval;
            }
            
        }

        if (activeEvents.size() > 0) {
            for (auto& counter : activeEvents) {
                if (counter.eventTimer > 0) {
                    counter.eventTimer--;
                }
                if (counter.eventTimer == 0) {
                    ChaosEventsActivator(counter.eventId, false);
                    ChaosEventsManager(EVENT_ACTION_REMOVE, counter.eventId);
                }
            }
        }
        frameCounter++;
    });
}

void DrawChaosTrackerEvents() {
    if (activeEvents.size() > 0) {
        for (auto& obj : activeEvents) {
            if (obj.eventId == EVENT_FAKE_TELEPORT) {
                ImGui::TextColored(colorOptions[COLOR_LIGHT_GREEN].colorCode, fakeText);
            } else {
                ImGui::TextColored(colorOptions[COLOR_LIGHT_GREEN].colorCode, obj.eventName);
            }
            ImGui::SameLine();
            float rightAlign = ImGui::GetContentRegionAvail().x;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + rightAlign - ImGui::CalcTextSize(formatChaosTimers(obj.eventTimer).c_str()).x);
            ImGui::TextColored(colorOptions[COLOR_LIGHT_GREEN].colorCode, formatChaosTimers(obj.eventTimer).c_str());
            ImGui::TextWrapped(obj.eventDescription);
        }
    }
}

void ChaosVotingColorSelector(uint32_t option) {
    std::vector<uint32_t> votes = {
        GetVotesForOption(votingObjectList[VOTE_OPTION_A].votingOption),
        GetVotesForOption(votingObjectList[VOTE_OPTION_B].votingOption),
        GetVotesForOption(votingObjectList[VOTE_OPTION_C].votingOption),
    };
    std::sort(votes.begin(), votes.end(), [](const auto& a, const auto& b) { return a > b; });

    if (option == votes[0]) {
        voteColor = colorOptions[COLOR_LIGHT_GREEN].colorCode; // First place color
    } else if (option == votes[1]) {
        voteColor = colorOptions[COLOR_YELLOW].colorCode; // Second place color
    } else if (option == votes[2]) {
        voteColor = colorOptions[COLOR_LIGHT_RED].colorCode; // Third place color
    }
}

void DrawChaosEventsVoting() {
    ChaosVotingStarted();
    ImGui::TextColored(colorOptions[COLOR_WHITE].colorCode, "Voting Active");
    UIWidgets::PaddedSeparator();
    uint32_t voteNum = 1;
    for (auto& voting : votingObjectList) {
        ChaosVotingColorSelector(GetVotesForOption(voting.votingOption));
        std::string str = std::to_string(voteNum) + " ";
        ImGui::TextColored(voteColor, str.c_str());
        ImGui::SameLine();
        ImGui::TextColored(voteColor, eventList[voting.votingOption].eventName);
        ImGui::SameLine();
        std::string voteCounter = std::to_string(GetVotesForOption(voting.votingOption));
        float rightAlign = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + rightAlign - ImGui::CalcTextSize(voteCounter.c_str()).x);
        ImGui::TextColored(voteColor, voteCounter.c_str());
        voteNum++;
    }
    UIWidgets::PaddedSeparator();
}

void DrawChaosSettings() {
    if (!openChaosSettings) {
        return;
    }
    ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Chaos Settings");
    ImGui::PushItemWidth(100.0f);
    if (UIWidgets::PaddedEnhancementSliderFloat("Window Size: %.1fx", "##windowSize",
        CVAR_ENHANCEMENT("ChaosWindowSize"), 1.0f, 3.0f, "", 1.0f, false, false, true, false)) {
        ChaosUpdateWindowSize();
    }
    if (UIWidgets::PaddedEnhancementSliderInt("Chaos Interval", "##chaosInterval",
        CVAR_ENHANCEMENT("ChaosInterval"), 1, 10, "%d Minutes", 5, true, true, false)) {
        ChaosUpdateInterval();
    }
    if (UIWidgets::PaddedEnhancementSliderInt("Voting Interval", "##voteInterval",
        CVAR_ENHANCEMENT("VotingInterval"), 30, 60, "%d Seconds", 60, true, true, false)) {
        ChaosUpdateVotingInterval();
    }
    ImGui::PopItemWidth();
    UIWidgets::PaddedSeparator();
    uint32_t row = 0;
    ImGui::BeginTable("ChaosSettings", 2);
    ImGui::TableNextColumn();
    for (auto& optionsList : eventList) {
        if (row == eventList.size() / 2) {
            ImGui::TableNextColumn();
        }
        std::string str1 = "##";
        std::string optionId = str1 + optionsList.eventName;
        optionId.erase(std::remove(optionId.begin(), optionId.end(), ' '), optionId.end());
        if (UIWidgets::PaddedEnhancementSliderInt(optionsList.eventName, optionId.c_str(),
                                                  optionsList.eventVariable, 1, 10, "%d Minutes", 5, true, true,
                                                  true)) {
            ChaosUpdateEventTimers();
        }
        row++;
    }
    ImGui::EndTable();

    ImGui::End();
}

void ChaosWindow::DrawElement() {
    ImGui::SetWindowFontScale(chaosWindowSize);
    std::string frameText;
    uint32_t timeRemaining;
    
    if (UIWidgets::PaddedEnhancementCheckbox("Enable Chaos Mode", CVAR_ENHANCEMENT("EnableChaosMode"), true, false)) {
        if (CVarGetInteger(CVAR_ENHANCEMENT("EnableChaosMode"), 0) == 0) {
            for (auto& removal : activeEvents) {
                ChaosEventsActivator(removal.eventId, false);
                ChaosEventsManager(EVENT_ACTION_REMOVE, removal.eventId);
            }
            frameCounter = 0;
            isVotingActive = false;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Chaos Settings")) {
        openChaosSettings = !openChaosSettings;
    }
    DrawChaosSettings();

    if (isVotingActive) {
        timeRemaining = votingInterval - frameCounter;
        frameText = "Voting Ends in " + formatChaosTimers(timeRemaining);
    } else {
        timeRemaining = chaosInterval - frameCounter;
        frameText = "Next Vote Begins in " + formatChaosTimers(timeRemaining);
    }
    
    ImGui::Text(frameText.c_str());
    UIWidgets::PaddedSeparator();

    if (isVotingActive) {
        DrawChaosEventsVoting();
    }

    ImGui::BeginChild("Chaos Events");
    DrawChaosTrackerEvents();

    if (CVarGetInteger(CVAR_ENHANCEMENT("EnableChaosMode"), 0)) {
        for (int i = EVENT_INVISIBILITY; i < EVENT_MAX; i++) {
            if (ImGui::Button(eventList[i].eventName)) {
                if (isEventIdPresent(eventList[i].eventId) == false) {
                    ChaosEventsManager(EVENT_ACTION_ADD, eventList[i].eventId);
                    ChaosEventsActivator(eventList[i].eventId, true);
                } else {
                    ChaosEventsManager(EVENT_ACTION_REMOVE, eventList[i].eventId);
                    ChaosEventsActivator(eventList[i].eventId, false);
                }
            }
        }
    }

    ImGui::EndChild();
}

void ChaosWindow::InitElement() {
    ChaosUpdateWindowSize();
    ChaosUpdateInterval();
    ChaosUpdateVotingInterval();
    ChaosUpdateEventTimers();
    ChaosEventActorMagnet();
    ChaosEventsRepeater();
    ChaosTrackerTimer();
    DrawChaosSettings();
}
