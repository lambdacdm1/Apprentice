class RaceMenu: public Singleton<RaceMenu>
{
public:
	RE::GFxValue racePanel;
	RE::GFxValue categoryList;
	RE::GFxValue itemList;
	
	RE::GFxValue itemListEntryList;
	RE::GFxValue categoryListEntryList;

	std::string_view RACESEX_MENU = RE::RaceSexMenu::MENU_NAME;
	RE::GPtr<RE::GFxMovieView> raceSexMovie;

	int numNewCols = 2;
			
	bool GetRaceSexMenu();
	bool GetCategoryList();
	bool GetRaceList();

	bool PopulateRaceList();
	bool PopulateCategoryList();
	
	RE::GFxValue BuildCategoryEntry(RE::GPtr<RE::GFxMovieView> a_movie, i32 typeInt, i32 filterInt, std::string_view textString, i32 flagInt, i32 priorityInt);
	void BuildTraitObject();
private:
};