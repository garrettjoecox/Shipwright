#include "Holiday.hpp"
#include <libultraship/libultraship.h>
#include "soh/UIWidgets.hpp"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "objects/object_dog/object_dog.h"
#include "soh/frame_interpolation.h"
#include "soh/Enhancements/randomizer/3drando/random.hpp"
#include "soh/Enhancements/randomizer/3drando/location_access.hpp"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/custom-collectible/CustomCollectible.h"
#include "soh/Notification/Notification.h"
#include "soh/Enhancements/nametag.h"

#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_md/object_md.h"
#include "objects/object_trap/object_trap.h"
#include "objects/object_toryo/object_toryo.h"
#include "src/overlays/actors/ovl_Door_Ana/z_door_ana.h"
extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"

extern PlayState* gPlayState;
void DoorAna_SetupAction(DoorAna* doorAna, DoorAnaActionFunc actionFunc);
void DoorAna_GrabPlayer(DoorAna* doorAna, PlayState* play);
}
extern GetItemEntry vanillaQueuedItemEntry;

#define AUTHOR "Fredomato"
#define CVAR(v) "gHoliday." AUTHOR "." v

static CollisionPoly snowballPoly;
static f32 raycastResult;

const s16 entrances[] = {
    0x0000, 0x0209, 0x0004, 0x0242, 0x0028, 0x0221, 0x0169, 0x0215, 0x0165, 0x024A, 0x0010, 0x021D, 0x0082, 0x01E1, 0x0037, 0x0205,
    0x0098, 0x02A6, 0x0088, 0x03D4, 0x0008, 0x03A8, 0x0467, 0x023D, 0x0433, 0x0443, 0x0437, 0x0447, 0x009C, 0x033C, 0x00C9, 0x026A,
    0x00C1, 0x0266, 0x0043, 0x03CC, 0x045F, 0x0309, 0x03A0, 0x03D0, 0x007E, 0x026E, 0x0530, 0x01D1, 0x0507, 0x03BC, 0x0388, 0x02A2,
    0x0063, 0x01D5, 0x0528, 0x03C0, 0x043B, 0x0067, 0x02FD, 0x0349, 0x0550, 0x04EE, 0x039C, 0x0345, 0x05C8, 0x05DC, 0x0072, 0x034D,
    0x030D, 0x0355, 0x037C, 0x03FC, 0x0380, 0x03C4, 0x004F, 0x0378, 0x02F9, 0x042F, 0x05D0, 0x05D4, 0x052C, 0x03B8, 0x016D, 0x01CD,
    0x00B7, 0x0201, 0x003B, 0x0463, 0x0588, 0x057C, 0x0578, 0x0340, 0x04C2, 0x03E8, 0x04BE, 0x0482, 0x0315, 0x045B, 0x0371, 0x0394,
    0x0272, 0x0211, 0x0053, 0x0472, 0x0453, 0x0351, 0x0384, 0x044B, 0x03EC, 0x04FF, 0x0700, 0x0800, 0x0701, 0x0801, 0x0702, 0x0802,
    0x0703, 0x0803, 0x0704, 0x0804, 0x0705, 0x0805, 0x0706, 0x0806, 0x0707, 0x0807, 0x0708, 0x0808, 0x0709, 0x0809, 0x070A, 0x080A,
    0x070B, 0x080B,         0x080C, 0x070D, 0x080D, 0x070E, 0x080E, 0x070F, 0x080F, 0x0710,         0x0711, 0x0811, 0x0712, 0x0812,
    0x0713, 0x0813, 0x0714, 0x0814, 0x0715, 0x0815, 0x0716, 0x0816, 0x0717, 0x0817, 0x0718, 0x0818, 0x0719, 0x0819,         0x081A,
    0x071B, 0x081B, 0x071C, 0x081C, 0x071D, 0x081D, 0x071E, 0x081E, 0x071F, 0x081F, 0x0720, 0x0820, 0x004B, 0x035D, 0x031C, 0x0361,
    0x002D, 0x050B, 0x044F, 0x0359, 0x05E0, 0x020D, 0x011E, 0x0286, 0x04E2, 0x04D6, 0x01DD, 0x04DA, 0x00FC, 0x01A9, 0x0185, 0x04DE,
    0x0102, 0x0189, 0x0117, 0x018D, 0x0276, 0x01FD, 0x00DB, 0x017D, 0x00EA, 0x0181, 0x0157, 0x01F9, 0x0328, 0x0560, 0x0129, 0x022D,
    0x0130, 0x03AC, 0x0123, 0x0365, 0x00B1, 0x0033, 0x0138, 0x025A, 0x0171, 0x025E, 0x00E4, 0x0195, 0x013D, 0x0191, 0x014D, 0x01B9,
    0x0246, 0x01C1, 0x0147, 0x01BD, 0x0108, 0x019D, 0x0225, 0x01A1, 0x0219, 0x027E, 0x0554, 0x00BB, 0x0282, 0x0600, 0x04F6, 0x0604,
    0x01F1, 0x0568, 0x05F4, 0x040F, 0x0252, 0x040B, 0x00C5, 0x0301, 0x0407, 0x000C, 0x024E, 0x0305, 0x0175, 0x0417, 0x0423, 0x008D,
    0x02F5, 0x0413, 0x02B2, 0x0457, 0x047A, 0x010E, 0x0608, 0x0564, 0x060C, 0x0610, 0x0580 
};

static bool midoGrottoInit = false;
static SkelAnime midoSkelAnime;
static Vec3s midoJointTable[17];
static Vec3s midoMorphTable[17];
int FredsQuestWoodCollected = 0;
int FredsQuestWoodOnHand = 0;
static int lastDisplayedCount = -1;
static bool FredsQuestComplete = false;
static SkelAnime collectionPointSkelAnime;
static Vec3s collectionPointJointTable[17];
static Vec3s collectionPointMorphTable[17];
static std::string collectionPointNametag;


static void RandomGrotto_WaitOpen(DoorAna* doorAna, PlayState* play) {
    if (!midoGrottoInit) {
        midoGrottoInit = true;
        SkelAnime_InitFlex(play, &midoSkelAnime, (FlexSkeletonHeader*)&gMidoSkel, (AnimationHeader*)&gMidoWalkingAnim, midoJointTable, midoMorphTable, 17);
    }
    SkelAnime_Update(&midoSkelAnime);

    Actor* actor = &doorAna->actor;
    Player* player = GET_PLAYER(play);
    if (!Player_InCsMode(play)) {
        Math_SmoothStepToF(&actor->world.pos.x, player->actor.world.pos.x, 0.1f, 10.0f, 0.0f);
        Math_SmoothStepToF(&actor->world.pos.z, player->actor.world.pos.z, 0.1f, 10.0f, 0.0f);
        Math_SmoothStepToF(&actor->world.pos.y, player->actor.world.pos.y, 0.1f, 10.0f, 0.0f);
    }

    Math_ApproachS(&doorAna->actor.shape.rot.y, doorAna->actor.yawTowardsPlayer, 5, 0xBB8);

    if (Math_StepToF(&actor->scale.x, 0.01f, 0.001f)) {
        if ((actor->targetMode != 0) && (play->transitionTrigger == TRANS_TRIGGER_OFF) && (player->stateFlags1 & PLAYER_STATE1_FLOOR_DISABLED) && (player->av1.actionVar1 == 0)) {
            play->nextEntranceIndex = RandomElement(entrances);
            DoorAna_SetupAction((DoorAna*)actor, DoorAna_GrabPlayer);
        } else {
            if (!Player_InCsMode(play) && !(player->stateFlags1 & (PLAYER_STATE1_ON_HORSE | PLAYER_STATE1_IN_WATER)) &&
                actor->xzDistToPlayer <= 15.0f && -50.0f <= actor->yDistToPlayer &&
                actor->yDistToPlayer <= 15.0f) {
                player->stateFlags1 |= PLAYER_STATE1_FLOOR_DISABLED;
                actor->targetMode = 1;
            } else {
                actor->targetMode = 0;
            }
        }
    }
    Actor_SetScale(actor, actor->scale.x);
}

static void RandomGrotto_Draw(Actor* actor, PlayState* play) {
    if (!midoGrottoInit) {
        return;
    }
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGrottoDL);

    Matrix_Translate(0.0f, -2700.0f, 0.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPSegment(POLY_OPA_DISP++, 0x08, (uintptr_t)gMidoEyeOpenTex);
    func_80034BA0(play, &midoSkelAnime, NULL, NULL, actor, 255);

    CLOSE_DISPS(play->state.gfxCtx);
}

static Vec3f FindValidPos(f32 distance) {
    Vec3f pos;
    pos.y = 9999.0f;
    while (true) {
        if (GET_PLAYER(gPlayState) != nullptr) {
            pos.x = GET_PLAYER(gPlayState)->actor.world.pos.x;
            pos.z = GET_PLAYER(gPlayState)->actor.world.pos.z;
        } else {
            pos.x = 0;
            pos.z = 0;
        }
        pos.x += (float)(Random(0, distance)) - distance / 2;
        pos.z += (float)(Random(0, distance)) - distance / 2;

        raycastResult = BgCheck_AnyRaycastFloor1(&gPlayState->colCtx, &snowballPoly, &pos);

        if (raycastResult > BGCHECK_Y_MIN) {
            pos.y = raycastResult;
            return pos;
        }
    }
}

// TODO: If in hyrule field and treeChopper is on, teleport somewhere else in hyrule field
static void SpawnRandomGrotto() {
    if (
        gPlayState->sceneNum == SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY || 
        gPlayState->sceneNum == SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT || 
        gPlayState->sceneNum == SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS
    ) {
        return;
    }

    Vec3f pos = FindValidPos(2000.0f);
    Actor* grotto = Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_ANA, pos.x, pos.y, pos.z, 0, 0, 0, 0, false);
    midoGrottoInit = false;
    DoorAna_SetupAction((DoorAna*)grotto, RandomGrotto_WaitOpen);
    grotto->draw = RandomGrotto_Draw;
}

void SpawnStick(Vec3f pos) {
    CustomCollectible::Spawn(pos.x, pos.y + 150.0f, pos.z, 0, CustomCollectible::KILL_ON_TOUCH | CustomCollectible::TOSS_ON_SPAWN, 0, [](Actor* actor, PlayState* play) {
        FredsQuestWoodOnHand++;
        Audio_PlaySoundGeneral(NA_SE_SY_METRONOME, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }, [](Actor* actor, PlayState* play) {
        Matrix_Scale(40.0f, 40.0f, 40.0f, MTXMODE_APPLY);
        for (int i = 4; i < 7; i++) {
            Matrix_RotateZYX(800 * i, 0, 800 * i, MTXMODE_APPLY);
            GetItem_Draw(play, GID_STICK);
        }
    });
}

Actor* specialTree = nullptr;

void ChooseSpecialTree() {
    Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_PROP].head;
    std::vector<Actor*> trees;

    specialTree = nullptr;

    while (actor != NULL) {
        if (ACTOR_EN_WOOD02 == actor->id && actor->params < 10) {
            trees.push_back(actor);
        }
        actor = actor->next;
    }

    if (trees.size() <= 1) {
        return;
    }

    specialTree = trees[Random(0, trees.size() - 1)];
}

extern "C" bool HandleTreeBonk(Actor* actor) {
    if (!CVarGetInteger(CVAR("FredsQuest.Enabled"), 0)) {
        return false; 
    }

    int damage = 2;
    // random chance of doing a crit
    if (Random(0, 100) < 30) {
        damage = 4;
    }

    if (actor->colChkInfo.health - damage <= 0) {
        if (specialTree == actor) {
            ChooseSpecialTree();

            for (int i = 0; i < CVarGetInteger(CVAR("FredsQuest.SpecialBreakDropRate"), 10); i++) {
                SpawnStick(actor->world.pos);
            }
        } else {
            for (int i = 0; i < CVarGetInteger(CVAR("FredsQuest.TreeBreakDropRate"), 3); i++) {
                SpawnStick(actor->world.pos);
            }
        }

        // Move tree (instead of killing and spawning another)
        actor->colChkInfo.health = 8;
        Vec3f pos = FindValidPos(5000.0f);
        actor->world.pos.x = pos.x;
        actor->world.pos.y = pos.y;
        actor->world.pos.z = pos.z;
    } else {
        actor->colChkInfo.health -= damage;
        for (int i = 0; i < CVarGetInteger(CVAR("FredsQuest.TreeBonkDropRate"), 1); i++) {
            SpawnStick(actor->world.pos);
        }
    }

    return true;
}

void DrawCrazyTaxiArrow(Actor* actor, PlayState* play) {
    if (specialTree == nullptr || !CVarGetInteger(CVAR("FredsQuest.CrazyTaxiArrow"), 0)) {
        return;
    }

    s16 yaw = Actor_WorldYawTowardActor(actor, specialTree);
    Math_ApproachS(&actor->shape.rot.y, yaw, 5, 10000);

    OPEN_DISPS(gPlayState->state.gfxCtx);

    Gfx_SetupDL_4Xlu(gPlayState->state.gfxCtx);

    Matrix_Scale(50.0f, 50.0f, 50.0f, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 70.0f, 0.0f, MTXMODE_APPLY);
    Matrix_RotateY(5.86f, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_MODELVIEW | G_MTX_LOAD);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 255, 0, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 0, 255);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gDebugArrowDL);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void SpawnCrazyTaxiArrow() {
    EnItem00* arrow = CustomCollectible::Spawn(0, 0, 0, 0, CustomCollectible::KEEP_ON_PLAYER, 0, NULL, NULL);
    arrow->actor.draw = DrawCrazyTaxiArrow;
}

void CollectionPoint_Update(Actor* actor, PlayState* play) {
    EnItem00* enItem00 = (EnItem00*)actor;

    SkelAnime_Update(&collectionPointSkelAnime);

    if (FredsQuestComplete) {
        return;
    }

    if (lastDisplayedCount != FredsQuestWoodCollected) {
        lastDisplayedCount = FredsQuestWoodCollected;
        collectionPointNametag = "Bring me wood!";
        if (FredsQuestWoodCollected > 0) {
            collectionPointNametag += std::string(" (") + std::to_string(FredsQuestWoodCollected) + "/" + std::to_string(CVarGetInteger(CVAR("FredsQuest.WoodNeeded"), 300)) + ")";
        }
        NameTag_RemoveAllForActor(actor);
        NameTag_RegisterForActorWithOptions(actor, collectionPointNametag.c_str(), { .yOffset = 100 });
    }

    if ((actor->xzDistToPlayer <= 200.0f) && (fabsf(actor->yDistToPlayer) <= fabsf(50.0f))) {
        if (FredsQuestWoodOnHand) {
            FredsQuestWoodCollected++;
            FredsQuestWoodOnHand--;
            Audio_PlaySoundGeneral(NA_SE_SY_METRONOME, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

            if (FredsQuestWoodCollected >= CVarGetInteger(CVAR("FredsQuest.WoodNeeded"), 300)) {
                FredsQuestComplete = true;
                collectionPointNametag = "You're a hero!";
                NameTag_RemoveAllForActor(actor);
                NameTag_RegisterForActorWithOptions(actor, collectionPointNametag.c_str(), { .yOffset = 100 });

                if (IS_RANDO && Rando::Context::GetInstance()->GetOption(RSK_TRIFORCE_HUNT)) {
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_TRIFORCE_PIECE).GetGIEntry_Copy();
                } else {
                    vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_HEART_CONTAINER).GetGIEntry_Copy();
                }

            }
        }
    }
}

void CollectionPoint_Draw(Actor* actor, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    SkelAnime_DrawSkeletonOpa(play, &collectionPointSkelAnime, NULL, NULL, actor);

    // For every 2% of the goal, draw a stick at a different angle, building a tree
    Matrix_Scale(40.0f, 40.0f, 40.0f, MTXMODE_APPLY);
    Matrix_Translate(0, 0, -300.0f, MTXMODE_APPLY);
    for (int i = 0; i < FredsQuestWoodCollected / (CVarGetInteger(CVAR("FredsQuest.WoodNeeded"), 300) / 50); i++) {
        float angle = 10 * i;
        float radius = (50 - i) * 0.5f; // Radius decreases as it goes up
        float height = 10.0f;    // Incremental height

        Matrix_Translate(radius * cosf(angle), height, radius * sinf(angle), MTXMODE_APPLY);
        Matrix_RotateY(angle, MTXMODE_APPLY);
        GetItem_Draw(play, GID_STICK);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void SpawnCollectionPoint() {
    EnItem00* collectionPoint = CustomCollectible::Spawn(859.0f, 347.0f, 5185.0f, 0xB000, 0, 0, NULL, NULL);
    collectionPoint->actor.update = CollectionPoint_Update;
    collectionPoint->actor.draw = CollectionPoint_Draw;
    collectionPoint->actor.flags |= ACTOR_FLAG_DRAW_WHILE_CULLED;
    SkelAnime_InitFlex(gPlayState, &collectionPointSkelAnime, (FlexSkeletonHeader*)&object_toryo_Skel_007150, 
        (AnimationHeader*)&object_toryo_Anim_000E50, collectionPointJointTable, collectionPointMorphTable, 17);
}

void RandomTrap_Update(Actor* actor, PlayState* play) {
    EnItem00* enItem00 = (EnItem00*)actor;

    enItem00->unk_158--;
    if (enItem00->unk_158 == 0) {
        Actor_Kill(actor);
        return;
    }

    Math_ApproachS(&actor->world.rot.y, actor->yawTowardsPlayer, 5, 0xBB8);
    actor->speedXZ = 3.0f;

    // TODO: CVar for speed
    // Multiply speed by distance
    actor->speedXZ += actor->xzDistToPlayer * 0.01f;
    if (actor->xzDistToPlayer > 1000.0f && actor->velocity.y < -3.0f) {
        actor->velocity.y += ABS(actor->yDistToPlayer) * 0.01f;
    }

    actor->shape.rot.y += 0x1000;

    if ((actor->xzDistToPlayer <= 50.0f) && (fabsf(actor->yDistToPlayer) <= fabsf(20.0f))) {
        // TODO: Random crowd control effect
        GameInteractor::RawAction::KnockbackPlayer(5.0f);
        Actor_Kill(actor);
    }

    if (actor->gravity != 0.0f) {
        Actor_MoveXZGravity(actor);
        Actor_UpdateBgCheckInfo(play, actor, 20.0f, 15.0f, 15.0f, 0x1D);
    }

    if (actor->bgCheckFlags & 0x0003) {
        actor->speedXZ = 0.0f;
    }
}

void RandomTrap_Draw(Actor* actor, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Scale(4.0f, 4.0f, 4.0f, MTXMODE_APPLY);
    Matrix_Translate(0, -200.0f, 0, MTXMODE_APPLY);
    func_8002EBCC(actor, play, 1);
    Gfx_DrawDListOpa(play, (Gfx*)gSlidingBladeTrapDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void SpawnRandomTrap() {
    Vec3f pos = FindValidPos(2000.0f);
    EnItem00* randomTrap = CustomCollectible::Spawn(pos.x, pos.y, pos.z, 0, CustomCollectible::TOSS_ON_SPAWN, 0, NULL, NULL);
    SoundSource_PlaySfxAtFixedWorldPos(gPlayState, &randomTrap->actor.world.pos, 20, NA_SE_EV_LIGHTNING);
    randomTrap->actor.update = RandomTrap_Update;
    randomTrap->actor.draw = RandomTrap_Draw;
    randomTrap->unk_158 = 20 * CVarGetInteger(CVAR("RandomTraps.Lifetime"), 30);
}

void OnSceneInit() {
    // Reset wood collected
    FredsQuestWoodCollected = 0;
    FredsQuestWoodOnHand = 0;
    lastDisplayedCount = -1;
    FredsQuestComplete = false;

    if (gPlayState->sceneNum != SCENE_HYRULE_FIELD) {
        return;
    }

    ChooseSpecialTree();
    SpawnCrazyTaxiArrow();
    SpawnCollectionPoint();
}

static void ConfigurationChanged() {
    COND_HOOK(OnSceneSpawnActors, CVarGetInteger(CVAR("FredsQuest.Enabled"), 0), OnSceneInit);

    COND_HOOK(OnPlayerUpdate, CVarGetInteger(CVAR("RandomTraps.Enabled"), 0), []() {
        if (rand() % CVarGetInteger(CVAR("RandomTraps.SpawnChance"), 400) == 0) { 
            SpawnRandomTrap();
        }
    });

    COND_HOOK(OnPlayerUpdate, CVarGetInteger(CVAR("FredsQuest.Enabled"), 0), []() {
        if (CVarGetInteger(CVAR("FredsQuest.EncumberedThreshold"), 60) == 0 || FredsQuestWoodOnHand <= CVarGetInteger(CVAR("FredsQuest.EncumberedThreshold"), 60)) {
            GameInteractor::State::RunSpeedModifier = 0;
        } else {
            GameInteractor::State::RunSpeedModifier = -2;
        }
    });

    COND_VB_SHOULD(VB_PLAYER_ROLL, CVarGetInteger(CVAR("FredsQuest.Enabled"), 0), {
        if (FredsQuestWoodOnHand > CVarGetInteger(CVAR("FredsQuest.EncumberedThreshold"), 0)) {
            *should = false;
        }
    });
}

static void DrawMenu() {
    ImGui::SeparatorText(AUTHOR);

    // UIWidgets::EnhancementSliderFloat("Xfloat", "Xfloat", CVAR("tmpxf"), 0.0f, 10.0f, "%.2f", 1.0f, false);
    // UIWidgets::EnhancementSliderFloat("Yfloat", "Yfloat", CVAR("tmpyf"), 0.0f, 10.0f, "%.2f", 1.0f, false);
    // UIWidgets::EnhancementSliderFloat("Zfloat", "Zfloat", CVAR("tmpzf"), 0.0f, 10.0f, "%.2f", 1.0f, false);
    // UIWidgets::EnhancementSliderInt("Xs", "Xs", CVAR("tmpxs"), 0, UINT16_MAX, "%d", 1, false);
    // UIWidgets::EnhancementSliderInt("Ys", "Ys", CVAR("tmpys"), 0, UINT16_MAX, "%d", 1, false);
    // UIWidgets::EnhancementSliderInt("Zs", "Zs", CVAR("tmpzs"), 0, UINT16_MAX, "%d", 1, false);
    if (UIWidgets::EnhancementCheckbox("Fred's Quest", CVAR("FredsQuest.Enabled"))) {
        ConfigurationChanged();
    }
    UIWidgets::Tooltip("Collect wood and bring it to the collection point in Hyrule Field for a small reward.");
    if (CVarGetInteger(CVAR("FredsQuest.Enabled"), 0)) {
        if (UIWidgets::EnhancementCheckbox("Crazy Taxi Arrow", CVAR("FredsQuest.CrazyTaxiArrow"))) {
            ConfigurationChanged();
        }
        if (UIWidgets::EnhancementSliderInt("Wood Needed", "##FredsQuest.WoodNeeded", CVAR("FredsQuest.WoodNeeded"), 0, 1000, "%d", 300, false)) {
            ConfigurationChanged();
        }
        if (UIWidgets::EnhancementSliderInt("Tree Bonk Drop Rate", "##FredsQuest.TreeBonkDropRate", CVAR("FredsQuest.TreeBonkDropRate"), 0, 10, "%d", 1, false)) {
            ConfigurationChanged();
        }
        if (UIWidgets::EnhancementSliderInt("Tree Break Drop Rate", "##FredsQuest.TreeBreakDropRate", CVAR("FredsQuest.TreeBreakDropRate"), 0, 50, "%d", 3, false)) {
            ConfigurationChanged();
        }
        if (UIWidgets::EnhancementSliderInt("Special Break Drop Rate", "##FredsQuest.SpecialBreakDropRate", CVAR("FredsQuest.SpecialBreakDropRate"), 0, 50, "%d", 10, false)) {
            ConfigurationChanged();
        }
        if (UIWidgets::EnhancementSliderInt("Encumbered Threshold", "##FredsQuest.EncumberedThreshold", CVAR("FredsQuest.EncumberedThreshold"), 0, 200, "%d", 60, false)) {
            ConfigurationChanged();
        }
        UIWidgets::Tooltip("If you have more than this many sticks, you will be encumbered and run slower. 0 for disabled");
    }
    if (UIWidgets::EnhancementCheckbox("Random Traps", CVAR("RandomTraps.Enabled"))) {
        ConfigurationChanged();
    }
    UIWidgets::Tooltip("Random traps will spawn around you at a configurable rate. (Currently only knockback)");
    if (CVarGetInteger(CVAR("RandomTraps.Enabled"), 0)) {
        if (UIWidgets::EnhancementSliderInt("Trap Lifetime (Seconds)", "##RandomTraps.Lifetime", CVAR("RandomTraps.Lifetime"), 0, 60, "%d", 30, false)) {
            ConfigurationChanged();
        }
        if (UIWidgets::EnhancementSliderInt("Spawn Chance", "##RandomTraps.SpawnChance", CVAR("RandomTraps.SpawnChance"), 40, 2000, "%d", 1000, false)) {
            ConfigurationChanged();
        }
    }
}

static void RegisterMod() {
    // #region Leave this alone unless you know what you are doing
    ConfigurationChanged();
    // #endregion
}

static Holiday holiday(DrawMenu, RegisterMod);
