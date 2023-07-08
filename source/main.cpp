#include <iostream>
#include <fstream>

#include <string>

#include <exception>

#include "./ini_parser/ini_parser.h"

int main(int argv, char** argc) {

	try {
		ini_parser parser("../source/ini_files/file1.ini");

		auto value = parser.get_value<std::string>("Section2.var1");

		std::cout << value << std::endl;
	}
	catch (const std::ifstream::failure& ex) {
		std::cout << ex.what() << std::endl;
	}
	catch (const std::invalid_argument& ex) {
		std::cout << ex.what() << std::endl;
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}

	return 0;
}