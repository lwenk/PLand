#pragma once
#include "components/IComponent.h"
#include <GLFW/glfw3.h>
#include <concepts>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace devtool {

class DevToolApp final {
    struct Impl;
    std::unique_ptr<Impl> impl;

public:
    explicit DevToolApp();
    ~DevToolApp();

    DevToolApp(DevToolApp&)                  = delete;
    DevToolApp& operator=(const DevToolApp&) = delete;

public:
    void show() const;

    void hide() const;

    bool visible() const;

    void appendError(std::string msg);

    template <typename T, typename... Args>
        requires std::derived_from<T, IMenu> && std::is_final_v<T>
    void registerMenu(Args&&... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        this->registerMenu(std::move(ptr));
    }

    void registerMenu(std::unique_ptr<IMenu> menu);

    [[nodiscard]] static std::unique_ptr<DevToolApp> make();
};


} // namespace devtool
