#include "LandRefundFailedEvent.h"

#include "pland/events/Helper.h"

namespace land {
namespace event {


mce::UUID const& LandRefundFailedEvent::targetPlayer() const { return mTargetPlayer; }
int64_t          LandRefundFailedEvent::refundAmount() const { return mRefundAmount; }

IMPLEMENT_EVENT_EMITTER(LandRefundFailedEvent);

} // namespace event
} // namespace land