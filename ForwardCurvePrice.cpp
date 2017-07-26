#include "Homework1/Homework1.hpp"

using namespace cfl;

  cfl::Function 
  prb::forwardLogLinearInterp(double dSpot, 
			 const std::vector<double> & rDeliveryTimes, 
			 const std::vector<double> & rForwardPrices,
			 double dInitialTime)
  {
	  PRECONDITION(rDeliveryTimes.size() == rForwardPrices.size());
	  PRECONDITION(rDeliveryTimes.size() > 0);
	  PRECONDITION(rDeliveryTimes.front() > dInitialTime);
	  PRECONDITION(std::equal(rDeliveryTimes.begin()+1, rDeliveryTimes.end(),
		  rDeliveryTimes.begin(), std::greater<double>()));
	  std::vector<double> uTimes(rDeliveryTimes),
		  uLogPrices(rForwardPrices.size() + 1);
	  uTimes.front() = dInitialTime;
	  uLogPrices.front() = std::log(dSpot);

	  uTimes.insert(uTimes.begin(), dInitialTime);

	  std::transform(rForwardPrices.begin(), rForwardPrices.end(),
		  uLogPrices.begin() + 1, [](double dX){return std::log(dX);});

	  cfl::Interp uLinear = NInterp::linear();

	  Function uLogInterp = uLinear.interpolate(uTimes.begin(),
		  uTimes.end(), uLogPrices.begin());

	  return cfl::exp(uLogInterp);
  }
  /*
 cfl::Function 
  prb::forwardLogLinearInterp(double dSpot, 
			 const std::vector<double> & rDeliveryTimes, 
			 const std::vector<double> & rForwardPrices,
			 double dInitialTime)
  {
	  PRECONDITION(rDeliveryTimes.size() == rForwardPrices.size());
	  PRECONDITION(dInitialTime < rDeliveryTimes.front());
	  std::vector<double> uTimes(rDeliveryTimes), uPrices(rForwardPrices);
	  uTimes.insert(uTimes.begin(), dInitialTime);
	  uPrices.insert(uPrices.begin(), dSpot);

	  std::transform(uPrices.begin(), uPrices.end(), uPrices.begin(),
		  [](double dX){return dX;});
	  cfl::Interp uLinear = NInterp::linear();

	  return cfl::exp(uLinear.interpolate(uTimes.begin(), uTimes.end(), uPrices.begin()));
  }
 */