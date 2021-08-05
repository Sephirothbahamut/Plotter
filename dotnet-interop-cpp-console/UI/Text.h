#pragma once
#include <vector>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "Base_types.h"
#include "Calcs.h"
#include "../utils.h"

namespace UI
	{
	inline sf::Font DEFAULT_FONT;

	class Text : public Drawable
		{
		private:
			sf::Text text;
			sf::Font DEFAULT_FONT;

			/*float width_single() const noexcept
				{
				sf::Text tmp("_", DEFAULT_FONT, text.getCharacterSize());
				return tmp.getLocalBounds().width;
				}
			float width_text() const noexcept { return width_single() * text.getString().getSize(); }
			float height_text() const noexcept
				{
				sf::Text tmp("|,_^'", DEFAULT_FONT, text.getCharacterSize());
				return tmp.getLocalBounds().height;
				}*/

			float width_text() const noexcept { return text.getLocalBounds().left + text.getLocalBounds().width; }
			float height_text() const noexcept { return text.getLocalBounds().top + text.getLocalBounds().height; }

			void update_alignment() noexcept
				{
				float x = 0, y = 0;
				auto lb = text.getLocalBounds();
				switch (halign)
					{
						case Alignment::beg: x = 0; break;
						case Alignment::mid: x = text.getLocalBounds().width / 2; break;
						case Alignment::end: x = text.getLocalBounds().width; break;
					}
				switch (valign)
					{
						case Alignment::beg: y = 0; break;
						case Alignment::mid: y = text.getLocalBounds().height / 2; break;
						case Alignment::end: y = text.getLocalBounds().height; break;
					}
				text.setOrigin(std::floor(x), std::floor(y));
				}

			virtual void ui_update() override { text.setPosition(sf::Vector2f(std::floor(position.x), std::floor(position.y))); update_alignment(); }

		public:
			Text(sf::String str = "PLACEHOLDER", unsigned text_size = 16) : text(str, DEFAULT_FONT, text_size) { DEFAULT_FONT.loadFromFile("consola.ttf"); } //TODO use global DEFAULT_FONT

			Alignment halign = Alignment::beg;
			Alignment valign = Alignment::beg;
			void set_string(sf::String string) { text.setString(string); }

			virtual calcs::max_f width_max (sf::Vector2f reference) const noexcept override { return calcs::max_f::val(width_text() ); }
			virtual calcs::min_f width_min (sf::Vector2f reference) const noexcept override { return calcs::min_f::val(width_text() ); }
			virtual calcs::max_f height_max(sf::Vector2f reference) const noexcept override { return calcs::max_f::val(height_text()); }
			virtual calcs::min_f height_min(sf::Vector2f reference) const noexcept override { return calcs::min_f::val(height_text()); }

			virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override
				{
				target.draw(text);
				}
		};
	}