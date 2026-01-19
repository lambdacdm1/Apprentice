#pragma once

#include "utils.h"

void RegisterString(RE::GFxValue* object, RE::GFxMovie* movie, std::string stringName, std::string stringContents)
{
	RE::GFxValue gfxString;
	movie->CreateString(&gfxString, stringContents.c_str());

	object->SetMember(stringName.c_str(), gfxString);
}

void AddBooleanMember(RE::GFxValue* entry, bool val, std::string_view name) {
	RE::GFxValue member(val);
	entry->SetMember(name.data(), &member);
}

void AddStringMember(RE::GFxValue* entry, std::string_view val, std::string_view name) {
	RE::GFxValue member(val);
	entry->SetMember(name.data(), &member);
}

void AddIntMember(RE::GFxValue* entry, i32 val, std::string_view name) {
	RE::GFxValue member(val);
	entry->SetMember(name.data(), &member);
}