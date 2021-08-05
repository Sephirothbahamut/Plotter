#pragma once

namespace UI::settings
	{
	inline sf::Color color_window = sf::Color(32.f, 32.f, 32.f, 255.f);
	inline sf::Color color_graphics_background = sf::Color(0.f, 0.f, 0.f, 255.f);
	inline unsigned font_size = 16u;
	}

#include "UI/Base_types.h"
#include "UI/Calcs.h"
#include "UI/Window.h"
#include "UI/Text.h"
#include "UI/Readings.h"
#include "UI/Plot.h"
#include "UI/Line.h"
#include "UI/Containers.h"
#include "UI/Wrappers.h"
#include "UI/Spacer.h"
#include "UI/Window_UI.h"
#include "UI/UI_Manager.h"