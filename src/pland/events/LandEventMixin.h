#pragma once
#include "ll/api/event/Event.h"

#include <concepts>
#include <memory>
#include <utility>

namespace land {
class Land;
}

namespace land::event {


template <typename T>
class LandEventMixin : public T {
protected:
    static_assert(std::derived_from<T, ll::event::Event>);

    static_assert(
        !ll::traits::is_derived_from_specialization_of_v<T, LandEventMixin>,
        "can't be derived from LandEventMixin twice"
    );

    template <typename... Args>
    explicit LandEventMixin<T>(std::shared_ptr<Land> land, Args&&... args)
    : T(std::forward<Args>(args)...),
      mLand(std::move(land)) {}

    std::shared_ptr<Land> mLand{nullptr};

public:
    [[nodiscard]] inline std::shared_ptr<Land> land() const { return mLand; }
};


} // namespace land::event