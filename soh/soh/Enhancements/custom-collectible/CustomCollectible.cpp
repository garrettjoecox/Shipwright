#include "CustomCollectible.h"
#include <libultraship/libultraship.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/3drando/random.hpp"
#include "soh/frame_interpolation.h"
#include "soh/Enhancements/custom-message/CustomMessageManager.h"

extern "C" {
#include "z64actor.h"
#include "functions.h"
#include "variables.h"
#include "macros.h"
#include "objects/object_md/object_md.h"
extern PlayState* gPlayState;
}

EnItem00* CustomCollectible::Spawn(f32 posX, f32 posY, f32 posZ, s16 rot, s16 flags, s16 params, ActorFunc actionFunc,
                            ActorFunc drawFunc) {
    if (!gPlayState) {
        return nullptr;
    }

    Actor* actor = Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_ITEM00, posX, posY, posZ, flags, rot, params, ITEM00_NONE, 0);
    EnItem00* enItem00 = (EnItem00*)actor;

    if (actionFunc != NULL) {
        enItem00->actionFunc = (EnItem00ActionFunc)actionFunc;
    }

    if (drawFunc != NULL) {
        actor->draw = drawFunc;
    }

    return enItem00;
}

void CustomCollectible_Init(Actor* actor, PlayState* play) {
    EnItem00* enItem00 = (EnItem00*)actor;

    if (CUSTOM_ITEM_FLAGS & CustomCollectible::STOP_BOBBING) {
        actor->shape.yOffset = 1250.0f;
    } else {
        actor->shape.yOffset = (Math_SinS(actor->shape.rot.y) * 150.0f) + 1250.0f;
    }

    if (CUSTOM_ITEM_FLAGS & CustomCollectible::HIDE_TILL_OVERHEAD) {
        Actor_SetScale(actor, 0.0f);
    } else {
        Actor_SetScale(actor, 0.015f);
    }

    if (CUSTOM_ITEM_FLAGS & CustomCollectible::KEEP_ON_PLAYER) {
        Math_Vec3f_Copy(&actor->world.pos, &GET_PLAYER(play)->actor.world.pos);
    }

    if (CUSTOM_ITEM_FLAGS & CustomCollectible::TOSS_ON_SPAWN) {
        actor->velocity.y = 8.0f;
        actor->speedXZ = 2.0f;
        actor->gravity = -1.4f;
        actor->world.rot.y = Rand_ZeroOne() * 40000.0f;
    }

    enItem00->unk_15A = -1;
}

// By default this will just assume the GID was passed in as the rot z, if you want different functionality you should
// override the draw
void CustomCollectible_Draw(Actor* actor, PlayState* play) {
    Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
    GetItem_Draw(play, CUSTOM_ITEM_PARAM);
}

void CustomCollectible_Update(Actor* actor, PlayState* play) {
    EnItem00* enItem00 = (EnItem00*)actor;
    Player* player = GET_PLAYER(play);

    if (!(CUSTOM_ITEM_FLAGS & CustomCollectible::STOP_SPINNING)) {
        actor->shape.rot.y += 960;
    }

    if (CUSTOM_ITEM_FLAGS & CustomCollectible::STOP_BOBBING) {
        actor->shape.yOffset = 1250.0f;
    } else {
        actor->shape.yOffset = (Math_SinS(actor->shape.rot.y) * 150.0f) + 1250.0f;
    }

    if (CUSTOM_ITEM_FLAGS & CustomCollectible::HIDE_TILL_OVERHEAD) {
        Actor_SetScale(actor, 0.0f);
    }

    if (CUSTOM_ITEM_FLAGS & CustomCollectible::KEEP_ON_PLAYER) {
        actor->gravity = 0.0f;
        Math_Vec3f_Copy(&actor->world.pos, &GET_PLAYER(play)->actor.world.pos);
    }

    if (CUSTOM_ITEM_FLAGS & CustomCollectible::KILL_ON_TOUCH) {
        // Pretty self explanatory, if the player is within range, kill the actor and call the action function
        if ((actor->xzDistToPlayer <= 50.0f) && (fabsf(actor->yDistToPlayer) <= fabsf(20.0f))) {
            if (enItem00->actionFunc != NULL) {
                enItem00->actionFunc(enItem00, play);
                CUSTOM_ITEM_FLAGS |= CustomCollectible::CALLED_ACTION;
            }
            Actor_Kill(actor);
        }
    } else if (CUSTOM_ITEM_FLAGS & CustomCollectible::GIVE_OVERHEAD) {
        // If the item hasn't been picked up (unk_15A == -1) and the player is within range
        if (enItem00->unk_15A == -1 && (actor->xzDistToPlayer <= 50.0f) &&
            (fabsf(actor->yDistToPlayer) <= fabsf(20.0f))) {
            // Fire the action function
            if (enItem00->actionFunc != NULL) {
                enItem00->actionFunc(enItem00, play);
                CUSTOM_ITEM_FLAGS |= CustomCollectible::CALLED_ACTION;
            }
            Sfx_PlaySfxCentered(NA_SE_SY_GET_ITEM);
            // Set the unk_15A to 15, this indicates the item has been picked up and will start the overhead animation
            enItem00->unk_15A = 15;
            CUSTOM_ITEM_FLAGS |= CustomCollectible::STOP_BOBBING;
            CUSTOM_ITEM_FLAGS |= CustomCollectible::KEEP_ON_PLAYER;
        }

        // If the item has been picked up
        if (enItem00->unk_15A > 0) {
            // Reduce the size a bit, but also makes it visible for HIDE_TILL_OVERHEAD
            Actor_SetScale(actor, 0.010f);

            // Decrement the unk_15A, which will be used to bob the item up and down
            enItem00->unk_15A--;

            // Account for the different heights of the player forms
            f32 height = 45.0f;
            // TODO: Check for adult?

            // Bob the item up and down
            actor->world.pos.y += (height + (Math_SinS(enItem00->unk_15A * 15000) * (enItem00->unk_15A * 0.3f)));
        }

        // Finally, once the bobbing animation is done, kill the actor
        if (enItem00->unk_15A == 0) {
            Actor_Kill(actor);
        }
    } else if (CUSTOM_ITEM_FLAGS & CustomCollectible::GIVE_ITEM_CUTSCENE) {
        // If the item hasn't been picked up and the player is within range
        if (!Actor_HasParent(actor, play) && enItem00->unk_15A == -1) {
            Actor_OfferGetItem(actor, play, GI_SHIP, 50.0f, 20.0f);
        } else {
            if (enItem00->unk_15A == -1) {
                CUSTOM_ITEM_FLAGS |= CustomCollectible::STOP_BOBBING;
                CUSTOM_ITEM_FLAGS |= CustomCollectible::KEEP_ON_PLAYER;
                CUSTOM_ITEM_FLAGS |= CustomCollectible::HIDE_TILL_OVERHEAD;
            }

            // Begin incrementing the unk_15A, indicating the item has been picked up
            enItem00->unk_15A++;

            // For the first 20 frames, wait while the player's animation plays
            if (enItem00->unk_15A >= 20) {
                // After the first 20 frames, show the item and call the action function
                if (enItem00->unk_15A == 20 && enItem00->actionFunc != NULL) {
                    enItem00->actionFunc(enItem00, play);
                    CUSTOM_ITEM_FLAGS |= CustomCollectible::CALLED_ACTION;
                }
                // Override the bobbing animation to be a fixed height
                actor->shape.yOffset = 900.0f;
                Actor_SetScale(actor, 0.007f);

                f32 height = 45.0f;
                // TODO: Check for adult?

                actor->world.pos.y += height;
            }

            // Once the player is no longer in the "Give Item" state, kill the actor
            if (!(player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM)) {
                Actor_Kill(actor);
            }
        }
    }

    if (actor->gravity != 0.0f) {
        Actor_MoveXZGravity(actor);
        Actor_UpdateBgCheckInfo(play, actor, 20.0f, 15.0f, 15.0f, 0x1D);
    }

    if (actor->bgCheckFlags & 0x0003) {
        actor->speedXZ = 0.0f;
    }

    Collider_UpdateCylinder(actor, &enItem00->collider);
    CollisionCheck_SetAC(play, &play->colChkCtx, &enItem00->collider.base);
}

void CustomCollectible::RegisterHooks() {
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::ShouldActorInit>(
        ACTOR_EN_ITEM00, [](void* actorRef, bool* should) {
            Actor* actor = (Actor*)actorRef;
            if (actor->params != ITEM00_NONE) {
                return;
            }

            actor->init = CustomCollectible_Init;
            actor->update = CustomCollectible_Update;
            actor->draw = CustomCollectible_Draw;
            actor->destroy = NULL;

            // Set the rotX/rotZ back to 0, the original values can be accessed from actor->home
            actor->world.rot.x = 0;
            actor->world.rot.z = 0;
        });
}
