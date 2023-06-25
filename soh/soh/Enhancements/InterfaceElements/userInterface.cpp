#include "userInterface.h"

#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>

void UserInterfaceWindow::DrawElement() {
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("User Interface", &mIsVisible, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }
    ImGui::Text("Sword");
    
    ImGui::End();
}

void UserInterfaceWindow::InitElement() {}
void UserInterfaceWindow::UpdateElement() {}