#include "userInterfaceSettings.h"
#include "soh/util.h"
#include "soh/OTRGlobals.h"
#include "soh/UIWidgets.hpp"

#include <spdlog/fmt/fmt.h>
#include <array>
#include <bit>
#include <map>
#include <string>
#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
	extern PlayState* gPlayState;
}

void UserInterfaceWindow::DrawElement() {
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Settings", &mIsVisible, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }
    ImGui::Text("Loading :");
    ImGui::End();
}