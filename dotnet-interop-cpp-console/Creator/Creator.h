#pragma once
#include <string>
#include <memory>
#include <mutex>
#include <optional>
#include <chrono>

#include "Computer.h"
//#include "Structure.h"
#include "../UI.h"

//parsing
#include <fstream>
#include "../SCDSF.h"

namespace creator
	{

	class UICreationException : public std::exception
		{
		public:
			inline UICreationException(const SCDSF::Node& node, const std::string str) :
				std::exception(("UI creation exception for node: " + node.to_string() + "\n" + str).c_str()) {}
		};

	struct UICreated
		{
		std::vector<UI::Window::Properties> windows_factories;
		std::vector<std::unique_ptr<Sensor>> sensors;
		std::chrono::milliseconds update_delay;
		};

	class UICreator
		{
		private:
			SCDSF::Node root;
			const Computer& computer;
			std::vector<UI::Window::Properties> windows_factories;
			std::map<std::string, std::unique_ptr<Sensor>> sensors;

			//Settings read from file beg
			sf::Color default_window_bg{sf::Color::Black};
			sf::Color default_graphics_bg{sf::Color::Black};
			sf::Color default_graphics_fg{sf::Color::White};
			std::chrono::milliseconds update_delay{1000};
			//Settings read from file end

			UICreator(const Computer& computer) : computer(computer) {}

			inline static UI::Alignment valign_from_str(const std::string& arg)
				{
				if (arg == "beg" || arg == "begin" || arg == "top" || arg == "up") { return UI::Alignment::beg; }
				else if (arg == "mid" || arg == "middle" || arg == "center") { return UI::Alignment::mid; }
				else if (arg == "end" || arg == "bot" || arg == "bottom" || arg == "down") { return UI::Alignment::mid; }
				else { throw std::exception("Error in custom ui file: invalid vertical alignment (beg|begin|top|up, mid|middle|center, end|bot|bottom|down)."); }
				}
			inline static UI::Alignment halign_from_str(const std::string& arg)
				{
				if (arg == "beg" || arg == "begin" || arg == "left" || arg == "ll") { return UI::Alignment::beg; }
				else if (arg == "mid" || arg == "middle" || arg == "center") { return UI::Alignment::mid; }
				else if (arg == "end" || arg == "right" || arg == "rr") { return UI::Alignment::mid; }
				else { throw std::exception("Error in custom ui file: invalid horizontal alignment (beg|begin|left|ll, mid|middle|center, end|right|rr)."); }
				}

			inline Sensor* get_sensor(const std::string& string)
				{
				auto it = sensors.find(string);
				if (it == sensors.end())
					{
					std::unique_ptr<Sensor> s(std::move(computer.get_sensor(string)));
					//if (s == nullptr) { throw std::exception(("Sensor \"" + string + "\" does not exist.").c_str()); }
					if (s == nullptr)
						{
						auto it = sensors.find("null");
						if (it == sensors.end())
							{
							auto s = std::make_unique<SensorMissing>();
							sensors.insert({"null", std::move(s)});
							return get_sensor("null");
							}
						return it->second.get();
						}

					sensors.insert({string, std::move(s)});
					return get_sensor(string);
					}
				else { return it->second.get(); }
				}
			inline Sensor* get_sensor(const std::vector<std::string>& strings)
				{
				if (strings.size() == 1) { return get_sensor(strings[0]); }
				else if(strings.size() == 2)
					{
					std::string composite_name = strings[0] + "+" + strings[1];
					auto it = sensors.find(composite_name);
					if (it == sensors.end())
						{
						Sensor* a = get_sensor(strings[0]);
						Sensor* b = get_sensor(strings[1]);
						sensors.insert({composite_name, std::make_unique<SensorSum>(a, b)});
						return get_sensor(strings);
						}
					else { return it->second.get(); }
					}
				}
			inline static sf::Color get_color(const std::vector<std::string>& strings)
				{
				if (strings.size() != 3 && strings.size() != 4) { throw std::exception("Cannot for color. A color attribute requires 3 or 4 values (red, green, blue, alpha)."); }
				return sf::Color(std::stoul(strings[0]), std::stoul(strings[1]), std::stoul(strings[2]), strings.size() == 4 ? std::stoul(strings[3]) : 255);
				}

			inline std::unique_ptr<UI::Element> create_element(const SCDSF::Node& node)
				{
#ifdef _DEBUG
				std::cout << "_________________________________________" << std::endl;
				std::cout << "PARSING ELEMENT:" << std::endl;
				std::cout << node.to_string() << std::endl;
#endif

				if (node.name == "horizontal")
					{
					if (node.children.size() == 0) { throw UICreationException(node, "Containers must have at least one child."); }
					std::vector<std::unique_ptr<UI::Element>> children;
					for (auto& n : node.children) { children.push_back(std::move(create_element(n))); }

					UI::Alignment alignment;
					if (node.attributes.size() == 0) { alignment = UI::Alignment::beg; }
					else if (node.has_attribute("valign") && node["valign"].size() == 1) { alignment = valign_from_str(node["valign"][0]); }
					else { throw UICreationException(node, "Either use no argument or a \"valign\" argument with values: beg|begin|top|up, mid|middle|center, or end|bot|bottom|down."); }
					return std::make_unique<UI::HBox>(alignment, children);
					}
				else if (node.name == "vertical")
					{
					if (node.children.size() == 0) { throw UICreationException(node, "Containers must have at least one child."); }
					std::vector<std::unique_ptr<UI::Element>> children;
					for (auto& n : node.children) { children.push_back(std::move(create_element(n))); }
					UI::Alignment alignment;
					if (node.attributes.size() == 0) { alignment = UI::Alignment::beg; }
					else if (node.has_attribute("halign") && node["halign"].size() == 1) { alignment = halign_from_str(node["halign"][0]); }
					else { throw UICreationException(node, "Either use no argument or a \"halign\" argument with values: beg|begin|left|ll, mid|middle|center, or end|right|rr."); }
					return std::make_unique<UI::VBox>(alignment, children);
					}
				else if (node.name == "size_wrapper")
					{
					if (node.children.size() == 0) { throw UICreationException(node, "Wrappers must have exactly one child."); }
					constexpr auto exception_string = "Optional arguments: \"halign\", \"valign\", \"max_width\", \"min_width\", \"max_height\", \"min_height\".\nThe min / max arguments accept any number.\nhalign accepts beg|begin|left|ll, mid|middle|center, or end|right|rr)\nvalign alignment accepts beg|begin|top|up, mid|middle|center, or end|bot|bottom|down.";
					UI::Alignment halign = UI::Alignment::beg;
					UI::Alignment valign = UI::Alignment::beg;
					std::optional<float> width_max = std::nullopt;
					std::optional<float> width_min = std::nullopt;
					std::optional<float> height_max = std::nullopt;
					std::optional<float> height_min = std::nullopt;

					for (const auto& attribute : node.attributes)
						{
						const auto& arg1 = attribute.first;
						if (arg1 != "default")
							{
							if (attribute.second.size() != 1) { throw UICreationException(node, exception_string); }

							const auto& arg2 = attribute.second[0];

							     if (arg1 == "halign")     { halign = halign_from_str(arg2); }
							else if (arg1 == "valign")     { valign = valign_from_str(arg2); }
							else if (arg1 == "width_max")  { width_max  = std::stof(arg2); }
							else if (arg1 == "width_min")  { width_min  = std::stof(arg2); }
							else if (arg1 == "height_max") { height_max = std::stof(arg2); }
							else if (arg1 == "height_min") { height_min = std::stof(arg2); }
							else { throw UICreationException(node, exception_string); }
							}
						}
					auto tmp = create_element(node.children[0]);
					return std::make_unique<UI::SizeWrapper>(tmp, halign, valign, width_max, width_min, height_max, height_min);
					}
				else if (node.name == "spacer")
					{
					if (node.children.size() != 0) { throw UICreationException(node, "Spacers do not support children must have exactly one child."); }

					bool hor_true_ver_false;
					     if (node.parent->name == "horizontal") { hor_true_ver_false = true; }
					else if (node.parent->name == "vertical")   { hor_true_ver_false = false; }
					else { throw UICreationException(node, "Spacers must be inside an horizontal or vertical group."); }

					constexpr auto exception_string = "Spacer optional arguments: \"max_width\", \"min_width\", \"max_height\", \"min_height\".\nThe min / max arguments accept any number.";
					std::optional<float> width_max  = std::nullopt;
					std::optional<float> width_min  = std::nullopt;
					std::optional<float> height_max = std::nullopt;
					std::optional<float> height_min = std::nullopt;

					for (const auto& attribute : node.attributes)
						{
						const auto& arg1 = attribute.first;
						if (arg1 != "default")
							{
							if (attribute.second.size() != 1) { throw UICreationException(node, exception_string); }

							const auto& arg2 = attribute.second[0];

							     if (arg1 == "width_max")  { width_max  = std::stof(arg2); }
							else if (arg1 == "width_min")  { width_min  = std::stof(arg2); }
							else if (arg1 == "height_max") { height_max = std::stof(arg2); }
							else if (arg1 == "height_min") { height_min = std::stof(arg2); }
							else { throw UICreationException(node, exception_string); }
							}
						}
					if (hor_true_ver_false) { return std::make_unique<UI::HSpacer>(width_max, width_min, height_max, height_min); }
					else                    { return std::make_unique<UI::VSpacer>(width_max, width_min, height_max, height_min); }
					}
				else if (node.name == "separator")
					{
					if (node.children.size() != 0) { throw UICreationException(node, "Separators do not support children must have exactly one child."); }

					bool hor_true_ver_false;
					     if (node.parent->name == "horizontal")	{ hor_true_ver_false = true;  }
					else if (node.parent->name == "vertical"  ) { hor_true_ver_false = false; }
					else { throw UICreationException(node, "Separators must be inside an horizontal or vertical group."); }

					sf::Color color = default_graphics_fg;
					for (const auto& attribute : node.attributes)
						{
						const auto& arg1 = attribute.first;
						if (arg1 != "default")
							{
							if (arg1 == "color") { color = get_color(attribute.second); }
							else { throw UICreationException(node, "Separator optional arguments: \"color\".\nColors are defined in red, green and blue values, plus additionally alpha for transparency (Example \"color: 255 0 0 80\" is a transparent red)."); }
							}
						else { throw UICreationException(node, "Separator optional arguments: \"color\".\nColors are defined in red, green and blue values, plus additionally alpha for transparency (Example \"color: 255 0 0 80\" is a transparent red)."); }
						}

					if (hor_true_ver_false) { return std::make_unique<UI::HSeparator>(color); }
					else                    { return std::make_unique<UI::VSeparator>(color); }
					}

				if (node.children.size() != 0 && node.name != "plot") { throw UICreationException(node, "Only containers (horizontal, vertical) and wrappers (size_wrapper) can have generic children, plots can only have plotlines as children."); }

				if (node.name == "text")
					{
					if (node.attributes.size() != 1 || !node.has_attribute("default") || node["default"].size() != 1) { throw UICreationException(node, "Expected a single string as unnamed argument. If your string contains spaces, wrap it with quation marks \"like this\"."); }
					return std::make_unique<UI::Text>(node["default"][0]);
					}
				else if (node.name == "reading")
					{
					if (!node.has_attribute("label") || node["label"].size() != 1 || !node.has_attribute("sensor"))
						{
						throw UICreationException(node, "Arguments must be \"label\" with some text and a \"sensor\". Optionally there can be 2 more sensors under \"sensor2\" and \"sensor3\".");
						}

					sf::String label = node["label"][0];
					Sensor* sensor1 = get_sensor(node["sensor"]);
					Sensor* sensor2 = nullptr;
					Sensor* sensor3 = nullptr;
					if (node.has_attribute("sensor2"))
						{
						sensor2 = get_sensor(node["sensor2"]);
						if (node.has_attribute("sensor3")) { sensor3 = get_sensor(node["sensor3"]); }
						}
					else if (node.has_attribute("sensor3")) { throw UICreationException(node, "In order to have a sensor3 there must be a sensor2."); }

					if (sensor3 != nullptr && sensor2 != nullptr) { return std::make_unique<UI::ReadingPercentValueOnTotal>(label, sensor1, sensor2, sensor3); }
					else if (sensor2 != nullptr) { return std::make_unique<UI::ReadingValueOnTotal>(label, sensor1, sensor2); }
					else { return std::make_unique<UI::ReadingValue>(label, sensor1); }
					}
				else if (node.name == "plot")
					{
					if (node.children.size() == 0) { throw UICreationException(node, "Plots must have at least one child and all children must be of type plot_line or plot_mountain."); }
					std::vector<std::unique_ptr<UI::PlotContent>> children;

					sf::Color bg_color = default_graphics_bg;
					if (node.has_attribute("color")) { bg_color = get_color(node["color"]); }

					for (auto& n : node.children)
						{
						if (n.name != "plot_line" && n.name != "plot_mountain") { throw UICreationException(node, "Plots must have at least one child and all children must be of type plot_line or plot_mountain."); }
						Sensor* sensor;
						sf::Color color = default_graphics_fg;
						float min = 0;
						float max = 100;
						bool fade = false;

						if(n.attributes.size() < 1 && !n.has_attribute("sensor")) { throw UICreationException(node, "There must be a \"sensor\" argument.\nOptional arguments: \"sensor\", \"color\", \"min:\", \"max:\".\nColors are defined in red, green and blue values, plus additionally alpha for transparency (Example \"color: 255 0 0 80\" is a transparent red).\nMin and max represent the minimum and maximum value this sensor can assume (default is 0-100 which works fine for percentages and celcious degrees. Something like fan rpms might require a maximum of ~3000)."); }
						for (const auto& attribute : n.attributes)
							{
							const auto& arg1 = attribute.first;
							if (arg1 != "default")
								{
								     if (arg1 == "sensor" && attribute.second.size() == 1) { sensor = get_sensor(attribute.second[0]); }
								else if (arg1 == "color")                                  { color = get_color(attribute.second); }
								else if (arg1 == "min"    && attribute.second.size() == 1) { min = std::stof(attribute.second[0]); }
								else if (arg1 == "max"    && attribute.second.size() == 1) { max = std::stof(attribute.second[0]); }
								else if (arg1 == "fade"   && attribute.second.size() == 1 && n.name == "plot_mountain") { fade = attribute.second[0] == "true"; }
								else { throw UICreationException(node, "There must be a \"sensor\" argument.\nOptional arguments: \"sensor\", \"color\", \"min\", \"max\".\nColors are defined in red, green and blue values, plus additionally alpha for transparency (Example \"color: 255 0 0 80\" is a transparent red).\nMin and max represent the minimum and maximum value this sensor can assume (default is 0-100 which works fine for percentages and celcious degrees. Something like fan rpms might require a maximum of ~3000)."); }
								}
							}
						     if (n.name == "plot_line")     { children.push_back(std::move(std::make_unique<UI::PlotLine>    (sensor, color, max, min))); }
						else if (n.name == "plot_mountain" && !fade) { children.push_back(std::move(std::make_unique<UI::PlotMountain<false>>(sensor, color, max, min))); }
						else if (n.name == "plot_mountain" &&  fade) { children.push_back(std::move(std::make_unique<UI::PlotMountain<true >>(sensor, color, max, min))); }
						}
					return std::make_unique<UI::Plot>(children, bg_color);
					}
				else if (node.name == "plot_line") { throw UICreationException(node, "Plot lines must be inside a plot."); }
				else if (node.name == "line")
					{
					if (!node.has_attribute("sensor")) { throw UICreationException(node, "There must be a \"sensor\" argument. There can be a \"color\" optional argument.\nColors are defined in red, green and blue values, plus additionally alpha for transparency (Example \"color: 255 0 0 80\" is a transparent red)."); }
					sf::Color color = default_graphics_fg;
					if (node.has_attribute("color")) { color = get_color(node["color"]); }
					float min = 0; float max = 100;
					if (node.has_attribute("min")) { min = std::stof(node["min"][0]); }
					if (node.has_attribute("max")) { max = std::stof(node["max"][0]); }
					return std::make_unique<UI::Line>(get_sensor(node["sensor"]), color, min, max);
					}

				throw UICreationException(node, "Invalid node");
				}

			inline void create_window(const SCDSF::Node& node)
				{
				if (node.children.size() != 1) { throw std::exception("Error in custom ui file: \"window\" must contain exactly one ui element."); }

				UI::Window::Properties p;

				for (const auto& attribute : node.attributes)
					{
					const auto& arg1 = attribute.first;
					if (arg1 != "default")
						{
						     if (arg1 == "color")                                    { p.color    = get_color(attribute.second); }
						else if (arg1 == "size"     && attribute.second.size() == 2) { p.size     = {std::stoul(attribute.second[0]), std::stoul(attribute.second[1])}; }
						else if (arg1 == "position" && attribute.second.size() == 2) { p.position = {std::stoi(attribute.second[0]),  std::stoi(attribute.second[1])};  } 
						else if (arg1 == "title")                                    { p.title    = attribute.second[0]; }
						else if (arg1 == "transparency")                              
								 {
								      if (attribute.second[0] == "no")          { p.transparency = UI::Window::Properties::Transparency::no; }
								 else if (attribute.second[0] == "glass")       { p.transparency = UI::Window::Properties::Transparency::glass; }
								 else if (attribute.second[0] == "transparent") { p.transparency = UI::Window::Properties::Transparency::transparent; }
								 }
						else if (arg1 == "borders") { p.borders = attribute.second[0] == "true"; }
						else { throw UICreationException(node, "Error in \"window\" arguments. Optional arguments are \"size\", \"position\", \"color\", \"borders\" and \"transparency\".\nColors are defined in red, green and blue values, plus additionally alpha for transparency (Example \"color: 255 0 0 80\" is a transparent red).\nSize and position are a pair of integers (Example \"size: 800 600\".\nTransparency can be \"no\", \"glass\" or \"transparent\".\nBorders can be \"true\" or \"false\"."); }
						}
					}

				std::unique_ptr<UI::Element> root = create_element(node.children[0]);
				p.root = std::move(root);

				windows_factories.push_back(std::move(p));
				}

			inline void create_ui(const SCDSF::Node& node)
				{
				for (const auto& child : node.children)
					{
					if (child.name != "window") { throw std::exception("Error in custom ui file: \"ui\" must only contain elements of type \"window\"."); }
					create_window(child);
					}
				}

		public:
			inline static UICreated from_file(std::string file_name, const Computer& computer)
				{
				UICreator creator(computer);
				std::ifstream file(file_name);
				SCDSF::Node root = SCDSF::from_stream(file);

				for (const auto& node : root.children)
					{
					if (node.name == "settings")
						{
						for (const auto& setting : node.children)
							{
							     if (setting.name == "update_delay" && setting.has_attribute("default")) { creator.update_delay        = std::chrono::milliseconds{std::stoll(setting["default"][0])}; }
							else if (setting.name == "window_bg"    && setting.has_attribute("default")) { creator.default_window_bg   = get_color(setting["default"]); }
							else if (setting.name == "graphics_bg"  && setting.has_attribute("default")) { creator.default_graphics_bg = get_color(setting["default"]); }
							else if (setting.name == "graphics_fg"  && setting.has_attribute("default")) { creator.default_graphics_fg = get_color(setting["default"]); }
							}
						}
					}

				for (const auto& node : root.children)
					{
					if (node.name == "ui")
						{
						if (creator.windows_factories.size()) { throw std::exception("Error in custom ui file: \"ui\" has been defined more than once."); }
						creator.create_ui(node);
						}
					}

				std::vector<std::unique_ptr<Sensor>> sensors_v;
				for (auto& s : creator.sensors) { sensors_v.push_back(std::move(s.second)); }

				return {std::move(creator.windows_factories), std::move(sensors_v), creator.update_delay};
				}
		};
	}