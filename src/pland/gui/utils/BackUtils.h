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

inline constexpr auto Text      = "back";
inline constexpr auto ImageData = "textures/ui/icon_import";
inline constexpr auto ImageType = "path";

inline void injectBackButton(SimpleForm& f, SimpleForm::ButtonCallback callback) {
    f.appendButton(Text, ImageData, ImageType, std::move(callback));
}

template <auto Fn, typename... Args>
inline void injectBackButton(SimpleForm& f, Args&&... args) {
    auto cb = wrapCallback<Fn, Args...>(std::forward<Args>(args)...);
    injectBackButton(f, std::move(cb));
}

template <typename T>
concept Injectable = requires(T t, std::string const& n, SimpleForm::ButtonCallback cb) {
    { t.appendButton(n, n, n, cb) } -> std::convertible_to<T&>;
};

template <Injectable T>
inline void injectBackButton(T& f, SimpleForm::ButtonCallback callback) {
    f.appendButton(Text, ImageData, ImageType, std::move(callback));
}

} // namespace land::gui::back_utils