#include "racemenu.h"
#include "utils.h"

bool RaceMenu::GetRaceSexMenu() {
	if (const auto ui{ RE::UI::GetSingleton() }) {
		if (auto menu{ ui->GetMenu(RACESEX_MENU) }) {
			if (auto a_movie{ menu->uiMovie }) {
				if (const auto scaleform{ SKSE::GetScaleformInterface() }) {
					raceSexMovie = a_movie;
					a_movie->GetVariable(&racePanel, "_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.racePanel");
					return true;
				}
			}
		}
	}
	return false;
}

bool RaceMenu::GetRaceList() {
	if (racePanel.GetMember("itemList", &itemList)) {
		if (itemList.GetMember("entryList", &itemListEntryList)) {
			return true;
		}
	}
	return false;
}

bool RaceMenu::GetCategoryList() {
	RE::GFxValue slidingCategoryList;
	if (racePanel.GetMember("slidingCategoryList", &slidingCategoryList)) {
		if (slidingCategoryList.GetMember("categoryList", &categoryList)) {
			if (categoryList.GetMember("entryList", &categoryListEntryList)) {
				logger::info("CategoryList found");
				return true;
			}
		}
	}
	return false;
}

bool RaceMenu::PopulateRaceList() {
	const auto size = itemListEntryList.GetArraySize();
	for (u32 i = size; i > 0; i--) {
		RE::GFxValue entry;
		if (!itemListEntryList.GetElement(i - 1, &entry))
			continue;

		RE::GFxValue entryText;
		if (!entry.GetMember("text", &entryText))
			continue;

		const std::string text = entryText.GetString();
		if (text.empty())
			continue;

		logger::info("El: {}", text);

	}
	return true;
}

bool RaceMenu::PopulateCategoryList() {
	if (GetCategoryList()) {
		const auto size = categoryListEntryList.GetArraySize();

		categoryListEntryList.SetArraySize(size + numNewCols);
		
		for (u32 i = size + numNewCols; i > numNewCols; i--) {
			RE::GFxValue entry;

			if (!categoryListEntryList.GetElement(i - numNewCols - 1, &entry))
				continue;

			RE::GFxValue entryText;
			entry.GetMember("text", &entryText);
			std::string text = entryText.GetString();
			logger::info("Moving entry: {} from {} to {}", text, i - numNewCols - 1, i);

			RE::GFxValue flag;
			entry.GetMember("flag", &flag);

			i32 val = flag.GetNumber();
			i32 newVal = val * 2 * numNewCols;
			logger::info("Got Flag {}", val);
			flag.SetNumber(newVal);
			logger::info("Set Flag {}", val * 2 * numNewCols);

			categoryListEntryList.SetElement(i, entry);
		}
		logger::info("Expanded CategoryList");

		RE::GFxValue classEntry = BuildCategoryEntry(raceSexMovie, 0, 1, "$CLASS", 4, -999);
		RE::GFxValue traitEntry = BuildCategoryEntry(raceSexMovie, 0, 1, "$TRAIT", 8, -998);

		categoryListEntryList.SetElement(1, classEntry);
		categoryListEntryList.SetElement(2, traitEntry);

		logger::info("Built new entries");
		for (u32 i = size + numNewCols; i > 0; i--) {
			RE::GFxValue entry;
			if (!categoryListEntryList.GetElement(i - 1, &entry))
				continue;

			RE::GFxValue entryText;
			if (!entry.GetMember("text", &entryText))
				continue;

			const std::string text = entryText.GetString();
			logger::info("Category entry: {}", text);
		}
	}
	return true;
	
}

RE::GFxValue RaceMenu::BuildCategoryEntry(RE::GPtr<RE::GFxMovieView> a_movie, i32 typeInt, i32 filterInt, std::string_view textString, i32 flagInt, i32 priorityInt) {
	RE::GFxValue entry;

	a_movie->CreateObject(&entry);

	AddBooleanMember(&entry, typeInt, "type");
	AddBooleanMember(&entry, false, "bDontHide");
	AddIntMember(&entry, filterInt, "filterFlag");
	AddStringMember(&entry, textString, "text");
	AddIntMember(&entry, flagInt, "flag");
	AddIntMember(&entry, priorityInt, "priority");
	AddBooleanMember(&entry, true, "enabled");

	return entry;
}