#pragma once

#include <array>
#include <vector>
#include <algorithm>
#include <stack>
#include <sstream>
#include <stdexcept>
#include <ostream>


/**
 * Multi-precision Arithmetic
 */
namespace mparith {


	///// BEGIN DECLARATION Types

	/**
	 * Number Width Type
	 */
	using t_Width = size_t;

	class ArithmeticException : public std::runtime_error {
	public:
		explicit ArithmeticException(const std::string &msg) noexcept : std::runtime_error(msg) {
			//
		}

	};

	template<t_Width Width>
	class Integer;

	template<t_Width Result_Width>
	class OverflowException : public ArithmeticException {
	public:
		explicit OverflowException(const Integer<Result_Width> &result) : ArithmeticException("Overflow Detected [" + result.Serialize() + "]"), _result(result) {
			//
		}

		const Integer<Result_Width> &Get_Result() const noexcept {
			return _result;
		};

	private:
		const Integer<Result_Width> _result;

	};

	///// END DECLARATION Types


	///// BEGIN DECLARATION Constants

	/**
	 * Unlimited Number Width Specifier
	 */
	constexpr t_Width kUnlimited = std::numeric_limits<size_t>::max();

	/**
	 * Minimal Number Width
	 */
	constexpr t_Width kWidth_Min = 4;

	/**
	 * Bits Count Of The Word Type Used By Numbers
	 */
	constexpr size_t kWord_Bits_Cnt = 8;

	///// END DECLARATION Constants


	///// BEGIN DECLARATION Util Functions

	/**
	 * Returns the max of the two parameters
	 * @param lhs lhs
	 * @param rhs rhs
	 * @return Max
	 */
	constexpr t_Width Get_Max(t_Width lhs, t_Width rhs) noexcept;

	/**
	 * Return whether a width is unlimited
	 * @param width Width
	 * @return Bool
	 */
	consteval bool Is_Unlimited(t_Width width) noexcept;

	///// END DECLARATION Util Functions


	///// BEGIN DECLARATION Integer

	/**
	 * Multi-precision Signed Integer Type
	 * @tparam Width Integer Width
	 */
	template<t_Width Width>
	class Integer {

		/**
		 * Integers of different widths are still friends
		 * @tparam Width_ Other
		 */
		template<t_Width Width_> friend class Integer;


	////// BEGIN DECLARATION Integer Constants

	public:
		static constexpr size_t kBits_Cnt = Width * kWord_Bits_Cnt;

	////// END DECLARATION Integer Constants


	////// BEGIN DECLARATION Integer Constructors, Copy, Move, Destructors

	public:
		Integer() noexcept;
		Integer(const std::string &str);

	private:
		template<size_t Bits_Cnt>
		Integer(const std::bitset<Bits_Cnt> &bitset) noexcept;

	public:
		template<t_Width Rhs_Width>
		Integer(const Integer<Rhs_Width> &rhs) noexcept;
		template<t_Width Rhs_Width>
		Integer &operator=(const Integer<Rhs_Width> &rhs) noexcept;

		template<t_Width Rhs_Width>
		Integer(Integer<Rhs_Width> &&rhs) noexcept;
		template<t_Width Rhs_Width>
		Integer &operator=(Integer<Rhs_Width> &&rhs) noexcept;

		~Integer() noexcept = default;

	////// END DECLARATION Integer Constructors, Copy, Move, Destructors


	////// BEGIN DECLARATION Integer Operations

	private:
		template<t_Width Width_>
		static Integer<Width_> Get_Normalized_New(t_Width actual_width) noexcept;

		template<t_Width Width_, t_Width New_Width>
		static Integer<New_Width> Get_Normalized_Existing(const Integer<Width_> &integer, t_Width actual_width) noexcept;

		template<t_Width Lhs_Width, t_Width Rhs_Width>
		static std::pair<Integer<Get_Max(Lhs_Width, Rhs_Width)>, Integer<Get_Max(Lhs_Width, Rhs_Width)>>
		Get_Normalized_Operands(const Integer<Lhs_Width> &lhs, const Integer<Lhs_Width> &rhs) noexcept;

		/*template<t_Width Lhs_Width, t_Width Rhs_Width>
		static std::pair<Integer<Get_Max(Lhs_Width, Rhs_Width)>, Integer<Get_Max(Lhs_Width, Rhs_Width)>>
		Get_Normalized_Positive_Operands(const Integer<Lhs_Width> &lhs, const Integer<Lhs_Width> &rhs) noexcept;*/

		template<t_Width Lhs_Width, t_Width Rhs_Width>
		static Integer<Get_Max(Lhs_Width, Rhs_Width)>
		Add(const Integer<Lhs_Width> &lhs, const Integer<Rhs_Width> &rhs, bool &overflow_flag, bool &carry_flag);

		template<t_Width Lhs_Width, t_Width Rhs_Width>
		static Integer<Get_Max(Lhs_Width, Rhs_Width)>
		Mul(const Integer<Lhs_Width> &lhs, const Integer<Rhs_Width> &rhs, bool &overflow_flag);

		template<t_Width Lhs_Width, t_Width Rhs_Width>
		static std::pair<Integer<Get_Max(Lhs_Width, Rhs_Width)>, Integer<Get_Max(Lhs_Width, Rhs_Width)>>
		Div_Mod(const Integer<Lhs_Width> &lhs, const Integer<Rhs_Width> &rhs);

	////// END DECLARATION Integer Operations


	////// BEGIN DECLARATION Integer Operators

	private:
		bool operator[](std::size_t idx) const noexcept;
		std::bitset<kWord_Bits_Cnt>::reference operator[](std::size_t idx) noexcept;

		Integer operator<<(size_t shift) const noexcept;
		void operator<<=(size_t shift) noexcept;
		Integer operator>>(size_t shift) const noexcept;
		void operator>>=(size_t shift) noexcept;

	public:
		template<t_Width Rhs_Width>
		bool operator==(const Integer<Rhs_Width> &rhs) const noexcept;
		template<t_Width Rhs_Width>
		bool operator!=(const Integer<Rhs_Width> &rhs) const noexcept;

		Integer operator-() const noexcept;

		template<t_Width Rhs_Width>
		Integer<Get_Max(Width, Rhs_Width)> operator+(const Integer<Rhs_Width> &rhs) const;
		template<t_Width Rhs_Width>
		void operator+=(const Integer<Rhs_Width> &rhs);

		Integer &operator++();
		Integer &operator--();

		template<t_Width Rhs_Width>
		Integer<Get_Max(Width, Rhs_Width)> operator-(const Integer<Rhs_Width> &rhs) const;
		template<t_Width Rhs_Width>
		void operator-=(const Integer<Rhs_Width> &rhs);

		template<t_Width Rhs_Width>
		Integer<Get_Max(Width, Rhs_Width)> operator*(const Integer<Rhs_Width> &rhs) const;
		template<t_Width Rhs_Width>
		void operator*=(const Integer<Rhs_Width> &rhs);

		template<t_Width Rhs_Width>
		Integer<Get_Max(Width, Rhs_Width)> operator/(const Integer<Rhs_Width> &rhs) const;
		template<t_Width Rhs_Width>
		void operator/=(const Integer<Rhs_Width> &rhs);

		template<t_Width Rhs_Width>
		Integer<Get_Max(Width, Rhs_Width)> operator%(const Integer<Rhs_Width> &rhs) const;
		template<t_Width Rhs_Width>
		void operator%=(const Integer<Rhs_Width> &rhs);

		Integer Factorial() const;

	////// END DECLARATION Integer Operators


	////// BEGIN DECLARATION Integer Serialization

	public:
		std::string Serialize() const noexcept;
		static Integer Deserialize(const std::string &str);

		friend std::ostream &operator<<(std::ostream &ostream, const Integer &integer) {
			ostream << integer.Serialize();
			return ostream;
		};

	////// END DECLARATION Integer Serialization


	////// BEGIN DECLARATION Integer Number Constants

	private:
		static const Integer<Width> kZero;
		static const Integer<Width> kOne;
		static const Integer<Width> kTwo;
		static const Integer<Width> kTen;

	////// END DECLARATION Integer Number Constants


	////// BEGIN DECLARATION Integer Util Functions

	private:
		static consteval bool Is_Limited() noexcept;
		static consteval bool Is_Unlimited() noexcept;

		template<t_Width New_Width>
		Integer<New_Width> Get_Normalized(size_t actual_width) const noexcept;

		constexpr t_Width Get_Actual_Width() const noexcept;
		constexpr size_t Get_Actual_Bits_Cnt() const noexcept;

		Integer Get_Inverse() const noexcept;
		Integer Get_Complement() const noexcept;

		size_t Get_Msb_Idx() const noexcept;
		std::bitset<kBits_Cnt> Get_Bitset() const noexcept;

		bool Is_Positive() const noexcept;
		Integer Get_Positive() const noexcept;

	////// END DECLARATION Integer Util Functions

	////// BEGIN DECLARATION Integer Asserts

	private:
		template<t_Width Rhs_Width>
		static consteval void Assert_Valid_Assignment_Widths() noexcept {
			static_assert(Width >= Rhs_Width, "lhs width must be >= rhs width");
		};

		static consteval void Assert_Minimal_Width() noexcept {
			static_assert(Width >= kWidth_Min, "Width must be >= minimal");
		}

		template<t_Width Width_>
		static consteval void Assert_Greater_Equal_Width() noexcept {
			static_assert(Width_ >= Width, "Other width must be >= width");
		}

		template<size_t Bits_Cnt>
		static consteval void Assert_Valid_Bits_Cnt() noexcept {
			static_assert(Bits_Cnt >= kWord_Bits_Cnt, "Bits count must be >= word");
			static_assert(Bits_Cnt % kWord_Bits_Cnt == 0, "Width must be divisible by word");
		}

	////// END DECLARATION Integer Asserts

	private:
		std::vector<std::bitset<kWord_Bits_Cnt>> _bits;

	};

	///// END DECLARATION Integer




	///// BEGIN DEFINITION Util Functions

	constexpr t_Width Get_Max(const t_Width lhs, const t_Width rhs) noexcept {
		return std::max(lhs, rhs);
	}

	consteval bool Is_Unlimited(const t_Width width) noexcept {
		return width == kUnlimited;
	}

	///// END DEFINITION Util Functions




	///// BEGIN DEFINITION Integer

	////// BEGIN DEFINITION Integer Constructors, Copy, Move, Destructors

	template<t_Width Width>
	Integer<Width>::Integer() noexcept {
		Assert_Minimal_Width();

		if constexpr (Is_Limited()) {
			_bits.resize(Width);
		}
		else {
			_bits.resize(kWidth_Min);
		}
	}

	template<t_Width Width>
	Integer<Width>::Integer(const std::string &str) {
		(*this) = Deserialize(str);
	}

	template<t_Width Width>
	template<size_t Bits_Cnt>
	Integer<Width>::Integer(const std::bitset<Bits_Cnt> &bitset) noexcept : Integer() {
		Assert_Valid_Bits_Cnt<Bits_Cnt>();

		for (size_t i = 0, idx = 0; i < Bits_Cnt / kWord_Bits_Cnt; ++i) {
			for (size_t j = 0; j < kWord_Bits_Cnt; ++j, ++idx) {
				if (bitset[idx]) {
					_bits[i].set(j);
				}
			}
		}
	}

	template<t_Width Width>
	template<t_Width Rhs_Width>
	Integer<Width>::Integer(const Integer<Rhs_Width> &rhs) noexcept : _bits(rhs._bits) {
		Assert_Valid_Assignment_Widths<Rhs_Width>();
	}

	template<t_Width Width>
	template<t_Width Rhs_Width>
	Integer<Width> &Integer<Width>::operator=(const Integer<Rhs_Width> &rhs) noexcept {
		Assert_Valid_Assignment_Widths<Rhs_Width>();

		_bits = rhs._bits;

		return *this;
	}

	template<t_Width Width>
	template<t_Width Rhs_Width>
	Integer<Width>::Integer(Integer<Rhs_Width> &&rhs) noexcept : _bits(std::move(rhs._bits)) {
		Assert_Valid_Assignment_Widths<Rhs_Width>();
	}

	template<t_Width Width>
	template<t_Width Rhs_Width>
	Integer<Width> &Integer<Width>::operator=(Integer<Rhs_Width> &&rhs) noexcept {
		Assert_Valid_Assignment_Widths<Rhs_Width>();

		_bits = std::move(rhs._bits);

		return *this;
	}

	////// END DEFINITION Integer Constructors, Copy, Move, Destructors


	////// BEGIN DEFINITION Integer Operations

	template<t_Width Width>
	template<t_Width Lhs_Width, t_Width Rhs_Width>
	std::pair<Integer<Get_Max(Lhs_Width, Rhs_Width)>, Integer<Get_Max(Lhs_Width, Rhs_Width)>>
	Integer<Width>::Get_Normalized_Operands(const Integer<Lhs_Width> &lhs, const Integer<Lhs_Width> &rhs) noexcept {
		constexpr t_Width Result_Width = Get_Max(Lhs_Width, Rhs_Width);
		const t_Width Result_Actual_Width = Get_Max(lhs.Get_Actual_Width(), rhs.Get_Actual_Width());

		return {
			lhs.template Get_Normalized<Result_Width>(Result_Actual_Width),
			rhs.template Get_Normalized<Result_Width>(Result_Actual_Width)
		};
	}

	/*template<t_Width Width>
	template<t_Width Lhs_Width, t_Width Rhs_Width>
	std::pair<Integer<Get_Max(Lhs_Width, Rhs_Width)>, Integer<Get_Max(Lhs_Width, Rhs_Width)>>
	Integer<Width>::Get_Normalized_Positive_Operands(const Integer<Lhs_Width> &lhs, const Integer<Lhs_Width> &rhs) noexcept {
		const auto [lhs_, rhs_] = Get_Normalized_Operands<Lhs_Width, Rhs_Width>(lhs, rhs);
		return {lhs_.Get_Positive(), rhs_.Get_Positive()};
	}*/

	template<t_Width Width>
	template<t_Width Lhs_Width, t_Width Rhs_Width>
	Integer<Get_Max(Lhs_Width, Rhs_Width)>
	Integer<Width>::Add(const Integer<Lhs_Width> &lhs, const Integer<Rhs_Width> &rhs, bool &overflow_flag, bool &carry_flag) {
		constexpr t_Width Result_Width = Get_Max(Lhs_Width, Rhs_Width);
		const t_Width Result_Actual_Width = Get_Max(lhs.Get_Actual_Width(), rhs.Get_Actual_Width());

		//const auto [left, right] = Get_Normalized_Operands<Lhs_Width, Rhs_Width>(lhs, rhs);
		const Integer<Result_Width> left = Get_Normalized_Existing<Lhs_Width, Result_Width>(lhs, Result_Actual_Width);
		const Integer<Result_Width> right = Get_Normalized_Existing<Rhs_Width, Result_Width>(rhs, Result_Actual_Width);

		Integer<Result_Width> result;
		if constexpr (result.Is_Unlimited()) {
			result = Get_Normalized_Existing<Result_Width, Result_Width>(result, Result_Actual_Width);
		}

		bool carry = false;
		for (size_t i = 0; i < left.Get_Actual_Bits_Cnt(); ++i) {
			result[i] = left[i] ^ right[i] ^ carry;
			carry = (left[i] & right[i]) | ((left[i] ^ right[i]) & carry);
		}

		if constexpr (result.Is_Unlimited()) {
			if (left.Is_Positive() && right.Is_Positive() && !result.Is_Positive()) {
				result._bits.emplace_back();
			}
			else if (!left.Is_Positive() && !right.Is_Positive() && result.Is_Positive()) {
				std::bitset<kWord_Bits_Cnt> bitset;
				bitset.set();
				result._bits.push_back(bitset);
			}
		}

		if constexpr (result.Is_Limited()) {
			if ((left.Is_Positive() && right.Is_Positive() && !result.Is_Positive()) ||
				(!left.Is_Positive() && !right.Is_Positive() && result.Is_Positive())) {
				overflow_flag = true;
			}
		}

		if (carry) {
			carry_flag = true;
		}

		return result;
	}

	template<t_Width Width>
	template<t_Width Lhs_Width, t_Width Rhs_Width>
	Integer<Get_Max(Lhs_Width, Rhs_Width)>
	Integer<Width>::Mul(const Integer<Lhs_Width> &lhs, const Integer<Rhs_Width> &rhs, bool &overflow_flag) {
		constexpr t_Width Result_Width = Get_Max(Width, Rhs_Width);
		const t_Width Result_Actual_Width = Get_Max(lhs.Get_Actual_Width(), rhs.Get_Actual_Width());
		const bool result_is_positive = !(lhs.Is_Positive() ^ rhs.Is_Positive());

		//auto [left, right] = Get_Normalized_Positive_Operands<Lhs_Width, Rhs_Width>(lhs, rhs);
		Integer<Result_Width> left = Get_Normalized_Existing<Lhs_Width, Result_Width>(lhs.Get_Positive(), Result_Actual_Width);
		Integer<Result_Width> right = Get_Normalized_Existing<Rhs_Width, Result_Width>(rhs.Get_Positive(), Result_Actual_Width);

		Integer<Result_Width> result;
		if constexpr (result.Is_Unlimited()) {
			const t_Width Extended_Result_Actual_Width = 2 * Get_Max(lhs.Get_Actual_Width(), rhs.Get_Actual_Width());
			left = Get_Normalized_Existing<Result_Width, Result_Width>(left, Extended_Result_Actual_Width);
			right = Get_Normalized_Existing<Result_Width, Result_Width>(right, Extended_Result_Actual_Width);
		}

		bool _, carry_flag;
		const size_t right_msb = right.Get_Msb_Idx();
		for (size_t i = 0; i <= right_msb; ++i) {
			if (right[0]) {
				result = Add(result, left, _, carry_flag);
			}

			left <<= 1;
			right >>= 1;
		}

		if constexpr (result.Is_Limited()) {
			if (carry_flag || !result.Is_Positive()) {
				overflow_flag = true;
			}
		}

		if constexpr (result.Is_Unlimited()) {
			const auto Shrink = [] <size_t Width_> (Integer<Width_> &integer) {
				const size_t msb_idx = integer.Get_Msb_Idx();
				const size_t size = Get_Max(((msb_idx + 1) / kWord_Bits_Cnt) + 1, kWidth_Min);
				integer._bits.resize(size);
			};

			Shrink(result);
		}

		if (result_is_positive) {
			return result;
		}
		else {
			return result.Get_Complement();
		}
	}

	template<t_Width Width>
	template<t_Width Lhs_Width, t_Width Rhs_Width>
	std::pair<Integer<Get_Max(Lhs_Width, Rhs_Width)>, Integer<Get_Max(Lhs_Width, Rhs_Width)>>
	Integer<Width>::Div_Mod(const Integer<Lhs_Width> &lhs, const Integer<Rhs_Width> &rhs) {
		if (rhs == kZero) {
			throw ArithmeticException{"Division By Zero"};
		}

		if (lhs == kZero) {
			return {kZero, kZero};
		}

		constexpr t_Width Result_Width = Get_Max(Lhs_Width, Rhs_Width);
		const t_Width Result_Actual_Width = Get_Max(lhs.Get_Actual_Width(), rhs.Get_Actual_Width());

		const bool result_is_positive = !(lhs.Is_Positive() ^ rhs.Is_Positive());

		//const auto [numerator, denominator] = Get_Normalized_Positive_Operands<Lhs_Width, Rhs_Width>(lhs, rhs);
		const Integer<Result_Width> numerator = Get_Normalized_Existing<Lhs_Width, Result_Width>(lhs.Get_Positive(), Result_Actual_Width);
		const Integer<Result_Width> denominator = Get_Normalized_Existing<Rhs_Width, Result_Width>(rhs.Get_Positive(), Result_Actual_Width);

		Integer<Result_Width> quotient = Get_Normalized_New<Result_Width>(Result_Actual_Width);
		Integer<Result_Width> remainder = Get_Normalized_New<Result_Width>(Result_Actual_Width);

		const size_t numerator_msb = numerator.Get_Msb_Idx();
		for (size_t i = 0; i <= numerator_msb; ++i) {
			const size_t idx = numerator_msb - i;

			remainder <<= 1;
			remainder[0] = numerator[idx];

			const Integer<Result_Width> difference = remainder - denominator;
			if (difference.Is_Positive()) {
				remainder = difference;
				quotient[idx] = true;
			}
		}

		if (result_is_positive) {
			return {quotient, remainder};
		}
		else {
			return {quotient.Get_Complement(), remainder};
		}
	}

	////// END DEFINITION Integer Operations


	////// BEGIN DEFINITION Integer Operators

	template<t_Width Width>
	bool Integer<Width>::operator[](const size_t idx) const noexcept {
		return _bits[idx / kWord_Bits_Cnt][idx % kWord_Bits_Cnt];
	}

	template<t_Width Width>
	std::bitset<kWord_Bits_Cnt>::reference Integer<Width>::operator[](const size_t idx) noexcept {
		return _bits[idx / kWord_Bits_Cnt][idx % kWord_Bits_Cnt];
	}

	template<t_Width Width>
	Integer<Width> Integer<Width>::operator<<(const size_t shift) const noexcept {
		if (shift == 0) {
			return *this;
		}

		if constexpr (Is_Limited()) {
			return Get_Bitset() << shift;
		}
		else {
			Integer result{*this};
			for (size_t i = result.Get_Actual_Bits_Cnt() - 1; i >= shift; --i) {
				result[i] = result[i - shift];
			}
			for (size_t i = 0; i < shift; ++i) {
				result[i] = false;
			}
			return result;
		}
	}

	template<t_Width Width>
	void Integer<Width>::operator<<=(const size_t shift) noexcept {
		*this = ((*this) << shift);
	}

	template<t_Width Width>
	Integer<Width> Integer<Width>::operator>>(const size_t shift) const noexcept {
		if (shift == 0) {
			return *this;
		}

		if constexpr (Is_Limited()) {
			return Get_Bitset() >> shift;
		}
		else {
			Integer result{*this};
			for (size_t i = shift; i < result.Get_Actual_Bits_Cnt(); ++i) {
				result[i - shift] = result[i];
			}
			for (size_t i = result.Get_Actual_Bits_Cnt() - shift; i < result.Get_Actual_Bits_Cnt(); ++i) {
				result[i] = false;
			}
			return result;
		}
	}

	template<t_Width Width>
	void Integer<Width>::operator>>=(const size_t shift) noexcept {
		*this = ((*this) >> shift);
	}

	template<t_Width Width>
	template<t_Width Rhs_Width>
	bool Integer<Width>::operator==(const Integer<Rhs_Width> &rhs) const noexcept {
		if (Is_Positive() ^ rhs.Is_Positive()) {
			return false;
		}

		constexpr t_Width Result_Width = Get_Max(Width, Rhs_Width);
		const t_Width Result_Actual_Width = Get_Max(Get_Actual_Width(), rhs.Get_Actual_Width());

		//const auto [left, right] = Get_Normalized_Operands<Width, Rhs_Width>(*this, rhs);
		const Integer<Result_Width> left = Get_Normalized_Existing<Width, Result_Width>(*this, Result_Actual_Width);
		const Integer<Result_Width> right = Get_Normalized_Existing<Rhs_Width, Result_Width>(rhs, Result_Actual_Width);

		if constexpr (!mparith::Is_Unlimited(Result_Width)) {
			return left.Get_Bitset() == right.Get_Bitset();
		}
		else {
			for (size_t i = 0; i < left.Get_Actual_Bits_Cnt(); ++i) {
				if (left[i] != right[i]) {
					return false;
				}
			}
			return true;
		}
	}

	template<t_Width Width>
	template<t_Width Rhs_Width>
	bool Integer<Width>::operator!=(const Integer<Rhs_Width> &rhs) const noexcept {
		return !((*this) == rhs);
	}

	template<t_Width Width>
	Integer<Width> Integer<Width>::operator-() const noexcept {
		return Get_Complement();
	}

	template<t_Width Width>
	template<t_Width Rhs_Width>
	Integer<Get_Max(Width, Rhs_Width)> Integer<Width>::operator+(const Integer<Rhs_Width> &rhs) const {
		bool overflow_flag = false, _;
		const auto result = Add<Width, Rhs_Width>(*this, rhs, overflow_flag, _);

		if (overflow_flag) {
			throw OverflowException<Get_Max(Width, Rhs_Width)>(result);
		}
		return result;
	}

	template<t_Width Width>
	template<t_Width Rhs_Width>
	void Integer<Width>::operator+=(const Integer<Rhs_Width> &rhs) {
		Assert_Valid_Assignment_Widths<Rhs_Width>();

		*this = ((*this) + rhs);
	}

	template<t_Width Width>
	Integer<Width> &Integer<Width>::operator++() {
		*this = ((*this) + kOne);

		return *this;
	}

	template<t_Width Width>
	Integer<Width> &Integer<Width>::operator--() {
		*this = ((*this) - kOne);

		return *this;
	}

	template<t_Width Width>
	template<t_Width Rhs_Width>
	Integer<Get_Max(Width, Rhs_Width)> Integer<Width>::operator-(const Integer<Rhs_Width> &rhs) const {
		return (*this) + rhs.Get_Complement();
	}

	template<t_Width Width>
	template<t_Width Rhs_Width>
	void Integer<Width>::operator-=(const Integer<Rhs_Width> &rhs) {
		Assert_Valid_Assignment_Widths<Rhs_Width>();

		*this = ((*this) - rhs);
	}

	template<t_Width Width>
	template<t_Width Rhs_Width>
	Integer<Get_Max(Width, Rhs_Width)> Integer<Width>::operator*(const Integer<Rhs_Width> &rhs) const {
		bool overflow_flag = false;
		const auto result = Mul(*this, rhs, overflow_flag);

		if (overflow_flag) {
			throw OverflowException<Get_Max(Width, Rhs_Width)>(result);
		}
		return result;
	}

	template<t_Width Width>
	template<t_Width Rhs_Width>
	void Integer<Width>::operator*=(const Integer<Rhs_Width> &rhs) {
		Assert_Valid_Assignment_Widths<Rhs_Width>();

		*this = ((*this) * rhs);
	}

	template<t_Width Width>
	template<t_Width Rhs_Width>
	Integer<Get_Max(Width, Rhs_Width)> Integer<Width>::operator/(const Integer<Rhs_Width> &rhs) const {
		return Div_Mod(*this, rhs).first;
	}

	template<t_Width Width>
	template<t_Width Rhs_Width>
	void Integer<Width>::operator/=(const Integer<Rhs_Width> &rhs) {
		Assert_Valid_Assignment_Widths<Rhs_Width>();

		*this = ((*this) / rhs);
	}

	template<t_Width Width>
	template<t_Width Rhs_Width>
	Integer<Get_Max(Width, Rhs_Width)> Integer<Width>::operator%(const Integer<Rhs_Width> &rhs) const {
		return Div_Mod(*this, rhs).second;
	}

	template<t_Width Width>
	template<t_Width Rhs_Width>
	void Integer<Width>::operator%=(const Integer<Rhs_Width> &rhs) {
		Assert_Valid_Assignment_Widths<Rhs_Width>();

		*this = ((*this) % rhs);
	}

	template<t_Width Width>
	Integer<Width> Integer<Width>::Factorial() const {
		if (!Is_Positive()) {
			throw ArithmeticException{"Factorial Of Negative Number"};
		}

		if (*this == kZero || *this == kOne) {
			return kOne;
		}

		bool overflow_flag = false;

		Integer result{*this};
		for (Integer multiplier{kTwo}; multiplier != *this; ++multiplier) {
			result = Mul(result, multiplier, overflow_flag);
		}

		if (overflow_flag) {
			throw OverflowException<Width>(result);
		}
		return result;
	}

	////// END DEFINITION Integer Operators


	////// BEGIN DEFINITION Integer Serialization

	template<t_Width Width>
	std::string Integer<Width>::Serialize() const noexcept {
		Integer copy{*this};
		bool is_positive = true;
		if (!copy.Is_Positive()) {
			copy = copy.Get_Complement();
			is_positive = false;
		}

		if (*this == kZero) {
			return "0";
		}

		std::stack<char> stack;
		for (; copy != kZero; copy /= kTen) {
			const Integer mod = copy % kTen;
			std::bitset<kWord_Bits_Cnt> bitset;
			for (size_t i = 0; i < bitset.size(); ++i) {
				bitset.set(i, mod[i]);
			}
			stack.push(static_cast<char>(bitset.to_ullong() + '0'));
		}

		std::ostringstream osstream;
		if (!is_positive) {
			osstream << static_cast<char>('-');
		}
		for (; !stack.empty(); stack.pop()) {
			osstream << stack.top();
		}
		return osstream.str();
	}

	template<t_Width Width>
	Integer<Width> Integer<Width>::Deserialize(const std::string &str) {
		bool overflow_flag = false, _;

		Integer result;
		bool is_positive = true;
		for (size_t i = 0; i < str.length(); ++i) {
			const char c = str[i];
			if (i == 0) {
				if (c == '+') {
					continue;
				}
				if (c == '-') {
					is_positive = false;
					continue;
				}
			}
			else {
				result = Mul(result, kTen, overflow_flag);
			}

			const std::bitset<kWord_Bits_Cnt> bits = c - '0';
			result = Add(result, Integer{bits}, overflow_flag, _);
		}

		if (overflow_flag) {
			throw OverflowException<Width>{result};
		}

		if (is_positive) {
			return result;
		}
		else {
			return result.Get_Complement();
		}
	}

	/*template<t_Width Width>
	std::ostream &operator<<(std::ostream &ostream, const Integer<Width> &integer) {
		ostream << integer.Serialize();
		return ostream;
	}*/

	////// END DEFINITION Integer Serialization


	////// BEGIN DEFINITION Integer Number Constants

	template<t_Width Width>
	const Integer<Width> Integer<Width>::kZero = Integer<Width>{};

	template<t_Width Width>
	const Integer<Width> Integer<Width>::kOne = [] () {
		Integer<Width> result;
		result[0] = true;
		return result;
	}();

	template<t_Width Width>
	const Integer<Width> Integer<Width>::kTwo = [] () {
		Integer<Width> result;
		result[1] = true;
		return result;
	}();

	template<t_Width Width>
	const Integer<Width> Integer<Width>::kTen = [] () {
		Integer<Width> result;
		result[1] = true;
		result[3] = true;
		return result;
	}();

	template<t_Width Width>
	template<t_Width Width_>
	Integer<Width_> Integer<Width>::Get_Normalized_New(t_Width actual_width) noexcept {
		Integer<Width_> result;
		return result.template Get_Normalized<Width_>(actual_width);
	}

	template<t_Width Width>
	template<t_Width Width_, t_Width New_Width>
	Integer<New_Width>
	Integer<Width>::Get_Normalized_Existing(const Integer<Width_> &integer, t_Width actual_width) noexcept {
		return integer.template Get_Normalized<New_Width>(actual_width);
	}

	////// END DEFINITION Integer Number Constants


	////// BEGIN DEFINITION Integer Util Functions

	template<t_Width Width>
	consteval bool Integer<Width>::Is_Limited() noexcept {
		return Width != kUnlimited;
	}

	template<t_Width Width>
	consteval bool Integer<Width>::Is_Unlimited() noexcept {
		return mparith::Is_Unlimited(Width);
	}

	template<t_Width Width>
	template<t_Width New_Width>
	Integer<New_Width> Integer<Width>::Get_Normalized(size_t actual_width) const noexcept {
		Assert_Greater_Equal_Width<New_Width>();

		if (Get_Actual_Width() == actual_width) {
			return *this;
		}

		std::vector<std::bitset<kWord_Bits_Cnt>> result_bits;
		result_bits.resize(actual_width);
		std::ranges::copy(_bits, result_bits.begin());

		if (!Is_Positive()) {
			std::ranges::for_each(result_bits.begin() + Get_Actual_Width(), result_bits.end(), [] (auto &bitset) {
				bitset.flip();
			});
		}

		Integer<New_Width> result;
		result._bits = std::move(result_bits);
		return result;
	}

	template<t_Width Width>
	constexpr t_Width Integer<Width>::Get_Actual_Width() const noexcept {
		if constexpr (Is_Limited()) {
			return Width;
		}

		return _bits.size();
	}

	template<t_Width Width>
	constexpr size_t Integer<Width>::Get_Actual_Bits_Cnt() const noexcept {
		return Get_Actual_Width() * kWord_Bits_Cnt;
	}

	template<t_Width Width>
	Integer<Width> Integer<Width>::Get_Inverse() const noexcept {
		if constexpr (Is_Limited()) {
			std::bitset<kBits_Cnt> bitset = Get_Bitset();
			bitset.flip();
			return bitset;
		}
		else {
			Integer result{*this};
			for (size_t i = 0; i < Get_Actual_Bits_Cnt(); ++i) {
				result[i] = !((*this)[i]);
			}
			return result;
		}
	}

	template<t_Width Width>
	Integer<Width> Integer<Width>::Get_Complement() const noexcept {
		bool _;
		return Add(Get_Inverse(), kOne, _, _);
	}

	template<t_Width Width>
	size_t Integer<Width>::Get_Msb_Idx() const noexcept {
		for (size_t i = Get_Actual_Bits_Cnt() - 1; i != 0; --i) {
			if ((*this)[i]) {
				return i;
			}
		}
		return 0;
	}

	template<t_Width Width>
	std::bitset<Integer<Width>::kBits_Cnt> Integer<Width>::Get_Bitset() const noexcept {
		std::bitset<kBits_Cnt> bitset;
		for (size_t i = 0, idx = 0; i < Width; ++i) {
			for (size_t j = 0; j < kWord_Bits_Cnt; ++j, ++idx) {
				if (_bits[i][j]) {
					bitset.set(idx);
				}
			}
		}

		return bitset;
	}

	template<t_Width Width>
	bool Integer<Width>::Is_Positive() const noexcept {
		return !(_bits[Get_Actual_Width() - 1][kWord_Bits_Cnt - 1]);
	}

	template<t_Width Width>
	Integer<Width> Integer<Width>::Get_Positive() const noexcept {
		if (Is_Positive()) {
			return *this;
		}
		else {
			return Get_Complement();
		}
	}


	////// END DEFINITION Integer Util Functions

	///// END DEFINITION Integer

}
