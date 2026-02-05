#include "pland/PLand.h"
#include "BuildInfo.h"

#include <memory>

#include "ll/api/Versions.h"
#include "ll/api/data/Version.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/ListenerBase.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/io/LogLevel.h"
#include "ll/api/mod/RegisterHelper.h"
#include "ll/api/thread/ThreadPoolExecutor.h"
#include "ll/api/utils/SystemUtils.h"

#include "drawer/DrawHandleManager.h"
#include "events/domain/ConfigReloadEvent.h"
#include "land/internal/LandScheduler.h"
#include "land/internal/SafeTeleport.h"
#include "pland/economy/EconomySystem.h"
#include "pland/internal/adapter/telemetry/Telemetry.h"
#include "pland/internal/command/Command.h"
#include "pland/internal/hooks/EventListener.h"
#include "pland/land/Config.h"
#include "pland/land/repo/LandRegistry.h"
#include "pland/selector/SelectorManager.h"
#include "pland/service/ServiceLocator.h"


#ifdef LD_DEVTOOL
#include "DevToolApp.h"
#endif

namespace land {


struct PLand::Impl {
    ll::mod::NativeMod&                             mSelf;
    std::unique_ptr<ll::thread::ThreadPoolExecutor> mThreadPoolExecutor{nullptr};
    std::unique_ptr<LandRegistry>                   mLandRegistry{nullptr};
    std::unique_ptr<internal::LandScheduler>        mLandScheduler{nullptr};
    std::unique_ptr<EventListener>                  mEventListener{nullptr};
    std::unique_ptr<internal::SafeTeleport>         mSafeTeleport{nullptr};
    std::unique_ptr<SelectorManager>                mSelectorManager{nullptr};
    std::unique_ptr<DrawHandleManager>              mDrawHandleManager{nullptr};
    std::unique_ptr<internal::adapter::Telemetry>   mTelemetry{nullptr};

    ll::event::ListenerPtr mConfigReloadListener{nullptr};

    std::unique_ptr<service::ServiceLocator> mServiceLocator{nullptr};

#ifdef LD_DEVTOOL
    std::unique_ptr<devtool::DevToolApp> mDevToolApp{nullptr};
#endif

    explicit Impl() : mSelf(*ll::mod::NativeMod::current()) {}
};


bool PLand::load() {
    auto& logger = getSelf().getLogger();
    logger.info("{}-{}-{}", BuildInfo::Tag, BuildInfo::Branch, BuildInfo::Commit);
    if (BuildInfo::Branch != "main") {
        logger.warn("This is a development build. It may not be stable and may contain bugs.");
    }
    if (auto res = ll::i18n::getInstance().load(getSelf().getLangDir()); !res) {
        logger.error("Load language file failed, plugin will use default language.");
        res.error().log(logger);
    }

    Config::tryLoad();
    logger.setLevel(Config::cfg.logLevel);

    mImpl->mThreadPoolExecutor = std::make_unique<ll::thread::ThreadPoolExecutor>("PLand-ThreadPool", 2);

    mImpl->mLandRegistry = std::make_unique<land::LandRegistry>();
    EconomySystem::getInstance().initialize();

#ifdef DEBUG
    logger.warn("Debug Mode");
    logger.setLevel(ll::io::LogLevel::Trace);
#endif

    return true;
}

bool PLand::enable() {
    internal::LandCommand::setup();
    mImpl->mLandScheduler     = std::make_unique<internal::LandScheduler>();
    mImpl->mEventListener     = std::make_unique<EventListener>();
    mImpl->mSafeTeleport      = std::make_unique<internal::SafeTeleport>();
    mImpl->mSelectorManager   = std::make_unique<SelectorManager>();
    mImpl->mDrawHandleManager = std::make_unique<DrawHandleManager>();
    mImpl->mTelemetry         = std::make_unique<internal::adapter::Telemetry>();
    if (Config::cfg.internal.telemetry) {
        mImpl->mTelemetry->launch(*getThreadPool());
    }

    mImpl->mServiceLocator = std::make_unique<service::ServiceLocator>(*this);

    mImpl->mConfigReloadListener = ll::event::EventBus::getInstance().emplaceListener<events::ConfigReloadEvent>(
        [this](events::ConfigReloadEvent& ev [[maybe_unused]]) {
            mImpl->mEventListener.reset();
            mImpl->mEventListener = std::make_unique<EventListener>();

            EconomySystem::getInstance().reload();

            if (ev.config().internal.telemetry) {
                mImpl->mTelemetry->launch(*getThreadPool());
            } else {
                mImpl->mTelemetry->shutdown();
            }

            mImpl->mDrawHandleManager.reset();
            mImpl->mDrawHandleManager = std::make_unique<DrawHandleManager>();
        }
    );

#ifdef LD_DEVTOOL
    if (land::Config::cfg.internal.devTools) {
        mImpl->mDevToolApp = devtool::DevToolApp::make();
    }
#endif

    return true;
}

bool PLand::disable() {
#ifdef LD_DEVTOOL
    if (Config::cfg.internal.devTools) {
        mImpl->mDevToolApp.reset();
    }
#endif
    ll::event::EventBus::getInstance().removeListener(mImpl->mConfigReloadListener);

    auto& logger = mImpl->mSelf.getLogger();
    mImpl->mTelemetry.reset();

    mImpl->mServiceLocator.reset();

    logger.debug("Saving land registry...");
    mImpl->mLandRegistry->save();

    logger.debug("Destroying resources...");
    mImpl->mLandScheduler.reset();
    mImpl->mEventListener.reset();
    mImpl->mSafeTeleport.reset();
    mImpl->mSelectorManager.reset();
    mImpl->mDrawHandleManager.reset();
    mImpl->mLandRegistry.reset();

    logger.debug("Destroying thread pool...");
    mImpl->mThreadPoolExecutor->destroy();
    mImpl->mThreadPoolExecutor.reset();
    return true;
}

bool PLand::unload() { return true; }

PLand& PLand::getInstance() {
    static PLand instance;
    return instance;
}

PLand::PLand() : mImpl(std::make_unique<Impl>()) {}

ll::mod::NativeMod&     PLand::getSelf() const { return mImpl->mSelf; }
SelectorManager*        PLand::getSelectorManager() const { return mImpl->mSelectorManager.get(); }
LandRegistry&           PLand::getLandRegistry() const { return *mImpl->mLandRegistry; }
DrawHandleManager*      PLand::getDrawHandleManager() const { return mImpl->mDrawHandleManager.get(); }
internal::SafeTeleport& PLand::getSafeTeleport() const { return *mImpl->mSafeTeleport; }

ll::thread::ThreadPoolExecutor* PLand::getThreadPool() const { return mImpl->mThreadPoolExecutor.get(); }
service::ServiceLocator&        PLand::getServiceLocator() const { return *mImpl->mServiceLocator; }

#ifdef LD_DEVTOOL
void PLand::setDevToolVisible(bool visible) {
    if (Config::cfg.internal.devTools) {
        if (visible) {
            mImpl->mDevToolApp->show();
        } else {
            mImpl->mDevToolApp->hide();
        }
    }
}
#endif

} // namespace land

LL_REGISTER_MOD(land::PLand, land::PLand::getInstance());
