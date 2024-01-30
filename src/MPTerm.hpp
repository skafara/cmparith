#pragma once

#include <concepts>
#include <istream>
#include <ostream>
#include <functional>
#include <deque>
#include <regex>

#include "mparith.hpp"


/**
 * Terminal Number Type Constraints
 * @tparam T Type
 */
template<typename T>
concept MPTerm_Number = requires(T t, std::string str, std::ostream &ostream) {
	{ T{str} } -> std::same_as<T>; // constructible from string
	{ T{t} } -> std::same_as<T>; // copy constructible
	{ t = t } -> std::same_as<T &>; // copy assignable
	{ T{std::move(t)} } -> std::same_as<T>; // move constructible
	{ t = std::move(t) } -> std::same_as<T &>; // move assignable

	{ t + t } -> std::same_as<T>;
	{ t - t } -> std::same_as<T>;
	{ t * t } -> std::same_as<T>;
	{ t / t } -> std::same_as<T>;
	{ t % t } -> std::same_as<T>;
	{ t.Factorial() } -> std::same_as<T>;

	{ ostream << t } -> std::same_as<std::ostream &>; // serializable to output stream
};

/**
 * Terminal
 * @tparam Number Number type to be worked with
 */
template<MPTerm_Number Number>
class MPTerm {
public:
	/**
	 * Runs a terminal
	 * @param istream Input commands stream
	 * @param ostream Output stream
	 */
	static void Run(std::istream &istream, std::ostream &ostream) noexcept;

private:
	/**
	 * Terminal Operation Type
	 */
	using t_MPTermOp = std::function<void (MPTerm &)>;

	/** Whitespaces */
	inline static const std::string kWhitespaces{" \t\n\r"};
	/**
	 * Bank Size
	 */
	static constexpr size_t kBank_Size = 5;

private:
	/**
	 * Transparently constructs
	 * @param ostream Output stream
	 */
	explicit MPTerm(std::ostream &ostream) noexcept;

	/**
	 * Based on provided symbol either returns
	 * - number represented by the provided number or
	 * - number from bank if it is a placeholder
	 * @param sym Symbol (number or $#)
	 * @return Number
	 */
	Number Get_Number(const std::string &sym) const;
	/**
	 * Saves a number to the bank
	 * @param number Number
	 */
	void Save_Result(const Number &number) noexcept;
	/**
	 * Prints latest result stored in the bank
	 */
	void Print_Latest_Result() const;
	/**
	 * Prints prompt symbol
	 */
	void Prompt() const noexcept;

	/**
	 * Strips leading and trailing whitespaces
	 * @param str String
	 * @return Stripped string
	 */
	static std::string Strip_Whitespaces(const std::string &str);
	/**
	 * Parses a terminal command and returns a Terminal Operation
	 * @param cmd Command
	 * @return Operation
	 */
	static t_MPTermOp Parse_Cmd(const std::string &cmd);

private:
	std::ostream &_ostream;
	std::deque<Number> _bank;

};

template<MPTerm_Number Number>
Number MPTerm<Number>::Get_Number(const std::string &sym) const {
	if (sym.starts_with('$')) {
		const size_t num = sym[1] - '0';
		if (_bank.size() < num) {
			throw std::out_of_range{"Out Of Bank Range"};
		}

		return _bank[num - 1];
	}
	else {
		return sym;
	}
}

template<MPTerm_Number Number>
void MPTerm<Number>::Save_Result(const Number &number) noexcept {
	if (_bank.size() >= kBank_Size) {
		_bank.pop_back();
	}
	_bank.push_front(number);
}

template<MPTerm_Number Number>
void MPTerm<Number>::Print_Latest_Result() const {
	_ostream << "$1 = " << _bank.front() << std::endl;
}

template<MPTerm_Number Number>
void MPTerm<Number>::Run(std::istream &istream, std::ostream &ostream) noexcept {
	MPTerm<Number> mpterm{ostream};
	mpterm.Prompt();
	for (std::string line; std::getline(istream, line);) {
		const std::string cmd = Strip_Whitespaces(line);

		if (line.empty()) {
			mpterm.Prompt();
			continue;
		}

		if (line == "exit") {
			break;
		}

		try {
			const t_MPTermOp op = Parse_Cmd(cmd);
			op(mpterm);
		}
		catch (const std::exception &e) {
			ostream << "[ERROR] " << e.what() << std::endl;
		}

		mpterm.Prompt();
	}
}

template<MPTerm_Number Number>
MPTerm<Number>::MPTerm(std::ostream &ostream) noexcept : _ostream(ostream) {
	//
}

template<MPTerm_Number Number>
void MPTerm<Number>::Prompt() const noexcept {
	_ostream << "> ";
}

template<MPTerm_Number Number>
std::string MPTerm<Number>::Strip_Whitespaces(const std::string &str) {
	const size_t begin = str.find_first_not_of(kWhitespaces);
	if (begin == std::string::npos) {
		return "";
	}
	const size_t end = str.find_last_not_of(kWhitespaces);
	return str.substr(begin, end - begin + 1);
}

template<MPTerm_Number Number>
MPTerm<Number>::t_MPTermOp MPTerm<Number>::Parse_Cmd(const std::string &cmd) {
	if (cmd == "bank") {
		return [] (MPTerm &mpterm) {
			for (size_t i = 0; i < mpterm._bank.size(); ++i) {
				mpterm._ostream << "$" << (i + 1) << " = " << mpterm._bank[i] << std::endl;
			}
		};
	}
	else if (cmd.find('!') != std::string::npos) {
		const std::regex factorial_pattern(R"((\$[1-5])!|(0|-?[1-9]\d*)!)");
		std::smatch match;
		if (!std::regex_match(cmd, match, factorial_pattern)) {
			throw std::invalid_argument{"Invalid Command Format"};
		}

		const std::string num_sym{match[1].matched ? match[1] : match[2]};
		return [num_sym] (MPTerm &mpterm) {
			const Number result = mpterm.Get_Number(num_sym).Factorial();
			mpterm.Save_Result(result);
			mpterm.Print_Latest_Result();
		};
	}
	else {
		const std::regex binary_op_pattern(R"((((\$[1-5])|(0|-?[1-9]\d*)))\s*([\+\-\*\/%])\s*(((\$[1-5])|(0|-?[1-9]\d*))))");
		std::smatch match;
		if (!std::regex_match(cmd, match, binary_op_pattern)) {
			throw std::invalid_argument{"Invalid Command Format"};
		}

		const std::string num1_sym = match[1];
		const std::string num2_sym = match[6];

		using t_Num_Op = std::function<Number (const Number &, const Number &)>;

		const std::string op_sym = match[5];
		t_Num_Op op;
		if (op_sym == "+") {
			op = [] (const auto &num1, const auto &num2) {
				return num1 + num2;
			};
		}
		else if (op_sym == "-") {
			op = [] (const auto &num1, const auto &num2) {
				return num1 - num2;
			};
		}
		else if (op_sym == "*") {
			op = [] (const auto &num1, const auto &num2) {
				return num1 * num2;
			};
		}
		else if (op_sym == "/") {
			op = [] (const auto &num1, const auto &num2) {
				return num1 / num2;
			};
		}
		else if (op_sym == "%") {
			op = [] (const auto &num1, const auto &num2) {
				return num1 % num2;
			};
		}

		return [num1_sym, op, num2_sym] (MPTerm &mpterm) {
			const Number result = op(mpterm.Get_Number(num1_sym), mpterm.Get_Number(num2_sym));
			mpterm.Save_Result(result);
			mpterm.Print_Latest_Result();
		};
	}
}
