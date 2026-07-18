/**
 * Add RaceMenu support via DLL injection.
 * Class/Trait tabs are added as new categories with an associated filterFlag that corresponds with new entries in the Race list
 * Classes/Traits entries are added to the Race list with a filterFlag corresponding with the above category flag.
 * This lets the system know that Category X with filterFlag Y should only show entries from the Race list with filterFlag Y.
 */
#pragma once

namespace RaceMenuHandler
{
    class OnItemPressHandler;
    class OnSelectionChangeHandler;

    constexpr int ENTRY_TYPE_CAT    = 0;
    constexpr int ENTRY_TYPE_RACE   = 1;
    constexpr int ENTRY_TYPE_SLIDER = 2;

    class RaceMenu : public Singleton<RaceMenu>
    {
    public:
        RE::GFxValue raceSexPanelsInstance;
        RE::GFxValue racePanel;
        RE::GFxValue categoryList;
        RE::GFxValue itemList;
        RE::GFxValue raceListEntryList;
        RE::GFxValue categoryListEntryList;

        std::string selectedClassCallback;
        std::string selectedTraitCallback;

        bool        bUseClass;
        bool        bUseTrait;
        std::string defaultClass = "";
        std::string defaultTrait = "";
        std::string defaultClassCallback = "";
        std::string defaultTraitCallback = "";

        RE::GPtr<RE::GFxMovieView>         raceSexMovie = nullptr;

        i32  numNewCols         = 0;
        bool bLimitedMenu = false;
        bool categoriesInjected = false;
        bool uiElementsCreated  = false;

        bool Install();

        bool PopulateEntryList(RE::GPtr<RE::GFxMovieView>);
        bool PopulateCategoryList(RE::GPtr<RE::GFxMovieView>);

        bool ReplaceEntryPressHandler(RE::GPtr<RE::GFxMovieView>);
        bool ReplaceSelectionChangeHandler(RE::GPtr<RE::GFxMovieView>);

        RE::GFxValue BuildCategoryEntry(RE::GFxValue*, i32, i32, std::string, i32, i32);
        RE::GFxValue BuildListEntry(RE::GFxValue*, i32, std::string, i32, std::string, i32, i32, std::string, bool, bool);
        bool         CreateUIElements(RE::GPtr<RE::GFxMovieView>);
        void         SendModEvents();
        void         SetDefaultSelections();

    private:
    };

    class OnItemPressHandler : public RE::GFxFunctionHandler
    {
    public:
        RE::GFxValue raceSexPanelsInstance;
        RE::GFxValue racePanel;
        RE::GFxValue itemList;
        RE::GFxValue itemListEntryList;
        RE::GFxValue playerInfoMc;
        RE::GFxValue raceDescriptionMovie;
        RE::GFxValue raceDescriptionField;

        RE::GFxValue _onItemPress;
        RE::GFxValue _updateBottomBar;
        RE::GFxValue _requestUpdate;
        RE::GFxValue loadingIcon;
        bool         isInstalled = false;

        RE::GFxValue traitValue;
        RE::GFxValue classValue;

        // Track selected class and trait callbacks for mod events
        std::string selectedClassCallback;
        std::string selectedTraitCallback;

        bool uiElementsCreated = false;

        OnItemPressHandler(const RE::GFxValue& a_orig) : _onItemPress(a_orig) {}

        void Install();
        void Call(Params& a_params) override;
        void UpdateClassTraitDisplay(const std::string& classText, const std::string& traitText);
        void Reset();
    };

    class OnSelectionChangeHandler : public RE::GFxFunctionHandler
    {
    public:
        RE::GFxValue raceSexPanelsInstance;
        RE::GFxValue racePanel;
        RE::GFxValue raceListEntryList;
        RE::GFxValue raceDescription;
        RE::GFxValue itemList;

        RE::GFxValue _showRaceDescription;
        RE::GFxValue _onSelectionChange;
        bool         isInstalled = false;
        i32          cachedIndex = -1;

        OnSelectionChangeHandler(const RE::GFxValue& a_orig) : _onSelectionChange(a_orig) {}

        void Install();
        void Call(Params& a_params) override;
        void Reset();
    };
} // namespace RaceMenuHandler
