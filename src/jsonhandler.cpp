#pragma once

#include "utils.h"
#include "interface.h"
#include <filesystem>

json GetJSONData::LoadJSON(std::string filename)
{
	std::ifstream file(filename);

	if (!file.is_open()) {
		logger::info("Failed to find {}", filename);
		return json::array();
	}

	json jf = json::parse(file);
	return jf; 
}

void GetJSONData::Call(Params& params)
{
	assert(params.argCount >= 1);
	const auto filename = params.args[0].GetString();
	
	auto jsonArray = LoadJSON(filename);
	logger::info("Loaded {} with {} elements", filename, jsonArray.size());

	RE::GFxValue* retVal = params.retVal;
	RE::GFxMovie* movie = params.movie;

	movie->CreateArray(retVal);
	for (auto& element : jsonArray)
	{
		RE::GFxValue elementObj;
		movie->CreateObject(&elementObj);

		RegisterString(&elementObj, params.movie, "Name", element["Name"].get<std::string>());
		RegisterString(&elementObj, params.movie, "Description", element["Description"].get<std::string>());
		RegisterString(&elementObj, params.movie, "UniqueKey", element["UniqueKey"].get<std::string>());
		
		retVal->PushBack(elementObj);
	}
}


void GetJSONData::InjectScaleform()
{
	auto ui = RE::UI::GetSingleton();
	if (auto ui{ RE::UI::GetSingleton() }) {
		if (auto menu{ ui->GetMenu(RE::RaceSexMenu::MENU_NAME) }) {
			if (auto movie{ menu->uiMovie.get() }) {

				/*RE::GFxValue global;
				movie->GetVariable(&global, "_global");

				auto traitData = new GetJSONData();
				auto dir = std::filesystem::current_path();
				std::filesystem::path pathname = dir / "Data/SKSE/Plugins/traits.json";
				if (!std::filesystem::exists(pathname)) {
					logger::info("Traits file not found.");
				}

				traitData->path_to_json = pathname.string();
				global.SetMember(
					"GetTraitData",
					traitData
				);

				auto classData = new GetJSONData();
				classData->path_to_json = "./classes.json";
				global.SetMember(
					"GetClassData",
					classData
				);*/

				if (const auto a_scaleform{ SKSE::GetScaleformInterface() }) {
					a_scaleform->Register(&FunctionManager::ScaleformRegister, "skse");
					movie->Invoke("_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.InitData", nullptr, nullptr, 0);
					logger::info("Registered Scaleform callback.");
					logger::info("Injected JSON parser successfully.");
					return;
				}
				logger::info("Failed to register Scaleform callback.");
			}
		}

	}
	logger::info("JSON parser failed injection.");

}
