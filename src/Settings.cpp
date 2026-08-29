#include "Settings.h"

bool Settings::LoadJsonFile()
{
    std::ifstream inFile(FILE_NAME);
    if (!inFile.is_open()) {
        logger::error("Failed to open Thaumaturgy.json!");
        return false;
    }

    logger::info("Parsing json");

    try {
        std::string json_str((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();

        settingsJson = nlohmann::json::parse(json_str);
        return true;
    }
    catch (const std::exception& e) {
        logger::critical(FMT_STRING("Failed to load Thaumaturgy.json!: {}"), e.what());
        return false;
    }
}

bool Settings::ParsePreLoadSettings()
{
    try {
        logger::info("Parsing pre load settings");

        if (settingsJson == nullptr) {
            logger::critical("No Thaumaturgy.json found!");
            return false;
        }

        auto debugLogs = Settings::settingsJson.value("DebugLogs", false);
        logger::info("\tDebug log: {}", debugLogs);
        if (debugLogs) {
            spdlog::default_logger()->set_level(spdlog::level::debug);
            spdlog::default_logger()->flush_on(spdlog::level::debug);
            logger::debug("Debug logging enabled");
        }

        if (settingsJson.contains("DisenchantExperienceMults")) {
            auto& disXpMults          = settingsJson["DisenchantExperienceMults"];
            fDisenchantingArmorExpMult  = disXpMults.value("fDisenchantingArmorExpMult", 0.01f);
            fDisenchantingWeaponExpMult = disXpMults.value("fDisenchantingWeaponExpMult", 0.15f);
        }

        bUseSoulGemMults = false;

        if (settingsJson.contains("SoulGemEnchantMults")) {
            auto& soulGemMults          = settingsJson["SoulGemEnchantMults"];
            fPettyMult                  = soulGemMults.value("fPettyMult", 1.0f);
            fLesserMult                 = soulGemMults.value("fLesserMult", 1.25f);
            fCommonMult                 = soulGemMults.value("fCommonMult", 1.5f);
            fGreaterMult                = soulGemMults.value("fGreaterMult", 1.75f);
            fGrandMult                  = soulGemMults.value("fGrandMult", 2.0f);
            
            logger::info("fDisenchantingArmorExpMult: {}", std::to_string(fDisenchantingArmorExpMult));
            logger::info("fDisenchantingWeaponExpMult: {}", std::to_string(fDisenchantingWeaponExpMult));
            logger::info("fEnchantPettyMult: {}", std::to_string(fPettyMult));
            logger::info("fEnchantLesserMult: ", std::to_string(fLesserMult));
            logger::info("fEnchantCommonMult: ", std::to_string(fCommonMult));
            logger::info("fEnchantGreaterMult: ", std::to_string(fGreaterMult));
            logger::info("fEnchantGrandMult: ", std::to_string(fGrandMult));
            bUseSoulGemMults = true;
        }


        logger::info("Settings loaded");
        
        return true;
    }
    catch (const std::exception& e) {
        logger::critical(FMT_STRING("Failed to parse Thaumaturgy.json!: {}"), e.what());
        return false;
    }
}

bool Settings::ParsePostLoadSettings()
{
    return true;
}

RE::BGSPerk* Settings::FindPerk(json file, std::string perkSectionName)
{
    auto perkSection = file[perkSectionName].get<json>();
    auto formId      = perkSection.value("formid", "");
    auto fileName    = perkSection.value("filename", "");

    if (formId.empty() && fileName.empty()) {
        logger::info("Perk formId or filename were empty");
        return nullptr;
    }

    auto parsedId = ParseFormID(formId);

    if (!parsedId) {
        logger::error("Could not parse perk formId");
        return nullptr;
    }

    logger::info(FMT_STRING("Parsed formId: {}"), parsedId);
    logger::info(FMT_STRING("Looking up form in: {}"), fileName);

    auto form = RE::TESDataHandler::GetSingleton()->LookupForm(parsedId, fileName);

    if (!form) {
        logger::error("Could not find perk");
        return nullptr;
    }

    logger::info("Found perk");

    RE::BGSPerk* foundPerk = skyrim_cast<RE::BGSPerk*>(form);
    return foundPerk;
}

RE::FormID Settings::ParseFormID(const std::string& str)
{
    RE::FormID         result;
    std::istringstream ss{ str };
    ss >> std::hex >> result;
    return result;
}
