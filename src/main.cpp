#include "Settings.h"
#include "Hooks.h"

#include <stddef.h>


SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    Init(skse);

    const auto plugin{ SKSE::PluginDeclaration::GetSingleton() };
    const auto name{ plugin->GetName() };
    const auto version{ plugin->GetVersion() };

    logger::init();
    logger::info("{} {} is loading...", name, version);
    SKSE::AllocTrampoline(98);

    if (!Settings::LoadJsonFile())
    {
        return false;
    }
    Settings::ParsePreLoadSettings();

    if (!Hooks::InstallHooks()) {
        logger::error("Hook installation failed.");
    }

    logger::info("{} has finished loading.", name);

    return true;
}
