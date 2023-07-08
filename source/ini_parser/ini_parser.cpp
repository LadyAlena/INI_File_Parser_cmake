#include "ini_parser.h"

#include <string>
#include <set>

#include <fstream>

#include <exception>
#include <regex>
#include <algorithm>

ini_parser::ini_parser(std::string filename) {
	fin = std::move(std::ifstream(filename));
	if (!fin.is_open()) { throw std::ifstream::failure("Could not open the file"); }
	process_file();
}

ini_parser::~ini_parser() {
	if (fin.is_open()) fin.close();
}

void ini_parser::process_file() {

	std::string input_str{""};

	std::string name_section{""};
	std::string name_var{""};
	std::string value_var{""};

	auto section_var = std::move(std::make_pair(name_section, name_var)); // key = name_section + name_var

	bool no_vars{};

	while (std::getline(fin, input_str)) {

		number_of_rows++;

		// deleting a comment line
		auto begin_comment = input_str.find(';');
		if (begin_comment != std::string::npos) input_str.erase(begin_comment, input_str.length());

		// searching for lines consisting entirely of spaces (empty lines) and deleting them
		bool empty_str{ true };
		for (const auto& ch : input_str) {
			if (ch != ' ') {
				empty_str = false;
				break;
			}
		}
		if (empty_str) input_str.clear();

		if (!input_str.size()) continue;

		auto simbol_equal = input_str.find('=');
		auto left_bracket = input_str.find('[');
		auto right_bracket = input_str.find(']');

		if (simbol_equal == std::string::npos &&
			left_bracket == std::string::npos &&
			right_bracket == std::string::npos) {
			throw std::invalid_argument("Error: Unknown string format on line " + std::to_string(number_of_rows));
		}

		// search for a string with a section name
		auto number_left_bracket = std::count(input_str.begin(), input_str.end(), '[');
		auto number_right_bracket = std::count(input_str.begin(), input_str.end(), ']');

		if (number_left_bracket || number_right_bracket) {

			if (number_left_bracket != number_right_bracket) {
				throw std::invalid_argument("Error: Incorrect format of the section name string on line " + std::to_string(number_of_rows));
			}

			name_section = input_str.substr(input_str.find('[') + 1, input_str.find(']') - 1);

			if (!check_correct_name(name_section)) {
				throw std::invalid_argument("Error: Incorrect name of the section on line " + std::to_string(number_of_rows));
			}

			no_vars = true;
			name_var = "";
			value_var = "";
		}

		// search for a string with a variable name and its value
		auto number_simbol_equal = std::count(input_str.begin(), input_str.end(), '=');

		if (number_simbol_equal) {

			if (number_simbol_equal > 1) {
				throw std::invalid_argument("Error: Incorrect format of the variable definition string on line " + std::to_string(number_of_rows));
			}

			if (no_vars) {
				no_vars = false;
				auto it = std::find_if(content_file.begin(),
					content_file.end(),
					[&name_section](const auto& pair) {
						auto key = pair.first;
						auto s = key.first == name_section;
						auto v = key.second == "";
						return  s && v;
					});
				if (it != content_file.end()) content_file.erase(it);
			}

			auto begin_value = input_str.find('=');

			name_var = input_str.substr(0, begin_value);
			name_var.erase(std::remove_if(name_var.begin(), name_var.end(), [](char c) { return std::isspace(c); }),
				name_var.end());

			if (!check_correct_name(name_var)) {
				throw std::invalid_argument("Error: Incorrect name of the variable on line " + std::to_string(number_of_rows));
			}

			value_var = input_str.substr(begin_value + 1, input_str.length());
		}

		section_var.first = name_section;
		section_var.second = name_var;
		content_file[section_var] = value_var;
	}
}

bool ini_parser::check_correct_name(std::string name) noexcept {
	std::regex pattern("[a-zA-Z_][a-zA-Z0-9_]*");
	return std::regex_match(name, pattern);
}

std::string ini_parser::get_names_sections() noexcept {

	if (!content_file.size()) {
		return "There are no sections in this file\n";
	}
	else {
		std::string result = "";

		std::set<std::string> sections;

		for (const auto& pair : content_file) {
			auto key = pair.first;
			sections.insert(key.first);
		}

		for (const auto& section : sections) {
			result += section + "\n";
		}

		return result;
	}
}

std::string ini_parser::get_names_vars(std::string name_section) noexcept {

	std::string result = "";
	size_t number_of_vars{};

	for (const auto& pair : content_file) {

		auto key = pair.first;

		if (key.first == name_section) {
			if (key.second != "") { number_of_vars++; }
			result += key.second + "\n";
		}
	}

	if (!number_of_vars) result = "There are no variables in this section\n";

	return result;
}