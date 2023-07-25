#include "SohGui.hpp"

#include <spdlog/spdlog.h>
#include <libultraship/libultraship.h>

bool ShouldClearTextureCacheAtEndOfFrame = false;

namespace SohGui {
    std::shared_ptr<SohMenuBar> mSohMenuBar;
    std::shared_ptr<LUS::GuiWindow> mInputEditorWindow;

    void SetupGuiElements() {
        auto gui = LUS::Context::GetInstance()->GetWindow()->GetGui();

        mSohMenuBar = std::make_shared<SohMenuBar>("gOpenMenuBar", CVarGetInteger("gOpenMenuBar", 0));
        gui->SetMenuBar(std::reinterpret_pointer_cast<LUS::GuiMenuBar>(mSohMenuBar));

        if (gui->GetMenuBar() && !gui->GetMenuBar()->IsVisible()) {
#if defined(__SWITCH__) || defined(__WIIU__)
            gui->GetGameOverlay()->TextDrawNotification(30.0f, true, "Press - to access enhancements menu");
#else
            gui->GetGameOverlay()->TextDrawNotification(30.0f, true, "Press F1 to access enhancements menu");
#endif
        }

        mInputEditorWindow = gui->GetGuiWindow("Input Editor");
        if (mInputEditorWindow == nullptr) {
            SPDLOG_ERROR("Could not find input editor window");
        }
    }

    void Destroy() {
        mInputEditorWindow = nullptr;
        mSohMenuBar = nullptr;
    }
}
