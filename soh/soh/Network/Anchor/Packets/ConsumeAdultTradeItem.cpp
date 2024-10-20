#ifdef ENABLE_REMOTE_CONTROL

#include "soh/Network/Anchor/Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/OTRGlobals.h"

extern "C" {
#include "functions.h"
#include "soh/Enhancements/randomizer/adult_trade_shuffle.h"
extern PlayState* gPlayState;
}

/**
 * CONSUME_ADULT_TRADE_ITEM
 * 
 * This is primarily to just get rid of used adult trade items to prevent confusion for other players. 
 * Whatever flags/items are given from adult trade checks are synced by other packets.
 */

void Anchor::SendPacket_ConsumeAdultTradeItem(u8 itemId) {
    if (!IsSaveLoaded()) {
        return;
    }

    nlohmann::json payload;
    payload["type"] = CONSUME_ADULT_TRADE_ITEM;
    payload["targetTeamId"] = CVarGetString(CVAR_REMOTE_ANCHOR("TeamId"), "default");
    payload["addToQueue"] = true;
    payload["itemId"] = itemId;

    SendJsonToRemote(payload);
}

void Anchor::HandlePacket_ConsumeAdultTradeItem(nlohmann::json payload) {
    if (!IsSaveLoaded()) {
        return;
    }

    uint8_t itemId = payload["itemId"].get<uint8_t>();
    gSaveContext.ship.quest.data.randomizer.adultTradeItems &= ~ADULT_TRADE_FLAG(itemId);
    Inventory_ReplaceItem(gPlayState, itemId, Randomizer_GetNextAdultTradeItem());
}

#endif // ENABLE_REMOTE_CONTROL
