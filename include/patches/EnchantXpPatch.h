#pragma once

#include "Settings.h"

namespace EnchantXpPatch
{
    REL::Relocation<std::uintptr_t> encFuncBase_Hook{ REL::RelocationID(50450, 51355) };
    void                            AddEnchantSkill(RE::PlayerCharacter* player, RE::ActorValue actorVal, float fAmount, RE::TESForm* soulGem);

    struct EnchantPatch : Xbyak::CodeGenerator
    {
        EnchantPatch()
        {
            Xbyak::Label xpFuncLabel;
            Xbyak::Label returnLabel;

            mov(r11, ptr[rbx + 0x18]);
            mov(r9, ptr[r11]);
            sub(rsp, 0x20);
            call(ptr[rip + xpFuncLabel]);
            add(rsp, 0x20);

            jmp(ptr[rip + returnLabel]);

            L(xpFuncLabel);
            dq(reinterpret_cast<std::uintptr_t>(AddEnchantSkill));

            L(returnLabel);
            dq(encFuncBase_Hook.address() + 0x279);
        }
    };

    void AddEnchantSkill(RE::PlayerCharacter* player, RE::ActorValue actorVal, float fAmount, RE::TESForm* soulGem)
    {
        float xpMult = 1.0f;

        if (soulGem && soulGem->GetFormType() == RE::FormType::SoulGem) {
            logger::debug("Soul gem soul level used {}", soulGem->GetName());

            auto* gemForm     = soulGem->As<RE::TESSoulGem>();
            auto  currentSoul = gemForm->GetContainedSoul();

            switch (currentSoul) {
            case RE::SOUL_LEVEL::kPetty:
                xpMult = Settings::fPettyMult;
                break;
            case RE::SOUL_LEVEL::kLesser:
                xpMult = Settings::fLesserMult;
                break;
            case RE::SOUL_LEVEL::kCommon:
                xpMult = Settings::fCommonMult;
                break;
            case RE::SOUL_LEVEL::kGreater:
                xpMult = Settings::fGreaterMult;
                break;
            case RE::SOUL_LEVEL::kGrand:
                xpMult = Settings::fGrandMult;
                break;
            }

            logger::debug("Using mult {}", xpMult);

            logger::debug("Adding {} XP", xpMult * fAmount);
            player->AddSkillExperience(actorVal, fAmount * xpMult);
        }
        else {
            logger::debug("Not soul gem");

            player->AddSkillExperience(actorVal, xpMult);
        }
    }

    bool InstallEnchantHook()
    {
        EnchantPatch code;
        code.ready();
        auto& trampoline = SKSE::GetTrampoline();
        trampoline.write_branch<6>(encFuncBase_Hook.address() + 0x273, trampoline.allocate(code));
        logger::info("Enchant hook installed");
        return true;
    }

}
