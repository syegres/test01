#include <string>
#include <iostream>
#include <exception>
#include <math.h>
#include <stdint.h>
#include <sstream>

namespace test01
{
	namespace
	{
		static const double fmult0[] = { 1., 10., 100., 1000., 10000, 100000 };
		static const double fmult1[] = { 1., 0.1, 0.01, 0.001, 0.0001, 0.00001 };

		inline double normalize(double val, unsigned precision, double) { return floor(val * fmult0[precision] + 0.5)*fmult1[precision]; }
		inline double denormalize(double val, unsigned precision) { return val; }
		inline double round(double val, unsigned precision) { return floor(val * fmult0[precision] + 0.5)*fmult1[precision]; }

		inline int64_t normalize(double val, unsigned precision, int64_t) { return int64_t(floor(val * fmult0[precision] + 0.5)); }
		inline double denormalize(int64_t val, unsigned precision) { return double(val) * fmult1[precision]; }
		inline double round(int64_t val, unsigned precision) { return val; }
	}


	template<typename value_t>
	class token_t
	{
	public:
		enum Enum { unknown, lbrace, rbrace, op_plus, op_minus, op_mult, op_div, number, eof };

		token_t(Enum type=unknown, value_t val=value_t{}) noexcept: _type(type), _val(val) {}
		Enum type() const { return _type; }
		value_t value() const { return _val; }

	private:
		Enum _type;
		value_t _val;
	};

	template<typename value_t>
	class skip_ws_lexer_t
	{
	public:
		using cit_t = std::string::const_iterator;
		using token = token_t<value_t>;

		skip_ws_lexer_t(const std::string::const_iterator& begin, const std::string::const_iterator& end, unsigned precision) noexcept : _begin(begin), _end(end), _precision(precision) { }
		skip_ws_lexer_t(const skip_ws_lexer_t& ini) noexcept : _begin(ini._begin), _end(ini._end), _precision(ini._precision) { }

		cit_t current() const { return _begin; }
		bool complete() const { return _begin == _end; }
		bool advance() { ++_begin; return !complete(); }
		unsigned precision() const { return _precision; }
		token get_token()
		{
			if (_last_token.type()!=token::unknown)
			{
				token ret(_last_token);
				_last_token = token(token::unknown);
				return ret;
			}
			while(!complete() && is_ws())
				advance();
			if (complete())
				return token::eof;
			if (is_digit() || is_decimal_point())
			{
				double val{};
				for( ; is_digit() && _begin != _end; ++_begin)
					val = val*10. + (*_begin-'0');
				if (is_decimal_point())
				{
					double denom = 10.;
					unsigned demon_count{};
					for( ++_begin; is_digit() && _begin != _end; ++_begin, denom *= 10, ++demon_count)
						if (demon_count < _precision)
							val += (*_begin-'0')/denom;
				}
				return token(token::number, normalize(val, _precision, value_t{}));
			}
			auto c = *_begin++;
			switch (c)
			{
				case '(': return token::lbrace;
				case ')': return token::rbrace;
				case '+': return token::op_plus;
				case '-': return token::op_minus;
				case '*': return token::op_mult;
				case '/': return token::op_div;
			}
			return token::unknown;
		}
		void return_token(const token& t)
		{
			_last_token = t;
		}

	private:
		cit_t _begin, _end;
		token _last_token{};
		unsigned _precision{2};
		bool is_ws() const { return *_begin == ' '; }
		bool is_digit() const { return *_begin >= '0' && *_begin <= '9'; }
		bool is_decimal_point() const { return *_begin == ',' || *_begin == '.'; }
	};

	template<typename value_t>
	class parser_state_t
	{
	public:
		using skip_ws_lexer = skip_ws_lexer_t<value_t>;
		parser_state_t(const std::string::const_iterator& begin, const std::string::const_iterator& end, unsigned precision)
			: _lexer(begin, end, precision)
		{ }
		bool success() const { return _success; }
		void success(bool v) { _success = v; }
		value_t value() const { return _value; }
		void value(value_t val) { _value = val; }
		skip_ws_lexer& lexer() { return _lexer; }
		unsigned precision() const { return _lexer.precision(); }
		unsigned recursion() const { return _recursion; }
		void recursion(unsigned rec) { _recursion = rec; }

	private:
		skip_ws_lexer _lexer;
		bool _success{false};
		value_t _value{};
		unsigned _recursion{};
	};

	class parser_exception: std::runtime_error
	{
		std::string _source, _reason;
		long _offset;
	public:
		parser_exception(const std::string& reason, std::string&& source, long offset)
			: std::runtime_error(reason), _source(source), _offset(offset)
		{
			std::ostringstream oss;
			oss << "Incorrect input at " << offset << ": " << _source.substr(offset);
			_reason = oss.str();
		}

		const char * what() const noexcept override
		{
			return _reason.c_str();
		}
	};

	template<typename value_t>
	class calculator
	{
	public:
		using parser_state = parser_state_t<value_t>;
		using token = typename parser_state::skip_ws_lexer::token;

		static double calculate(const std::string& input, unsigned precision=2)
		{
			return parse(input.begin(), input.end(), precision);
		}

	private:
		static double parse(std::string::const_iterator begin, std::string::const_iterator end, unsigned precision=2)
		{
			parser_state state(begin, end, precision);
			auto ret = expression(state);
			state.success(state.success() && state.lexer().complete());
			if (!ret.success())
				throw parser_exception("failed parsing", std::move(std::string(begin, end)), end-ret.lexer().current());
			return denormalize(ret.value(), precision);
		}

		static const parser_state& expression(parser_state& input)
		{
			value_t val{};
			bool stop = false;
			if (primary_expression(input, true).success())
				val = input.value();
			while(input.success() && !stop)
			{
				auto t = input.lexer().get_token();
				switch(t.type())
				{
					case token::op_minus:
						if (primary_expression(input, false).success())
							val = round(val - input.value(), input.precision());
						break;
					case token::op_plus:
						if (primary_expression(input, false).success())
							val = round(val + input.value(), input.precision());
						break;
					case token::rbrace:
						if (input.recursion()==0)
						{
							input.success(false);
							return input;
						}
						else
						{
							input.lexer().return_token(t);
							stop = true;
						}

						break;
					default:
						input.lexer().return_token(t);
						stop = true;
						break;
				}
			}
			if (input.success())
				input.value(val);
			return input;
		}

		static const parser_state& primary_expression(parser_state& input, bool allow_unary_expression)
		{
			value_t val{};
			bool stop = false;
			if (operand_expression(input, allow_unary_expression).success())
				val = input.value();
			while(input.success() && !stop)
			{
				auto t = input.lexer().get_token();
				auto prec = input.precision();
				switch(t.type())
				{
					case token::op_mult:
						if(operand_expression(input, false).success())
							val = round(normalize(denormalize(val, prec) * denormalize(input.value(), prec), prec, value_t{}), prec);
						break;
					case token::op_div:
						if (operand_expression(input, false).success())
							val = round(normalize(denormalize(val, prec) / denormalize(input.value(), prec), prec, value_t{}), prec);
						break;
					default:
						input.lexer().return_token(t);
						stop = true;
						break;
				}
			}
			if (input.success())
				input.value(val);
			return input;
		}

		static const parser_state& operand_expression(parser_state& input, bool allow_unary_expression)
		{
			value_t mult = 1.;
			bool try_more = false;
			do
			{
				input.success(false);
				auto t = input.lexer().get_token();
				switch(t.type())
				{
					case token::op_minus:
						if (!allow_unary_expression || try_more)
						{
							input.lexer().return_token(t);
							input.success(false);
							return input;
						}
						else
							mult = -1., try_more = true;
						break;
					case token::op_plus:
						if (!allow_unary_expression || try_more)
						{
							input.lexer().return_token(t);
							input.success(false);
							return input;
						}
						else
							mult = +1., try_more = true;
						break;
					case token::number:
						input.success(true);
						input.value(mult * t.value());
						return input;
						break;
					case token::lbrace:
						input.recursion(input.recursion()+1);
						expression(input);
						input.recursion(input.recursion()-1);
						input.success(input.success() && input.lexer().get_token().type() == token::rbrace);
						if (input.success())
							input.value(mult*input.value());
						return input;
						break;
					default:
						input.lexer().return_token(t);
						return input;
				}
			} while(try_more);
			return input;
		}
	};
}
