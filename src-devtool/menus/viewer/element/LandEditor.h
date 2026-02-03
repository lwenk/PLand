#pragma once
#include "components/CodeEditor.h"

#include "pland/Global.h"
#include "pland/land/Land.h"

namespace devtool::viewer {

class LandEditor : public CodeEditor {
    land::WeakLand land_;

    friend class LandCacheViewerWindow;

public:
    explicit LandEditor(land::SharedLand land);

    void renderMenuElement() override;
};

} // namespace devtool::viewer