#include <iostream>

#include "fct_utiles.hpp"

void clearLine(int nChar)
{
	std::cout << '\r';	// Returning to the beginning of the line
	for(int i=0; i < nChar; i++)
	{
		std::cout << ' ';
	}
}

