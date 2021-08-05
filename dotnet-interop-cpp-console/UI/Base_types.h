#pragma once
#include <vector>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "Calcs.h"
#include "../utils.h"

#include <utils/memory.h>

namespace UI
	{
	class Element;	    using Element_ptr      = std::unique_ptr<Element>;      using Element_obs      = utils::observer_ptr<Element>;		using Elements      = std::vector<Element_ptr>;      using Elements_obs      = std::vector<Element_obs>;
	class Drawable;	    using Drawable_ptr     = std::unique_ptr<Drawable>;     using Drawable_obs     = utils::observer_ptr<Drawable>;		using Drawables     = std::vector<Drawable_ptr>;     using Drawables_obs     = std::vector<Drawable_obs>;
	/*const*/ using cDrawable_ptr = std::unique_ptr<const Drawable>; using cDrawable_obs = utils::observer_ptr<const Drawable>; using cDrawables_obs = std::vector<cDrawable_obs>;
	class Drawable;	    using Drawable_ptr     = std::unique_ptr<Drawable>;     using Drawable_obs     = utils::observer_ptr<Drawable>;		using Drawables     = std::vector<Drawable_ptr>;     using Drawables_obs     = std::vector<Drawable_obs>;
	class Wrapper;	    using Wrapper_ptr      = std::unique_ptr<Wrapper>;      using Wrapper_obs      = utils::observer_ptr<Wrapper>;		using Wrappers      = std::vector<Wrapper_ptr>;      using Wrappers_obs      = std::vector<Wrapper_obs>;
	class Container;    using Container_ptr    = std::unique_ptr<Container>;    using Container_obs    = utils::observer_ptr<Container>;	using Containers    = std::vector<Container_ptr>;    using Containers_obs    = std::vector<Container_obs>;
	class Updatable;    using Updatable_ptr    = std::unique_ptr<Updatable>;    using Updatable_obs    = utils::observer_ptr<Updatable>;	using Updatables    = std::vector<Updatable_ptr>;    using Updatables_obs    = std::vector<Updatable_obs>;
	class Interactable;	using Interactable_ptr = std::unique_ptr<Interactable>; using Interactable_obs = utils::observer_ptr<Interactable>;	using Interactables = std::vector<Interactable_ptr>; using Interactables_obs = std::vector<Interactable_obs>;

	struct event_capture
		{
		bool mouse_captured{false};
		bool redraw{false};
		};

	struct resizing_exception : std::runtime_error
		{
		resizing_exception(const std::string& str) : std::runtime_error{"Could not resize element respecting the constraints.\n" + str} {}
		resizing_exception() : std::runtime_error{"Could not resize element respecting the constraints."} {}
		};

	class Element
		{
		private:
			sf::RectangleShape rect;

		protected:
			sf::Vector2f _position;
			sf::Vector2f _size;

			virtual void ui_update() {}

			bool contains(const sf::Vector2f& at)
				{
				return at.x >= position.x && at.x <= position.x + size.x
					&& at.y >= position.y && at.y <= position.y + size.y;
				}

		public:
			Element(sf::Color color) { rect.setOutlineColor(color); rect.setFillColor(sf::Color(color.r, color.g, color.b, 20)); rect.setOutlineThickness(1); }

			sf::Vector2f get_position() { return _position; }
			sf::Vector2f get_size()     { return _size; }
			float get_x()               { return _position.x; }
			float get_y()               { return _position.y; }
			float get_width()           { return _size.x; }
			float get_height()          { return _size.y; }

			__declspec(property(get = get_x))        float x;
			__declspec(property(get = get_y))        float y;
			__declspec(property(get = get_width ))   float width;
			__declspec(property(get = get_height))   float height;
			__declspec(property(get = get_position)) sf::Vector2f position;
			__declspec(property(get = get_size    )) sf::Vector2f size;

			virtual calcs::max_f width_max (sf::Vector2f reference) const noexcept = 0;
			virtual calcs::min_f width_min (sf::Vector2f reference) const noexcept = 0;
			virtual calcs::max_f height_max(sf::Vector2f reference) const noexcept = 0;
			virtual calcs::min_f height_min(sf::Vector2f reference) const noexcept = 0;

			void ui_update_begin(sf::Vector2f new_position, sf::Vector2f new_size) noexcept
				{
				_position = new_position;
				rect.setPosition({x + 1, y + 1});

				if (size != new_size)
					{
					auto wmax = width_max (new_size);
					auto hmax = height_max(new_size);

					_size.x = wmax.infinity() || wmax.adapt() ? new_size.x : std::min(new_size.x, wmax.value);
					_size.y = hmax.infinity() || hmax.adapt() ? new_size.y : std::min(new_size.y, hmax.value);

					rect.setSize({width - 2, height - 2});
					}
				ui_update();
				}

			virtual void ui_debug_draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept { target.draw(rect, states); }

			virtual cDrawables_obs   get_drawables()                       const noexcept { return {}; }
			virtual Updatables_obs   get_updatables()                            noexcept { return {}; }
			virtual Interactable_obs get_interactable_at(const sf::Vector2f& at) noexcept { return {}; }
		};

	class Drawable : public Element, public sf::Drawable
		{
		public:
			Drawable() : Element(sf::Color::Cyan) {}
			Drawable(sf::Color c) : Element(c) {}

			virtual cDrawables_obs get_drawables() const noexcept override final { return {this}; }
		};

	class Updatable : public Drawable
		{
		public:
			Updatables_obs get_updatables() noexcept override final { return {this}; }
			virtual void update() noexcept = 0;
		};

	class Interactable : public Drawable
		{
		public:
			Interactable() : Drawable(sf::Color::Green) {}

			virtual void on_enter() {}
			virtual void on_leave() {}
			virtual void on_mouse_pressed(sf::Mouse::Button button) {}
			virtual void on_mouse_released(sf::Mouse::Button button) {}
			virtual void on_mouse_global_released(sf::Mouse::Button button) {}
			virtual void on_focus_gain() {}
			virtual void on_focus_lose() {}
#ifdef UI_DEBUG_INPUT
			void debug_on_enter()                                         { std::cout << "Mouse entered  " << id << '\n'; }
			void debug_on_leave()                                         { std::cout << "Mouse left     " << id << '\n'; }
			void debug_on_mouse_pressed (sf::Mouse::Button button)        { std::cout << "Mouse pressed  " << id << '\n'; }
			void debug_on_mouse_released(sf::Mouse::Button button)        { std::cout << "Mouse released " << id << '\n'; }
			void debug_on_mouse_global_released(sf::Mouse::Button button) { std::cout << "Mouse glob rel " << id << '\n'; }
			void debug_on_focus_gain()                                    { std::cout << "Focus gained   " << id << '\n'; }
			void debug_on_focus_lose()                                    { std::cout << "Focus lost     " << id << '\n'; }
#endif

			Interactable_obs get_interactable_at(const sf::Vector2f& at) noexcept override final { return contains(at) ? this : nullptr; }
		};

	class Wrapper : public Element
		{
		protected:
			Element_ptr element;

		public:
			Wrapper(std::unique_ptr<Element>& element) : Element(sf::Color::Yellow), element(std::move(element)) {}

			virtual void ui_debug_draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override final { Element::ui_debug_draw(target, states); element->ui_debug_draw(target, states); }

			cDrawables_obs   get_drawables()                       const noexcept override final { return element->get_drawables(); }
			Updatables_obs   get_updatables()                            noexcept override final { return element->get_updatables(); }
			Interactable_obs get_interactable_at(const sf::Vector2f& at) noexcept override final { return element->get_interactable_at(at); }
		};

	class Container : public Element
		{
		protected:
			Elements elements;

		public:
			Container(Elements& elements) : Element(sf::Color::Red), elements(std::move(elements)) {}

			void ui_debug_draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override final
				{
				Element::ui_debug_draw(target, states);
				for (auto& element_ptr : elements) { element_ptr->ui_debug_draw(target, states); }
				}

			cDrawables_obs get_drawables() const noexcept override final
				{
				cDrawables_obs tmp;
				for (auto& element_ptr : elements) { tmp += element_ptr->get_drawables(); }
				return tmp;
				}
			Updatables_obs get_updatables() noexcept override final
				{
				Updatables_obs tmp;
				for (auto& element_ptr : elements) { tmp += element_ptr->get_updatables(); }
				return tmp;
				}
			Interactable_obs get_interactable_at(const sf::Vector2f& at) noexcept override final
				{
				for (auto& element_ptr : elements) { auto tmp = element_ptr->get_interactable_at(at); if (tmp) { return tmp; } }
				return nullptr;
				}
		};
	}