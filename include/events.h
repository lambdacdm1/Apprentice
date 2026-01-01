#pragma once

namespace Events
{
	class RaceSexMenuWatcher : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
	{
	public:
		RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*);
		static void Register();

		bool bGetJSONDataRegistered = false;
	};

}