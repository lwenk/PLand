#include "HelperMenu.h"

#include "element/About.h"

namespace devtool::helper {

HelperMenu::HelperMenu() : IMenu("帮助") { this->registerElement<element::About>(); }

} // namespace devtool::helper