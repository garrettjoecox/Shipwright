#pragma once

#include <array>
#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <libultraship/libultraship.h>

class UserInterfaceWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override {};
    void DrawElement() override {};
    void UpdateElement() override{};
};
