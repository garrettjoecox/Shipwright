#ifdef ENABLE_REMOTE_CONTROL

#ifdef __cplusplus
#include <libultraship/libultraship.h>

#include "z64actor.h"
#include "./GameInteractor.h"

typedef struct {
    uint32_t clientId;
    std::string clientVersion;
    std::string name;
    Color_RGB8 color;
    uint8_t scene;
    PosRot posRot;
} AnchorClient;

typedef struct {
    uint32_t clientId;
    std::string clientVersion;
    std::string name;
    Color_RGB8 color;
} AnchorClientWithoutPos;

class GameInteractorAnchor {
    private:
        bool isEnabled;

        void HandleRemoteJson(nlohmann::json payload);
    public:
        static GameInteractorAnchor* Instance;
        static std::map<uint32_t, AnchorClient> AnchorClients;
        static std::vector<uint32_t> FairyIndexToClientId;
        static std::string clientVersion;

        void Enable();
        void Disable();

        void TransmitJsonToRemote(nlohmann::json payload);
};
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

void Anchor_RegisterHooks();
void Anchor_PushSaveStateToRemote();
void Anchor_RequestSaveStateFromRemote();
uint8_t Anchor_GetClientScene(uint32_t fairyIndex);
PosRot Anchor_GetClientPosition(uint32_t fairyIndex);
Color_RGB8 Anchor_GetClientColor(uint32_t fairyIndex);
void Anchor_SpawnClientFairies();
void Anchor_SkipLocation(uint32_t locationIndex, bool skipped);
void Anchor_UpdateBeansBought(uint8_t amount);
void Anchor_ConsumeAdultTradeItem(uint8_t itemId);
void Anchor_UpdateKeyCount(int16_t sceneNum, int8_t amount);

#ifdef __cplusplus
}
#endif
