#ifndef Z64SCENE_H
#define Z64SCENE_H

#include "command_macros_base.h"

typedef struct {
    /* 0x00 */ uintptr_t vromStart;
    /* 0x04 */ uintptr_t vromEnd;
    char* fileName;
} RomFile; // size = 0x8

typedef struct {
    /* 0x00 */ RomFile sceneFile;
    /* 0x08 */ RomFile titleFile;
    /* 0x10 */ u8  unk_10;
    /* 0x11 */ u8  config;
    /* 0x12 */ u8  unk_12;
    /* 0x13 */ u8  unk_13;
} SceneTableEntry; // size = 0x14

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ u32 data2;
} SCmdBase;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ void* segment;
} SCmdSpawnList;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  num;
    /* 0x04 */ void* segment;
} SCmdActorList;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ void* segment;
} SCmdUnused02;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ void* segment;
} SCmdColHeader;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  num;
    /* 0x04 */ void* segment;
} SCmdRoomList;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x02 */ char pad[2];
    /* 0x04 */ u8  x;
    /* 0x05 */ u8  y;
    /* 0x06 */ u8  z;
    /* 0x07 */ u8  unk_07;
} SCmdWindSettings;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ void* segment;
} SCmdEntranceList;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  cUpElfMsgNum;
    /* 0x04 */ u32 keepObjectId;
} SCmdSpecialFiles;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  gpFlag1;
    /* 0x04 */ u32 gpFlag2;
} SCmdRoomBehavior;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ void* segment;
} SCmdMesh;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  num;
    /* 0x04 */ void* segment;
} SCmdObjectList;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  num;
    /* 0x04 */ void* segment;
} SCmdLightList;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ void* segment;
} SCmdPathList;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  num;
    /* 0x04 */ void* segment;
} SCmdTransiActorList;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  num;
    /* 0x04 */ void* segment;
} SCmdLightSettingList;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x02 */ char pad[2];
    /* 0x04 */ u8  hour;
    /* 0x05 */ u8  min;
    /* 0x06 */ u8  unk_06;
} SCmdTimeSettings;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x02 */ char pad[2];
    /* 0x04 */ u8  skyboxId;
    /* 0x05 */ u8  unk_05;
    /* 0x06 */ u8  unk_06;
} SCmdSkyboxSettings;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x02 */ char pad[2];
    /* 0x04 */ u8  unk_04;
    /* 0x05 */ u8  unk_05;
} SCmdSkyboxDisables;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ u32 data2;
} SCmdEndMarker;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ void* segment;
} SCmdExitList;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  specId;
    /* 0x02 */ char pad[4];
    /* 0x06 */ u8  natureAmbienceId;
    /* 0x07 */ u8  seqId;
} SCmdSoundSettings;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x02 */ char pad[5];
    /* 0x07 */ u8  echo;
} SCmdEchoSettings;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ void* segment;
} SCmdCutsceneData;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  data1;
    /* 0x04 */ void* segment;
} SCmdAltHeaders;

typedef struct {
    /* 0x00 */ u8  code;
    /* 0x01 */ u8  cameraMovement;
    /* 0x04 */ u32 area;
} SCmdMiscSettings;

typedef struct {
    u8 headerType;
} MeshHeaderBase;

typedef struct {
    MeshHeaderBase base;
    u8 numEntries;
    Gfx* dListStart;
    Gfx* dListEnd;
} MeshHeader0;

typedef struct {
    Gfx* opaqueDList;
    Gfx* translucentDList;
} MeshEntry0;

typedef struct {
    MeshHeaderBase base;
    u8 format;
    u32 entryRecord;
} MeshHeader1Base;

typedef struct {
    MeshHeader1Base base;
    void* imagePtr; // 0x08
    u32 unknown; // 0x0C
    u32 unknown2; // 0x10
    u16 bgWidth; // 0x14
    u16 bgHeight; // 0x16
    u8 imageFormat; // 0x18
    u8 imageSize; // 0x19
    u16 imagePal; // 0x1A
    u16 imageFlip; // 0x1C
} MeshHeader1Single;

typedef struct {
    MeshHeader1Base base;
    u8 bgCnt;
    void* bgRecordPtr;
} MeshHeader1Multi;

typedef struct {
    u16 unknown; // 0x00
    s8 bgID; // 0x02
    void* imagePtr; // 0x04
    u32 unknown2; // 0x08
    u32 unknown3; // 0x0C
    u16 bgWidth; // 0x10
    u16 bgHeight; // 0x12
    u8 imageFmt; // 0x14
    u8 imageSize; // 0x15
    u16 imagePal; // 0x16
    u16 imageFlip; // 0x18
} BackgroundRecord;

typedef struct {
    s16 playerXMax, playerZMax;
    s16 playerXMin, playerZMin;
    Gfx* opaqueDList;
    Gfx* translucentDList;
} MeshEntry2;

typedef struct {
    MeshHeaderBase base;
    u8 numEntries;
    Gfx* dListStart;
    Gfx* dListEnd;
} MeshHeader2;

typedef struct {
    /* 0x00 */ u8 ambientColor[3];
    /* 0x03 */ s8 diffuseDir1[3];
    /* 0x06 */ u8 diffuseColor1[3];
    /* 0x09 */ s8 diffuseDir2[3];
    /* 0x0C */ u8 diffuseColor2[3];
    /* 0x0F */ u8 fogColor[3];
    /* 0x12 */ u16 fogNear;
    /* 0x14 */ u16 fogFar;
} LightSettings; // size = 0x16

typedef struct {
    /* 0x00 */ u8 count; // number of points in the path
    /* 0x04 */ Vec3s* points; // Segment Address to the array of points
} Path; // size = 0x8

typedef union {
    SCmdBase              base;
    SCmdSpawnList         spawnList;
    SCmdActorList         actorList;
    SCmdUnused02          unused02;
    SCmdRoomList          roomList;
    SCmdEntranceList      entranceList;
    SCmdObjectList        objectList;
    SCmdLightList         lightList;
    SCmdPathList          pathList;
    SCmdTransiActorList   transiActorList;
    SCmdLightSettingList  lightSettingList;
    SCmdExitList          exitList;
    SCmdColHeader         colHeader;
    SCmdMesh              mesh;
    SCmdSpecialFiles      specialFiles;
    SCmdCutsceneData      cutsceneData;
    SCmdRoomBehavior      roomBehavior;
    SCmdWindSettings      windSettings;
    SCmdTimeSettings      timeSettings;
    SCmdSkyboxSettings    skyboxSettings;
    SCmdSkyboxDisables    skyboxDisables;
    SCmdEndMarker         endMarker;
    SCmdSoundSettings     soundSettings;
    SCmdEchoSettings      echoSettings;
    SCmdMiscSettings      miscSettings;
    SCmdAltHeaders        altHeaders;
} SceneCmd; // size = 0x8

#ifdef __cplusplus
enum SceneID : int {
#else
enum SceneID {
#endif
    /* 0x00 */ SCENE_DEKU_TREE,
    /* 0x01 */ SCENE_DODONGOS_CAVERN,
    /* 0x02 */ SCENE_JABU_JABU,
    /* 0x03 */ SCENE_FOREST_TEMPLE,
    /* 0x04 */ SCENE_FIRE_TEMPLE,
    /* 0x05 */ SCENE_WATER_TEMPLE,
    /* 0x06 */ SCENE_SPIRIT_TEMPLE,
    /* 0x07 */ SCENE_SHADOW_TEMPLE,
    /* 0x08 */ SCENE_BOTTOM_OF_THE_WELL,
    /* 0x09 */ SCENE_ICE_CAVERN,
    /* 0x0A */ SCENE_GANONS_TOWER,
    /* 0x0B */ SCENE_GERUDO_TRAINING_GROUND,
    /* 0x0C */ SCENE_THIEVES_HIDEOUT,
    /* 0x0D */ SCENE_INSIDE_GANONS_CASTLE,
    /* 0x0E */ SCENE_GANONS_TOWER_COLLAPSE_INTERIOR,
    /* 0x0F */ SCENE_INSIDE_GANONS_CASTLE_COLLAPSE,
    /* 0x10 */ SCENE_TREASURE_BOX_SHOP,
    /* 0x11 */ SCENE_DEKU_TREE_BOSS,
    /* 0x12 */ SCENE_DODONGOS_CAVERN_BOSS,
    /* 0x13 */ SCENE_JABU_JABU_BOSS,
    /* 0x14 */ SCENE_FOREST_TEMPLE_BOSS,
    /* 0x15 */ SCENE_FIRE_TEMPLE_BOSS,
    /* 0x16 */ SCENE_WATER_TEMPLE_BOSS,
    /* 0x17 */ SCENE_SPIRIT_TEMPLE_BOSS,
    /* 0x18 */ SCENE_SHADOW_TEMPLE_BOSS,
    /* 0x19 */ SCENE_GANONDORF_BOSS,
    /* 0x1A */ SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR,
    /* 0x1B */ SCENE_MARKET_ENTRANCE_DAY,
    /* 0x1C */ SCENE_MARKET_ENTRANCE_NIGHT,
    /* 0x1D */ SCENE_MARKET_ENTRANCE_RUINS,
    /* 0x1E */ SCENE_BACK_ALLEY_DAY,
    /* 0x1F */ SCENE_BACK_ALLEY_NIGHT,
    /* 0x20 */ SCENE_MARKET_DAY,
    /* 0x21 */ SCENE_MARKET_NIGHT,
    /* 0x22 */ SCENE_MARKET_RUINS,
    /* 0x23 */ SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY,
    /* 0x24 */ SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT,
    /* 0x25 */ SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS,
    /* 0x26 */ SCENE_KNOW_IT_ALL_BROS_HOUSE,
    /* 0x27 */ SCENE_TWINS_HOUSE,
    /* 0x28 */ SCENE_MIDOS_HOUSE,
    /* 0x29 */ SCENE_SARIAS_HOUSE,
    /* 0x2A */ SCENE_KAKARIKO_CENTER_GUEST_HOUSE,
    /* 0x2B */ SCENE_BACK_ALLEY_HOUSE,
    /* 0x2C */ SCENE_BAZAAR,
    /* 0x2D */ SCENE_KOKIRI_SHOP,
    /* 0x2E */ SCENE_GORON_SHOP,
    /* 0x2F */ SCENE_ZORA_SHOP,
    /* 0x30 */ SCENE_POTION_SHOP_KAKARIKO,
    /* 0x31 */ SCENE_POTION_SHOP_MARKET,
    /* 0x32 */ SCENE_BOMBCHU_SHOP,
    /* 0x33 */ SCENE_HAPPY_MASK_SHOP,
    /* 0x34 */ SCENE_LINKS_HOUSE,
    /* 0x35 */ SCENE_DOG_LADY_HOUSE,
    /* 0x36 */ SCENE_STABLE,
    /* 0x37 */ SCENE_IMPAS_HOUSE,
    /* 0x38 */ SCENE_LAKESIDE_LABORATORY,
    /* 0x39 */ SCENE_CARPENTERS_TENT,
    /* 0x3A */ SCENE_GRAVEKEEPERS_HUT,
    /* 0x3B */ SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC,
    /* 0x3C */ SCENE_FAIRYS_FOUNTAIN,
    /* 0x3D */ SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS,
    /* 0x3E */ SCENE_GROTTOS,
    /* 0x3F */ SCENE_REDEAD_GRAVE,
    /* 0x40 */ SCENE_GRAVE_WITH_FAIRYS_FOUNTAIN,
    /* 0x41 */ SCENE_ROYAL_FAMILYS_TOMB,
    /* 0x42 */ SCENE_SHOOTING_GALLERY,
    /* 0x43 */ SCENE_TEMPLE_OF_TIME,
    /* 0x44 */ SCENE_CHAMBER_OF_THE_SAGES,
    /* 0x45 */ SCENE_CASTLE_COURTYARD_GUARDS_DAY,
    /* 0x46 */ SCENE_CASTLE_COURTYARD_GUARDS_NIGHT,
    /* 0x47 */ SCENE_CUTSCENE_MAP,
    /* 0x48 */ SCENE_WINDMILL_AND_DAMPES_GRAVE,
    /* 0x49 */ SCENE_FISHING_POND,
    /* 0x4A */ SCENE_CASTLE_COURTYARD_ZELDA,
    /* 0x4B */ SCENE_BOMBCHU_BOWLING_ALLEY,
    /* 0x4C */ SCENE_LON_LON_BUILDINGS,
    /* 0x4D */ SCENE_MARKET_GUARD_HOUSE,
    /* 0x4E */ SCENE_POTION_SHOP_GRANNY,
    /* 0x4F */ SCENE_GANON_BOSS,
    /* 0x50 */ SCENE_HOUSE_OF_SKULLTULA,
    /* 0x51 */ SCENE_HYRULE_FIELD,
    /* 0x52 */ SCENE_KAKARIKO_VILLAGE,
    /* 0x53 */ SCENE_GRAVEYARD,
    /* 0x54 */ SCENE_ZORAS_RIVER,
    /* 0x55 */ SCENE_KOKIRI_FOREST,
    /* 0x56 */ SCENE_SACRED_FOREST_MEADOW,
    /* 0x57 */ SCENE_LAKE_HYLIA,
    /* 0x58 */ SCENE_ZORAS_DOMAIN,
    /* 0x59 */ SCENE_ZORAS_FOUNTAIN,
    /* 0x5A */ SCENE_GERUDO_VALLEY,
    /* 0x5B */ SCENE_LOST_WOODS,
    /* 0x5C */ SCENE_DESERT_COLOSSUS,
    /* 0x5D */ SCENE_GERUDOS_FORTRESS,
    /* 0x5E */ SCENE_HAUNTED_WASTELAND,
    /* 0x5F */ SCENE_HYRULE_CASTLE,
    /* 0x60 */ SCENE_DEATH_MOUNTAIN_TRAIL,
    /* 0x61 */ SCENE_DEATH_MOUNTAIN_CRATER,
    /* 0x62 */ SCENE_GORON_CITY,
    /* 0x63 */ SCENE_LON_LON_RANCH,
    /* 0x64 */ SCENE_OUTSIDE_GANONS_CASTLE,
    // Debug only scenes
    /* 0x65 */ SCENE_TEST01,
    /* 0x66 */ SCENE_BESITU,
    /* 0x67 */ SCENE_DEPTH_TEST,
    /* 0x68 */ SCENE_SYOTES,
    /* 0x69 */ SCENE_SYOTES2,
    /* 0x6A */ SCENE_SUTARU,
    /* 0x6B */ SCENE_HAIRAL_NIWA2,
    /* 0x6C */ SCENE_SASATEST,
    /* 0x6D */ SCENE_TESTROOM,
    /* 0x6E */ SCENE_ID_MAX
};

// Scene commands

typedef enum {
    /* 0x00 */ SCENE_CMD_ID_SPAWN_LIST,
    /* 0x01 */ SCENE_CMD_ID_ACTOR_LIST,
    /* 0x02 */ SCENE_CMD_ID_UNUSED_2,
    /* 0x03 */ SCENE_CMD_ID_COLLISION_HEADER,
    /* 0x04 */ SCENE_CMD_ID_ROOM_LIST,
    /* 0x05 */ SCENE_CMD_ID_WIND_SETTINGS,
    /* 0x06 */ SCENE_CMD_ID_ENTRANCE_LIST,
    /* 0x07 */ SCENE_CMD_ID_SPECIAL_FILES,
    /* 0x08 */ SCENE_CMD_ID_ROOM_BEHAVIOR,
    /* 0x09 */ SCENE_CMD_ID_UNDEFINED_9,
    /* 0x0A */ SCENE_CMD_ID_MESH_HEADER,
    /* 0x0B */ SCENE_CMD_ID_OBJECT_LIST,
    /* 0x0C */ SCENE_CMD_ID_LIGHT_LIST,
    /* 0x0D */ SCENE_CMD_ID_PATH_LIST,
    /* 0x0E */ SCENE_CMD_ID_TRANSITION_ACTOR_LIST,
    /* 0x0F */ SCENE_CMD_ID_LIGHT_SETTINGS_LIST,
    /* 0x10 */ SCENE_CMD_ID_TIME_SETTINGS,
    /* 0x11 */ SCENE_CMD_ID_SKYBOX_SETTINGS,
    /* 0x12 */ SCENE_CMD_ID_SKYBOX_DISABLES,
    /* 0x13 */ SCENE_CMD_ID_EXIT_LIST,
    /* 0x14 */ SCENE_CMD_ID_END,
    /* 0x15 */ SCENE_CMD_ID_SOUND_SETTINGS,
    /* 0x16 */ SCENE_CMD_ID_ECHO_SETTINGS,
    /* 0x17 */ SCENE_CMD_ID_CUTSCENE_DATA,
    /* 0x18 */ SCENE_CMD_ID_ALTERNATE_HEADER_LIST,
    /* 0x19 */ SCENE_CMD_ID_MISC_SETTINGS,
    /* 0x20 */ SCENE_CMD_ID_MAX
} SceneCommandTypeID;

#define SCENE_CMD_SPAWN_LIST(numSpawns, spawnList) \
    { SCENE_CMD_ID_SPAWN_LIST, numSpawns, CMD_PTR(spawnList) }

#define SCENE_CMD_ACTOR_LIST(numActors, actorList) \
    { SCENE_CMD_ID_ACTOR_LIST, numActors, CMD_PTR(actorList) }

#define SCENE_CMD_UNUSED_02(unk, data) \
    { SCENE_CMD_ID_UNUSED_2, unk, CMD_PTR(data) }

#define SCENE_CMD_COL_HEADER(colHeader) \
    { SCENE_CMD_ID_COLLISION_HEADER, 0, CMD_PTR(colHeader) }

#define SCENE_CMD_ROOM_LIST(numRooms, roomList) \
    { SCENE_CMD_ID_ROOM_LIST, numRooms, CMD_PTR(roomList) }

#define SCENE_CMD_WIND_SETTINGS(xDir, yDir, zDir, strength) \
    { SCENE_CMD_ID_WIND_SETTINGS, 0, CMD_BBBB(xDir, yDir, zDir, strength) }

#define SCENE_CMD_ENTRANCE_LIST(entranceList) \
    { SCENE_CMD_ID_ENTRANCE_LIST, 0, CMD_PTR(entranceList) }

#define SCENE_CMD_SPECIAL_FILES(elfMessageFile, keepObjectId) \
    { SCENE_CMD_ID_SPECIAL_FILES, elfMessageFile, CMD_W(keepObjectId) }

#define SCENE_CMD_ROOM_BEHAVIOR(curRoomUnk3, curRoomUnk2, showInvisActors, disableWarpSongs) \
    { SCENE_CMD_ID_ROOM_BEHAVIOR, curRoomUnk3, \
        curRoomUnk2 | _SHIFTL(showInvisActors, 8, 1) | _SHIFTL(disableWarpSongs, 10, 1) }

#define SCENE_CMD_UNK_09() \
    { SCENE_CMD_ID_UNDEFINED_9, 0, CMD_W(0) }

#define SCENE_CMD_MESH(meshHeader) \
    { SCENE_CMD_ID_MESH_HEADER, 0, CMD_PTR(meshHeader) }

#define SCENE_CMD_OBJECT_LIST(numObjects, objectList) \
    { SCENE_CMD_ID_OBJECT_LIST, numObjects, CMD_PTR(objectList) }

#define SCENE_CMD_LIGHT_LIST(numLights, lightList) \
    { SCENE_CMD_ID_POS_LIGHT_LIST, numLights, CMD_PTR(lightList) } 

#define SCENE_CMD_PATH_LIST(pathList) \
    { SCENE_CMD_ID_PATH_LIST, 0, CMD_PTR(pathList) }

#define SCENE_CMD_TRANSITION_ACTOR_LIST(numActors, list) \
    { SCENE_CMD_ID_TRANSITION_ACTOR_LIST, numActors, CMD_PTR(list) }

#define SCENE_CMD_ENV_LIGHT_SETTINGS(numLightSettings, lightSettingsList) \
    { SCENE_CMD_ID_LIGHT_SETTINGS_LIST, numLightSettings, CMD_PTR(lightSettingsList) }

#define SCENE_CMD_TIME_SETTINGS(hour, min, speed) \
    { SCENE_CMD_ID_TIME_SETTINGS, 0, CMD_BBBB(hour, min, speed, 0) }

#define SCENE_CMD_SKYBOX_SETTINGS(skyboxId, weather, isIndoors) \
    { SCENE_CMD_ID_SKYBOX_SETTINGS, 0, CMD_BBBB(skyboxId, weather, isIndoors, 0) }

#define SCENE_CMD_SKYBOX_DISABLES(disableSky, disableSunMoon) \
    { SCENE_CMD_ID_SKYBOX_DISABLES, 0, CMD_BBBB(disableSky, disableSunMoon, 0, 0) }

#define SCENE_CMD_EXIT_LIST(exitList) \
    { SCENE_CMD_ID_EXIT_LIST, 0, CMD_PTR(exitList) }

#define SCENE_CMD_END() \
    { SCENE_CMD_ID_END, 0, CMD_W(0) }

#define SCENE_CMD_SOUND_SETTINGS(specId, natureAmbienceId, seqId) \
    { SCENE_CMD_ID_SOUND_SETTINGS, specId, CMD_BBBB(0, 0, natureAmbienceId, seqId) }

#define SCENE_CMD_ECHO_SETTINGS(echo) \
    { SCENE_CMD_ID_ECHO_SETTINGS, 0, CMD_BBBB(0, 0, 0, echo) }

#define SCENE_CMD_CUTSCENE_DATA(cutsceneData) \
    { SCENE_CMD_ID_CUTSCENE_DATA, 0, CMD_PTR(cutsceneData) }

#define SCENE_CMD_ALTERNATE_HEADER_LIST(alternateHeaderList) \
    { SCENE_CMD_ID_ALTERNATE_HEADER_LIST, 0, CMD_PTR(alternateHeaderList) }

#define SCENE_CMD_MISC_SETTINGS(camMode, worldMapLocation) \
    { SCENE_CMD_ID_MISC_SETTINGS, camMode, CMD_W(worldMapLocation) }


#endif
