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

void Showcase_Limited() {
	std::cout << "LIMITED PRECISION [4B]" << std::endl << std::endl;

	const std::string str1{"-123456"};
	const std::string str2{"678"};
	const mparith::Integer<4> int1{str1};
	const mparith::Integer<4> int2{str2};

	std::cout << "str1: " << str1 << ", str2: " << str2 << std::endl;
	std::cout << "int1: " << int1 << ", int2: " << int2 << std::endl;
	std::cout << int1 << " + " << int2 << " = " << (int1 + int2) << std::endl;
	std::cout << int1 << " - " << int2 << " = " << (int1 - int2) << std::endl;
	std::cout << int1 << " * " << int2 << " = " << (int1 * int2) << std::endl;
	std::cout << int1 << " / " << int2 << " = " << (int1 / int2) << std::endl;
	std::cout << int1 << " % " << int2 << " = " << (int1 % int2) << std::endl << std::endl;

	std::cout << "int3" << " = " << "int2" << std::endl;
	const mparith::Integer<4> int3 = int2;
	std::cout << "int3: " << int3 << ", int2: " << int2 << std::endl << std::endl;

	const std::string str4{"12"};
	const mparith::Integer<4> int4{str4};
	std::cout << "str4: " << str4 << std::endl;
	std::cout << "int4: " << int4 << std::endl;
	std::cout << int4 << "!" << " = " << (int4.Factorial()) << std::endl << std::endl;
	std::cout << "-" << int4 << " = " << (-int4) << std::endl;
}

void Showcase_Overflow() {
	std::cout << "OVERFLOW DETECTION [4B]" << std::endl << std::endl;

	const std::string str1{"2147483648"};
	std::cout << "str1: " << str1 << std::endl;
	std::cout << "int1: ";
	try {
		const mparith::Integer<4> int1{str1};
	}
	catch (const mparith::OverflowException<4> &e) {
		std::cout << e.what() << std::endl << std::endl;
	}

	const std::string str2{"2147483647"};
	mparith::Integer<4> int2{str2};
	std::cout << "str2: " << str2 << " (2^32 - 1)" << std::endl;
	std::cout << "int2: " << int2 << std::endl;
	std::cout << "++" << int2 << " = ";
	try {
		++int2;
	}
	catch (const mparith::OverflowException<4> &e) {
		std::cout << e.what() << std::endl;
	}
}

void Showcase_Unlimited() {
	std::cout << "UNLIMITED PRECISION" << std::endl << std::endl;

	const std::string str1{"-123456789123456789123456789123456789"};
	const std::string str2{"987654321987654321987654321"};
	const mparith::Integer<mparith::kUnlimited> int1{str1};
	const mparith::Integer<mparith::kUnlimited> int2{str2};

	std::cout << "str1: " << str1 << ", str2: " << str2 << std::endl;
	std::cout << "int1: " << int1 << ", int2: " << int2 << std::endl;
	std::cout << int1 << " + " << int2 << " = " << (int1 + int2) << std::endl;
	std::cout << int1 << " - " << int2 << " = " << (int1 - int2) << std::endl;
	std::cout << int1 << " * " << int2 << " = " << (int1 * int2) << std::endl;
	std::cout << int1 << " / " << int2 << " = " << (int1 / int2) << std::endl;
	std::cout << int1 << " % " << int2 << " = " << (int1 % int2) << std::endl << std::endl;

	const std::string str3{"123"};
	const mparith::Integer<mparith::kUnlimited> int3{str3};
	std::cout << "str3: " << str3 << std::endl;
	std::cout << "int3: " << int3 << std::endl;
	std::cout << int3 << "!" << " = " << (int3.Factorial()) << std::endl << std::endl;

	std::cout << "-" << int3 << " = " << (-int3) << std::endl;
}

void Showcase_Widths() {
	std::cout << "WIDTHS" << std::endl << std::endl;

	const std::string str1{"-1234567890"};
	const mparith::Integer<4> int1_4{str1};

	std::cout << "str1: " << str1 << std::endl;
	std::cout << "int1[4B]: " << int1_4 << std::endl;

	std::cout << int1_4 << " * " << int1_4 << " = ";
	try {
		int1_4 * int1_4;
	}
	catch (const mparith::OverflowException<4> &e) {
		std::cout << e.what() << std::endl << std::endl;
	}

	std::cout << "int2[Unlimited]" << " = " << "int1" << std::endl;
	const mparith::Integer<mparith::kUnlimited> int2_u = int1_4;

	std::cout << "int2[Unlimited]: " << int2_u << std::endl << std::endl;
	std::cout << int2_u << " * " << int2_u << " = " << (int2_u * int2_u) << std::endl;
}

void Showcase() {
	Showcase_Limited();
	std::cout << std::endl;
	Showcase_Overflow();
	std::cout << std::endl;
	Showcase_Unlimited();
	std::cout << std::endl;
	Showcase_Widths();
}

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
			MPTerm<mparith::Integer<mparith::kUnlimited>>::Run(std::cin, std::cout);
		}
		else if (mode_match[0] == "2") {
			MPTerm<mparith::Integer<32>>::Run(std::cin, std::cout);
		}
		else {
			Showcase();
		}
	}

	return EXIT_SUCCESS;
}
