#include "SampleExam2/SampleExam2.hpp"

using namespace cfl;

  cfl::Function 
  prb::discountSwapLogLinearInterp(const std::vector<double> & rSwapRates, 
			      double dPeriod, double dInitialTime)
  {
	  PRECONDITION(dInitialTime >= 0);
	  unsigned iSize(rSwapRates.size());
	  std::vector<double> uDiscount(iSize + 1), uTimes(iSize + 1); // size m+1
	  double dPartialSum(0.0); // Sum at index 0 is zero
	 uDiscount.front() = 1.;

	  for(unsigned iI = 0; iI < iSize; iI++)
	  {
		  uDiscount[iI+1] = (1. - rSwapRates[iI] * dPartialSum * dPeriod) / (1. + rSwapRates[iI] * dPeriod);		
		  dPartialSum += uDiscount[iI+1];
	  }

	  // Create time points
	  uTimes.front() = dInitialTime;
	  std::transform(uTimes.begin(), uTimes.end()-1, uTimes.begin() + 1,
		  [dPeriod](double dX){return dX + dPeriod;});

	  // Take the log of discounts
	  std::transform(uDiscount.begin(), uDiscount.end(), uDiscount.begin(), [](double dX){return std::log(dX);});

	  // Interpolation
      cfl::Interp uLinear = NInterp::linear();
	  Function uLogDiscountFunction = uLinear.interpolate(uTimes.begin(), uTimes.end(), uDiscount.begin());
		
	  return cfl::exp(uLogDiscountFunction);
  }

  /*
    cfl::Function 
  prb::discountSwapLogLinearInterp(const std::vector<double> & rSwapRates, 
			      double dPeriod, double dInitialTime)
{
	std::vector<double> uLogDiscount(rSwapRates), uTimes(rSwapRates.size());
	uTimes.front() = dInitialTime;
	std::transform(uTimes.begin(), uTimes.end()-1, uTimes.begin()+1,
		[dPeriod](double dX){return dX + dPeriod;});
	double dPartialSum(0.0);
	for(unsigned iI = 0; iI < rSwapRates.size(); iI++)
	{
		uLogDiscount[iI] = (1. - rSwapRates[iI] * dPartialSum * dPeriod) / (1. + dPeriod * rSwapRates[iI]);
		dPartialSum += uLogDiscount[iI];
	}
	std::transform(uLogDiscount.begin(), uLogDiscount.end(), uLogDiscount.begin(), [](double dX){return std::log(dX);});
	cfl::Interp uLinear = NInterp::linear();

	cfl::Function linearInterp = uLinear.interpolate(uTimes.begin(), uTimes.end(), uLogDiscount.begin());
	return cfl::exp(linearInterp);

}*/