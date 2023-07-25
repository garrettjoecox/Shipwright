#include "SohMenuBar.h"

#include <ImGui/imgui.h>
#include <libultraship/libultraship.h>

#include "UIWidgets.hpp"
#include "OTRGlobals.h"
#include "z64.h"
#include "Enhancements/game-interactor/GameInteractor.h"

enum SeqPlayers {
    /* 0 */ SEQ_BGM_MAIN,
    /* 1 */ SEQ_FANFARE,
    /* 2 */ SEQ_SFX,
    /* 3 */ SEQ_BGM_SUB,
    /* 4 */ SEQ_MAX
};

std::string GetWindowButtonText(const char* text, bool menuOpen) {
    char buttonText[100] = "";
    if (menuOpen) {
        strcat(buttonText, ICON_FA_CHEVRON_RIGHT " ");
    }
    strcat(buttonText, text);
    if (!menuOpen) { strcat(buttonText, "  "); }
    return buttonText;
}

namespace SohGui {

void DrawMenuBarIcon() {
    static bool gameIconLoaded = false;
    if (!gameIconLoaded) {
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTexture("Game_Icon", "textures/icons/gIcon.png");
        gameIconLoaded = true;
    }

    if (LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("Game_Icon")) {
#ifdef __SWITCH__
        ImVec2 iconSize = ImVec2(20.0f, 20.0f);
        float posScale = 1.0f;
#elif defined(__WIIU__)
        ImVec2 iconSize = ImVec2(16.0f * 2, 16.0f * 2);
        float posScale = 2.0f;
#else
        ImVec2 iconSize = ImVec2(16.0f, 16.0f);
        float posScale = 1.0f;
#endif
        ImGui::SetCursorPos(ImVec2(5, 2.5f) * posScale);
        ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("Game_Icon"), iconSize);
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(25, 0) * posScale);
    }
}

void DrawShipMenu() {
    if (ImGui::BeginMenu("Ship")) {
        if (ImGui::MenuItem("Hide Menu Bar",
#if !defined(__SWITCH__) && !defined(__WIIU__)
         "F1"
#else
         "[-]"
#endif
        )) {
            LUS::Context::GetInstance()->GetWindow()->GetGui()->GetMenuBar()->ToggleVisibility();
        }
        UIWidgets::Spacer(0);
#if !defined(__SWITCH__) && !defined(__WIIU__)
        if (ImGui::MenuItem("Toggle Fullscreen", "F11")) {
            LUS::Context::GetInstance()->GetWindow()->ToggleFullscreen();
        }
        UIWidgets::Spacer(0);
#endif
        if (ImGui::MenuItem("Reset",
#ifdef __APPLE__
            "Command-R"
#elif !defined(__SWITCH__) && !defined(__WIIU__)
            "Ctrl+R"
#else
            ""
#endif
        )) {
            std::reinterpret_pointer_cast<LUS::ConsoleWindow>(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch("reset");
        }
#if !defined(__SWITCH__) && !defined(__WIIU__)
        UIWidgets::Spacer(0);
        if (ImGui::MenuItem("Quit")) {
            LUS::Context::GetInstance()->GetWindow()->Close();
        }
#endif
        ImGui::EndMenu();
    }
}

extern std::shared_ptr<LUS::GuiWindow> mInputEditorWindow;

void DrawSettingsMenu() {
    if (ImGui::BeginMenu("Settings")) {
        if (ImGui::BeginMenu("Audio")) {
            UIWidgets::PaddedEnhancementSliderFloat("Master Volume: %d %%", "##Master_Vol", "gGameMasterVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true);
            if (UIWidgets::PaddedEnhancementSliderFloat("Main Music Volume: %d %%", "##Main_Music_Vol", "gMainMusicVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                Audio_SetGameVolume(SEQ_BGM_MAIN, CVarGetFloat("gMainMusicVolume", 1.0f));
            }
            if (UIWidgets::PaddedEnhancementSliderFloat("Sub Music Volume: %d %%", "##Sub_Music_Vol", "gSubMusicVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                Audio_SetGameVolume(SEQ_BGM_SUB, CVarGetFloat("gSubMusicVolume", 1.0f));
            }
            if (UIWidgets::PaddedEnhancementSliderFloat("Sound Effects Volume: %d %%", "##Sound_Effect_Vol", "gSFXMusicVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                Audio_SetGameVolume(SEQ_SFX, CVarGetFloat("gSFXMusicVolume", 1.0f));
            }
            if (UIWidgets::PaddedEnhancementSliderFloat("Fanfare Volume: %d %%", "##Fanfare_Vol", "gFanfareVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                Audio_SetGameVolume(SEQ_FANFARE, CVarGetFloat("gFanfareVolume", 1.0f));
            }

            static std::unordered_map<LUS::AudioBackend, const char*> audioBackendNames = {
                { LUS::AudioBackend::WASAPI, "Windows Audio Session API" },
                { LUS::AudioBackend::PULSE, "PulseAudio" },
                { LUS::AudioBackend::SDL, "SDL" },
            };

            ImGui::Text("Audio API (Needs reload)");
            auto currentAudioBackend = LUS::Context::GetInstance()->GetAudio()->GetAudioBackend();

            if (LUS::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->size() <= 1) {
                UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
            }
            if (ImGui::BeginCombo("##AApi", audioBackendNames[currentAudioBackend])) {
                for (uint8_t i = 0; i < LUS::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->size(); i++) {
                    auto backend = LUS::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->data()[i];
                    if (ImGui::Selectable(audioBackendNames[backend], backend == currentAudioBackend)) {
                        LUS::Context::GetInstance()->GetAudio()->SetAudioBackend(backend);
                    }
                }
                ImGui::EndCombo();
            }
            if (LUS::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->size() <= 1) {
                UIWidgets::ReEnableComponent("");
            }

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Controller")) {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 (12.0f, 6.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
            if (mInputEditorWindow) {
                if (ImGui::Button(GetWindowButtonText("Controller Mapping", CVarGetInteger("gControllerConfigurationEnabled", 0)).c_str(), ImVec2 (-1.0f, 0.0f))) {
                    mInputEditorWindow->ToggleVisibility();
                }
            }
            UIWidgets::PaddedSeparator();
            ImGui::PopStyleColor(1);
            ImGui::PopStyleVar(3);
        #ifndef __SWITCH__
            UIWidgets::EnhancementCheckbox("Menubar Controller Navigation", "gControlNav");
            UIWidgets::Tooltip("Allows controller navigation of the SOH menu bar (Settings, Enhancements,...)\nCAUTION: This will disable game inputs while the menubar is visible.\n\nD-pad to move between items, A to select, and X to grab focus on the menu bar");
        #endif
            UIWidgets::PaddedEnhancementCheckbox("Show Inputs", "gInputEnabled", true, false);
            UIWidgets::Tooltip("Shows currently pressed inputs on the bottom right of the screen");
            UIWidgets::PaddedEnhancementSliderFloat("Input Scale: %.1f", "##Input", "gInputScale", 1.0f, 3.0f, "", 1.0f, false, true, true, false);
            UIWidgets::Tooltip("Sets the on screen size of the displayed inputs from the Show Inputs setting");
            UIWidgets::PaddedEnhancementSliderInt("Simulated Input Lag: %d frames", "##SimulatedInputLag", "gSimulatedInputLag", 0, 6, "", 0, true, true, false);
            UIWidgets::Tooltip("Buffers your inputs to be executed a specified amount of frames later");

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Graphics")) {
        #ifndef __APPLE__
            UIWidgets::EnhancementSliderFloat("Internal Resolution: %d %%", "##IMul", "gInternalResolution", 0.5f, 2.0f, "", 1.0f, true);
            UIWidgets::Tooltip("Multiplies your output resolution by the value inputted, as a more intensive but effective form of anti-aliasing");
        #endif
        #ifndef __WIIU__
            UIWidgets::PaddedEnhancementSliderInt("MSAA: %d", "##IMSAA", "gMSAAValue", 1, 8, "", 1, true, true, false);
            UIWidgets::Tooltip("Activates multi-sample anti-aliasing when above 1x up to 8x for 8 samples for every pixel");
        #endif

            { // FPS Slider
                const int minFps = 20;
                static int maxFps;
                if (LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() == LUS::WindowBackend::DX11) {
                    maxFps = 360;
                } else {
                    maxFps = LUS::Context::GetInstance()->GetWindow()->GetCurrentRefreshRate();
                }
                int currentFps = fmax(fmin(OTRGlobals::Instance->GetInterpolationFPS(), maxFps), minFps);
            #ifdef __WIIU__
                UIWidgets::Spacer(0);
                // only support divisors of 60 on the Wii U
                if (currentFps > 60) {
                    currentFps = 60;
                } else {
                    currentFps = 60 / (60 / currentFps);
                }

                int fpsSlider = 1;
                if (currentFps == 20) {
                    ImGui::Text("FPS: Original (20)");
                } else {
                    ImGui::Text("FPS: %d", currentFps);
                    if (currentFps == 30) {
                        fpsSlider = 2;
                    } else { // currentFps == 60
                        fpsSlider = 3;
                    }
                }
                if (CVarGetInteger("gMatchRefreshRate", 0)) {
                    UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
                }

                if (ImGui::Button(" - ##WiiUFPS")) {
                    fpsSlider--;
                }
                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);

                UIWidgets::Spacer(0);

                ImGui::PushItemWidth(std::min((ImGui::GetContentRegionAvail().x - 60.0f), 260.0f));
                ImGui::SliderInt("##WiiUFPSSlider", &fpsSlider, 1, 3, "", ImGuiSliderFlags_AlwaysClamp);
                ImGui::PopItemWidth();

                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
                if (ImGui::Button(" + ##WiiUFPS")) {
                    fpsSlider++;
                }

                if (CVarGetInteger("gMatchRefreshRate", 0)) {
                    UIWidgets::ReEnableComponent("");
                }
                if (fpsSlider > 3) {
                    fpsSlider = 3;
                } else if (fpsSlider < 1) {
                    fpsSlider = 1;
                }

                if (fpsSlider == 1) {
                    currentFps = 20;
                } else if (fpsSlider == 2) {
                    currentFps = 30;
                } else if (fpsSlider == 3) {
                    currentFps = 60;
                }
                CVarSetInteger("gInterpolationFPS", currentFps);
                LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            #else
                bool matchingRefreshRate =
                    CVarGetInteger("gMatchRefreshRate", 0) && LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() != LUS::WindowBackend::DX11;
                UIWidgets::PaddedEnhancementSliderInt(
                    (currentFps == 20) ? "FPS: Original (20)" : "FPS: %d",
                    "##FPSInterpolation", "gInterpolationFPS", minFps, maxFps, "", 20, true, true, false, matchingRefreshRate);
            #endif
                if (LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() == LUS::WindowBackend::DX11) {
                    UIWidgets::Tooltip(
                        "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics. This is purely "
                        "visual and does not impact game logic, execution of glitches etc.\n\n"
                        "A higher target FPS than your monitor's refresh rate will waste resources, and might give a worse result."
                    );
                } else {
                    UIWidgets::Tooltip(
                        "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics. This is purely "
                        "visual and does not impact game logic, execution of glitches etc."
                    );
                }
            } // END FPS Slider

            if (LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() == LUS::WindowBackend::DX11) {
                UIWidgets::Spacer(0);
                if (ImGui::Button("Match Refresh Rate")) {
                    int hz = LUS::Context::GetInstance()->GetWindow()->GetCurrentRefreshRate();
                    if (hz >= 20 && hz <= 360) {
                        CVarSetInteger("gInterpolationFPS", hz);
                        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                    }
                }
            } else {
                UIWidgets::PaddedEnhancementCheckbox("Match Refresh Rate", "gMatchRefreshRate", true, false);
            }
            UIWidgets::Tooltip("Matches interpolation value to the current game's window refresh rate");

            if (LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() == LUS::WindowBackend::DX11) {
                UIWidgets::PaddedEnhancementSliderInt(CVarGetInteger("gExtraLatencyThreshold", 80) == 0 ? "Jitter fix: Off" : "Jitter fix: >= %d FPS",
                    "##ExtraLatencyThreshold", "gExtraLatencyThreshold", 0, 360, "", 80, true, true, false);
                UIWidgets::Tooltip("When Interpolation FPS setting is at least this threshold, add one frame of input lag (e.g. 16.6 ms for 60 FPS) in order to avoid jitter. This setting allows the CPU to work on one frame while GPU works on the previous frame.\nThis setting should be used when your computer is too slow to do CPU + GPU work in time.");
            }

            UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);

            
            static std::unordered_map<LUS::WindowBackend, const char*> windowBackendNames = {
                { LUS::WindowBackend::DX11, "DirectX" },
                { LUS::WindowBackend::SDL_OPENGL, "OpenGL"},
                { LUS::WindowBackend::SDL_METAL, "Metal" },
                { LUS::WindowBackend::GX2, "GX2"}
            };

            ImGui::Text("Renderer API (Needs reload)");
            LUS::WindowBackend runningWindowBackend = LUS::Context::GetInstance()->GetWindow()->GetWindowBackend();
            LUS::WindowBackend configWindowBackend;
            int configWindowBackendId = LUS::Context::GetInstance()->GetConfig()->GetInt("Window.Backend.Id", -1);
            if (configWindowBackendId != -1 && configWindowBackendId < static_cast<int>(LUS::WindowBackend::BACKEND_COUNT)) {
                configWindowBackend = static_cast<LUS::WindowBackend>(configWindowBackendId);
            } else {
                configWindowBackend = runningWindowBackend;
            }

            if (LUS::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends()->size() <= 1) {
                UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
            }
            if (ImGui::BeginCombo("##RApi", windowBackendNames[configWindowBackend])) {
                for (size_t i = 0; i < LUS::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends()->size(); i++) {
                    auto backend = LUS::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends()->data()[i];
                    if (ImGui::Selectable(windowBackendNames[backend], backend == configWindowBackend)) {
                        LUS::Context::GetInstance()->GetConfig()->SetInt("Window.Backend.Id", static_cast<int>(backend));
                        LUS::Context::GetInstance()->GetConfig()->SetString("Window.Backend.Name",
                                                                            windowBackendNames[backend]);
                        LUS::Context::GetInstance()->GetConfig()->Save();
                    }
                }
                ImGui::EndCombo();
            }
            if (LUS::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends()->size() <= 1) {
                UIWidgets::ReEnableComponent("");
            }

            if (LUS::Context::GetInstance()->GetWindow()->CanDisableVerticalSync()) {
                UIWidgets::PaddedEnhancementCheckbox("Enable Vsync", "gVsyncEnabled", true, false);
            }

            if (LUS::Context::GetInstance()->GetWindow()->SupportsWindowedFullscreen()) {
                UIWidgets::PaddedEnhancementCheckbox("Windowed fullscreen", "gSdlWindowedFullscreen", true, false);
            }

            if (LUS::Context::GetInstance()->GetWindow()->GetGui()->SupportsViewports()) {
                UIWidgets::PaddedEnhancementCheckbox("Allow multi-windows", "gEnableMultiViewports", true, false, false, "", UIWidgets::CheckboxGraphics::Cross, true);
                UIWidgets::Tooltip("Allows windows to be able to be dragged off of the main game window. Requires a reload to take effect.");
            }

            // If more filters are added to LUS, make sure to add them to the filters list here
            ImGui::Text("Texture Filter (Needs reload)");

            static const char* textureFilters[3] = {
#ifdef __WIIU__
                "",
#else
                "Three-Point",
#endif
                "Linear", "None"
            };

            UIWidgets::EnhancementCombobox("gTextureFilter", textureFilters, FILTER_THREE_POINT);

            UIWidgets::Spacer(0);

            LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGameOverlay()->DrawSettings();

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Languages")) {
            if (UIWidgets::EnhancementRadioButton("English", "gLanguages", LANGUAGE_ENG)) {
                GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
            }
            if (UIWidgets::EnhancementRadioButton("German", "gLanguages", LANGUAGE_GER)) {
                GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
            }
            if (UIWidgets::EnhancementRadioButton("French", "gLanguages", LANGUAGE_FRA)) {
                GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
}

void SohMenuBar::DrawElement() {
    if (ImGui::BeginMenuBar()) {
        DrawMenuBarIcon();

        static ImVec2 sWindowPadding(8.0f, 8.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, sWindowPadding);

        DrawShipMenu();

        ImGui::SetCursorPosY(0.0f);

        DrawSettingsMenu();

        ImGui::PopStyleVar(1);
        ImGui::EndMenuBar();
    }
}

} // namespace SohGui 