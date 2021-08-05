#pragma once
#include <vector>
#include <limits>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <cmath>
#include <iostream>

#include <SFML/Graphics.hpp>

#include <utils/compilation/debug.h>
#include <utils/memory.h>
#include <utils/id_pool.h>
#include <utils/extensions.h>

#include "Base_types.h"

//#define UI_DEBUG_INPUT
//#define UI_DEBUG_DRAW

namespace UI
	{
	class Window_UI
		{
		public:
			Window_UI(Element_ptr&& new_root) : root{std::move(new_root)} {}

			      Element_obs get_root()       noexcept { return root.get(); }
			const Element_obs get_root() const noexcept { return root.get(); }

			float min_height() const noexcept { return root->height_min({std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()}).value; }
			float min_width () const noexcept { return root->width_min ({std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()}).value; }

			void draw          (sf::RenderTarget& rt) const noexcept
				{
#ifdef UI_DEBUG_DRAW
				root->ui_debug_draw(rt, {});
#endif
				for (const auto& drawable : drawables) { rt.draw(*drawable); }
				}

			void update() 
				{
				for (Updatable_obs updatable : updatables) { updatable->update(); }
				}

			void setup()
				{
				drawables  = root->get_drawables();
				updatables = root->get_updatables();
				}

			void on_resize(float width, float height) 
				{
				root->ui_update_begin({0.f, 0.f}, {width, height});
				}

			bool on_mouse_move(sf::Event::MouseMoveEvent e)
				{
				Interactable_obs element = root->get_interactable_at({static_cast<float>(e.x), static_cast<float>(e.y)});

				if (mouseover != element)
					{
					if (mouseover != nullptr) 
						{
#ifdef UI_DEBUG_INPUT
						mouseover->debug_on_leave();
#endif
						mouseover->on_leave();
						}
					if (element   != nullptr) 
						{ 
#ifdef UI_DEBUG_INPUT
						element  ->debug_on_enter();
#endif
						element  ->on_enter();
						}
					mouseover = element;
					}
				return element != nullptr;
				}
			bool on_mouse_pressed(sf::Event::MouseButtonEvent e)
				{
				bool ret = on_mouse_move({e.x, e.y});

				if (mouseover != focused)
					{
					if (focused   != nullptr)
						{ 
#ifdef UI_DEBUG_INPUT
						focused  ->debug_on_focus_lose(); 
#endif
						focused  ->on_focus_lose(); 
						}
					if (mouseover != nullptr)
						{
#ifdef UI_DEBUG_INPUT
						mouseover->debug_on_focus_gain();
#endif
						mouseover->on_focus_gain();
						}
					focused = mouseover;
					}

				if (ret)
					{
					focused = mouseover;
#ifdef UI_DEBUG_INPUT
					mouseover->debug_on_mouse_pressed(e.button);
#endif
					mouseover->on_mouse_pressed(e.button);
					}
				return ret;
				}
			bool on_mouse_released(sf::Event::MouseButtonEvent e)
				{
				bool ret = on_mouse_move({e.x, e.y});
				if (ret)
					{
#ifdef UI_DEBUG_INPUT
					mouseover->debug_on_mouse_released(e.button);
#endif
					mouseover->on_mouse_released(e.button);
					}
				if (focused != nullptr) 
					{
#ifdef UI_DEBUG_INPUT
					focused->debug_on_mouse_global_released(e.button);
#endif
					focused->on_mouse_global_released(e.button); 
					}
				return ret;
				}

			event_capture handle_event(sf::Event event)
				{
				event_capture ret;

				switch (event.type)
					{
					case sf::Event::Resized:             ret.redraw = true; on_resize           (event.size.width, event.size.height); break;
					case sf::Event::MouseMoved:          ret.redraw = true; on_mouse_move       (event.mouseMove);                     break;
					case sf::Event::MouseButtonPressed:  ret.redraw = true; ret.mouse_captured = on_mouse_pressed (event.mouseButton); break;
					case sf::Event::MouseButtonReleased: ret.redraw = true; ret.mouse_captured = on_mouse_released(event.mouseButton); break;
					}
				return ret;
				}

		private:

			Element_ptr root;

			cDrawables_obs    drawables;
			Updatables_obs    updatables;
			Interactables_obs interactables;

			Interactable_obs focused  {nullptr};
			Interactable_obs mouseover{nullptr};
		};
	}