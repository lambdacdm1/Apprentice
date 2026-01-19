#include "events.h"
#include "interface.h"
#include "racemenu.h"

namespace Events
{
	RE::BSEventNotifyControl RaceSexMenuWatcher::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		if (!a_event || !a_event->opening)
			return RE::BSEventNotifyControl::kContinue;

			if (auto ui{ RE::UI::GetSingleton() }) {
				if (auto menu{ ui->GetMenu(RE::RaceSexMenu::MENU_NAME) }) {
					if (auto movie{ menu->uiMovie.get() }) {
						const auto scaleform = SKSE::GetScaleformInterface();
						movie->Invoke("_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.InitData", nullptr, nullptr, 0);
						//bGetJSONDataRegistered = true;
					}
				}
			}

		return RE::BSEventNotifyControl::kContinue;
	}

	void RaceSexMenuWatcher::Register()
	{
		if (auto ui{ RE::UI::GetSingleton() }) {
			if ( auto dataHandler { RE::TESDataHandler::GetSingleton()}) {
				
				// Separate out RaceMenu support
				if (dataHandler->LookupLoadedModByName("RaceMenu.esp")) {
					static RaceMenuWatcher watcher;
					ui->AddEventSink(&watcher);

				}
				else {
					static RaceSexMenuWatcher watcher;
					ui->AddEventSink(&watcher);
				}
			}
		}
	}

	RE::BSEventNotifyControl RaceMenuWatcher::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		if (!a_event || !a_event->opening)
			return RE::BSEventNotifyControl::kContinue;

		if (const auto raceMenuInjector{ RaceMenu::GetSingleton() }) {
			if (raceMenuInjector->GetRaceSexMenu()) {
				raceMenuInjector->PopulateCategoryList();
			}
		}

		return RE::BSEventNotifyControl::kContinue;
	}
}