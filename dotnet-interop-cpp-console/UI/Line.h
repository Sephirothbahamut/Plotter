#pragma once
#include <vector>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "../utils.h"
#include "../sensors.h"
#include "Computer.h"
#include "../UI.h"

namespace UI
	{
	class Line : public Updatable
		{
		public:
			Line(Sensor* sensor, sf::Color& color, float min = 0, float max = 100) : sensor{sensor}, min{min}, max{max}
				{
				rect_bg.setFillColor(UI::settings::color_graphics_background);
				rect_val.setFillColor(color);

				update();
				}

			void update() noexcept override 
				{
				float percent = utils::percent_in_range(sensor->value().value_or(0), min, max);

				if (width > height)
					{
					rect_val.setOrigin(0.f, 0.f);
					rect_val.setSize({width * percent, height});
					}
				else
					{
					rect_val.setOrigin(0.f, height * (1.f - percent));
					rect_val.setSize({width, height * percent});
					}
				}

			calcs::max_f width_max (sf::Vector2f reference) const noexcept override { return calcs::max_f::infinite(); }
			calcs::min_f width_min (sf::Vector2f reference) const noexcept override { return calcs::min_f::zero(); }
			calcs::max_f height_max(sf::Vector2f reference) const noexcept override { return calcs::max_f::infinite(); }
			calcs::min_f height_min(sf::Vector2f reference) const noexcept override { return calcs::min_f::zero(); }

			virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override
				{
				target.draw(rect_bg,  states);
				target.draw(rect_val, states);
				}

		private:
			sf::RectangleShape rect_bg;
			sf::RectangleShape rect_val;
			Sensor* sensor;
			float min{  0.f};
			float max{100.f};

			virtual void ui_update() override
				{
				rect_bg.setPosition(position);
				rect_val.setPosition(position);
				rect_bg.setSize(size);
				}
		};
	}