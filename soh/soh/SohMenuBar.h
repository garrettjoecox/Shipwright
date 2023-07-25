#pragma once

#include <libultraship/libultraship.h>

namespace SohGui {

class SohMenuBar : public LUS::GuiMenuBar {
    public:
        using LUS::GuiMenuBar::GuiMenuBar;
    protected:
        void DrawElement() override;
        void InitElement() override {};
        void UpdateElement() override {};
};

}