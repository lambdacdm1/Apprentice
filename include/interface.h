#pragma once

#include <optional>
#include <typeindex>
#include <unordered_map>
#include "SKSE/API.h"
#include "jsonhandler.h"

namespace ScaleformInjector 
{
	bool InstallHooks(RE::GFxMovieView* a_view, RE::GFxValue* a_root);
};



struct FunctionManager
{
	// Attach a Scaleform function handler to the provided GFx scope
	template <class F>
	static inline void AttachFunction(RE::GFxMovieView* a_view, RE::GFxValue& a_scope, const char* a_methodname)
	{
		RE::GFxFunctionHandler* fn = nullptr;
		auto it = gfx_functions.find(std::type_index(typeid(F)));
		if (it != gfx_functions.end()) {
			fn = it->second;
		}
		else {
			fn = new F;
			gfx_functions.emplace(std::type_index(typeid(F)), fn);
		}

		RE::GFxValue fnValue;
		a_view->CreateFunction(&fnValue, fn);
		if (!a_scope.SetMember(a_methodname, fnValue)) {
			logger::error("Unable to bind function {}", a_methodname);
		}
	}

	// Convenience overload for existing callers that have a GPtr
	template <class F>
	static inline void AttachFunction(RE::GPtr<RE::GFxMovieView> a_view, RE::GFxValue& a_scope, const char* a_methodname)
	{
		AttachFunction<F>(a_view.get(), a_scope, a_methodname);
	}

	// Create an object on the provided root (or _global if root is null) and attach it
	static inline std::optional<RE::GFxValue> MakeFunctionObject(RE::GFxMovieView* a_view, RE::GFxValue* a_root, const char* a_classname)
	{
		RE::GFxValue* target = a_root;
		RE::GFxValue global;
		if (!target) {
			if (!a_view->GetVariable(&global, "_global")) {
				logger::error("Unable to get _global from view");
				return std::nullopt;
			}
			target = &global;
		}

		RE::GFxValue skse;
		a_view->CreateObject(&skse);
		if (!target->SetMember(a_classname, skse)) {
			logger::error("Failed to attach function object {} to root instance", a_classname);
			return std::nullopt;
		}
		return skse;
	}

	// Overload that keeps existing call sites working
	static inline std::optional<RE::GFxValue> MakeFunctionObject(RE::GPtr<RE::GFxMovieView> a_view, const char* a_classname)
	{
		return MakeFunctionObject(a_view.get(), nullptr, a_classname);
	}

	// Scaleform registration callback that will be called by SKSE when a movie registers
	static bool ScaleformRegister(RE::GFxMovieView* a_view, RE::GFxValue* a_root)
	{
		if (!a_view || !a_root) {
			return false;
		}

		auto skse = MakeFunctionObject(a_view, a_root, "skse");
		if (!skse) {
			logger::error("Failed to create SKSE function object in Scaleform callback");
			return false;
		}

		// Attach functions that should be exposed to Scaleform
		AttachFunction<GetJSONData>(a_view, *skse, "GetJSONData");

		return true;
	}

private:
	static inline std::unordered_map<std::type_index, RE::GFxFunctionHandler*> gfx_functions{};
};