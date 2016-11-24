#include <iostream>
#include "calculator.hpp"

namespace test01
{
}

int main(int argc, char* argv[])
{
	for(auto i=0; i<argc; ++i)
	{
		try
		{
			std::cout << argv[i] << " === " << test01::calculator::calculate(argv[i]) << std::endl;
		}
		catch(test01::parser_exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}

	}
	return 0;
}