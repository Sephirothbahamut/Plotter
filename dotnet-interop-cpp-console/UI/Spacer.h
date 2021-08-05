#pragma once
#include <optional>

#include "Base_types.h"

namespace UI
	{
	class Spacer : public Element
		{
		public:
			Spacer(
				std::optional<float> max_width  = std::nullopt, std::optional<float> min_width  = std::nullopt,
				std::optional<float> max_height = std::nullopt, std::optional<float> min_height = std::nullopt) :
				Element{sf::Color::Magenta},
				custom_max_width {max_width .has_value() ? calcs::max_f::val(max_width .value()) : calcs::max_f::zero()},
				custom_min_width {min_width .has_value() ? calcs::min_f::val(min_width .value()) : calcs::min_f::zero()},
				custom_max_height{max_height.has_value() ? calcs::max_f::val(max_height.value()) : calcs::max_f::zero()},
				custom_min_height{min_height.has_value() ? calcs::min_f::val(min_height.value()) : calcs::min_f::zero()}
				{}

			calcs::max_f width_max (sf::Vector2f reference) const noexcept override       { return custom_max_width; }
			calcs::min_f width_min (sf::Vector2f reference) const noexcept override final { return custom_min_width; }
			calcs::max_f height_max(sf::Vector2f reference) const noexcept override       { return custom_max_height; }
			calcs::min_f height_min(sf::Vector2f reference) const noexcept override final { return custom_min_height; }

		protected:
			calcs::max_f custom_max_width;
			calcs::min_f custom_min_width;
			calcs::max_f custom_max_height;
			calcs::min_f custom_min_height;
		};
	struct VSpacer : Spacer 
		{
		VSpacer(std::optional<float> max_width = std::nullopt, std::optional<float> min_width = std::nullopt, std::optional<float> max_height = std::nullopt, std::optional<float> min_height = std::nullopt)
			: Spacer{max_width, min_width, max_height, min_height}
			{
			if (custom_max_height == calcs::max_f::zero()) { custom_max_height = calcs::max_f::adaptive(); }
			}
		};
	struct HSpacer : Spacer 
		{
		HSpacer(std::optional<float> max_width = std::nullopt, std::optional<float> min_width = std::nullopt, std::optional<float> max_height = std::nullopt, std::optional<float> min_height = std::nullopt)
			: Spacer{max_width, min_width, max_height, min_height}
			{
			if (custom_max_width  == calcs::max_f::zero()) { custom_max_width  = calcs::max_f::adaptive(); }
			}
		};

	class Separator : public Drawable
		{
		public:
			Separator(sf::Color color) : Drawable{sf::Color::Magenta} 
				{
				line.setFillColor(color);
				}

			void ui_update() override final
				{
				line.setPosition(position);
				line.setSize(size);
				}
			
			calcs::max_f width_max (sf::Vector2f reference) const noexcept override       { return calcs::max_f::val(1.f); }
			calcs::min_f width_min (sf::Vector2f reference) const noexcept override final { return calcs::min_f::val(1.f); }
			calcs::max_f height_max(sf::Vector2f reference) const noexcept override       { return calcs::max_f::val(1.f); }
			calcs::min_f height_min(sf::Vector2f reference) const noexcept override final { return calcs::min_f::val(1.f); }

		private:
			sf::RectangleShape line;

			virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override
				{
				target.draw(line);
				}
		};
	struct VSeparator : Separator
		{
		VSeparator(sf::Color color) : Separator(color) {}
		calcs::max_f width_max(sf::Vector2f reference) const noexcept override { return calcs::max_f::adaval(1.f); }
		};
	struct HSeparator : Separator
		{
		HSeparator(sf::Color color) : Separator(color) {}
		calcs::max_f height_max(sf::Vector2f reference) const noexcept override { return calcs::max_f::adaval(1.f); }
		};
	}