#pragma once
#include <vector>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "Base_types.h"
#include "Calcs.h"
#include "../utils.h"

namespace UI
	{
	class HBox : public Container
		{
		public:
			HBox(Alignment vertical_alignment, std::vector<std::unique_ptr<Element>>& elements) : Container(elements), valign(vertical_alignment) {}
			Alignment valign;

			virtual calcs::max_f width_max(sf::Vector2f reference) const noexcept override
				{
				calcs::max_f max = calcs::max_f::zero();
				for (auto& element : elements) { max = calcs::max_f::sum(max, element->width_max(reference)); }
				return max;
				}
			virtual calcs::min_f width_min(sf::Vector2f reference) const noexcept override
				{
				calcs::min_f min = calcs::min_f::zero();
				for (auto& element : elements) { min = calcs::min_f::sum(min, element->width_min(reference)); }
				return min;
				}
			virtual calcs::max_f height_max(sf::Vector2f reference) const noexcept override
				{
				calcs::max_f max = calcs::max_f::zero();
				for (auto& element : elements) { max = calcs::max_f::merge(max, element->height_max(reference)); }
				return max;
				}
			virtual calcs::min_f height_min(sf::Vector2f reference) const noexcept override
				{
				calcs::min_f min = calcs::min_f::zero();
				for (auto& element : elements) { min = calcs::min_f::merge(min, element->height_min(reference)); }
				return min;
				}

		protected:
			virtual void ui_update() override
				{
				std::vector<calcs::Limits> limits; limits.reserve(elements.size());
				for (const auto& element : elements) 
					{
					auto min = element->width_min(size);
					auto max = element->width_max(size);
					limits.emplace_back(min.value, max.value, max.type); 
					}

				std::vector<calcs::PosSize> sizes = calcs::possizes(limits, width);

				for (int i = 0; i < elements.size(); i++)
					{
					auto& element = elements[i];
					calcs::PosSize ver = calcs::possize(calcs::Limits(element->height_min(size), element->height_max(size)), height, valign);
					elements[i]->ui_update_begin(position + calcs::pos(sizes[i], ver), calcs::size(sizes[i], ver));
					}
				}
		private:
		};
	class VBox : public Container
		{
		public:
			VBox(Alignment horizontal_alignment, std::vector<std::unique_ptr<Element>>& elements) : Container(elements), halign(horizontal_alignment) {}
			Alignment halign;

			virtual calcs::max_f width_max(sf::Vector2f reference) const noexcept override
				{
				calcs::max_f max = calcs::max_f::zero();
				for (auto& element : elements) { max = calcs::max_f::merge(max, element->width_max(reference)); }
				return max;
				}
			virtual calcs::min_f width_min(sf::Vector2f reference) const noexcept override
				{
				calcs::min_f min = calcs::min_f::zero();
				for (auto& element : elements) { min = calcs::min_f::merge(min, element->width_min(reference)); }
				return min;
				}
			virtual calcs::max_f height_max(sf::Vector2f reference) const noexcept override
				{
				calcs::max_f max = calcs::max_f::zero();
				for (auto& element : elements) { max = calcs::max_f::sum(max, element->height_max(reference)); }
				return max;
				}
			virtual calcs::min_f height_min(sf::Vector2f reference) const noexcept override
				{
				calcs::min_f min = calcs::min_f::zero();
				for (auto& element : elements) { min = calcs::min_f::sum(min, element->height_min(reference)); }
				return min;
				}

		protected:
			virtual void ui_update() override
				{
				std::vector<calcs::Limits> limits; limits.reserve(elements.size());
				for (const auto& element : elements)
					{
					auto min = element->height_min(size);
					auto max = element->height_max(size);
					limits.emplace_back(min.value, max.value, max.type);
					}

				std::vector<calcs::PosSize> sizes = calcs::possizes(limits, height);

				for (int i = 0; i < elements.size(); i++)
					{
					calcs::PosSize hor = calcs::possize(calcs::Limits(elements[i]->width_min(size), elements[i]->width_max(size)), width, halign);
					elements[i]->ui_update_begin(position + calcs::pos(hor, sizes[i]), calcs::size(hor, sizes[i]));
					}
				}
		private:
		};
	}