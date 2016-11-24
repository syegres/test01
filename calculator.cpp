#include <iostream>
#include <string.h>
#include <functional>
#include <stdlib.h>
#include "calculator.hpp"

using namespace test01;
using floating = calculator<double>;
using integer = calculator<int64_t>;

int main(int argc, char* argv[])
{
	std::function<double(const std::string&, unsigned)> calc_func = floating::calculate;
	unsigned precision = 2;
	for(auto i=1; i<argc; ++i)
	{
		if (strcmp(argv[i], "--integer") == 0)
		{
			calc_func = integer::calculate;
			std::cout << "*** Switched to integer calculator" << std::endl;
		}
		else if (strcmp(argv[i], "--floating") == 0)
		{
			calc_func = floating::calculate;
			std::cout << "*** Switched to floating calculator" << std::endl;
		}
		else if (strstr(argv[i], "--precision=")==argv[i])
		{
			precision = atoi(argv[i]+12);
			std::cout << "*** Changed precision to " << precision << std::endl;
		}
		else
			try
			{
				std::cout << argv[i] << " -> " << calc_func(argv[i], precision) << std::endl;
			}
			catch(test01::parser_exception& ex)
			{
				std::cout << "!!! " << argv[i] << " -> " << ex.what() << std::endl;
			}
	}
	return 0;
}