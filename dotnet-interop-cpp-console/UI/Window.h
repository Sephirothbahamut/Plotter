#pragma once
#pragma comment (lib, "dwmapi.lib")//without this dwmapi.h doesn't work :shrugs: no idea whatsoever where the compiler is taking this file from
#include <vector>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <Windows.h>

#include <utils/window.h>

#include "Window_UI.h"
#include "../UI.h"
#include "../utils.h"

namespace UI
	{
	class Window;

	LONG_PTR sfml_window_proc;
	std::map<HWND, utils::observer_ptr<Window>> hwnd_to_window;

	class Window
		{
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
			{
			Window& window = *hwnd_to_window[hwnd];

			switch (msg)
				{
				case WM_SIZE:
					{
					unsigned width  = std::max(static_cast<unsigned>LOWORD(lParam), window.min_size.x);  // Macro to get the low-order word.
					unsigned height = std::max(static_cast<unsigned>HIWORD(lParam), window.min_size.y);

					if (window.siz.x != width || window.siz.y != height)
						{
						window.handle_resize(width, height);
						}
					}
					break;
				case WM_GETMINMAXINFO:
					{
					LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
					lpMMI->ptMinTrackSize.x = window.min_size.x;
					lpMMI->ptMinTrackSize.y = window.min_size.y;
					}
					break;
				}

			return CallWindowProc(reinterpret_cast<WNDPROC>(sfml_window_proc), hwnd, msg, wParam, lParam);
			}

		public:
			struct Properties 
				{
				enum struct Transparency { no, transparent, glass };

				std::string title{"Untitled"};
				sf::Vector2u size{800u, 600u};
				sf::Vector2i position{0, 0};
				std::unique_ptr<Element> root{nullptr};
				sf::Color color{sf::Color::Black};
				Transparency transparency{Transparency::no};
				bool borders{true};
				};

			sf::Window owner;

			Window(Properties& p) :
				window{sf::VideoMode{p.size.x, p.size.y}, p.title, p.borders ? sf::Style::Default : sf::Style::None, sf::ContextSettings{0U, 0U, 8U}},
				size{p.size.x, p.size.y},
				clear_color{p.color},
				ui{std::move(p.root)}
				{
				HWND hwnd = window.getSystemHandle();
				hwnd_to_window[hwnd] = this;
				window.setActive(false);
				window.setPosition(p.position);

				// ======== Responsive resize callback
				sfml_window_proc = GetWindowLongPtr(hwnd, GWLP_WNDPROC);
				SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WndProc));

				// ======== Transparency
				switch (p.transparency)
					{
					case Properties::Transparency::no:                                                                break;
					case Properties::Transparency::transparent: utils::window::make_glass_DWM(hwnd);                  break;
					case Properties::Transparency::glass:       utils::window::make_glass_CompositionAttribute(hwnd); break;
					}
				}

			void setup()
				{
				ui.setup();
				min_size = {static_cast<unsigned>(ui.min_width()), static_cast<unsigned>(ui.min_height())};
				window.setSize({std::max(min_size.x, window.getSize().x), std::max(min_size.y, window.getSize().y)});
				}

			void start()
				{
				draw_thread = std::thread(&Window::loop_draw, this);

				ui.on_resize(static_cast<float>(size.x), static_cast<float>(size.y));
				redraw();

				loop_events();
				}

			void update()
				{
				std::unique_lock<std::mutex> lock(mutex);
				ui.update();
				redraw();
				}

			HWND get_handle() { return window.getSystemHandle(); }

			__declspec(property(get = get_pos, put = set_pos)) sf::Vector2i pos;
			__declspec(property(get = get_siz, put = set_siz)) sf::Vector2u siz;

			void         set_pos(const sf::Vector2i& new_pos)       noexcept { window.setPosition(new_pos); }
			sf::Vector2i get_pos()                            const noexcept { return window.getPosition(); }
			void         set_siz(const sf::Vector2u& new_siz)       noexcept { window.setSize(new_siz); }
			sf::Vector2u get_siz()                            const noexcept { return window.getSize(); }

			bool is_open() { return window.isOpen(); }

		private:
			sf::RenderWindow window;
			UI::Window_UI ui;
			sf::Vector2u size;
			sf::Vector2u min_size;

			sf::Color clear_color;

			std::mutex mutex;
			std::thread draw_thread;
			std::condition_variable condition;
			std::atomic_bool resized{false};

			void loop_draw()
				{
				window.setActive(true);
				while (window.isOpen())
					{
					std::unique_lock<std::mutex> lock(mutex);
					condition.wait(lock);
					_draw();
					}
				}

			void loop_events()
				{
				while (window.isOpen()) { events(); }
				}

			void events()
				{
				sf::Event event;

				if (window.waitEvent(event))
					{
					std::unique_lock lock{mutex};
					auto captured = ui.handle_event(event);
					if (!captured.mouse_captured)
						{
						switch (event.type)
							{
							case sf::Event::Closed:  window.close(); break;
							//case sf::Event::Resized: handle_resize(event.size.width, event.size.height); break; //DON'T, managed by the windows procedure
							}
						}
					if (captured.redraw) { redraw(); }
					}
				}
			void _draw()
				{
				window.clear(clear_color);

				ui.draw(window);

				window.display();
				}

			void redraw() { condition.notify_one(); }

			bool resize_recursion{false};
			void handle_resize(unsigned width, unsigned height) noexcept
				{
				if (!resize_recursion)
					{
					resize_recursion = true;
					
					//TODO
					//Readd the unique lock. Right now we can't because window.setSize causes the windows procedure to be re-called which in turn re-calls this function
					//and we can't capture the lock twice.
					//We need window.setSize to force SFML to rebuild the context. We can remove the "resize_recursion" checks if we manage to recreate the window context with the new size without calling window.setSize


					std::unique_lock<std::mutex> lock{mutex};
					size = {width, height};
					try
						{
						ui.on_resize(static_cast<float>(width), static_cast<float>(height)); //can throw
						}
					catch (const UI::resizing_exception& e)
						{
						std::cout << "RESIZING EXCEPTION" << std::endl;
						}

					window.setSize({size.x, size.y});
					window.setView(sf::View{sf::FloatRect{0, 0, static_cast<float>(size.x), static_cast<float>(size.y)}});

					resized = true;
					redraw();

					resize_recursion = false;
					}
				}
		};
	}