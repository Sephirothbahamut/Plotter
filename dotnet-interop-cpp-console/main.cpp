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
int mainq(int argc, char** argv)
	{

	sf::RenderWindow awnd{sf::VideoMode{800, 600}, "a"};
	sf::RenderWindow bwnd{sf::VideoMode{800, 600}, "b"};
	HWND ahwnd = awnd.getSystemHandle();
	HWND bhwnd = bwnd.getSystemHandle();

	utils::window::make_glass_CompositionAttribute(ahwnd);
	utils::window::make_glass_DWM(bhwnd);

	sf::Color bg;

	while (awnd.isOpen())
		{
		sf::Event e;
		if (awnd.waitEvent(e))
			{
			switch (e.type)
				{
				case sf::Event::Closed: awnd.close(); bwnd.close(); break;
				case sf::Event::KeyReleased:
					switch (e.key.code)
						{
						case sf::Keyboard::Q: bg = sf::Color{  0,   0,   0,   0}; break;
						case sf::Keyboard::W: bg = sf::Color{  0,   0,   0,  10}; break;
						case sf::Keyboard::E: bg = sf::Color{  0,   0,   0,  80}; break;
						case sf::Keyboard::R: bg = sf::Color{  0,   0,   0, 100}; break;
						case sf::Keyboard::T: bg = sf::Color{  0,   0,   0, 150}; break;
						case sf::Keyboard::Y: bg = sf::Color{  0,   0,   0, 200}; break;
						case sf::Keyboard::U: bg = sf::Color{  0,   0,   0, 255}; break;
						case sf::Keyboard::A: bg = sf::Color{  0,   0, 255,   0}; break;
						case sf::Keyboard::S: bg = sf::Color{  0,   0, 255,  10}; break;
						case sf::Keyboard::D: bg = sf::Color{  0,   0, 255,  80}; break;
						case sf::Keyboard::F: bg = sf::Color{  0,   0, 255, 100}; break;
						case sf::Keyboard::G: bg = sf::Color{  0,   0, 255, 150}; break;
						case sf::Keyboard::H: bg = sf::Color{  0,   0, 255, 200}; break;
						case sf::Keyboard::J: bg = sf::Color{  0,   0, 255, 255}; break;
						}
					break;
				}
			}
		awnd.clear(bg);
		bwnd.clear(bg);

		sf::RectangleShape rect{{100, 50}};
		rect.setFillColor({255, 0, 0, 255});
		awnd.draw(rect); bwnd.draw(rect);
		rect.setPosition({50, 0});
		rect.setFillColor({0, 255, 0, 100});
		awnd.draw(rect); bwnd.draw(rect);

		awnd.display();
		bwnd.display();
		}

	return 0;
	}

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
