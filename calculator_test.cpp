#include <gtest/gtest.h>
#include "calculator.hpp"

namespace test01
{
}

using namespace test01;

TEST( calculator, given_test )
{
	EXPECT_EQ(calculator::calculate("-1 + 5 - 3"), 1);
	EXPECT_EQ(calculator::calculate("-10 + (8 * 2.5) - (3 / 1,5)"), 8);
	EXPECT_EQ(calculator::calculate("1 + (2 * (2.5 + 2.5 + (3 - 2))) - (3 / 1.5)"), 11);
	EXPECT_THROW(calculator::calculate("1.1 + 2.1 + abc"), parser_exception);
}

TEST( calculator, op_priority )
{
	EXPECT_EQ(calculator::calculate("1 + 2 * 3"), 7);
	EXPECT_EQ(calculator::calculate("(1 + 2) * 3"), 9);
	EXPECT_EQ(calculator::calculate("1 + 6 / 3"), 3);
	EXPECT_EQ(calculator::calculate("(1 + 5) / 3"), 2);
}

TEST( calculator, unary_expr )
{
	EXPECT_EQ(calculator::calculate("-+1"), 1);
	EXPECT_EQ(calculator::calculate("-1 + 5 - 3"), 1);
	EXPECT_EQ(calculator::calculate("-1 + 5 + (-3)"), 1);
	EXPECT_EQ(calculator::calculate("-1 + 5 + (-3 + 1)"), 2);
	EXPECT_EQ(calculator::calculate("-1 + 5 + (-3 + (+1))"), 2);
	EXPECT_EQ(calculator::calculate("-1 + 5 + (-3 * 2)"), -2);
	EXPECT_EQ(calculator::calculate("-1 + 5 + (3 * (-2))"), -2);
	EXPECT_THROW(calculator::calculate("-1 + 5 + -3"), parser_exception);
}

TEST( calculator, precision )
{
	EXPECT_EQ(calculator::calculate("-1.01 + 5.02 - 3.04"), 0.99);
	EXPECT_EQ(calculator::calculate("-1.01999 + 5.02888 - 3.04777"), 0.99);
}
