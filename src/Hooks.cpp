#include "Hooks.h"
#include "patches/DisenchantXpPatch.h"
#include "patches/EnchantXpPatch.h"

namespace Hooks
{
    bool InstallHooks()
    {
        DisenchantXpPatch::InstallDisenchantHook();
        EnchantXpPatch::InstallEnchantHook();
        return true;
    }
}
