#include "ScoreBoardEconomy.h"
#include "pland/PLand.h"

#include "ll/api/service/Bedrock.h"

#include "mc/world/level/Level.h"
#include "mc/world/scores/PlayerScoreSetFunction.h"
#include "mc/world/scores/ScoreInfo.h"
#include "mc/world/scores/ScoreboardOperationResult.h"
#include <mc/world/actor/player/Player.h>
#include <mc/world/scores/Objective.h>
#include <mc/world/scores/Scoreboard.h>
#include <mc/world/scores/ScoreboardId.h>


namespace land::economy::detail {


ScoreBoardEconomy::ScoreBoardEconomy() = default;

llong ScoreBoardEconomy::get(Player& player) const {
    auto& cfg = getConfig();

    Scoreboard& scoreboard = ll::service::getLevel()->getScoreboard();
    Objective*  obj        = scoreboard.getObjective(cfg.scoreboardName);
    if (!obj) {
        land::PLand::getInstance().getSelf().getLogger().error(
            "[ScoreBoardEconomy] Could not find scoreboard: {}",
            cfg.scoreboardName
        );
        return 0;
    }

    ScoreboardId const& id = scoreboard.getScoreboardId(player);
    if (id.mRawID == ScoreboardId::INVALID().mRawID) {
        scoreboard.createScoreboardId(player);
    }

    return obj->getPlayerScore(id).mValue;
}
llong ScoreBoardEconomy::get(mce::UUID const& uuid) const {
    auto player = ll::service::getLevel()->getPlayer(uuid);
    if (!player) {
        land::PLand::getInstance().getSelf().getLogger().error(
            "[ScoreBoardEconomy] Offline operations on the scoreboard are not supported"
        );
        return 0;
    }
    return get(*player);
}

bool ScoreBoardEconomy::set(Player& player, llong amount) const {
    auto& cfg = getConfig();

    Scoreboard& scoreboard = ll::service::getLevel()->getScoreboard();
    Objective*  obj        = scoreboard.getObjective(cfg.scoreboardName);
    if (!obj) {
        land::PLand::getInstance().getSelf().getLogger().error(
            "[ScoreBoardEconomy] Could not find scoreboard: {}",
            cfg.scoreboardName
        );
        return false;
    }
    const ScoreboardId& id = scoreboard.getScoreboardId(player);
    if (id.mRawID == ScoreboardId::INVALID().mRawID) {
        scoreboard.createScoreboardId(player);
    }
    ScoreboardOperationResult result;
    scoreboard.modifyPlayerScore(result, id, *obj, static_cast<int>(amount), PlayerScoreSetFunction::Set);
    return result == ScoreboardOperationResult::Success;
}
bool ScoreBoardEconomy::set(mce::UUID const& uuid, llong amount) const {
    auto player = ll::service::getLevel()->getPlayer(uuid);
    if (!player) {
        land::PLand::getInstance().getSelf().getLogger().error(
            "[ScoreBoardEconomy] Offline operations on the scoreboard are not supported"
        );
        return false;
    }
    return set(*player, amount);
}

bool ScoreBoardEconomy::add(Player& player, llong amount) const {
    auto& cfg = getConfig();

    Scoreboard& scoreboard = ll::service::getLevel()->getScoreboard();
    Objective*  obj        = scoreboard.getObjective(cfg.scoreboardName);
    if (!obj) {
        land::PLand::getInstance().getSelf().getLogger().error(
            "[ScoreBoardEconomy] Could not find scoreboard: {}",
            cfg.scoreboardName
        );
        return false;
    }
    const ScoreboardId& id = scoreboard.getScoreboardId(player);
    if (id.mRawID == ScoreboardId::INVALID().mRawID) {
        scoreboard.createScoreboardId(player);
    }
    ScoreboardOperationResult result;
    scoreboard.modifyPlayerScore(result, id, *obj, static_cast<int>(amount), PlayerScoreSetFunction::Add);
    return result == ScoreboardOperationResult::Success;
}
bool ScoreBoardEconomy::add(mce::UUID const& uuid, llong amount) const {
    auto player = ll::service::getLevel()->getPlayer(uuid);
    if (!player) {
        land::PLand::getInstance().getSelf().getLogger().error(
            "[ScoreBoardEconomy] Offline operations on the scoreboard are not supported"
        );
        return false;
    }
    return add(*player, amount);
}

bool ScoreBoardEconomy::reduce(Player& player, llong amount) const {
    auto& cfg = getConfig();

    Scoreboard& scoreboard = ll::service::getLevel()->getScoreboard();
    Objective*  obj        = scoreboard.getObjective(cfg.scoreboardName);
    if (!obj) {
        land::PLand::getInstance().getSelf().getLogger().error(
            "[ScoreBoardEconomy] Could not find scoreboard: {}",
            cfg.scoreboardName
        );
        return false;
    }
    const ScoreboardId& id = scoreboard.getScoreboardId(player);
    if (id.mRawID == ScoreboardId::INVALID().mRawID) {
        scoreboard.createScoreboardId(player);
    }
    ScoreboardOperationResult result;
    scoreboard.modifyPlayerScore(result, id, *obj, static_cast<int>(amount), PlayerScoreSetFunction::Subtract);
    return result == ScoreboardOperationResult::Success;
}
bool ScoreBoardEconomy::reduce(mce::UUID const& uuid, llong amount) const {
    auto player = ll::service::getLevel()->getPlayer(uuid);
    if (!player) {
        land::PLand::getInstance().getSelf().getLogger().error(
            "[ScoreBoardEconomy] Offline operations on the scoreboard are not supported"
        );
        return false;
    }
    return reduce(*player, amount);
}

bool ScoreBoardEconomy::transfer(Player& from, Player& to, llong amount) const {
    if (!reduce(from, amount)) {
        return false;
    }
    if (!add(to, amount)) {
        (void)add(from, amount); // rollback
        return false;
    }
    return true;
}
bool ScoreBoardEconomy::transfer(mce::UUID const& from, mce::UUID const& to, llong amount) const {
    auto fromPlayer = ll::service::getLevel()->getPlayer(from);
    auto toPlayer   = ll::service::getLevel()->getPlayer(to);

    if (!fromPlayer || !toPlayer) {
        land::PLand::getInstance().getSelf().getLogger().error(
            "[ScoreBoardEconomy] Offline operations on the scoreboard are not supported"
        );
        return false;
    }
    return transfer(*fromPlayer, *toPlayer, amount);
}


} // namespace land::economy::detail