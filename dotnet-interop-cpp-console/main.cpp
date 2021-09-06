#include <iostream>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <map>
#include <string>
#include <conio.h>
#include <Windows.h>

#include <Computer.h>
#include <utils/window.h>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "utils.h"
#include "sensors.h"
#include "UI.h"
#include "Creator/Structure.h"
#include "Creator/Creator.h"

#include <utils/window.h>

int main(int argc, char** argv)
	{
	try
		{
		UI::DEFAULT_FONT.loadFromFile("consola.ttf");

		std::unique_ptr<Computer> computer = std::make_unique<Computer>();
		computer->update();
		std::cout << computer->to_string() << std::endl;
		std::cout << "======================================================" << std::endl;
		std::cout << "Press any key to continue . . . ";
		_getch();

		auto tmp = creator::UICreator::from_file("Plotter_data.txt", *computer);

		UI::Manager manager(computer, tmp);
		manager.start();

		}
	catch (std::exception e) { std::cout << e.what() << std::endl; }

	return 0;
	}
