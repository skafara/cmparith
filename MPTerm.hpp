#pragma once

#include <concepts>
#include <istream>
#include <ostream>
#include <functional>
#include <deque>
#include <regex>

#include "mparith.hpp"


template<mparith::t_Width Precision>
class MPTerm {
public:
	static void Run(std::istream &istream, std::ostream &ostream) noexcept;

private:
	using t_MPTermOp = std::function<void (MPTerm &)>;

	static const std::string kWhitespaces;
	static constexpr size_t kBank_Size = 5;

private:
	explicit MPTerm(std::ostream &ostream) noexcept;

	mparith::Integer<Precision> Get_Number(const std::string &sym) const;
	void Save_Result(const mparith::Integer<Precision> &integer) noexcept;
	void Print_Latest_Result() const;
	void Prompt() const noexcept;

	static std::string Strip_Whitespaces(const std::string &str);
	static t_MPTermOp Parse_Cmd(const std::string &cmd);

private:
	std::ostream &_ostream;

	std::deque<mparith::Integer<Precision>> _bank;

};

template<mparith::t_Width Precision>
const std::string MPTerm<Precision>::kWhitespaces{" \t\n\r"};

template<mparith::t_Width Precision>
mparith::Integer<Precision> MPTerm<Precision>::Get_Number(const std::string &sym) const {
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

template<mparith::t_Width Precision>
void MPTerm<Precision>::Save_Result(const mparith::Integer<Precision> &integer) noexcept {
	if (_bank.size() >= kBank_Size) {
		_bank.pop_back();
	}
	_bank.push_front(integer);
}

template<mparith::t_Width Precision>
void MPTerm<Precision>::Print_Latest_Result() const {
	_ostream << "$1 = " << _bank.front() << std::endl;
}

template<mparith::t_Width Precision>
void MPTerm<Precision>::Run(std::istream &istream, std::ostream &ostream) noexcept {
	MPTerm<Precision> mpterm{ostream};
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
			mpterm.Prompt();
		}
		catch (const std::exception &e) {
			ostream << "[ERROR] " << e.what() << std::endl;
			mpterm.Prompt();
		}
	}
}

template<mparith::t_Width Precision>
MPTerm<Precision>::MPTerm(std::ostream &ostream) noexcept : _ostream(ostream) {
	//
}

template<mparith::t_Width Precision>
void MPTerm<Precision>::Prompt() const noexcept {
	_ostream << "> ";
}

template<mparith::t_Width Precision>
std::string MPTerm<Precision>::Strip_Whitespaces(const std::string &str) {
	const size_t begin = str.find_first_not_of(kWhitespaces);
	if (begin == std::string::npos) {
		return "";
	}
	const size_t end = str.find_last_not_of(kWhitespaces);
	return str.substr(begin, end - begin + 1);
}

template<mparith::t_Width Precision>
MPTerm<Precision>::t_MPTermOp MPTerm<Precision>::Parse_Cmd(const std::string &cmd) {
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
			const mparith::Integer result = mpterm.Get_Number(num_sym).Factorial();
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

		using t_Num_Op = std::function<mparith::Integer<Precision> (const mparith::Integer<Precision> &, const mparith::Integer<Precision> &)>;

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
			const mparith::Integer result = op(mpterm.Get_Number(num1_sym), mpterm.Get_Number(num2_sym));
			mpterm.Save_Result(result);
			mpterm.Print_Latest_Result();
		};
	}
}
