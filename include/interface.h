#pragma once

#include "jsonhandler.h"

namespace ScaleformInjector 
{
	bool InstallHooks(RE::GFxMovieView* a_view, RE::GFxValue* a_root);

	void Register();
};