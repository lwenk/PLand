#pragma once
#include "components/IComponent.h"

namespace devtool::helper::element {


class AboutWindow final : public IWindow {
public:
    explicit AboutWindow();

    void render() override;
};


class About final : public IMenuElement {
    std::unique_ptr<AboutWindow> window_;

public:
    explicit About();

    bool* getSelectFlag() override;
    bool  isSelected() const override;

    void tick() override;
};

} // namespace devtool::helper::element