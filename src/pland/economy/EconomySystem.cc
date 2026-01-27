#include "pland/economy/EconomySystem.h"
#include "pland/PLand.h"
#include "pland/economy/impl/EmtpyEconomy.h"
#include "pland/economy/impl/LegacyMoneyEconomy.h"
#include "pland/economy/impl/ScoreBoardEconomy.h"
#include "pland/infra/Config.h"
#include <memory>
#include <stdexcept>


namespace land {


std::shared_ptr<economy::IEconomy> EconomySystem::createEconomySystem() const {
    auto& cfg = getConfig();
    if (!cfg.enabled) {
        PLand::getInstance().getSelf().getLogger().debug("using internals::EmptyEconomy");
        return std::make_shared<economy::detail::EmtpyEconomy>();
    }

    switch (cfg.kit) {
    case EconomyKit::LegacyMoney: {
        PLand::getInstance().getSelf().getLogger().debug("using internals::LegacyMoneyEconomy");
        return std::make_shared<economy::detail::LegacyMoneyEconomy>();
    }
    case EconomyKit::ScoreBoard: {
        PLand::getInstance().getSelf().getLogger().debug("using internals::ScoreBoardEconomy");
        return std::make_shared<economy::detail::ScoreBoardEconomy>();
    }
    }

    throw std::runtime_error("Unknown economy kit, please check config!");
}

EconomySystem& EconomySystem::getInstance() {
    static EconomySystem instance;
    return instance;
}

std::shared_ptr<economy::IEconomy> EconomySystem::getEconomyInterface() const {
    std::lock_guard lock(mInstanceMutex);
    if (!mEconomySystem) {
        throw std::runtime_error("internals::IEconomyInterface not initialized.");
    }
    return mEconomySystem;
}

EconomyConfig& EconomySystem::getConfig() const { return Config::cfg.economy; }

void EconomySystem::initEconomySystem() {
    std::lock_guard lock(mInstanceMutex);
    if (!mEconomySystem) {
        mEconomySystem = createEconomySystem();
    }
}
void EconomySystem::reloadEconomySystem() {
    std::lock_guard lock(mInstanceMutex);
    mEconomySystem = createEconomySystem();
}


EconomySystem::EconomySystem() = default;

std::shared_ptr<economy::IEconomy> EconomySystem::operator->() const { return mEconomySystem; }


} // namespace land
