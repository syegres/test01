#include <iostream>
#include "calculator.hpp"

using namespace test01;
using floating = calculator<double,2>;
using integer = calculator<int64_t, 2>;

int main(int argc, char* argv[])
{
	for(auto i=0; i<argc; ++i)
	{
		try
		{
			std::cout << argv[i] << " === " << floating::calculate(argv[i]) << std::endl;
		}
		catch(test01::parser_exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}

	}
	return 0;
}