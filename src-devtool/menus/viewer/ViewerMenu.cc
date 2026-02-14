#include "ViewerMenu.h"

#include "element/LandCacheViewer.h"

namespace devtool::viewer {

ViewerMenu::ViewerMenu() : IMenu("Viewer") { this->registerElement<LandCacheViewer>(); }

} // namespace devtool::viewer