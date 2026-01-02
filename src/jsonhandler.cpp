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
