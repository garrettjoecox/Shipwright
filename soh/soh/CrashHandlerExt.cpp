#include "CrashHandlerExp.h"
#include "variables.h"
#include "z64.h"
#include "z64actor.h"
#include <string.h>
#include <stdio.h>
#include <array>

#define WRITE_VAR_LINE(buff, len, varName, varValue) \
    append_str(buff, len, varName);                  \
    append_line(buff, len, varValue);
#define WRITE_VAR(buff, len, varName, varValue) \
    append_str(buff, len, varName);             \
    append_str(buff, len, varValue);

extern "C" PlayState* gPlayState;

static std::array<const char*, ACTORCAT_MAX> sCatToStrArray{
    "SWITCH", "BG", "PLAYER", "EXPLOSIVE", "NPC", "ENEMY", "PROP", "ITEMACTION", "MISC", "BOSS", "DOOR", "CHEST",
};

static std::array<const char*, SCENE_ID_MAX> sSceneIdToStrArray{
    "SCENE_DEKU_TREE",
    "SCENE_DODONGOS_CAVERN",
    "SCENE_JABU_JABU",
    "SCENE_FOREST_TEMPLE",
    "SCENE_FIRE_TEMPLE",
    "SCENE_WATER_TEMPLE",
    "SCENE_SPIRIT_TEMPLE",
    "SCENE_SHADOW_TEMPLE",
    "SCENE_BOTTOM_OF_THE_WELL",
    "SCENE_ICE_CAVERN",
    "SCENE_GANONS_TOWER",
    "SCENE_GERUDO_TRAINING_GROUND",
    "SCENE_THIEVES_HIDEOUT",
    "SCENE_INSIDE_GANONS_CASTLE",
    "SCENE_GANONS_TOWER_COLLAPSE_INTERIOR",
    "SCENE_INSIDE_GANONS_CASTLE_COLLAPSE",
    "SCENE_TREASURE_BOX_SHOP",
    "SCENE_DEKU_TREE_BOSS",
    "SCENE_DODONGOS_CAVERN_BOSS",
    "SCENE_JABU_JABU_BOSS",
    "SCENE_FOREST_TEMPLE_BOSS",
    "SCENE_FIRE_TEMPLE_BOSS",
    "SCENE_WATER_TEMPLE_BOSS",
    "SCENE_SPIRIT_TEMPLE_BOSS",
    "SCENE_SHADOW_TEMPLE_BOSS",
    "SCENE_GANONDORF_BOSS",
    "SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR",
    "SCENE_MARKET_ENTRANCE_DAY",
    "SCENE_MARKET_ENTRANCE_NIGHT",
    "SCENE_MARKET_ENTRANCE_RUINS",
    "SCENE_BACK_ALLEY_DAY",
    "SCENE_BACK_ALLEY_NIGHT",
    "SCENE_MARKET_DAY",
    "SCENE_MARKET_NIGHT",
    "SCENE_MARKET_RUINS",
    "SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY",
    "SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT",
    "SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS",
    "SCENE_KNOW_IT_ALL_BROS_HOUSE",
    "SCENE_TWINS_HOUSE",
    "SCENE_MIDOS_HOUSE",
    "SCENE_SARIAS_HOUSE",
    "SCENE_KAKARIKO_CENTER_GUEST_HOUSE",
    "SCENE_BACK_ALLEY_HOUSE",
    "SCENE_BAZAAR",
    "SCENE_KOKIRI_SHOP",
    "SCENE_GORON_SHOP",
    "SCENE_ZORA_SHOP",
    "SCENE_POTION_SHOP_KAKARIKO",
    "SCENE_POTION_SHOP_MARKET",
    "SCENE_BOMBCHU_SHOP",
    "SCENE_HAPPY_MASK_SHOP",
    "SCENE_LINKS_HOUSE",
    "SCENE_DOG_LADY_HOUSE",
    "SCENE_STABLE",
    "SCENE_IMPAS_HOUSE",
    "SCENE_LAKESIDE_LABORATORY",
    "SCENE_CARPENTERS_TENT",
    "SCENE_GRAVEKEEPERS_HUT",
    "SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC",
    "SCENE_FAIRYS_FOUNTAIN",
    "SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS",
    "SCENE_GROTTOS",
    "SCENE_REDEAD_GRAVE",
    "SCENE_GRAVE_WITH_FAIRYS_FOUNTAIN",
    "SCENE_ROYAL_FAMILYS_TOMB",
    "SCENE_SHOOTING_GALLERY",
    "SCENE_TEMPLE_OF_TIME",
    "SCENE_CHAMBER_OF_THE_SAGES",
    "SCENE_CASTLE_COURTYARD_GUARDS_DAY",
    "SCENE_CASTLE_COURTYARD_GUARDS_NIGHT",
    "SCENE_CUTSCENE_MAP",
    "SCENE_WINDMILL_AND_DAMPES_GRAVE",
    "SCENE_FISHING_POND",
    "SCENE_CASTLE_COURTYARD_ZELDA",
    "SCENE_BOMBCHU_BOWLING_ALLEY",
    "SCENE_LON_LON_BUILDINGS",
    "SCENE_MARKET_GUARD_HOUSE",
    "SCENE_POTION_SHOP_GRANNY",
    "SCENE_GANON_BOSS",
    "SCENE_HOUSE_OF_SKULLTULA",
    "SCENE_HYRULE_FIELD",
    "SCENE_KAKARIKO_VILLAGE",
    "SCENE_GRAVEYARD",
    "SCENE_ZORAS_RIVER",
    "SCENE_KOKIRI_FOREST",
    "SCENE_SACRED_FOREST_MEADOW",
    "SCENE_LAKE_HYLIA",
    "SCENE_ZORAS_DOMAIN",
    "SCENE_ZORAS_FOUNTAIN",
    "SCENE_GERUDO_VALLEY",
    "SCENE_LOST_WOODS",
    "SCENE_DESERT_COLOSSUS",
    "SCENE_GERUDOS_FORTRESS",
    "SCENE_HAUNTED_WASTELAND",
    "SCENE_HYRULE_CASTLE",
    "SCENE_DEATH_MOUNTAIN_TRAIL",
    "SCENE_DEATH_MOUNTAIN_CRATER",
    "SCENE_GORON_CITY",
    "SCENE_LON_LON_RANCH",
    "SCENE_OUTSIDE_GANONS_CASTLE",
};

static void append_str(char* buf, size_t* len, const char* str) {
    while (*str != '\0')
        buf[(*len)++] = *str++;
}

static void append_line(char* buf, size_t* len, const char* str) {
    while (*str != '\0')
        buf[(*len)++] = *str++;
    buf[(*len)++] = '\n';
}

static void CrashHandler_WriteActorData(char* buffer, size_t* pos) {
    char intCharBuffer[16];
    append_line(buffer, pos, "Actor Id      Params");
    for (unsigned int i = 0; i < ACTORCAT_MAX; i++) {

        ActorListEntry* entry = &gPlayState->actorCtx.actorLists[i];
        Actor* cur;

        if(entry->length == 0) {
            continue;
        }
        WRITE_VAR_LINE(buffer, pos, "Actor Cat: ", sCatToStrArray[i]);
        cur = entry->head;
        while (cur != nullptr) {
            // Actor ID
            snprintf(intCharBuffer, sizeof(intCharBuffer), "0x%03X       ", cur->id);
            append_str(buffer, pos, intCharBuffer);

            // Actor Params
            snprintf(intCharBuffer, sizeof(intCharBuffer), "0x%04X", cur->params);
            append_line(buffer, pos, intCharBuffer);

            cur = cur->next;
        }
    }
}

extern "C" void CrashHandler_PrintSohData(char* buffer, size_t* pos) {
    char intCharBuffer[16];
    append_line(buffer, pos, "Build Information:");
    WRITE_VAR_LINE(buffer, pos, "Game Version: ", (const char*)gBuildVersion);
    WRITE_VAR_LINE(buffer, pos, "Build Date: ", (const char*)gBuildDate);

    if (gPlayState != nullptr) {
        append_line(buffer, pos, "Actors:");
        CrashHandler_WriteActorData(buffer, pos);
        
        WRITE_VAR_LINE(buffer, pos, "Scene: ", sSceneIdToStrArray[gPlayState->sceneId]);

        snprintf(intCharBuffer, sizeof(intCharBuffer), "%i", gPlayState->roomCtx.curRoom.num);
        WRITE_VAR_LINE(buffer, pos, "Room: ", intCharBuffer);
    }
}