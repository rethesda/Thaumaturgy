#pragma once

#include "Settings.h"

namespace DisenchantXpPatch
{
    REL::Relocation<std::uintptr_t> disFuncBase_Hook{ REL::RelocationID(50459, 51363) };
    bool                            InstallDisenchantHook();
    void                            AddDisenchantSkill(RE::PlayerCharacter* player, RE::ActorValue actorVal, float fAmount, RE::InventoryEntryData* item);

    struct DisenchantPatch : Xbyak::CodeGenerator
    {
        DisenchantPatch()
        {
            Xbyak::Label xpFuncLabel;
            Xbyak::Label returnLabel;

            mov(r9, r14);
            sub(rsp, 0x20);
            call(ptr[rip + xpFuncLabel]);
            add(rsp, 0x20);

            jmp(ptr[rip + returnLabel]);

            L(xpFuncLabel);
            dq(reinterpret_cast<std::uintptr_t>(AddDisenchantSkill));

            L(returnLabel);
            dq(disFuncBase_Hook.address() + 0xC0);
        }
    };

    void AddDisenchantSkill(RE::PlayerCharacter* player, RE::ActorValue actorVal, float fAmount, RE::InventoryEntryData* item)
    {
        if (item && (item->object->GetFormType() == RE::FormType::Weapon)) {
            auto* weapon      = item->object->As<RE::TESObjectWEAP>();
            auto* baseEnchant = weapon->formEnchanting->data.baseEnchantment;

            float xpAmount = 1.0f;
            if (baseEnchant) {
                std::int32_t cost = baseEnchant->data.costOverride;
                xpAmount          = cost * Settings::fDisenchantingWeaponExpMult;
            }

            logger::debug("Disenchant weapon: Adding {} xp", xpAmount);

            player->AddSkillExperience(actorVal, xpAmount);
        }
        else {
            auto* form = item->object->As<RE::TESEnchantableForm>();
            if (form) {
                auto* baseEnchant = form->formEnchanting->data.baseEnchantment;
                float xpAmount    = 1.0f;
                if (baseEnchant) {
                    std::int32_t cost = baseEnchant->data.costOverride;
                    xpAmount          = cost * Settings::fDisenchantingArmorExpMult;
                }

                logger::debug("Disenchant armor: Adding {} xp", xpAmount);

                player->AddSkillExperience(actorVal, xpAmount);
            }
            else {
                logger::debug("Not weapon or armor");
                player->AddSkillExperience(actorVal, fAmount);
            }
        }
    }

    bool InstallDisenchantHook()
    {
        DisenchantPatch code;
        code.ready();

        auto& trampoline = SKSE::GetTrampoline();
        trampoline.write_branch<6>(disFuncBase_Hook.address() + 0xBA, trampoline.allocate(code));
        logger::debug("Disenchant hook installed");
        return true;
    }
}
