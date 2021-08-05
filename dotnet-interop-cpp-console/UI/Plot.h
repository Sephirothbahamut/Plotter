#pragma once
#include <vector>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "../utils.h"
#include "../sensors.h"
#include "Text.h"
#include "Computer.h"
#include "../UI.h"

namespace UI
	{
	class PlotContent : public sf::Drawable
		{
		protected:
			Sensor* sensor;
			utils::Buffer<float> buffer_percent;
			float lower;
			float upper;

			sf::Vector2f _position;
			sf::Vector2f _size;
			sf::Vector2f get_position() { return _position; }
			sf::Vector2f get_size() { return _size; }
			float get_x() { return _position.x; }
			float get_y() { return _position.y; }
			float get_width() { return _size.x; }
			float get_height() { return _size.y; }

			__declspec(property(get = get_x)) float x;
			__declspec(property(get = get_y)) float y;
			__declspec(property(get = get_width)) float width;
			__declspec(property(get = get_height)) float height;
			__declspec(property(get = get_position)) sf::Vector2f position;
			__declspec(property(get = get_size)) sf::Vector2f size;

		public:
			PlotContent(Sensor* sensor, sf::Color color = sf::Color(255, 255, 255, 255), float upper = 100, float lower = 0) : sensor(sensor), color(color), upper(upper), lower(lower) {}
			sf::Color color;

			virtual void ui_update_begin(sf::Vector2f new_position, sf::Vector2f new_size) = 0;
			virtual void update() noexcept = 0;

			virtual void resize_data(size_t size)
				{
				buffer_percent.resize(size);
				}
		};
	class PlotLine : public PlotContent
		{
		protected:
			sf::VertexArray points;
			void update_vertex_coordinates(size_t index)
				{
				float x = utils::percent_in_range(index, 0, buffer_percent.size() - 1) * width;
				float y = height - (buffer_percent[index] * height);
				points[index].position = position + sf::Vector2f(x, y);
				}

		public:
			PlotLine(Sensor* sensor, sf::Color color = sf::Color(255, 255, 255, 255), float upper = 100, float lower = 0) : PlotContent(sensor, color, upper, lower), points(sf::PrimitiveType::LineStrip) {}

			virtual void ui_update_begin(sf::Vector2f new_position, sf::Vector2f new_size) override
				{
				_position = new_position;

				if (size != new_size)
					{
					_size = new_size;
					resize_data((size_t)(width / 2.f));
					}

				for (size_t i = 0; i < buffer_percent.size(); i++) { update_vertex_coordinates(i); }
				}

			virtual void update() noexcept override
				{
				if (buffer_percent.size() != 0)
					{
					if (sensor->value().has_value()) { buffer_percent.push(utils::percent_in_range(sensor->value().value_or(0), lower, upper)); }
					else { buffer_percent.push(buffer_percent.last); }

					for (size_t i = 0; i < buffer_percent.size(); i++) { update_vertex_coordinates(i); }
					//TODO: push positions back instead of recalculating everything
					/*for (size_t i = 0; i < buffer_percent.size() - 1; i++) { points[i].position = points[i + 1].position; }
					update_vertex_coordinates(buffer_percent.size() - 1);*/
					}
				}

			virtual void resize_data(size_t size) override
				{
				PlotContent::resize_data(size);
				points.resize(size);
				for (size_t i = 0; i < points.getVertexCount(); i++) { points[i].color = color; }
				}

			virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override { target.draw(points, states); }
		};

	template <bool fade = false>
	class PlotMountain : public PlotContent
		{
		protected:
			sf::VertexArray points;
			void update_vertex_coordinates(size_t index)
				{
				float x = utils::percent_in_range(index, 0, buffer_percent.size() - 1) * width;
				float y = height - (buffer_percent[index] * height);
				points[index * 2].position = position + sf::Vector2f(x, y);
				if constexpr (!fade) { points[index * 2 + 1].position = position + sf::Vector2f(x, height); }
				else { points[index * 2 + 1].position = position + sf::Vector2f(x, std::min(height, y + height / 10)); }
				}

		public:
			PlotMountain(Sensor* sensor, sf::Color color = sf::Color(255, 255, 255, 255), float upper = 100, float lower = 0, bool fade = false) : PlotContent(sensor, color, upper, lower), points(sf::PrimitiveType::TrianglesStrip ) {}

			virtual void ui_update_begin(sf::Vector2f new_position, sf::Vector2f new_size) override
				{
				_position = new_position;

				if (size != new_size)
					{
					_size = new_size;
					resize_data((size_t)(width / 2.f));
					}

				for (size_t i = 0; i < buffer_percent.size(); i++) { update_vertex_coordinates(i); }
				}

			virtual void update() noexcept override
				{
				if (buffer_percent.size() != 0)
					{
					if (sensor->value().has_value()) { buffer_percent.push(utils::percent_in_range(sensor->value().value_or(0), lower, upper)); }
					else { buffer_percent.push(buffer_percent.last); }

					for (size_t i = 0; i < buffer_percent.size(); i++) { update_vertex_coordinates(i); }
					//TODO: push positions back instead of recalculating everything
					/*for (size_t i = 0; i < buffer_percent.size() - 1; i++) { points[i].position = points[i + 1].position; }
					update_vertex_coordinates(buffer_percent.size() - 1);*/
					}
				}

			virtual void resize_data(size_t size) override
				{
				PlotContent::resize_data(size);
				points.resize(size * 2);
				if constexpr (!fade) { for (size_t i = 0; i < points.getVertexCount(); i++) { points[i].color = color; } }
				else { for (size_t i = 0; i < points.getVertexCount(); i++) { points[i].color = (i % 2 == 0) ? color : sf::Color(color.r, color.g, color.b, 0); } }
				}

			virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override { target.draw(points, states); }
		};

	class Plot : public Updatable
		{
		private:
			class Canvas : public sf::Drawable
				{
				private:
					sf::RectangleShape rect;
					sf::VertexArray lines;

				public:
					Canvas(sf::Color bg_color = sf::Color::Black) : lines(sf::PrimitiveType::Lines, 36u)
						{
						rect.setFillColor(bg_color);
						sf::Color line_color(50, 50, 50, 255);
						for (size_t i = 0; i < lines.getVertexCount(); i++) { lines[i].color = line_color; }
						}

					void ui_update_begin(sf::Vector2f position, sf::Vector2f size)
						{
						size_t index = 0;
						for (size_t i = 1; i < 10; i++)
							{
							lines[index++].position = position + sf::Vector2f(0, (float)i / 10.f * (float)size.y);
							lines[index++].position = position + sf::Vector2f(size.x, (float)i / 10.f * (float)size.y);
							}
						for (size_t i = 1; i < 10; i++)
							{
							lines[index++].position = position + sf::Vector2f((float)i / 10.f * (float)size.x, 0);
							lines[index++].position = position + sf::Vector2f((float)i / 10.f * (float)size.x, size.y);
							}
						rect.setSize(sf::Vector2f(size.x - 2, size.y - 2));
						rect.setPosition(sf::Vector2f(position.x + 1, position.y + 1));
						}

					virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override { target.draw(rect, states); target.draw(lines, states); }
				};

			sf::RectangleShape border;
			Canvas canvas;
			std::vector<std::unique_ptr<PlotContent>> lines;

			virtual void ui_update() noexcept override
				{
				border.setSize(sf::Vector2f(width - 2, height - 2));
				border.setPosition(sf::Vector2f(x + 1, y + 1));

				for (auto& line_ptr : lines) { line_ptr->ui_update_begin(position, size); }
				for (auto& line_ptr : lines) { line_ptr->ui_update_begin({position.x + 1, position.y + 1}, {size.x - 2, size.y - 2}); }

				canvas.ui_update_begin(position, size);
				}

		public:
			Plot(std::vector<std::unique_ptr<PlotContent>>& lines, sf::Color bg_color = sf::Color::Black) : lines(std::move(lines)), canvas(bg_color)
				{
				border.setFillColor(sf::Color::Transparent);
				border.setOutlineColor(sf::Color(180, 180, 180, 255));
				border.setOutlineThickness(1);
				}

			virtual void update() noexcept override { for (auto& line_ptr : lines) { line_ptr->update(); } }

			virtual calcs::max_f width_max (sf::Vector2f reference) const noexcept override { return calcs::max_f::infinite(); }
			virtual calcs::min_f width_min (sf::Vector2f reference) const noexcept override { return calcs::min_f::zero(); }
			virtual calcs::max_f height_max(sf::Vector2f reference) const noexcept override { return calcs::max_f::infinite(); }
			virtual calcs::min_f height_min(sf::Vector2f reference) const noexcept override { return calcs::min_f::zero(); }

			virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override
				{
				target.draw(canvas);
				for (auto& line_ptr : lines) { target.draw(*line_ptr, states); }
				target.draw(border);
				}
		};
	}