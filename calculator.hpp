#include <string>
#include <iostream>
#include <exception>
#include <tuple>
#include <stack>

namespace test01
{
	using value_t = double;
	const int precision = 2;

	class token
	{
	public:
		enum Enum { unknown, lbrace, rbrace, op_plus, op_minus, op_mult, op_div, number, eof };

		token(Enum type=unknown, value_t val=value_t{}) noexcept: _type(type), _val(val) {}
		Enum type() const { return _type; }
		value_t value() const { return _val; }

	private:
		Enum _type;
		value_t _val;
	};

	class skip_ws_lexer
	{
		using cit_t = std::string::const_iterator;
		cit_t _begin, _end;
		token _last_token{};
		bool is_ws() const { return *_begin == ' '; }
		bool is_digit() const { return *_begin >= '0' && *_begin <= '9'; }
		bool is_decimal_point() const { return *_begin == ',' || *_begin == '.'; }

	public:
		skip_ws_lexer(const std::string::const_iterator& begin, const std::string::const_iterator& end) noexcept : _begin(begin), _end(end) { }
		skip_ws_lexer(const skip_ws_lexer& ini) noexcept : _begin(ini._begin), _end(ini._end) { }

		cit_t current() const { return _begin; }
		bool complete() const { return _begin == _end; }
		bool advance() { ++_begin; return !complete(); }
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
				value_t val{};
				for( ; is_digit() && _begin != _end; ++_begin)
					val = val*10. + (*_begin-'0');
				if (is_decimal_point())
				{
					value_t denom = 10.;
					for( ++_begin; is_digit() && _begin != _end; ++_begin, denom *= 10)
						val += (*_begin-'0')/denom;
				}
				return token(token::number, val);
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
	};

	class parser_state
	{
	public:
		parser_state(const std::string::const_iterator& begin, const std::string::const_iterator& end)
			: _lexer(begin, end)
		{ }
		bool success() const { return _success; }
		void success(bool v) { _success = v; }
		value_t value() const { return _value; }
		void value(value_t val) { _value = val; }
		skip_ws_lexer& lexer() { return _lexer; }

	private:
		skip_ws_lexer _lexer;
		bool _success{false};
		value_t _value{};
	};

	class parser_exception: std::runtime_error
	{
		std::string _source;
		long _offset;
	public:
		parser_exception(const std::string& reason, std::string&& source, long offset)
			: std::runtime_error(reason), _source(source), _offset(offset)
		{}

		const char * what() const noexcept override
		{
			return nullptr;
		}
	};

	class calculator
	{
	public:
		static value_t calculate(const std::string& input)
		{
			return parse(input.begin(), input.end());
		}

	private:
		static value_t parse(std::string::const_iterator begin, std::string::const_iterator end)
		{
			parser_state state(begin, end);
			auto ret = expression(state);
			state.success(state.success() && state.lexer().complete());
			if (!ret.success())
				throw parser_exception("failed parsing", std::move(std::string(begin, end)), end-ret.lexer().current());
			return ret.value();
		}

		static const parser_state& expression(parser_state& input)
		{
			value_t val{};
			if (primary_expression(input, true).success())
				val = input.value();
			while(input.success())
			{
				auto t = input.lexer().get_token();
				switch(t.type())
				{
					case token::op_minus:
						if (primary_expression(input, false).success())
							val -= input.value();
						break;
					case token::op_plus:
						if (primary_expression(input, false).success())
							val += input.value();
						break;
					default:
						input.lexer().return_token(t);
						return input;
				}
			}
			if (input.success())
				input.value(val);
			return input;
		}

		static const parser_state& primary_expression(parser_state& input, bool allow_unary_expression)
		{
			value_t val{};
			if (operand_expression(input, allow_unary_expression).success())
				val = input.value();
			while(input.success())
			{
				auto t = input.lexer().get_token();
				switch(t.type())
				{
					case token::op_mult:
						if(operand_expression(input, false).success())
							val *= input.value();
						break;
					case token::op_div:
						if (operand_expression(input, false).success())
							val /= input.value();
						break;
					default:
						input.lexer().return_token(t);
						return input;
				}
			}
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
							input.lexer().return_token(t);
						else
							mult = -1., try_more = true;
						break;
					case token::op_plus:
						if (!allow_unary_expression || try_more)
							input.lexer().return_token(t);
						else
							mult = +1., try_more = true;
						break;
					case token::number:
						input.success(true);
						input.value(mult * t.value());
						return input;
						break;
					case token::lbrace:
						expression(input);
						input.success(input.success() && input.lexer().get_token().type() == token::rbrace);
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
