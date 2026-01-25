#pragma once

class Settings
{
public:
    inline static float fDisenchantingArmorExpMult;
    inline static float fDisenchantingWeaponExpMult;
    inline static float fPettyMult;
    inline static float fLesserMult;
    inline static float fCommonMult;
    inline static float fGreaterMult;
    inline static float fGrandMult;

	using json = nlohmann::json;

	Settings() = delete;
    static bool         LoadJsonFile();
    static bool         ParsePreLoadSettings();
    static bool         ParsePostLoadSettings();
    static RE::BGSPerk* FindPerk(json file, std::string perkSectionName);
    static RE::FormID    ParseFormID(const std::string& str);

    inline static bool  debug_logging{};

	static inline json settingsJson;

	static inline constexpr char FILE_NAME[] = "Data\\SKSE\\Plugins\\Thaumaturgy.json";
};
