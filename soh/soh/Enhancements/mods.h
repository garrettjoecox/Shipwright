#include <stdint.h>

#ifndef MODS_H
#define MODS_H

#ifdef __cplusplus
extern "C" {
#endif

void UpdateDirtPathFixState(int32_t sceneId);
void UpdateMirrorModeState(int32_t sceneId);
void InitMods();

#ifdef __cplusplus
}
#endif

#endif
