#include "settings.h"
#include <simpleini.h>

void Settings::LoadSettings()
{
    CSimpleIniA ini;
    ini.SetUnicode();
    ini.LoadFile(R"(.\Data\SKSE\Plugins\Apprentice.ini)");

    bUseClass              = ini.GetBoolValue("", "bRaceMenuUseClass", true);
    bUseTrait              = ini.GetBoolValue("", "bRaceMenuUseTrait", true);
    MAG_ClassTrackerFormID = ini.GetValue("", "sMAG_ClassTrackerFormID", "");
    MAG_TraitTrackerFormID = ini.GetValue("", "sMAG_TraitTrackerFormID", "");
    FileName = ini.GetValue("", "sModFileName", "");
}

RE::FormID Settings::ParseFormID(const std::string& str)
{
    RE::FormID         result;
    std::istringstream ss{ str };
    ss >> std::hex >> result;
    return result;
}

void Settings::LoadForms()
{
    logger::info("Loading forms");
    auto        dataHandler = RE::TESDataHandler::GetSingleton();

    MAG_ClassTracker = dataHandler->LookupForm(ParseFormID(MAG_ClassTrackerFormID), FileName)->As<RE::TESGlobal>();
    MAG_TraitTracker = dataHandler->LookupForm(ParseFormID(MAG_TraitTrackerFormID), FileName)->As<RE::TESGlobal>();

    logger::info("Forms loaded");
}
