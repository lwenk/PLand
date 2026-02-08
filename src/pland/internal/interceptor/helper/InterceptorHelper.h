#pragma once
#include "pland/PLand.h"
#include "pland/land/Land.h"
#include "pland/land/repo/LandRegistry.h"
#include "pland/reflect/TypeName.h"

#include "EventTrace.h"

#include <memory>

namespace land::internal::interceptor {

/**
 * 检查玩家是否拥有特权
 * @param land 领地
 * @param uuid 玩家UUID
 * @return 是否拥有特权
 */
inline bool hasPrivilege(std::shared_ptr<Land> const& land, mce::UUID const& uuid) {
    TRACE_ADD_SCOPE("hasPrivilege");
    TRACE_LOG("land={}", land ? land->getName() : "nullptr");
    if (!land) return true; // 领地不存在 => 放行
    bool isOperator = PLand::getInstance().getLandRegistry().isOperator(uuid);
    bool isOwner    = land->isOwner(uuid);
    TRACE_LOG("isOperator={}, isOwner={}", isOperator, isOwner);
    return isOperator || isOwner;
}

/**
 * 检查环境权限
 * @tparam Member 权限字段
 * @param land 领地
 * @return 是否有权限
 */
template <bool EnvironmentPerms::* Member>
inline bool hasEnvironmentPermission(std::shared_ptr<Land> const& land) {
    TRACE_ADD_SCOPE(reflect::extractFunctionSignature(__FUNCSIG__));
    TRACE_LOG("land={}", land ? land->getName() : "nullptr");
    if (!land) return true; // 领地不存在 => 放行
    bool result = land->getPermTable().environment.*Member;
    TRACE_LOG("{}={}", reflect::extractTemplateInnerLeafName(__FUNCSIG__), result);
    return result;
}

/**
 * 检查玩家成员访客权限
 */
template <RolePerms::Entry RolePerms::* Member>
inline bool hasMemberOrGuestPermission(std::shared_ptr<Land> const& land, mce::UUID const& uuid) {
    TRACE_ADD_SCOPE(reflect::extractFunctionSignature(__FUNCSIG__));
    TRACE_LOG("land={}", land ? land->getName() : "nullptr");
    if (!land) return true; // 领地不存在 => 放行

    auto& ptable = land->getPermTable();
    auto  entry  = ptable.role.*Member;
    TRACE_LOG("{}: member={}, guest={}", reflect::extractTemplateInnerLeafName(__FUNCSIG__), entry.member, entry.guest);
    bool isMember = land->isMember(uuid);
    TRACE_LOG("isMember={}", isMember);
    if (isMember && entry.member) return true;
    return entry.guest;
}


/**
 * 检查玩家角色权限
 * @tparam Member 权限字段
 * @param land 领地
 * @param uuid 玩家 UUID
 * @return 是否有权限
 */
template <RolePerms::Entry RolePerms::* Member>
inline bool hasRolePermission(std::shared_ptr<Land> const& land, mce::UUID const& uuid) {
    TRACE_ADD_SCOPE(reflect::extractFunctionSignature(__FUNCSIG__));
    if (hasPrivilege(land, uuid)) return true;             // 领地不存在 / 管理员 / 主人 => 放行
    return hasMemberOrGuestPermission<Member>(land, uuid); // 成员 / 访客
}

/**
 * 检查访客是否有权限
 * @tparam Member 权限字段
 * @param land 领地
 * @note 此函数仅用于解决某些无法归类的问题
 * @note 如：铜傀儡 + 容器权限, 且容器权限归为角色权限
 * @note 现有权限模型下铜傀儡不是有效的角色类型, 权限也无法划分为环境类权限
 */
template <RolePerms::Entry RolePerms::* Member>
inline bool hasGuestPermission(std::shared_ptr<Land> const& land) {
    TRACE_ADD_SCOPE(reflect::extractFunctionSignature(__FUNCSIG__));
    TRACE_LOG("land={}", land ? land->getName() : "nullptr");
    if (!land) return true; // 领地不存在 => 放行
    auto& ptable = land->getPermTable();
    auto  entry  = ptable.role.*Member;
    TRACE_LOG("{}: member={}, guest={}", reflect::extractTemplateInnerLeafName(__FUNCSIG__), entry.member, entry.guest);
    return entry.guest;
}


} // namespace land::internal::interceptor