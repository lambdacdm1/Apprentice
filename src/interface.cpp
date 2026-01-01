#pragma once

#include "interface.h"
#include "jsonhandler.h"

GetJSONData* getJSONData = nullptr;

bool ScaleformInjector::InstallHooks(RE::GFxMovieView* a_view, RE::GFxValue*)
{

	RE::GFxValue globals;
	std::string swfName = a_view->GetMovieDef()->GetFileURL();
		
	logger::debug("{}", swfName);
	bool result = a_view->GetVariable(&globals, "_global");
	if (result && swfName == "Interface/RaceSex_menu.swf") {
		RE::GFxValue LAM;
		a_view->CreateObject(&LAM);

		RE::GFxValue fnValue;

		if (getJSONData == nullptr) {
			getJSONData = new GetJSONData;
		}

		a_view->CreateFunction(&fnValue, getJSONData);
		LAM.SetMember("GetJSONData", fnValue);

		globals.SetMember("LAM", LAM);
		logger::debug("JSON Getter installed.");
		//a_view->Invoke("_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.InitData", nullptr, nullptr, 0);
		return true;
	}
	return false;
}
