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
#include "soh_assets.h"

#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_md/object_md.h"
#include "objects/object_trap/object_trap.h"
#include "objects/object_toryo/object_toryo.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"
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
static int midoSpeedChangeTimer = 0;
static Vec3f midoKickedPosition = {-9999.0f, -9999.0f, -9999.0f};
static int fireWallTick = 0;
static int moveMidoTimer = 0;

static Vec3f FindValidPos(f32 distance) {
    Vec3f pos;
    pos.y = 9999.0f;
    int attempts = 0;
    while (attempts < 100) {
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
        attempts++;
    }
    return pos;
}

static void Mido_Update(Actor* actor, PlayState* play) {
    SkelAnime_Update(&midoSkelAnime);

    if (FredsQuestComplete) {
        actor->speedXZ = 0;
        return;
    }

    Math_ApproachS(&actor->world.rot.y, actor->yawTowardsPlayer, 5, 0xBB8);
    actor->shape.rot.y = actor->world.rot.y;

    if (midoSpeedChangeTimer <= 0) {
        if (CVarGetInteger(CVAR("MadMido.MinSpeed"), 1) >= CVarGetInteger(CVAR("MadMido.MaxSpeed"), 10)) {
            actor->speedXZ = CVarGetInteger(CVAR("MadMido.MinSpeed"), 1);
        } else {
            actor->speedXZ = Random(CVarGetInteger(CVAR("MadMido.MinSpeed"), 1), CVarGetInteger(CVAR("MadMido.MaxSpeed"), 10));
        }
        midoSpeedChangeTimer = CVarGetInteger(CVAR("MadMido.SpeedChange"), 30) * 20;
    } else {
        midoSpeedChangeTimer--;
    }

    if (actor->xzDistToPlayer > 1000.0f && actor->velocity.y < -3.0f) {
        actor->velocity.y += ABS(actor->yDistToPlayer) * 0.01f;
    }

    if ((actor->xzDistToPlayer <= 50.0f) && (fabsf(actor->yDistToPlayer) <= fabsf(20.0f))) {
        GameInteractor::RawAction::KnockbackPlayer(1.0f);
        midoKickedPosition = FindValidPos(20000.0f);
        midoKickedPosition.y += 1000.0f;

        actor->speedXZ = 0.0f;
        midoSpeedChangeTimer = CVarGetInteger(CVAR("MadMido.SpeedChange"), 30) * 20;
    }

    if (actor->gravity != 0.0f) {
        Actor_MoveXZGravity(actor);
        Actor_UpdateBgCheckInfo(play, actor, 20.0f, 15.0f, 15.0f, 0x1D);
    }

    if (moveMidoTimer <= 0) {
        Vec3f pos = FindValidPos(2000.0f);
        moveMidoTimer = 20 * 60;
        actor->world.pos.x = pos.x;
        actor->world.pos.y = pos.y;
        actor->world.pos.z = pos.z;
    } else {
        moveMidoTimer--;
    }

    // if (actor->bgCheckFlags & 0x0003) {
    //     actor->speedXZ = 0.0f;
    // }
}

static void Mido_Draw(Actor* actor, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Scale(2.0f, 2.0f, 2.0f, MTXMODE_APPLY);
    Matrix_Translate(0.0f, -200.0f, 0.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPSegment(POLY_OPA_DISP++, 0x08, (uintptr_t)gMidoEyeOpenTex);
    func_80034BA0(play, &midoSkelAnime, NULL, NULL, actor, 255);

    CLOSE_DISPS(play->state.gfxCtx);
}

static void SpawnMido() {
    Vec3f pos = FindValidPos(2000.0f);
    moveMidoTimer = 20 * 60;
    EnItem00* mido = CustomCollectible::Spawn(pos.x, pos.y, pos.z, 0, 0, 0, NULL, NULL);
    SkelAnime_InitFlex(gPlayState, &midoSkelAnime, (FlexSkeletonHeader*)&gMidoSkel, (AnimationHeader*)&gMidoWalkingAnim, midoJointTable, midoMorphTable, 17);
    mido->actor.update = Mido_Update;
    mido->actor.draw = Mido_Draw;
    mido->actor.gravity = -0.9f;
    mido->actor.flags |= ACTOR_FLAG_DRAW_WHILE_CULLED & ACTOR_FLAG_UPDATE_WHILE_CULLED;
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
            actor->flags |= ACTOR_FLAG_DRAW_WHILE_CULLED & ACTOR_FLAG_UPDATE_WHILE_CULLED;
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
    if (!CVarGetInteger(CVAR("FredsQuest.Enabled"), 0) || gPlayState->sceneNum != SCENE_HYRULE_FIELD) {
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
        Vec3f pos = FindValidPos(10000.0f);
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
    Vec3f flamePos;
    actor->flags |= ACTOR_FLAG_DRAW_WHILE_CULLED & ACTOR_FLAG_UPDATE_WHILE_CULLED;
    fireWallTick = (fireWallTick + 1) % 8;

    SkelAnime_Update(&collectionPointSkelAnime);

    if (FredsQuestComplete) {
        for (int i = 0; i < 2; i++) {
            flamePos.x = Rand_CenteredFloat(400.0f) + actor->world.pos.x + 200.0f;
            flamePos.y = Rand_CenteredFloat(1000.0f) + actor->world.pos.y;
            flamePos.z = Rand_CenteredFloat(400.0f) + actor->world.pos.z + 100.0f;
            EffectSsEnFire_SpawnVec3f(play, actor, &flamePos, 300, 0, 0, -1);
        }
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

const char* effigyDls[12] = {
    gEffigy7DL,
    gEffigy8DL,
    gEffigy9DL,
    gEffigy10DL,
    gEffigy11DL,
    gEffigy1DL,
    gEffigy2DL,
    gEffigy3DL,
    gEffigy4DL,
    gEffigy6DL,
    gEffigy12DL,
    gEffigy5DL,
};

static const char* sFireballTexs[] = {
    gFireTempleFireball0Tex, gFireTempleFireball1Tex, gFireTempleFireball2Tex, gFireTempleFireball3Tex,
    gFireTempleFireball4Tex, gFireTempleFireball5Tex, gFireTempleFireball6Tex, gFireTempleFireball7Tex,
};

void CollectionPoint_Draw(Actor* actor, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    if (FredsQuestComplete) {
        Matrix_Push();
        Gfx_SetupDL(POLY_XLU_DISP, 0x14);
        gSPSegment(POLY_XLU_DISP++, 0x08, (uintptr_t)sFireballTexs[fireWallTick]);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x01, 255, 255, 0, 150);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 255);
        Matrix_Scale(16.0f, 16.0f, 16.0f, MTXMODE_APPLY);
        Matrix_Translate(0, 0.0f, -500.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_MODELVIEW | G_MTX_LOAD);
        gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gFireTempleFireballUpperHalfDL);
        Matrix_Pop();
    }

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    SkelAnime_DrawSkeletonOpa(play, &collectionPointSkelAnime, NULL, NULL, actor);

    int woodCollected = FredsQuestWoodCollected;
    int woodNeeded = CVarGetInteger(CVAR("FredsQuest.WoodNeeded"), 300);

    Matrix_Scale(8.0f, 8.0f, 8.0f, MTXMODE_APPLY);
    Matrix_Translate(0, 0.0f, -2000.0f, MTXMODE_APPLY);
    // Render effigy based on percentage of wood collected
    for (int i = 0; i < ((float)woodCollected / woodNeeded * 12); i++) {
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_MODELVIEW | G_MTX_LOAD);
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)effigyDls[i]);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void SpawnCollectionPoint() {
    EnItem00* collectionPoint = CustomCollectible::Spawn(859.0f, 347.0f, 5185.0f, 0xB000, 0, 0, NULL, NULL);
    collectionPoint->actor.update = CollectionPoint_Update;
    collectionPoint->actor.draw = CollectionPoint_Draw;
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
    CVarClear(CVAR("FredsQuest.SpeedModifier"));

    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_PO_FIELD, CVarGetInteger(CVAR("FredsQuest.Enabled"), 0), [](void* actor, bool* should) {
        *should = false;
    });

    COND_HOOK(OnPlayerUpdate, CVarGetInteger(CVAR("MadMido.Enabled"), 0) && gPlayState->sceneNum == SCENE_HYRULE_FIELD, []() {
        Player* player = GET_PLAYER(gPlayState);
        if (midoKickedPosition.y != -9999.0f) {
            Math_ApproachF(&player->actor.world.pos.x, midoKickedPosition.x, 1.0f, 100.0f);
            Math_ApproachF(&player->actor.world.pos.y, midoKickedPosition.y, 1.0f, 100.0f);
            Math_ApproachF(&player->actor.world.pos.z, midoKickedPosition.z, 1.0f, 100.0f);
            float distancexyz = Math3D_Vec3f_DistXYZ(&player->actor.world.pos, &midoKickedPosition);
            if (distancexyz < 100.0f) {
                midoKickedPosition.y = -9999.0f;
            }
        }
    });

    COND_HOOK(OnPlayerUpdate, CVarGetInteger(CVAR("RandomTraps.Enabled"), 0), []() {
        if (rand() % CVarGetInteger(CVAR("RandomTraps.SpawnChance"), 400) == 0) { 
            SpawnRandomTrap();
        }
    });

    COND_HOOK(OnPlayerUpdate, CVarGetInteger(CVAR("FredsQuest.Enabled"), 0) && gPlayState->sceneNum == SCENE_HYRULE_FIELD, []() {
        if (CVarGetInteger(CVAR("FredsQuest.EncumberedThreshold"), 60) != 0 && FredsQuestWoodOnHand >= CVarGetInteger(CVAR("FredsQuest.EncumberedThreshold"), 60)) {
            // The further past the threshold, the slower you go
            CVarSetFloat(CVAR("FredsQuest.SpeedModifier"), MAX(1.0f - (float)(FredsQuestWoodOnHand - CVarGetInteger(CVAR("FredsQuest.EncumberedThreshold"), 60)) / 100.0f, 0.7f));
        } else {
            CVarClear(CVAR("FredsQuest.SpeedModifier"));
        }
    });

    if (gPlayState->sceneNum != SCENE_HYRULE_FIELD) {
        return;
    }

    ChooseSpecialTree();
    SpawnCrazyTaxiArrow();
    SpawnCollectionPoint();
    if (CVarGetInteger(CVAR("MadMido.Enabled"), 0)) {
        SpawnMido();
    }
}

static void ConfigurationChanged() {
    CVarClear(CVAR("FredsQuest.SpeedModifier"));
    COND_HOOK(OnSceneSpawnActors, CVarGetInteger(CVAR("FredsQuest.Enabled"), 0), OnSceneInit);
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
        if (UIWidgets::EnhancementSliderInt("Wood Needed", "##FredsQuest.WoodNeeded", CVAR("FredsQuest.WoodNeeded"), 50, 1000, "%d", 300, false)) {
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
    if (UIWidgets::EnhancementCheckbox("Mad Mido", CVAR("MadMido.Enabled"))) {
        ConfigurationChanged();
    }
    UIWidgets::Tooltip("Mido will chase you around and kick you if you get too close.");
    if (CVarGetInteger(CVAR("MadMido.Enabled"), 0)) {
        if (UIWidgets::EnhancementSliderInt("Speed Change (Seconds)", "##MadMido.SpeedChange", CVAR("MadMido.SpeedChange"), 3, 500, "%d", 30, false)) {
            ConfigurationChanged();
        }
        UIWidgets::Tooltip("How often Mido will change speed.");
        if (UIWidgets::EnhancementSliderInt("Minimum Speed", "##MadMido.MinSpeed", CVAR("MadMido.MinSpeed"), 1, 10, "%d", 1, false)) {
            ConfigurationChanged();
        }
        if (UIWidgets::EnhancementSliderInt("Maximum Speed", "##MadMido.MaxSpeed", CVAR("MadMido.MaxSpeed"), 1, 30, "%d", 10, false)) {
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
