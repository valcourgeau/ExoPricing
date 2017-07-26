#include "Session1/Session1.hpp"

using namespace cfl;


 cfl::Function 
  prb::discountLiborLogLinearInterp(const std::vector<double> & rLiborPeriods,
			       const std::vector<double> & rLiborRates, 
			       double dInitialTime)
  {
	  PRECONDITION(rLiborPeriods.size() == rLiborRates.size());
	  std::vector<double> uTimes(rLiborPeriods.size() + 1);
	  uTimes.front() = dInitialTime;

	  std::transform(rLiborPeriods.begin(), rLiborPeriods.end(), uTimes.begin() + 1,
		  [dInitialTime](double dX){return dX + dInitialTime;});

	  std::vector<double> uDiscount(uTimes.size());
	  uDiscount.front() = 1.;

	  for(unsigned iI = 1; iI<uDiscount.size(); iI++)
	  {
		  uDiscount[iI] = 1. / (1 + rLiborRates[iI-1] * rLiborPeriods[iI-1]);
	  }
	  
	  std::vector<double> uLogDiscount(uDiscount.size());
	  std::transform(uDiscount.begin(), uDiscount.end(), uLogDiscount.begin(), [](double dX){return std::log(dX);} );
  
	  cfl::Interp uLinear = NInterp::linear();
	  Function uLogDiscountFunction = uLinear.interpolate(uTimes.begin(), uTimes.end(), uLogDiscount.begin());
	
	  return cfl::exp(uLogDiscountFunction);
 }

 cfl::Function 
	prb::discountLiborFit(const std::vector<double> & rLiborPeriods,
		   const std::vector<double> & rLiborRates, 
		   double dInitialTime)
 {
	PRECONDITION(rLiborPeriods.size() == rLiborRates.size());

	double dYield(0.);
	for(unsigned int iI = 0; iI < rLiborRates.size(); iI++)
	{
		dYield += std::log(1+rLiborRates[iI] * rLiborPeriods[iI]) / rLiborPeriods[iI];
	}
	dYield /= rLiborPeriods.size();

	return cfl::Data::discount(dYield, dInitialTime);
 }
 
 /*
cfl::Function 
  prb::discountLiborLogLinearInterp(const std::vector<double> & rLiborPeriods,
			       const std::vector<double> & rLiborRates, 
			       double dInitialTime)
{
	return cfl::Function();
}*/

 /*
  cfl::Function 
  prb::discountLiborLogLinearInterp(const std::vector<double> & rLiborPeriods,
			       const std::vector<double> & rLiborRates, 
			       double dInitialTime)
  {
	  PRECONDITION(std::equal(rLiborPeriods.begin(), rLiborPeriods.end()-1, rLiborPeriods.begin()+1,
		  std::greater<double>()));
	  std::vector<double> uTimes(rLiborPeriods.size() + 1);
	  uTimes.front() = dInitialTime;

	  std::vector<double> uLogDiscount(rLiborPeriods.size() + 1);
	  uLogDiscount.front() = 1.;
	  std::transform(rLiborPeriods.begin(), rLiborPeriods.end(), uTimes.begin() + 1,
		  [dInitialTime](double dX){return dX + dInitialTime;});

	  std::transform(rLiborPeriods.begin(), rLiborPeriods.end(), rLiborRates.begin(), 
		  uLogDiscount.begin()+1, std::multiplies<double>());
	  std::transform(uLogDiscount.begin(), uLogDiscount.end(), uLogDiscount.begin(),
		  [](double dX){return 1. / (1. + dX);});
	  std::transform(uLogDiscount.begin(), uLogDiscount.end(), uLogDiscount.begin(),
		  [](double dX){return std::log(dX);});

	   
	   cfl::Interp uLinear = NInterp::linear();
	   Function uLogDiscountFunction = uLinear.interpolate(uTimes.begin(), 
		   uTimes.end(), uLogDiscount.begin());
	
	   return cfl::exp(uLogDiscountFunction);
  }*/