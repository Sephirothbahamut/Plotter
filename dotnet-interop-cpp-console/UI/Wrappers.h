#pragma once
#include <vector>
#include <optional>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "../utils.h"

namespace UI
	{
	class SizeWrapper : public Wrapper
		{
		private:
			std::optional<calcs::max_f> custom_max_width;
			std::optional<calcs::min_f> custom_min_width;
			std::optional<calcs::max_f> custom_max_height;
			std::optional<calcs::min_f> custom_min_height;
			Alignment halign;
			Alignment valign;

			virtual void ui_update() override
				{
				calcs::PosSize hor = calcs::possize(calcs::Limits(element->width_min (size), element->width_max (size)), width,  halign);
				calcs::PosSize ver = calcs::possize(calcs::Limits(element->height_min(size), element->height_max(size)), height, valign);

				element->ui_update_begin(position + calcs::pos(hor, ver), calcs::size(hor, ver));
				}

		public:
			SizeWrapper(std::unique_ptr<Element>& element, Alignment halign = Alignment::beg, Alignment valign = Alignment::beg,
				std::optional<float> max_width  = std::nullopt, std::optional<float> min_width  = std::nullopt,
				std::optional<float> max_height = std::nullopt, std::optional<float> min_height = std::nullopt)
				: Wrapper(element), halign(halign), valign(valign)
				{
				if (max_width.has_value())  { custom_max_width  = calcs::max_f::val(max_width.value()); }
				if (min_width.has_value())  { custom_min_width  = calcs::min_f::val(min_width.value()); }
				if (max_height.has_value()) { custom_max_height = calcs::max_f::val(max_height.value()); }
				if (min_height.has_value()) { custom_min_height = calcs::min_f::val(min_height.value()); }
				}

			virtual calcs::max_f width_max (sf::Vector2f reference) const noexcept override { return custom_max_width .has_value() ? custom_max_width .value() : element->width_max (reference); }
			virtual calcs::min_f width_min (sf::Vector2f reference) const noexcept override { return custom_min_width .has_value() ? custom_min_width .value() : element->width_min (reference); }
			virtual calcs::max_f height_max(sf::Vector2f reference) const noexcept override { return custom_max_height.has_value() ? custom_max_height.value() : element->height_max(reference); }
			virtual calcs::min_f height_min(sf::Vector2f reference) const noexcept override { return custom_min_height.has_value() ? custom_min_height.value() : element->height_min(reference); }
		};
	}