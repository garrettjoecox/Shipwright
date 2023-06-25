#include "userInterface.h"
#include "../../util.h"
#include "../../OTRGlobals.h"
#include "../../UIWidgets.hpp"

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
    if (!ImGui::Begin("User Interface", &mIsVisible, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }
    if (ImGui::BeginMenu("Developer Tools")) {
        ImGui::Text("Sword");
        ImGui::EndMenu();
    }
    
    ImGui::End();
}

void InitElement() {}
void DrawElement() {}