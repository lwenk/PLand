#pragma once
#include "components/CodeEditor.h"
#include <memory>

namespace land {
class Land;
}
namespace devtool::viewer {

class LandEditor : public CodeEditor {
    std::weak_ptr<land::Land> land_;

    friend class LandCacheViewerWindow;

public:
    explicit LandEditor(std::shared_ptr<land::Land> land);

    void renderMenuElement() override;
};

} // namespace devtool::viewer