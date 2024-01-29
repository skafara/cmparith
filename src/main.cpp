#include <iostream>
#include <regex>

#include "MPTerm.hpp"


void Print_Help() {
	std::cout << "Usage:" << std::endl;
	std::cout << "\tmpcalc <mode>" << std::endl << std::endl;
	std::cout << "\t<mode>\t- 1: Terminal [Unlimited Precision]" << std::endl;
	std::cout << "\t\t- 2: Terminal [32B Precision]" << std::endl;
	std::cout << "\t\t- 3: mparith Library Showcase" << std::endl;
}

void Print_Error(const std::string &text) {
	std::cerr << "[ERROR] " << text << std::endl;
}

constexpr size_t kExpected_Args_Cnt = 1;

int main(int argc, char **argv) {
	if (argc != kExpected_Args_Cnt + 1) {
		Print_Error("Invalid Arguments Count");
		Print_Help();
		return EXIT_FAILURE;
	}

	const std::string mode_parameter{argv[1]};
	const std::regex mode_pattern("(1|2|3)");
	std::smatch mode_match;
	if (!std::regex_match(mode_parameter, mode_match, mode_pattern)) {
		Print_Error("Invalid Mode Parameter");
		Print_Help();
		return EXIT_FAILURE;
	}
	else {
		if (mode_match[0] == "1") {
			MPTerm<mparith::kUnlimited>::Run(std::cin, std::cout);
		}
		else if (mode_match[0] == "2") {
			MPTerm<32>::Run(std::cin, std::cout);
		}
		else {
			//
		}
	}

	return EXIT_SUCCESS;
}
