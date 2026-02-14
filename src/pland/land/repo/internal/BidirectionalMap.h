#pragma once
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"

#include <type_traits>


namespace land::internal {

template <typename T>
using ValueType = std::conditional_t<std::is_arithmetic_v<T> || std::is_enum_v<T>, T, T const&>;

/**
 * @brief 双向映射表
 * @tparam K 键
 * @tparam V 值
 */
template <typename K, typename V>
class BidirectionalMap {
public:
    using ValuesSet = absl::flat_hash_set<V>; // Key -> Values 值集合
    using KeysSet   = absl::flat_hash_set<K>; // Value -> Keys 键集合

    using ForwardMap = absl::flat_hash_map<K, ValuesSet>; // Key -> Values
    using ReverseMap = absl::flat_hash_map<V, KeysSet>;   // Value -> Keys

    using KeyType   = ValueType<K>;
    using ValueType = ValueType<V>;

    BidirectionalMap() = default;

    ValuesSet& forward(KeyType key) { return mForwardMap[key]; }

    KeysSet& reverse(ValueType value) { return mReverseMap[value]; }

    ValuesSet const& forward_at(KeyType key) const { return mForwardMap.at(key); }
    KeysSet const&   reverse_at(ValueType value) const { return mReverseMap.at(value); }

    /**
     * @brief 插入键值对到双向表
     */
    void insert(KeyType key, ValueType value) {
        mForwardMap[key].insert(value);
        mReverseMap[value].insert(key);
    }

    void erase_value(KeyType key, ValueType value) {
        if (auto it = mForwardMap.find(key); it != mForwardMap.end()) {
            it->second.erase(value); // 删除正向表容器里的值
            if (it->second.empty()) mForwardMap.erase(it);
        }
        if (auto it = mReverseMap.find(value); it != mReverseMap.end()) {
            it->second.erase(key); // 删除反向表容器里的值
            if (it->second.empty()) mReverseMap.erase(it);
        }
    }

    void erase_key(KeyType key, ValueType value) {
        mForwardMap.erase(key);
        mReverseMap.erase(value);
    }

    bool contains(KeyType key, ValueType value) const {
        auto itF = mForwardMap.find(key);
        if (itF == mForwardMap.end() || !itF->second.contains(value)) return false;

        auto itR = mReverseMap.find(value);
        return itR != mReverseMap.end() && itR->second.contains(key);
    }

    bool has_key(KeyType key) const { return mForwardMap.contains(key); }
    bool has_value(ValueType val) const { return mReverseMap.contains(val); }

    ForwardMap const& forward_map() const { return mForwardMap; }
    ReverseMap const& reverse_map() const { return mReverseMap; }

private:
    ForwardMap mForwardMap;
    ReverseMap mReverseMap;
};


} // namespace land::internal