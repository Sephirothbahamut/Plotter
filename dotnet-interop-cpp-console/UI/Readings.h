#pragma once
#include <vector>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "../utils.h"
#include "../sensors.h"
#include "Computer.h"

namespace UI
	{
	class ReadingValue : public Updatable
		{
		private:
			Text title, value;
			Sensor* sensor_value;

			virtual void ui_update() override
				{
				title.ui_update_begin(position, sf::Vector2f(0, 0));
				value.ui_update_begin(sf::Vector2f(x + width, y), sf::Vector2f(0, 0));
				}
		public:
			ReadingValue(const sf::String& name_string, Sensor* sensor_value) : title(name_string), sensor_value(sensor_value)
				{
				title.halign = Alignment::beg;
				title.valign = Alignment::beg;

				value.halign = Alignment::end;
				value.valign = Alignment::beg;

				update();
				}

			void update() noexcept override { value.set_string(value_string(*sensor_value)); }

			calcs::max_f width_max (sf::Vector2f reference) const noexcept override { return calcs::max_f::infval(calcs::max_f::sum(value.width_max(reference), title.width_max(reference))); }
			calcs::min_f width_min (sf::Vector2f reference) const noexcept override { return calcs::min_f::sum(title.width_min(reference), value.width_min(reference)); }
			calcs::max_f height_max(sf::Vector2f reference) const noexcept override { return title.height_max(reference); }
			calcs::min_f height_min(sf::Vector2f reference) const noexcept override { return title.height_min(reference); }



			virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override
				{
				target.draw(title, states);
				target.draw(value, states);
				}
		};
	class ReadingValueOnTotal : public Updatable
		{
		public:
			ReadingValueOnTotal(const sf::String& name_string, Sensor* sensor_value, Sensor* sensor_total) : title(name_string), sensor_value(sensor_value)
				{
				title.halign = Alignment::beg;
				title.valign = Alignment::beg;

				value_on_total.halign = Alignment::end;
				value_on_total.valign = Alignment::beg;

				update();
				}

			void update() noexcept override
				{
				value_on_total.set_string(value_string(*sensor_value) + "/" + value_string(*sensor_total));
				}

			calcs::max_f width_max (sf::Vector2f reference) const noexcept override { return calcs::max_f::infval(calcs::max_f::sum(value_on_total.width_max(reference), title.width_max(reference))); }
			calcs::min_f width_min (sf::Vector2f reference) const noexcept override { return calcs::min_f::sum(title.width_min(reference), value_on_total.width_min(reference)); }
			calcs::max_f height_max(sf::Vector2f reference) const noexcept override { return title.height_max(reference); }
			calcs::min_f height_min(sf::Vector2f reference) const noexcept override { return title.height_min(reference); }

			virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override
				{
				target.draw(title, states);
				target.draw(value_on_total, states);
				}

		private:
			Text title, value_on_total;
			Sensor* sensor_value;
			Sensor* sensor_total;

			virtual void ui_update() override
				{
				title.ui_update_begin(position, sf::Vector2f(0, 0));
				value_on_total.ui_update_begin(sf::Vector2f(x + width, y), sf::Vector2f(0, 0));
				}
		};
	class ReadingPercentValueOnTotal : public Updatable
		{
		public:
			ReadingPercentValueOnTotal(const sf::String& name_string, Sensor* sensor_percent, Sensor* sensor_value, Sensor* sensor_total) : title(name_string), sensor_value(sensor_value), sensor_percent(sensor_percent), sensor_total(sensor_total)
				{
				title.halign = Alignment::beg;
				title.valign = Alignment::beg;

				percent.halign = Alignment::mid;
				percent.valign = Alignment::beg;

				value_on_total.halign = Alignment::end;
				value_on_total.valign = Alignment::beg;

				update();
				}

			void update() noexcept override
				{
				percent.set_string(value_string(*sensor_percent));
				value_on_total.set_string(value_string(*sensor_value) + "/" + value_string(*sensor_total));
				}

			calcs::max_f width_max (sf::Vector2f reference) const noexcept override { return calcs::max_f::infval(calcs::max_f::sum(percent.width_max(reference), calcs::max_f::sum(value_on_total.width_max(reference), title.width_max(reference)))); }
			calcs::min_f width_min (sf::Vector2f reference) const noexcept override { return calcs::min_f::sum(percent.width_min(reference), calcs::min_f::sum(value_on_total.width_min(reference), title.width_min(reference))); }
			calcs::max_f height_max(sf::Vector2f reference) const noexcept override { return title.height_max(reference); }
			calcs::min_f height_min(sf::Vector2f reference) const noexcept override { return title.height_min(reference); }

			virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override
				{
				target.draw(title, states);
				target.draw(percent, states);
				target.draw(value_on_total, states);
				}

		private:
			Text title, percent, value_on_total;
			Sensor* sensor_value;
			Sensor* sensor_total;
			Sensor* sensor_percent;

			virtual void ui_update() override
				{
				title.ui_update_begin(position, sf::Vector2f(0, 0));
				value_on_total.ui_update_begin(sf::Vector2f(x + width, y), sf::Vector2f(0, 0));
				float end_title = title.width_min(size).value;
				float begin_vot = width - value_on_total.width_min(size).value;
				float center_perc = (begin_vot + end_title) / 2;
				percent.ui_update_begin(sf::Vector2f(x + center_perc, y), sf::Vector2f(0, 0));
				}
		};
	}