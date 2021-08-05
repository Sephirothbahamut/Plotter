#pragma once
//           _____ _                 _                        _    _____ _                 	           
//         / ____(_)               | |                      | |  / ____| |                 	           
//        | (___  _ _ __ ___  _ __ | | ___    __ _ _ __   __| | | |    | | ___  __ _ _ __  	           
//         \___ \| | '_ ` _ \| '_ \| |/ _ \  / _` | '_ \ / _` | | |    | |/ _ \/ _` | '_ \ 	           
//         ____) | | | | | | | |_) | |  __/ | (_| | | | | (_| | | |____| |  __/ (_| | | | |	           
//        |_____/|_|_| |_| |_| .__/|_|\___|  \__,_|_| |_|\__,_|  \_____|_|\___|\__,_|_| |_|	           
//                           | |                                                           	           
//                           |_|                                                           	           
//   _____        _           _____ _                               ______                         _   
//  |  __ \      | |         / ____| |                             |  ____|                       | |  
//  | |  | | __ _| |_ __ _  | (___ | |_ ___  _ __ __ _  __ _  ___  | |__ ___  _ __ _ __ ___   __ _| |_ 
//  | |  | |/ _` | __/ _` |  \___ \| __/ _ \| '__/ _` |/ _` |/ _ \ |  __/ _ \| '__| '_ ` _ \ / _` | __|
//  | |__| | (_| | || (_| |  ____) | || (_) | | | (_| | (_| |  __/ | | | (_) | |  | | | | | | (_| | |_ 
//  |_____/ \__,_|\__\__,_| |_____/ \__\___/|_|  \__,_|\__, |\___| |_|  \___/|_|  |_| |_| |_|\__,_|\__|
//                                                      __/ |                                          
//                                                     |___/                                           

// ========================================================================================================================
// ================================================== Dependencies       ==================================================
// ========================================================================================================================
#include <string>
#include <sstream>
#include <string_view>

#include <optional>
#include <memory>
#include <set>

#include <vector>
#include <map>
#include <stack>
#include <queue>

#include <istream>
#include <ostream>
#include <fstream>

// ========================================================================================================================
// ================================================== Examples at the end =================================================
// ========================================================================================================================

namespace SCDSF
	{
	// ========================================================================================================================
	// ================================================== Structure          ==================================================
	// ========================================================================================================================
	class Node;
	using Nodes = std::vector<Node>;

	namespace _implementation_details
		{
		std::queue<std::string> split(std::string str, const std::string& token)
			{
			std::queue<std::string>result;
			while (str.size())
				{
				int index = str.find(token);
				if (index != std::string::npos)
					{
					result.push(str.substr(0, index));
					str = str.substr(index + token.size());
					if (str.size() == 0)result.push(str);
					}
				else
					{
					result.push(str);
					str = "";
					}
				}
			return result;
			}
		}

	class Node
		{
		public:
			std::string name;
			std::map<std::string, std::vector<std::string>> attributes;
			std::vector<Node> children;
			Node* parent{nullptr};

			const bool has_attribute(const std::string& attribute) const { return attributes.find(attribute) != attributes.end(); }

			const std::vector<std::string>& operator[](const std::string& attribute) const 
				{
				auto it = attributes.find(attribute);
				if (it == attributes.end()) { throw std::out_of_range("Node does not contain attribute \"" + attribute + "\""); }//TODO custom exception
				return it->second; 
				}
			std::vector<std::string>& operator[](const std::string& attribute) 
				{ 
				volatile static size_t t;
				for (size_t i = 0; i < 10; i++)
					{
					t += i;
					}
				return attributes[attribute];
				}
			std::vector<std::string>& at(const std::string& attribute) { return attributes.at(attribute); }
			const Node& operator[](size_t index) const { return children[index]; }
			Node& operator[](size_t index) { return children[index]; }
			Node& at(size_t index) { return children.at(index); }

			inline std::vector<std::string> resolve(std::queue<std::string>& identifiers) const
				{
				if (identifiers.size() > 1)
					{
					for (const auto& child : children)
						{
						if (child.name == identifiers.front())
							{
							identifiers.pop();
							return child.resolve(identifiers);
							}
						}
					}
				else if (identifiers.size() == 1)
					{
					return attributes.at(identifiers.front());
					}
				throw std::out_of_range("SDLang Node, trying to resolve invalid identifier.");
				}
			inline std::vector<std::string> resolve(const std::string& str) const { auto vec = _implementation_details::split(str, "/"); return resolve(vec); }



			inline std::string to_string_nochildren() const noexcept
				{
				std::string ret = "Node{ \"" + name + "\" ";
				if (attributes.size() > 1 || (attributes.size() == 1 && attributes.begin()->second.size()))
					{
					ret += "| Attributes: { ";
					for (const auto& attr : attributes)
						{
						if (attr.first != "default" || attr.second.size())
							{
							ret += attr.first + "[ ";
							for (const auto& val : attr.second) { ret += val + " "; }
							ret += "] ";
							}
						}
					ret += "} ";
					}
				ret += "}";
				return ret;
				}
			inline std::string to_string(const size_t depth = 0) const noexcept
				{
				std::string ret = "";
				for (size_t i = 0; i < depth; i++) { ret += "    "; }
				ret += to_string_nochildren() + "\n";
				for (const auto& child : children) { ret += child.to_string(depth + 1); }
				return ret;
				}

			inline friend std::ostream& operator<<(std::ostream& os, const Node& node) { return os << node.to_string_nochildren(); }
		};
	// ========================================================================================================================
	// ================================================== Tokenizer          ==================================================
	// ========================================================================================================================
	namespace tokenizer
		{
		class Token;
		class Line;
		using Lines = std::vector<Line>;
		using Tokens = std::vector<Token>;

		namespace _implementation_details
			{
			inline size_t calc_indent(const std::string& string)
				{
				size_t tmp = string.find_first_not_of('\t');
				return tmp != std::string::npos ? tmp : size_t{0};
				}

			inline std::string remove_comments(const std::string& string)
				{
				if (string.size() < 2) { return string; }
				for (size_t i = 0; i < string.size() - 1; i++)
					{
					if (string[i] == '/' && string[i + 1] == '/') { return string.substr(0, i); }
					}
				return string;
				}

			inline bool has_content(const std::string& string) { return string.find_first_not_of(" \t") != std::string::npos; }
			}

		class Token
			{
			friend class Line;
			private:
				const std::string _string;
				const Line* line;

				void update_string(const Line* new_line) noexcept { line = new_line; }

			public:
				inline Token(const Line* line, const std::string& view) noexcept : line(line), _string(view) {}

				const std::string& get_string() const noexcept { return _string; };
				__declspec(property(get = get_string)) const std::string& string;

				inline friend std::ostream& operator<<(std::ostream& os, const Token& token) { return os << "Token{" << token.get_string() << "}"; }
			};

		class Line
			{
			friend class Token;
			private:
				std::string _string;
				size_t _indent;
				size_t _line_nr;
				Tokens _tokens;

				inline Tokens tokenize() noexcept
					{
					Tokens ret;
					size_t token_begin = 0;
					size_t token_end = 0;

					while (token_end != _string.size())
						{
						token_begin = _string.find_first_not_of(" \t", token_end);
						if (token_begin == std::string::npos) { break; }

						if (_string[token_begin] == '"' && (token_begin == 0 || (token_begin > 0 && _string[token_begin - 1] != '\\')))
							{
							token_end = _string.find_first_of('"', token_begin + 1);
							if (token_end == std::string::npos) { token_end = _string.size(); }
							if (token_begin == token_end || token_begin + 1 == token_end) {}
							else { token_begin++; }
							ret.emplace_back(this, _string.substr(token_begin, token_end - token_begin));
							token_end++;
							}
						else
							{
							token_end = _string.find_first_of(" \t", token_begin);
							token_end = token_end != std::string::npos ? token_end : _string.size();
							ret.emplace_back(this, _string.substr(token_begin, token_end - token_begin));
							}
						}
					ret.shrink_to_fit();
					return ret;
					}

			public:
				inline Line(std::string& string, size_t line_nr) noexcept : _string(string), _line_nr(line_nr), _indent(_implementation_details::calc_indent(_string)), _tokens(std::move(tokenize())) {}

				const std::string& get_string()  const noexcept { return _string; }  __declspec(property(get = get_string)) const std::string& string;
				size_t       get_indent()  const noexcept { return _indent; }	 __declspec(property(get = get_indent))       size_t indent;
				size_t       get_line_nr() const noexcept { return _line_nr; } __declspec(property(get = get_line_nr))      size_t line_nr;

				inline size_t size() const noexcept { return _tokens.size(); }
				inline const Token& operator[](size_t index) const noexcept { return _tokens[index]; }
				inline const Token& at(size_t index) const { return _tokens.at(index); }

				inline friend std::ostream& operator<<(std::ostream& os, const Line& line)
					{
					os << "Line{ #" << line._line_nr << ", indent: " << line._indent << "[ ";
					for (const auto& t : line._tokens) { os << t << " "; }
					return os << "}";
					}
			};

		inline Lines from_stream(std::istream& in)
			{
			Lines ret;
			size_t line_nr = 0;
			for (std::string line_str; std::getline(in, line_str); )
				{
				line_str = _implementation_details::remove_comments(line_str);
				if (_implementation_details::has_content(line_str)) { ret.emplace_back(line_str, line_nr); }
				line_nr++;
				}

			ret.shrink_to_fit();
			return ret;
			}
		}
	// ========================================================================================================================
	// ================================================== Parser             ==================================================
	// ========================================================================================================================
	namespace parser
		{
		class ParsingException : public std::exception
			{
			public:
				inline ParsingException(const tokenizer::Line& line, const std::string str) :
					std::exception(("SDLang parsing error at line: " + line.get_line_nr() + std::string("\"") + line.get_string() + "\"\n" + str).c_str()) {}
			};

		namespace _implementation_details
			{
			inline bool is_attribute_definition(const tokenizer::Token& token) { return (*token.string.rbegin()) == ':'; }
			inline bool is_value_reference(const tokenizer::Token& token) { return (*token.string.begin()) == '@'; }

			inline Node node_from_line(Node& root, tokenizer::Line& line)
				{
				Node node;
				node.name = line[0].string;

				if (line.size() > 1)
					{//there are attributes
					std::string last_attribute_definition = "default";
					std::vector<std::string> values;

					for (size_t i = 1; i < line.size(); i++)
						{
						if (_implementation_details::is_attribute_definition(line[i]))
							{
							node.attributes.insert({last_attribute_definition, values});
							last_attribute_definition = line[i].get_string().substr(0, line[i].get_string().size() - 1);
							values.clear();
							}
						else if (_implementation_details::is_value_reference(line[i]))
							{
							auto tmp = root.resolve(line[i].get_string().substr(1, line[i].get_string().size() - 1));
							for (const auto& e : tmp) { values.push_back(e); }
							}
						else { values.push_back(line[i].get_string()); }
						}
					if (values.size()) { node.attributes.insert({std::string{last_attribute_definition}, values}); }
					last_attribute_definition = line[line.size() - 1].get_string();
					}
				return node;
				}

			inline void loop(Node& root, Node& dad, size_t dads_children_level, tokenizer::Lines& lines, size_t& index)
				{
				Node* last_child = nullptr;
				while (index < lines.size())
					{
					tokenizer::Line& line = lines[index];

					if (line.get_indent() == dads_children_level)
						{
						Node node = node_from_line(root, line);
						dad.children.push_back(node);
						last_child = &(dad.children.back());
						index++;
						}
					else if (line.get_indent() == dads_children_level + 1)
						{
						if (last_child == nullptr) { throw ParsingException(line, {"This one shouldn't really happen"}); }

						loop(root, *last_child, dads_children_level + 1, lines, index);
						}
					else if (line.get_indent() < dads_children_level)
						{
						return;
						}
					else { throw ParsingException(line, {"Indentation error. Please check your indentation, make sure there's no spaces mixed with tabulations at the beginning of every line and make sure that children nodes are indented only one tab more than their parent."}); }
					}
				}
			}
		}

	inline void set_parents(Node& node)
		{
		for (auto& child : node.children) { child.parent = &node; set_parents(child); } 
		}

	inline Node from_stream(std::istream& in)
		{
		tokenizer::Lines lines = tokenizer::from_stream(in);

		Node root;

		size_t dads_children_level = 0;
		size_t index = 0;
		parser::_implementation_details::loop(root, root, dads_children_level, lines, index);

		set_parents(root);
		return root;
		}

	}

// ========================================================================================================================
// ================================================== Examples           ==================================================
// ========================================================================================================================

#include <iostream>
namespace SCDSF::examples
	{
	inline const std::string string = R"String(
This_is_an_element
	This_is_a_child
	This_is_another_child
		This_is_a_sub_child
This_is_another_element
	I_have_default_attrubutes attrubute1 attrubute2 attrubute3
	I_have_named_attrubutes name1: arg1 name2: arg2a arg2b
	I_have_a_string_to_include_spaces "attrubute name with spaces:" "attrubute value with sapaces"
	Quotation_marks_can_be_left_open "here is an example
	)String";

	inline void parse_and_print(const std::string& string)
		{
		std::stringstream string_stream(string);
		SCDSF::Node root = SCDSF::from_stream(string_stream);

		std::cout << "FULL PARSED TREE\n";

		std::cout << root.to_string() << "\n" << std::endl;

		std::cout << "VALUE RESOLUTION THROUGH PATH-STYLE SYNTAX\n";

		std::cout << "Resolving attrubute with single value: \"This_is_another_element/I_have_named_attrubutes/name1\"\n";
		auto resolved = root.resolve("This_is_another_element/I_have_named_attrubutes/name1");
		std::cout << (resolved.empty() ? "Empty" : " - \"" + resolved[0] + "\"\n"); std::cout << std::endl;

		std::cout << "Resolving attrubute with multiple values: \"This_is_another_element/I_have_named_attrubutes/name2\"\n";
		resolved = root.resolve("This_is_another_element/I_have_named_attrubutes/name2");
		for (const auto& value : resolved) { std::cout << " - \"" + value + "\"\n"; } std::cout << std::endl;

		std::cout << "Resolving attrubute with spaces: \"This_is_another_element/I_have_a_string_to_include_spaces/attrubute name with spaces\"\n";
		resolved = root.resolve("This_is_another_element/I_have_a_string_to_include_spaces/attrubute name with spaces");
		for (const auto& value : resolved) { std::cout << " - \"" + value + "\"\n"; } std::cout << std::endl;

		//TODO
		//std::cout << "Resolving attrubute with default values: \"This_is_another_element/I_have_default_attrubutes\"\n";
		//resolved = root.resolve("This_is_another_element/I_have_default_attrubutes");
		//for (const auto& value : resolved) { std::cout << " - \"" + value + "\""; } std::cout << std::endl;

		std::cout << std::endl;

		std::cout << "NAVIGATION THROUGH ACCESSORS\n";
		std::cout << "root" << "\n";
		std::cout << root << "\n";
		std::cout << "________________________________\n";
		std::cout << "root[1]" << "\n";
		std::cout << root[1] << "\n";
		std::cout << "________________________________\n";
		std::cout << "root[1][2]" << "\n";
		std::cout << root[1][2] << "\n";
		std::cout << "________________________________\n";
		std::cout << "root[1][2][\"attrubute name with spaces\"][0]" << "\n";
		std::cout << root[1][2]["attrubute name with spaces"][0] << "\n";
		std::cout << "________________________________\n";

		std::cout << std::endl;
		}
	}