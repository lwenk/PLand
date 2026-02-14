#pragma once
#include "repo/LandContext.h"


namespace land {


class LandTemplatePermTable {
public:
    LDAPI explicit LandTemplatePermTable(LandPermTable permTable);

    LDAPI LandPermTable const& get() const;

    LDAPI void set(LandPermTable const& permTable);

    LDAPI bool isDirty() const;
    LDAPI void markDirty();
    LDAPI void resetDirty();

private:
    std::atomic_bool mDirty{false};
    LandPermTable    mTemplatePermTable;
};


} // namespace land