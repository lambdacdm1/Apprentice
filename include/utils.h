#pragma once 

void RegisterString(RE::GFxValue* object, RE::GFxMovie* movie, std::string stringName, std::string stringContents);

void AddBooleanMember(RE::GFxValue* entry, bool val, std::string_view name);
void AddStringMember(RE::GFxValue* entry, std::string_view val, std::string_view name);
void AddIntMember(RE::GFxValue* entry, i32 val, std::string_view name);