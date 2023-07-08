#pragma once
#include <string>
#include <map>

#include <sstream>
#include <fstream>

#include <exception>
#include <algorithm>

class ini_parser {
public:
	ini_parser(std::string filename);
	~ini_parser();

	template<typename T>
	T get_value(std::string request) {

		T result{};

		auto separator = request.find(".");
		std::string name_section_request = request.substr(0, separator);
		std::string name_var_request = request.substr(separator + 1, request.length());

		auto section_exist = std::find_if(content_file.begin(),
			content_file.end(),
			[name_section_request](const auto& pair) {
				auto key = pair.first;
				return key.first == name_section_request;
			});

		auto variable_exist = std::find_if(content_file.begin(),
			content_file.end(),
			[name_section_request, name_var_request](const auto& pair) {
				auto key = pair.first;
				return (key.first == name_section_request) &&
					(key.second == name_var_request);
			});

		if (section_exist == content_file.end()) {

			std::string list_sections = get_names_sections();

			throw std::invalid_argument("Section is not found.\nPerhaps you meant:\n" + list_sections);
		}
		else if (variable_exist == content_file.end()) {

			std::string list_vars = get_names_vars(name_section_request);

			throw std::invalid_argument("Variable " + name_var_request + " is not found in the " +
				name_section_request + ".\nPerhaps you meant:\n" + list_vars);
		}

		auto key_request = std::move(std::make_pair(name_section_request, name_var_request));

		std::string value_request = content_file[key_request];

		if constexpr (std::is_same_v<T, std::string>) {
			result = value_request;
		}
		else {
			std::istringstream iss(value_request);
			if (!(iss >> result)) {
				throw std::invalid_argument("Error converting a string to a number");
			}
		}

		return result;
	}

private:
	void process_file();

	bool check_correct_name(std::string name) noexcept;

	std::string get_names_sections() noexcept;
	std::string get_names_vars(std::string name_section) noexcept;

	std::ifstream fin;
	std::map<std::pair<std::string, std::string>, std::string> content_file{};
	size_t number_of_rows{};
};