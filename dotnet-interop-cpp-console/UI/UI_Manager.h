#pragma once
#include <string>
#include <memory>
#include <mutex>
#include <optional>
#include "Computer.h"
//#include "Structure.h"
#include "../UI.h"

#include "../Creator/Creator.h"

#include <utils/window.h>
#define NOMINMAX
#include <Windows.h>

#include <ShObjIdl_core.h>

namespace UI
	{
	class Manager
		{
		private:
			std::mutex mutex;
			std::vector<std::unique_ptr<Sensor>> sensors;//only to guarantee existence of sensors pointed to by windows contents.
			std::unique_ptr<Computer> computer;
			std::vector<std::unique_ptr<UI::Window>> windows;
			std::vector<std::unique_ptr<std::thread>> threads;

			std::chrono::milliseconds update_delay{500};

			void window_thread(UI::Window::Properties& p)
				{
				UI::Window* window = nullptr;
				if (true)
					{
					std::lock_guard lock(mutex);
					windows.push_back(std::make_unique<UI::Window>(p));
					window = windows.back().get();
					}
				window->setup();
				window->start();
				}

			void add_window(UI::Window::Properties& p)
				{
				threads.push_back(std::make_unique<std::thread>([&] { window_thread(p); }));
				}

		public:
			Manager(std::unique_ptr<Computer>& computer, creator::UICreated& created)
				: computer{std::move(computer)}, sensors{std::move(created.sensors)}, update_delay{created.update_delay}
				{
				for (auto& f : created.windows_factories) { add_window(f); }
				}

			void start()
				{
				bool windows_open = true;

				Sensor& s = *sensors[0];

				while (windows_open)
					{
					std::this_thread::sleep_for(update_delay);
					//sf::sleep(update_delay);
					bool windows_open = false;
					computer->update();
					for (auto& window_ptr : windows)
						{
						if (window_ptr->is_open())
							{
							window_ptr->update();
							windows_open = true;

							if (s.value()) 
								{
								//SetProgressState(window_ptr->get_handle())
								}
							}
						}
					}

				for (auto& thread : threads) { thread->join(); }
				}
		};
	}