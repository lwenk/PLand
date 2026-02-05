
#include "pland/internal/hooks/EventListener.h"
#include "pland/internal/hooks/listeners/ListenerHelper.h"

#include "ll/api/event/EventBus.h"
#include "ll/api/event/player/PlayerJoinEvent.h"

#include "pland/Global.h"
#include "pland/PLand.h"
#include "pland/drawer/DrawHandleManager.h"
#include "pland/land/Land.h"
#include "pland/land/repo/LandContext.h"
#include "pland/land/repo/LandRegistry.h"
#include "pland/selector/SelectorManager.h"


namespace land {

void EventListener::registerLLSessionListeners() {
    auto* db     = &PLand::getInstance().getLandRegistry();
    auto* bus    = &ll::event::EventBus::getInstance();
    auto* logger = &land::PLand::getInstance().getSelf().getLogger();

    // PlayerJoin and PlayerDisconnect are fundamental and not behind a config flag.
    mListenerPtrs.push_back(bus->emplaceListener<ll::event::PlayerJoinEvent>([db,
                                                                              logger](ll::event::PlayerJoinEvent& ev) {
        if (ev.self().isSimulatedPlayer()) return;
        if (!db->hasPlayerSettings(ev.self().getUuid())) {
            db->setPlayerSettings(ev.self().getUuid(), PlayerSettings{}); // 新玩家
        }

        auto xuid  = ev.self().getXuid();
        auto lands = db->getLandsWhereRaw([&xuid](LandContext const& land) {
            return land.mOwnerDataIsXUID && land.mLandOwner == xuid;
        });

        if (!lands.empty()) {
            logger->info("Update land owner data from xuid to uuid for player {}", ev.self().getRealName());
            auto& uuid = ev.self().getUuid();
            for (auto& land : lands) {
                land->migrateOwner(uuid);
            }
        }
    }));
}

} // namespace land
