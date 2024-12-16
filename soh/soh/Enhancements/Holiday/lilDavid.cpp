#include "Holiday.hpp"

#include "utils/StringHelper.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;
}

#include "src/overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "src/overlays/actors/ovl_En_Bom/z_en_bom.h"

extern "C" {
    void func_809B45E0(EnArrow*, PlayState*);
    void func_809B4640(EnArrow*, PlayState*);
}

#define AUTHOR "lilDavid"
#define CVAR(v) "gHoliday." AUTHOR "." v

static void OnConfigurationChanged() {
    if (!CVarGetInteger(CVAR("BombArrows.Enabled"), 0))
        CVarSetInteger(CVAR("BombArrows.Active"), 0);

    COND_HOOK(OnSaveFile, CVarGetInteger(CVAR("BombArrows.Enabled"), 0), [](int32_t file, int sectionID) {
        std::string cvar = StringHelper::Sprintf("%s%d", CVAR("BombArrows.Save"), file);
        CVarSetInteger(cvar.c_str(), CVarGetInteger(CVAR("BombArrows.Active"), 0));
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    });

    COND_HOOK(OnLoadFile, CVarGetInteger(CVAR("BombArrows.Enabled"), 0), [](int32_t file) {
        std::string cvar = StringHelper::Sprintf("%s%d", CVAR("BombArrows.Save"), file);
        CVarSetInteger(CVAR("BombArrows.Active"), CVarGetInteger(cvar.c_str(), 0));
    });

    COND_HOOK(OnCopyFile, CVarGetInteger(CVAR("BombArrows.Enabled"), 0), [](int32_t from, int32_t to) {
        std::string cvarFrom = StringHelper::Sprintf("%s%d", CVAR("BombArrows.Save"), from);
        std::string cvarTo = StringHelper::Sprintf("%s%d", CVAR("BombArrows.Save"), to);
        CVarSetInteger(cvarTo.c_str(), CVarGetInteger(cvarFrom.c_str(), 0));
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    });

    COND_HOOK(OnDeleteFile, CVarGetInteger(CVAR("BombArrows.Enabled"), 0), [](int32_t file) {
        std::string cvar = StringHelper::Sprintf("%s%d", CVAR("BombArrows.Save"), file);
        CVarSetInteger(cvar.c_str(), 0);
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    });

    COND_ID_HOOK(OnActorInit, ACTOR_EN_ARROW, CVarGetInteger(CVAR("BombArrows.Enabled"), 0), [](void* actorRef) {
        EnArrow* arrow = (EnArrow*) actorRef;
        if (!CVarGetInteger(CVAR("BombArrows.Active"), 0) ||
            arrow->actor.params != ARROW_NORMAL || AMMO(ITEM_BOMB) == 0 ||
            gSaveContext.minigameState == 1 || gPlayState->shootingGalleryStatus > 1)
            return;

        EnBom* bomb = (EnBom*) Actor_SpawnAsChild(&gPlayState->actorCtx, &arrow->actor, gPlayState, ACTOR_EN_BOM,
                arrow->actor.world.pos.x, arrow->actor.world.pos.y, arrow->actor.world.pos.z,
                0, 0, 0, BOMB_BODY);
        if (bomb == nullptr)
            return;

        Actor_SetScale(&bomb->actor, 0.003f);
        bomb->timer = 65;
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_ARROW, CVarGetInteger(CVAR("BombArrows.Enabled"), 0), [](void* actorRef) {
        EnArrow* arrow = (EnArrow*) actorRef;
        if (!arrow->actor.child || arrow->actor.child->id != ACTOR_EN_BOM)
            return;

        EnBom* bomb = (EnBom*) arrow->actor.child;
        bomb->actor.world.pos = arrow->actor.world.pos;
        f32 r = 8.0f;
        f32 xrot = arrow->actor.world.rot.x;
        f32 yrot = arrow->actor.world.rot.y;
        bomb->actor.world.pos.x += r * Math_CosS(xrot) * Math_SinS(yrot);
        bomb->actor.world.pos.y -= r * Math_SinS(xrot) + 2.0f;
        bomb->actor.world.pos.z += r * Math_CosS(xrot) * Math_CosS(yrot);

        if (arrow->actor.parent == nullptr) {
            if (bomb->timer > 60) {
                Inventory_ChangeAmmo(ITEM_BOMB, -1);
            }
            bomb->timer = 52;
        } else {
            bomb->timer = 62;
        }

        if (arrow->actionFunc == func_809B45E0 ||
            arrow->actionFunc == func_809B4640 ||
            arrow->actor.params == ARROW_NORMAL_LIT)
        {
            arrow->actor.child = nullptr;
            bomb->actor.parent = nullptr;
            bomb->timer = 2;
            Actor_Kill(&arrow->actor);
        }
    });

    COND_ID_HOOK(OnActorKill, ACTOR_EN_ARROW, CVarGetInteger(CVAR("BombArrows.Enabled"), 0), [](void* actorRef) {
        EnArrow* arrow = (EnArrow*) actorRef;
        if (!arrow->actor.child || arrow->actor.child->id != ACTOR_EN_BOM)
            return;
        Actor_Kill(arrow->actor.child);
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_BOM, CVarGetInteger(CVAR("BombArrows.Enabled"), 0), [](void* actorRef) {
        EnBom* bomb = (EnBom*) actorRef;
        if (!bomb->actor.parent || bomb->actor.parent->id != ACTOR_EN_ARROW)
            return;

        if (bomb->timer > 55 && bomb->timer < 60)
            bomb->timer += 4;
        if (bomb->timer > 45 && bomb->timer < 50)
            bomb->timer += 4;
    });
}

static void DrawMenu() {
    ImGui::SeparatorText(AUTHOR);
    if (UIWidgets::EnhancementCheckbox("Bomb Arrows", CVAR("BombArrows.Enabled"))) {
        OnConfigurationChanged();
    }
    UIWidgets::Tooltip("Equip bombs over an already equipped Bow to shoot bomb arrows");
}

static void RegisterMod() {
    // #region Leave this alone unless you know what you are doing
    OnConfigurationChanged();
    // #endregion

    CVarSetInteger(CVAR("BombArrows.Active"), 0);
}

static Holiday holiday(DrawMenu, RegisterMod);
