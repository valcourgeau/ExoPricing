#include "Homework1/Homework1.hpp"

cfl::Function prb::forwardFX(double dSpotFX, 
			  const cfl::Function & rDomesticDiscount, 
			  const cfl::Function & rForeignDiscount)
{
	return dSpotFX * rForeignDiscount / rDomesticDiscount;
}