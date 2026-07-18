#include "events.h"
#include "racemenu.h"
#include "settings.h"

namespace Events
{

	bool RaceSexMenuWatcher::Register()
    {
        const auto                ui = RE::UI::GetSingleton();
        static RaceSexMenuWatcher watcher;
        ui->AddEventSink(&watcher);
        logger::info("RaceSexMenuWatcher installed.");
        return true;
    }

    RE::BSEventNotifyControl RaceSexMenuWatcher::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
        if (!a_event || a_event->menuName != RE::RaceSexMenu::MENU_NAME)
            return RE::BSEventNotifyControl::kContinue;

        const auto ui = RE::UI::GetSingleton();
        if (!ui)
            return RE::BSEventNotifyControl::kContinue;

        const auto raceMenuHandler = RaceMenuHandler::RaceMenu::GetSingleton();
        if (!raceMenuHandler) {
            return RE::BSEventNotifyControl::kContinue;
        }

        
        if (a_event->opening) {
            if (auto menu{ ui->GetMenu(a_event->menuName) }) {
                if (auto a_movie{ menu->uiMovie.get() }) {

                    const auto settingsHandler = Settings::GetSingleton();
                    settingsHandler->LoadForms();

                    if (raceMenuHandler->Install()) {
                        bIsRaceMenu = true;
                        logger::info("Succesfully installed RaceMenu integration.");
                    }
                    else {
                        logger::info("Failed to install RaceMenu integration. Falling back to vanilla.");
                    }

                    a_movie->Invoke("_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.InitData", nullptr, nullptr, 0);

                }
            }
        }
        else {
            if (bIsRaceMenu) {
                raceMenuHandler->SendModEvents();
            }
        }

        return RE::BSEventNotifyControl::kContinue;
	}
} // namespace Events
