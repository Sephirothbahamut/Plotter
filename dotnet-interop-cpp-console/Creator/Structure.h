#pragma once
#include <vector>
#include <string>
#include <fstream>
#include "../utils.h"

namespace creator
	{
	class Node
		{
		public:
			::std::string name;
			::std::string argstring;
			::std::vector<::std::string> args;
			::std::vector<::std::unique_ptr<Node>> children;

			Node(const ::std::string& name, const ::std::string argstring, const ::std::vector<::std::string>& args) : name(name), argstring(argstring), args(args) {}

			::std::string to_string(size_t depth = 0) const noexcept
				{
				::std::string ret = "";
				for (size_t i = 0; i < depth; i++) { ret += "\t"; }
				ret += name + " : " + argstring + "\n";
				for (auto& child : children) { ret += child->to_string(depth + 1); }
				return ret;
				}
		};

	class StructureParser
		{
		private:
			struct Line
				{
				int indent;
				::std::string argstring;
				::std::vector<::std::string> tokens;
				Line(int indent, const ::std::string argstring, const ::std::vector<::std::string>& tokens) : indent(indent), argstring(argstring), tokens(tokens) {}
				};

			
			static int loop(::std::vector<Line>& lines, int i, Node* parent, int depth)
				{
				Node* last_node = nullptr;
				while (i < lines.size())
					{
					Line& line = lines[i];
					::std::unique_ptr<Node> node = ::std::make_unique<Node>(line.tokens[0], line.argstring, ::std::vector<::std::string>(line.tokens.begin() + 1, line.tokens.end()));

					if (line.indent == depth)
						{
						last_node = node.get();
						parent->children.push_back(::std::move(node));
						}
					else if (line.indent > depth) { i = loop(lines, i, last_node, line.indent); continue; }
					else //if (line.indent < depth)
						{
						return i;
						}
					i++;
					}
				return lines.size();
				}

		public:

			static ::std::unique_ptr<Node> from_file(::std::string file_name)
				{
				::std::ifstream in(file_name);
				if (!in.is_open()) { return nullptr; }

				::std::unique_ptr<Node> root = ::std::make_unique<Node>("", "", ::std::vector<::std::string>());
				::std::vector<Line> lines;

				for (::std::string line; ::std::getline(in, line); )
					{
					size_t comment_index = line.find_first_of('/');
					while (comment_index != ::std::string::npos)
						{
						if (comment_index < line.size() + 1 && line[comment_index + 1] == '/') { line.erase(comment_index, line.size() - comment_index);  break; }
						comment_index = line.find_first_of('/', comment_index + 1);
						}

					size_t indent = line.find_first_not_of('\t');
					line = line.erase(0, indent);

					::std::string argstring = line;
					size_t argstart = argstring.find_first_not_of(' ', argstring.find_first_of(' '));
					argstring = argstring.erase(0, argstart);

					::std::vector<::std::string> tokens = utils::split(line, ' ');
					if (tokens.size()) { lines.emplace_back(indent, argstring, tokens); }
					}

				loop(lines, 0, root.get(), 0);

				return root;
				}

		};
		/**/


	}