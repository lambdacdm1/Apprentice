#pragma once

class Settings : public Singleton<Settings>
{
public:
    void LoadSettings();
    void LoadForms();

    static RE::FormID ParseFormID(const std::string& str);

    bool           bUseClass;
    bool           bUseTrait;
    std::string FileName;
    std::string MAG_ClassTrackerFormID;
    std::string MAG_TraitTrackerFormID;
    RE::TESGlobal* MAG_ClassTracker;
    RE::TESGlobal* MAG_TraitTracker;
};
