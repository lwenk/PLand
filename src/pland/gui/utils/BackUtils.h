#pragma once
#include "ll/api/form/SimpleForm.h"

#include <tuple>
#include <type_traits>

namespace land::gui::back_utils {

using ll::form::SimpleForm;

template <auto Fn, typename... Args>
SimpleForm::ButtonCallback wrapCallback(Args&&... args) {
    if constexpr (sizeof...(Args) == 0 && std::is_convertible_v<decltype(Fn), SimpleForm::ButtonCallback>) {
        return Fn; // 无参且可转换
    }
    return [args = std::make_tuple(std::forward<Args>(args)...)](Player& player) mutable {
        std::apply(
            [&player](auto&&... unpacked) { Fn(player, std::forward<decltype(unpacked)>(unpacked)...); },
            std::move(args)
        );
    };
}

inline void injectBackButton(SimpleForm& f, SimpleForm::ButtonCallback callback) {
    f.appendButton("back", "textures/ui/icon_import", "path", std::move(callback));
}

template <auto Fn, typename... Args>
inline void injectBackButton(SimpleForm& f, Args&&... args) {
    auto cb = wrapCallback<Fn, Args...>(std::forward<Args>(args)...);
    injectBackButton(f, std::move(cb));
}


} // namespace land::gui::back_utils