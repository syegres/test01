#include <gtest/gtest.h>
#include "calculator.hpp"

namespace test01
{
}

using namespace test01;
using floating = calculator<double>;
using integer = calculator<int64_t>;

TEST( floating, given_test )
{
	EXPECT_EQ(floating::calculate("-1 + 5 - 3"), 1);
	EXPECT_EQ(floating::calculate("-10 + (8 * 2.5) - (3 / 1,5)"), 8);
	EXPECT_EQ(floating::calculate("1 + (2 * (2.5 + 2.5 + (3 - 2))) - (3 / 1.5)"), 11);
	EXPECT_THROW(floating::calculate("1.1 + 2.1 + abc"), parser_exception);
}

TEST( integer, given_test )
{
	EXPECT_EQ(integer::calculate("-1 + 5 - 3"), 1);
	EXPECT_EQ(integer::calculate("-10 + (8 * 2.5) - (3 / 1,5)"), 8);
	EXPECT_EQ(integer::calculate("1 + (2 * (2.5 + 2.5 + (3 - 2))) - (3 / 1.5)"), 11);
	EXPECT_THROW(integer::calculate("1.1 + 2.1 + abc"), parser_exception);
}

TEST( floating, op_priority )
{
	EXPECT_EQ(floating::calculate("1 + 2 * 3"), 7);
	EXPECT_EQ(floating::calculate("(1 + 2) * 3"), 9);
	EXPECT_EQ(floating::calculate("1 + 6 / 3"), 3);
	EXPECT_EQ(floating::calculate("(1 + 5) / 3"), 2);
}

TEST( integer, op_priority )
{
	EXPECT_EQ(integer::calculate("1 + 2 * 3"), 7);
	EXPECT_EQ(integer::calculate("(1 + 2) * 3"), 9);
	EXPECT_EQ(integer::calculate("1 + 6 / 3"), 3);
	EXPECT_EQ(integer::calculate("(1 + 5) / 3"), 2);
}

TEST( floating, unary_expr )
{
	EXPECT_THROW(floating::calculate("-+1"), parser_exception);
	EXPECT_EQ(floating::calculate("- 1"), -1);
	EXPECT_EQ(floating::calculate("+ 1"), +1);
	EXPECT_EQ(floating::calculate("+1"), +1);
	EXPECT_EQ(floating::calculate("-(-1)"), +1);
	EXPECT_EQ(floating::calculate("+(+1)"), +1);
	EXPECT_EQ(floating::calculate("-1 + 5 - 3"), 1);
	EXPECT_EQ(floating::calculate("-1 + 5 + (-3)"), 1);
	EXPECT_EQ(floating::calculate("-1 + 5 + (-3 + 1)"), 2);
	EXPECT_EQ(floating::calculate("-1 + 5 + (-3 + (+1))"), 2);
	EXPECT_EQ(floating::calculate("-1 + 5 + (-3 * 2)"), -2);
	EXPECT_EQ(floating::calculate("-1 + 5 + (3 * (-2))"), -2);
	EXPECT_THROW(floating::calculate("-1 + 5 + -3"), parser_exception);
}

TEST( integer, unary_expr )
{
	EXPECT_THROW(integer::calculate("-+1"), parser_exception);
	EXPECT_EQ(integer::calculate("- 1"), -1);
	EXPECT_EQ(integer::calculate("+ 1"), +1);
	EXPECT_EQ(integer::calculate("+1"), +1);
	EXPECT_EQ(integer::calculate("-(-1)"), +1);
	EXPECT_EQ(integer::calculate("+(+1)"), +1);
	EXPECT_EQ(integer::calculate("-1 + 5 - 3"), 1);
	EXPECT_EQ(integer::calculate("-1 + 5 + (-3)"), 1);
	EXPECT_EQ(integer::calculate("-1 + 5 + (-3 + 1)"), 2);
	EXPECT_EQ(integer::calculate("-1 + 5 + (-3 + (+1))"), 2);
	EXPECT_EQ(integer::calculate("-1 + 5 + (-3 * 2)"), -2);
	EXPECT_EQ(integer::calculate("-1 + 5 + (3 * (-2))"), -2);
	EXPECT_THROW(integer::calculate("-1 + 5 + -3"), parser_exception);
}

TEST( floating, precision )
{
	EXPECT_DOUBLE_EQ(floating::calculate("-1.01 + 5.02 - 3.04"), 0.97);
	EXPECT_DOUBLE_EQ(floating::calculate("-1.01999 + 5.02888 - 3,04777"), 0.97);
	EXPECT_DOUBLE_EQ(floating::calculate("3.141592653589793/2,718281828459045"), 1.16);
}

TEST( integer, precision )
{
	EXPECT_DOUBLE_EQ(integer::calculate("-1.01 + 5.02 - 3.04"), 0.97);
	EXPECT_DOUBLE_EQ(integer::calculate("-1.01999 + 5.02888 - 3,04777"), 0.97);
	EXPECT_DOUBLE_EQ(integer::calculate("3.141592653589793/2,718281828459045"), 1.16);
}
