#include "Holiday.hpp"
#include "LL.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;

// TODO: Include anything you need here from C land
}

// TODO: Change this to YourName
#define AUTHOR "LL"
#define CVAR(v) "gHoliday." AUTHOR "." v

static ImVec4 customColorZero = RAINBOW_PRESETS[0][0];
static ImVec4 customColorOne = RAINBOW_PRESETS[0][1];
static ImVec4 customColorMinusZero = RAINBOW_PRESETS[0][2];
static ImVec4 customColorMinusOne = RAINBOW_PRESETS[0][3];

ImVec4 Color_LUSToImGui(Color_RGBA8 color) {
    ImVec4 result;

    result.x = color.r / 255.0f;
    result.y = color.g / 255.0f;
    result.z = color.b / 255.0f;
    result.w = color.a / 255.0f;

    return result;
}

Color_RGBA8 Color_ImGuiToLUS(ImVec4 color) {
    Color_RGBA8 result;

    result.r = static_cast<uint8_t>(color.x * 255);
    result.g = static_cast<uint8_t>(color.y * 255);
    result.b = static_cast<uint8_t>(color.z * 255);
    result.a = static_cast<uint8_t>(color.w * 255);

    return result;
}

static void OnConfigurationChanged() {
    Color_RGBA8 c1 = CVarGetColor(CVAR("lCustomRainbow1"), Color_ImGuiToLUS(RAINBOW_PRESETS[0][0]));
    Color_RGBA8 c2 = CVarGetColor(CVAR("lCustomRainbow2"), Color_ImGuiToLUS(RAINBOW_PRESETS[0][1]));
    Color_RGBA8 c3 = CVarGetColor(CVAR("lCustomRainbow3"), Color_ImGuiToLUS(RAINBOW_PRESETS[0][2]));
    Color_RGBA8 c4 = CVarGetColor(CVAR("lCustomRainbow4"), Color_ImGuiToLUS(RAINBOW_PRESETS[0][3]));

    customColorZero = Color_LUSToImGui((Color_RGBA8)c1);
    customColorOne = Color_LUSToImGui((Color_RGBA8)c2);
    customColorMinusZero = Color_LUSToImGui((Color_RGBA8)c3);
    customColorMinusOne = Color_LUSToImGui((Color_RGBA8)c4);

    // TODO: Register any hooks or things that need to run on startup and when the main CVar is toggled
    // Note: Hooks should be registered/unregistered depending on the CVar state (Use COND_HOOK or COND_ID_HOOK)

    // COND_HOOK(OnSceneSpawnActors, CVarGetInteger(CVAR("Enabled"), 0), []() {
    //     // Spawn your own actors?
    // });
    
    // COND_ID_HOOK(OnActorInit, ACTOR_OBJ_TSUBO, CVarGetInteger(CVAR("DoSomethingWithPots"), 0), [](void* actorRef) {
    //     // Do something with pots?
    // });
}

static void DrawMenu() {
    ImGui::SeparatorText(AUTHOR);
    if (ImGui::BeginMenu("Customize Rainbows")) {
        UIWidgets::EnhancementCheckbox("Enable", CVAR("lEnableCustomRainbows"));
        if (CVarGetInteger(CVAR("lEnableCustomRainbows"), 0)) {
            ImGui::ColorEdit3("Color 1", (float*)&customColorZero,      ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit3("Color 2", (float*)&customColorOne,       ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit3("Color 3", (float*)&customColorMinusZero, ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit3("Color 4", (float*)&customColorMinusOne,  ImGuiColorEditFlags_NoInputs);

            UIWidgets::PaddedText("Presets", true, false);
            size_t rainbowPresetIdx = 0;
            if (UIWidgets::EnhancementCombobox(CVAR("lCustomRainbowPreset"), RAINBOW_PRESET_NAMES, 0) &&
                    (rainbowPresetIdx = CVarGetInteger(CVAR("lCustomRainbowPreset"), 0)) <= RAINBOW_PRESET_LEN) { //paranoia
                customColorZero = RAINBOW_PRESETS[rainbowPresetIdx][0];
                customColorOne = RAINBOW_PRESETS[rainbowPresetIdx][1];
                customColorMinusZero = RAINBOW_PRESETS[rainbowPresetIdx][2];
                customColorMinusOne = RAINBOW_PRESETS[rainbowPresetIdx][3];
            }

            Color_RGBA8 color1, color2, color3, color4;
            color1.r = static_cast<uint8_t>(customColorZero.x * 255.0f);
            color1.g = static_cast<uint8_t>(customColorZero.y * 255.0f);
            color1.b = static_cast<uint8_t>(customColorZero.z * 255.0f);

            color2.r = static_cast<uint8_t>(customColorOne.x * 255.0f);
            color2.g = static_cast<uint8_t>(customColorOne.y * 255.0f);
            color2.b = static_cast<uint8_t>(customColorOne.z * 255.0f);

            color3.r = static_cast<uint8_t>(customColorMinusZero.x * 255.0f);
            color3.g = static_cast<uint8_t>(customColorMinusZero.y * 255.0f);
            color3.b = static_cast<uint8_t>(customColorMinusZero.z * 255.0f);

            color4.r = static_cast<uint8_t>(customColorMinusOne.x * 255.0f);
            color4.g = static_cast<uint8_t>(customColorMinusOne.y * 255.0f);
            color4.b = static_cast<uint8_t>(customColorMinusOne.z * 255.0f);

            CVarSetColor(CVAR("lCustomRainbow1"), color1);
            CVarSetColor(CVAR("lCustomRainbow2"), color2);
            CVarSetColor(CVAR("lCustomRainbow3"), color3);
            CVarSetColor(CVAR("lCustomRainbow4"), color4);

            OnConfigurationChanged();
        }

        ImGui::EndMenu();

    }
    //if (UIWidgets::EnhancementCheckbox("DoSomethingWithPots", CVAR("DoSomethingWithPots"))) {
    //    OnConfigurationChanged();
    //}
}

static void RegisterMod() {
    // #region Leave this alone unless you know what you are doing
    OnConfigurationChanged();
    // #endregion

    // TODO: Anything you want to run once on startup
}

// TODO: Uncomment this line to enable the mod
static Holiday holiday(DrawMenu, RegisterMod);
