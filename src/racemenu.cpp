#include "racemenu.h"
#include "jsonhandler.h"
#include "settings.h"
#include "utils.h"

#include <array>
#include <sstream>

i32 classFlag = 1u << 29;
i32 traitFlag = 1u << 30;

namespace RaceMenuHandler
{
    /* Inject Scaleform elements into RaceMenu UI*/
    bool RaceMenu::Install()
    {
        if (const auto ui{ RE::UI::GetSingleton() }) {
            if (const auto menu{ ui->GetMenu(RE::RaceSexMenu::MENU_NAME) }) {
                if (auto a_movie{ menu->uiMovie }) {
                    racePanel             = RE::GFxValue{};
                    raceSexPanelsInstance = RE::GFxValue{};
                    raceListEntryList     = RE::GFxValue{};
                    categoryListEntryList = RE::GFxValue{};
                    
                    categoriesInjected = false;
                    uiElementsCreated  = false;

                    if (!a_movie->GetVariable(&racePanel, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.racePanel")) {
                        // Element only present in RaceMenu, element not found -> cannot install RaceMenu
                        return false;
                    }
                    a_movie->GetVariable(&raceSexPanelsInstance, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance");
                    a_movie->GetVariable(&raceListEntryList, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.racePanel.itemList.entryList");
                    a_movie->GetVariable(&categoryListEntryList, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.racePanel.slidingCategoryList.categoryList.entryList");

                    bLimitedMenu = Utils::GetBooleanMember(raceSexPanelsInstance, "bLimitedMenu");

                    const auto settingsHandler = Settings::GetSingleton();
                    bUseClass                  = settingsHandler->bUseClass;
                    bUseTrait                  = settingsHandler->bUseTrait;

                    if (!bLimitedMenu) {
                        SetDefaultSelections();
                        ReplaceEntryPressHandler(a_movie);
                        ReplaceSelectionChangeHandler(a_movie);
                        PopulateCategoryList(a_movie);
                        PopulateEntryList(a_movie);
                        CreateUIElements(a_movie);
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /* Add Classes/Traits to Race list (helps leverage existing Scaleform functions for Race)  */
    bool RaceMenu::PopulateEntryList(RE::GPtr<RE::GFxMovieView> a_movie)
    {
        const u32                       size = raceListEntryList.GetArraySize();
        static JSONHandler::GetJSONData getJSONData;

        auto      classEntryJSON = getJSONData.LoadJSON("Data/SKSE/Plugins/app_classes.json");
        auto      traitEntryJSON = getJSONData.LoadJSON("Data/SKSE/Plugins/app_traits.json");
        u32 newSize        = size;
        if (bUseClass)
            newSize += classEntryJSON.size();
        if (bUseTrait)
            newSize += traitEntryJSON.size();

        // Expand RaceList by numClassSize + numTraitSize
        raceListEntryList.SetArraySize(newSize);

        // Add Class entries
        if (bUseClass) {
            for (auto i = 0; i < classEntryJSON.size(); i++) {
                RE::GFxValue classEntry;

                json el = classEntryJSON[i];

                a_movie->CreateObject(&classEntry);

                // Populate the classEntry
                BuildListEntry(&classEntry, ENTRY_TYPE_RACE, el["Name"], classFlag, el["Description"], 0, i, el["UniqueKey"], true, false);

                // Set the classEntry in the entryList
                raceListEntryList.SetElement(size + i, classEntry);

                RE::GFxValue _onItemPress;
                raceSexPanelsInstance.GetMember("onItemPress", &_onItemPress);
            }
        }

        // Add Trait entries
        if (bUseTrait) {
            for (auto i = 0; i < traitEntryJSON.size(); i++) {
                RE::GFxValue traitEntry;

                json el = traitEntryJSON[i];

                a_movie->CreateObject(&traitEntry);

                // Populate the traitEntry
                BuildListEntry(&traitEntry, ENTRY_TYPE_RACE, el["Name"], traitFlag, el["Description"], 0, 0, el["UniqueKey"], false, true);

                // Set the traitEntry in the entryList
                raceListEntryList.SetElement(size + classEntryJSON.size() + i, traitEntry);
            }
        }

        logger::info("Class and Trait entryList Injected");

        return true;
    }

    /* Add new categories to list of tabs*/
    bool RaceMenu::PopulateCategoryList(RE::GPtr<RE::GFxMovieView> a_movie)
    {
        const u32 size    = categoryListEntryList.GetArraySize();
        const u32 newSize = size + bUseClass + bUseTrait;

        categoryListEntryList.SetArraySize(newSize);

        RE::GFxValue classEntry;
        RE::GFxValue traitEntry;

        a_movie->CreateObject(&classEntry);
        a_movie->CreateObject(&traitEntry);

        if (bUseClass && bUseTrait) {
            BuildCategoryEntry(&classEntry, 0, 1, "$APPCLASS", classFlag, -970);
            BuildCategoryEntry(&traitEntry, 0, 1, "$APPTRAIT", traitFlag, -965);
            categoryListEntryList.SetElement(newSize - 1, classEntry);
            categoryListEntryList.SetElement(newSize - 2, traitEntry);
        }
        if (bUseClass && !bUseTrait) {
            BuildCategoryEntry(&classEntry, 0, 1, "$APPCLASS", classFlag, -970);
            categoryListEntryList.SetElement(newSize - 1, classEntry);
        }
        if (!bUseClass && bUseTrait) {
            BuildCategoryEntry(&traitEntry, 0, 1, "$APPTRAIT", traitFlag, -965);
            categoryListEntryList.SetElement(newSize - 1, traitEntry);
        }

        logger::info("CategoryList Injected");
        categoriesInjected = true;

        return true;
    }

    /* Replace the handler for when a Race selection is clicked */
    bool RaceMenu::ReplaceEntryPressHandler(RE::GPtr<RE::GFxMovieView> a_movie)
    {
        // Remove RaceMenu onItemPress listener
        RE::GFxValue result;
        RE::GFxValue argsRemove[3];
        argsRemove[0].SetString("itemPress");
        argsRemove[1] = raceSexPanelsInstance;
        argsRemove[2].SetString("onItemPress");
        raceSexPanelsInstance.Invoke("removeEventListener", &result, argsRemove, 3);
        logger::info("Removed Event Listener");

        // Stash the original
        RE::GFxValue _onItemPress;
        raceSexPanelsInstance.GetMember("onItemPress", &_onItemPress);
        raceSexPanelsInstance.SetMember("__onItemPress", _onItemPress);
        RE::GFxValue onItemPress = nullptr;

        RE::GPtr<OnItemPressHandler> onItemPressHandler;
        onItemPressHandler = RE::make_gptr<OnItemPressHandler>(_onItemPress);

        a_movie->CreateFunction(&onItemPress, onItemPressHandler.get());
        raceSexPanelsInstance.SetMember("onItemPress", onItemPress);

        // Replace with new onItemPress listener
        RE::GFxValue argsAdd[3];
        argsAdd[0].SetString("itemPress");
        argsAdd[1] = raceSexPanelsInstance;
        argsAdd[2].SetString("onItemPress");
        raceSexPanelsInstance.Invoke("addEventListener", &result, argsAdd, 3);
        logger::info("Readded new Event Listener");

        return true;
    }

    /* Replace the handler for when a Race selection is highlighted */
    bool RaceMenu::ReplaceSelectionChangeHandler(RE::GPtr<RE::GFxMovieView> a_movie)
    {
        // Remove RaceMenu onSelectionChangeHandler listener
        RE::GFxValue result;
        RE::GFxValue argsRemove[3];
        argsRemove[0].SetString("selectionChange");
        argsRemove[1] = raceSexPanelsInstance;
        argsRemove[2].SetString("onSelectionChange");
        raceSexPanelsInstance.Invoke("removeEventListener", &result, argsRemove, 3);
        logger::info("Removed Event Listener");

        // Stash the original
        RE::GFxValue _onSelectionChange;
        raceSexPanelsInstance.GetMember("onSelectionChange", &_onSelectionChange);
        raceSexPanelsInstance.SetMember("__onSelectionChange", _onSelectionChange);
        RE::GFxValue onSelectionChange = nullptr;

        RE::GPtr<OnSelectionChangeHandler> onSelectionChangeHandler;
        onSelectionChangeHandler = RE::make_gptr<OnSelectionChangeHandler>(_onSelectionChange);

        a_movie->CreateFunction(&onSelectionChange, onSelectionChangeHandler.get());
        raceSexPanelsInstance.SetMember("onSelectionChange", onSelectionChange);

        // Replace with new onSelectionChange listener
        RE::GFxValue argsAdd[3];
        argsAdd[0].SetString("selectionChange");
        argsAdd[1] = raceSexPanelsInstance;
        argsAdd[2].SetString("onSelectionChange");
        raceSexPanelsInstance.Invoke("addEventListener", &result, argsAdd, 3);
        logger::info("Readded new Event Listener");

        return true;
    }

    /* Build Scaleform element for new category */
    RE::GFxValue RaceMenu::BuildCategoryEntry(RE::GFxValue* a_entry, i32 typeInt, i32 filterInt, std::string textString, i32 flagInt, i32 priorityInt)
    {
        Utils::AddBooleanMember(a_entry, false, "bDontHide");
        Utils::AddIntMember(a_entry, filterInt, "filterFlag");
        Utils::AddStringMember(a_entry, textString.data(), "text");
        Utils::AddIntMember(a_entry, flagInt, "flag");
        Utils::AddIntMember(a_entry, priorityInt, "priority");
        Utils::AddBooleanMember(a_entry, true, "enabled");

        return a_entry;
    }

    /* Build Scaleform element for new list entry */
    RE::GFxValue RaceMenu::BuildListEntry(RE::GFxValue* a_entry, i32 type, std::string text, i32 filterFlag, std::string raceDesc, i32 equipState, i32 raceID,
                                          std::string callbackName, bool isClass, bool isTrait)
    {
        Utils::AddIntMember(a_entry, type, "type");
        Utils::AddStringMember(a_entry, text.data(), "text");
        Utils::AddIntMember(a_entry, filterFlag, "filterFlag");
        Utils::AddStringMember(a_entry, callbackName.data(), "callbackName");
        Utils::AddStringMember(a_entry, raceDesc.data(), "raceDescription");
        Utils::AddIntMember(a_entry, equipState, "equipState");
        Utils::AddIntMember(a_entry, raceID, "raceID");
        Utils::AddBooleanMember(a_entry, true, "enabled");
        Utils::AddBooleanMember(a_entry, isClass, "isClass");
        Utils::AddBooleanMember(a_entry, isTrait, "isTrait");

        return a_entry;
    }

    /* Add misc UI elements */
    bool RaceMenu::CreateUIElements(RE::GPtr<RE::GFxMovieView> a_movie)
    {
        if (!a_movie) {
            return false;
        }

        RE::GFxValue playerInfoMc;
        if (!a_movie->GetVariable(&playerInfoMc, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.bottomBar.playerInfo")) {
            logger::warn("Could not get PlayerInfo_mc");
            return false;
        }

        // Pull existing BottomBar fields to duplicate from
        RE::GFxValue playerNameLabel;
        if (!a_movie->GetVariable(&playerNameLabel, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.bottomBar.playerInfo.NameLabel")) {
            logger::warn("Could not get PlayerNameLabel TextField for reference");
            return false;
        }

        RE::GFxValue playerNameValue;
        if (!a_movie->GetVariable(&playerNameValue, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.bottomBar.playerInfo.PlayerName")) {
            logger::warn("Could not get PlayerNameValue TextField for reference");
            return false;
        }

        RE::GFxValue xPos;
        RE::GFxValue yPos;
        RE::GFxValue width;
        RE::GFxValue height;
        RE::GFxValue depth;
        RE::GFxValue textFormat;

        RE::GFxValue traitLabel;
        RE::GFxValue traitValue;
        RE::GFxValue classLabel;
        RE::GFxValue classValue;
        
        RE::GFxValue argsCreate[6];
        RE::GFxValue fmtArgs[1];

        // Create Trait Label
        if (bUseTrait) {
            playerNameLabel.GetMember("_x", &xPos);
            playerNameLabel.GetMember("_y", &yPos);
            playerNameLabel.GetMember("_width", &width);
            playerNameLabel.GetMember("_height", &height);
            playerNameLabel.Invoke("getTextFormat", &textFormat, nullptr, 0);
            playerInfoMc.Invoke("getNextHighestDepth", &depth, nullptr, 0);

            argsCreate[0].SetString("TraitLabel");
            argsCreate[1].SetNumber(depth.GetNumber() + 10);
            argsCreate[2].SetNumber(xPos.GetNumber() - 150);
            argsCreate[3].SetNumber(yPos.GetNumber());
            argsCreate[4].SetNumber(width.GetNumber());
            argsCreate[5].SetNumber(height.GetNumber());
            playerInfoMc.Invoke("createTextField", nullptr, argsCreate, 6);

            playerInfoMc.GetMember("TraitLabel", &traitLabel);
            Utils::AddStringMember(&traitLabel, "$APPTRAIT", "text");

            fmtArgs[0] = textFormat;
            traitLabel.Invoke("setTextFormat", nullptr, fmtArgs, 1);
            traitLabel.Invoke("setNewTextFormat", nullptr, fmtArgs, 1);
        }

        // Create Trait Value
        playerNameValue.GetMember("_x", &xPos);
        playerNameValue.GetMember("_y", &yPos);
        playerNameValue.GetMember("_width", &width);
        playerNameValue.GetMember("_height", &height);
        playerNameValue.Invoke("getTextFormat", &textFormat, nullptr, 0);
        playerInfoMc.Invoke("getNextHighestDepth", &depth, nullptr, 0);

        argsCreate[0].SetString("TraitValue");
        argsCreate[1].SetNumber(depth.GetNumber() + 10);
        argsCreate[2].SetNumber(xPos.GetNumber() - 150);
        argsCreate[3].SetNumber(yPos.GetNumber());
        argsCreate[4].SetNumber(width.GetNumber());
        argsCreate[5].SetNumber(height.GetNumber());
        playerInfoMc.Invoke("createTextField", nullptr, argsCreate, 6);

        playerInfoMc.GetMember("TraitValue", &traitValue);
        Utils::AddStringMember(&traitValue, defaultTrait.c_str(), "text");

        fmtArgs[0] = textFormat;
        traitValue.Invoke("setTextFormat", nullptr, fmtArgs, 1);
        traitValue.Invoke("setNewTextFormat", nullptr, fmtArgs, 1);

        // Create Class Label
        if (bUseClass) {
            playerNameLabel.GetMember("_x", &xPos);
            playerNameLabel.GetMember("_y", &yPos);
            playerNameLabel.GetMember("_width", &width);
            playerNameLabel.GetMember("_height", &height);
            playerNameLabel.Invoke("getTextFormat", &textFormat, nullptr, 0);
            playerInfoMc.Invoke("getNextHighestDepth", &depth, nullptr, 0);

            argsCreate[0].SetString("ClassLabel");
            argsCreate[1].SetNumber(depth.GetNumber() + 11);
            argsCreate[2].SetNumber(xPos.GetNumber() - 300);
            argsCreate[3].SetNumber(yPos.GetNumber());
            argsCreate[4].SetNumber(width.GetNumber());
            argsCreate[5].SetNumber(height.GetNumber());
            playerInfoMc.Invoke("createTextField", nullptr, argsCreate, 6);

            playerInfoMc.GetMember("ClassLabel", &classLabel);
            Utils::AddStringMember(&classLabel, "$APPCLASS", "text");

            fmtArgs[0] = textFormat;
            classLabel.Invoke("setTextFormat", nullptr, fmtArgs, 1);
            classLabel.Invoke("setNewTextFormat", nullptr, fmtArgs, 1);
        }

        // Create Class Value
        playerNameValue.GetMember("_x", &xPos);
        playerNameValue.GetMember("_y", &yPos);
        playerNameValue.GetMember("_width", &width);
        playerNameValue.GetMember("_height", &height);
        playerNameValue.Invoke("getTextFormat", &textFormat, nullptr, 0);
        playerInfoMc.Invoke("getNextHighestDepth", &depth, nullptr, 0);

        argsCreate[0].SetString("ClassValue");
        argsCreate[1].SetNumber(depth.GetNumber() + 10);
        argsCreate[2].SetNumber(xPos.GetNumber() - 300);
        argsCreate[3].SetNumber(yPos.GetNumber());
        argsCreate[4].SetNumber(width.GetNumber());
        argsCreate[5].SetNumber(height.GetNumber());
        playerInfoMc.Invoke("createTextField", nullptr, argsCreate, 6);

        playerInfoMc.GetMember("ClassValue", &classValue);
        Utils::AddStringMember(&classValue, defaultClass.c_str(), "text");

        fmtArgs[0] = textFormat;
        classValue.Invoke("setTextFormat", nullptr, fmtArgs, 1);
        classValue.Invoke("setNewTextFormat", nullptr, fmtArgs, 1);

        uiElementsCreated = true;
        logger::info("Class/Trait UI elements created successfully");

        return true;
    }

    /* Trigger callback */
    void RaceMenu::SendModEvents()
    {
        const auto settingsHandler = Settings::GetSingleton();

        // Send mod events for selected class and trait
        if (!selectedClassCallback.empty()) {
            Utils::SendModEvent("ClassMenu_Callback", selectedClassCallback, 1.0f);
            logger::info("Sent ClassMenu_Callback mod event with value: {}", selectedClassCallback);
            defaultClassCallback  = selectedClassCallback;
            selectedClassCallback = "";
        }
        else if (settingsHandler->MAG_ClassTracker) {
            Utils::SendModEvent("ClassMenu_Callback", defaultClassCallback, 1.0f);
            logger::info("Sent fallback ClassMenu_Callback mod event with value: {}", defaultClassCallback);
        }


        if (!selectedTraitCallback.empty()) {
            Utils::SendModEvent("TraitMenu_Callback", selectedTraitCallback, 1.0f);
            logger::info("Sent TraitMenu_Callback mod event with value: {}", selectedTraitCallback);
            defaultTraitCallback  = selectedTraitCallback;
            selectedTraitCallback = "";
        }
        else if (settingsHandler->MAG_TraitTracker) {
            Utils::SendModEvent("TraitMenu_Callback", defaultTraitCallback, 1.0f);
            logger::info("Sent fallback TraitMenu_Callback mod event with value: {}", defaultTraitCallback);
        }
    }

    /* Check TESGlobal for pre-existing selections */
    void RaceMenu::SetDefaultSelections()
    {
        const auto  settingsHandler      = Settings::GetSingleton();
        static JSONHandler::GetJSONData getJSONData;

        auto classEntryJSON = getJSONData.LoadJSON("Data/SKSE/Plugins/app_classes.json");
        auto traitEntryJSON = getJSONData.LoadJSON("Data/SKSE/Plugins/app_traits.json");

        if (settingsHandler->MAG_ClassTracker) {
            int                classTracker = static_cast<int>(settingsHandler->MAG_ClassTracker->value);
            if (classTracker > 0) {
                std::ostringstream previousClassSS;
                previousClassSS << "CLASS" << std::setw(3) << std::setfill('0') << classTracker;

                defaultClassCallback = previousClassSS.str();
                defaultClass         = classEntryJSON[classTracker - 1]["Name"];
            }
        }

        if (settingsHandler->MAG_TraitTracker) {
            int                traitTracker = static_cast<int>(settingsHandler->MAG_TraitTracker->value);

            if (traitTracker > 0) {
                std::ostringstream previousTraitSS;
                previousTraitSS << "TRAIT" << std::setw(3) << std::setfill('0') << traitTracker;

                defaultTraitCallback = previousTraitSS.str();
                defaultTrait         = traitEntryJSON[traitTracker - 1]["Name"];
            }
            
        }
    }

    /* Install ItemPress handler */
    void OnItemPressHandler::Install()
    {
        if (const auto ui{ RE::UI::GetSingleton() }) {
            if (const auto menu{ ui->GetMenu(RE::RaceSexMenu::MENU_NAME) }) {
                if (const auto a_movie{ menu->uiMovie }) {
                    a_movie->GetVariable(&raceSexPanelsInstance, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance");
                    a_movie->GetVariable(&itemListEntryList, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.racePanel.itemList.entryList");
                    a_movie->GetVariable(&racePanel, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.racePanel");
                    a_movie->GetVariable(&playerInfoMc, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.bottomBar.playerInfo");
                    a_movie->GetVariable(&raceDescriptionMovie, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.racePanel.raceDescription");

                    // Cache useful members
                    racePanel.GetMember("itemList", &itemList);
                    racePanel.GetMember("updateBottomBar", &_updateBottomBar);
                    // raceDescriptionMovie.GetMember("textField", &raceDescriptionField);
                    itemList.GetMember("requestUpdate", &_requestUpdate);
                    isInstalled = true;

                    playerInfoMc.GetMember("TraitValue", &traitValue);
                    playerInfoMc.GetMember("ClassValue", &classValue);
                }
            }
        }
    }

    void OnItemPressHandler::Call(Params& a_params)
    {
        assert(a_params.argCount >= 1);
        auto& event = a_params.args[0];

        if (!isInstalled) {
            Install();
            logger::info("OnItemPressHandler Installed");
        }

        RE::GFxValue listState;
        itemList.GetMember("listState", &listState);

        RE::GFxValue activeEntry;
        listState.GetMember("activeEntry", &activeEntry);

        RE::GFxValue indexVal;
        event.GetMember("index", &indexVal);
        i32 index = static_cast<i32>(indexVal.GetNumber());

        RE::GFxValue pressedEntry;
        itemListEntryList.GetElement(index, &pressedEntry);

        RE::GFxValue typeVal;
        pressedEntry.GetMember("type", &typeVal);
        i32 type = static_cast<i32>(typeVal.GetNumber());

        RE::GFxValue result;

        bool isClass = Utils::GetBooleanMember(pressedEntry, "isClass");
        bool isTrait = Utils::GetBooleanMember(pressedEntry, "isTrait");

        // Check if Class/Trait entry
        if (Utils::IsClassOrTrait(pressedEntry)) {
            // Store the callback for later mod event dispatch
            if (pressedEntry.HasMember("callbackName")) {
                RE::GFxValue callbackVal;
                pressedEntry.GetMember("callbackName", &callbackVal);
                std::string callback = callbackVal.GetString();
                logger::info("callback {}", callback);

                const auto raceMenuInjector = RaceMenuHandler::RaceMenu::GetSingleton();

                if (isClass) {
                    raceMenuInjector->selectedClassCallback = callback;
                    logger::info("Selected Class callback: {}", callback);
                }
                if (isTrait) {
                    raceMenuInjector->selectedTraitCallback = callback;
                    logger::info("Selected Trait callback: {}", callback);
                }
            }

            // Update the UI display with the selected name
            RE::GFxValue nameVal;
            pressedEntry.GetMember("text", &nameVal);
            std::string selectedName = nameVal.GetString();

            if (isClass) {
                UpdateClassTraitDisplay(selectedName, "");
            }
            else if (isTrait) {
                UpdateClassTraitDisplay("", selectedName);
            }

            listState.SetMember("activeEntry", pressedEntry);

            itemList.Invoke("requestUpdate", &result, nullptr, 0);
            racePanel.Invoke("updateBottomBar", &result, nullptr, 0);

            RE::GFxValue sliderID;
            double       sliderIDVal = 0.0;
            if (pressedEntry.GetMember("sliderID", &sliderID)) {
                sliderIDVal = sliderID.GetNumber();
            }

            RE::GFxValue raceID;
            double       raceIDVal = 0.0;

            if (pressedEntry.GetMember("raceID", &raceID)) {
                raceIDVal = raceID.GetNumber();
            }

            RE::GFxValue result;
            RE::GFxValue argsShow[2];
            argsShow[0].SetNumber(raceIDVal);
            argsShow[1].SetNumber(sliderIDVal);
            argsShow[0].SetBoolean(true);

            raceSexPanelsInstance.Invoke("ChangeRace", &result, argsShow, 0);
            return;
        }

        raceSexPanelsInstance.Invoke("__onItemPress", &result, a_params.args, a_params.argCount);
        logger::info("Delegated to original onItemPress");
    }

    /* Set display text for Class/Trait */
    void OnItemPressHandler::UpdateClassTraitDisplay(const std::string& classText, const std::string& traitText)
    {
        const auto raceMenuInjector = RaceMenuHandler::RaceMenu::GetSingleton();
        const auto       dataHandler      = RE::TESDataHandler::GetSingleton();
        const auto settingsHandler  = Settings::GetSingleton();

        if (!classText.empty()) {
            Utils::AddStringMember(&classValue, classText.c_str(), "text");
            logger::info("Updated Class display to: {}", classText);
        }

        if (!traitText.empty()) {
            Utils::AddStringMember(&traitValue, traitText.c_str(), "text");
            logger::info("Updated Trait display to: {}", traitText);
        }

    }

    /* Install SelectionChange handler */
    void OnSelectionChangeHandler::Install()
    {
        if (const auto ui{ RE::UI::GetSingleton() }) {
            if (const auto menu{ ui->GetMenu(RE::RaceSexMenu::MENU_NAME) }) {
                if (const auto a_movie{ menu->uiMovie }) {
                    a_movie->GetVariable(&raceSexPanelsInstance, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance");
                    a_movie->GetVariable(&racePanel, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.racePanel");
                    a_movie->GetVariable(&raceListEntryList, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.racePanel.itemList.entryList");
                    a_movie->GetVariable(&raceDescription, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.raceDescription.textField");

                    // Cache useful
                    racePanel.GetMember("itemList", &itemList);

                    isInstalled = true;
                    logger::info("OnSelectionChangeHandler Installed");
                }
            }
        }
    }

    void OnSelectionChangeHandler::Call(Params& a_params)
    {
        assert(a_params.argCount >= 1);
        auto& event = a_params.args[0];

        if (!isInstalled) {
            Install();
        }

        RE::GFxValue indexVal;
        event.GetMember("index", &indexVal);
        i32 index = static_cast<i32>(indexVal.GetNumber());

        if (index == cachedIndex) {
            return;
        }
        cachedIndex = index;

        RE::GFxValue listState;
        if (!itemList.GetMember("listState", &listState)) {
            logger::info("no listState");
            return;
        }

        RE::GFxValue selectedEntry;
        if (!raceListEntryList.GetElement(index, &selectedEntry)) {
            return;
        }


        RE::GFxValue catFlag;
        if (selectedEntry.GetMember("flag", &catFlag)) {
            return;
        }

        RE::GFxValue filterFlag;
        if (!selectedEntry.GetMember("filterFlag", &filterFlag)) {
            logger::info("No filterFlag");
            return;
        }

        i32 filterFlagVal = Utils::GetIntMember(selectedEntry, "filterFlag");

        RE::GFxValue result;
        raceSexPanelsInstance.Invoke("__onSelectionChange", &result, a_params.args, a_params.argCount);

        if (filterFlagVal > 0) {
            if ((filterFlagVal & classFlag) || (filterFlagVal & traitFlag)) {
                std::string_view descriptionVal = Utils::GetStringMember(selectedEntry, "raceDescription");
                raceDescription.SetText(descriptionVal.data());

                RE::GFxValue argsShow[1];
                argsShow[0].SetBoolean(true);
                raceSexPanelsInstance.Invoke("ShowRaceDescription", &result, argsShow, 1);
            }
        }
    }
} // namespace RaceMenuHandler
