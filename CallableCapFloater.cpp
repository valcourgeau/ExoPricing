#include "SampleExam3/SampleExam3.hpp"

using namespace cfl;  
/*
cfl::Slice liborRate(unsigned iTime, cfl::InterestRateModel & rModel, double dPeriod)
{
	return (1./rModel.discount(iTime, rModel.eventTimes()[iTime] + dPeriod) - 1.)/dPeriod;
}



cfl::MultiFunction prb::callableCappedFloater(const cfl::Data::CashFlow & rCap, 
					   double dLiborSpread, 
					   cfl::InterestRateModel & rModel)
  {
	  double dPeriod(rCap.period);
	  std::vector<double> uEventTimes(rCap.numberOfPayments);
	  uEventTimes.front() = rModel.initialTime();
	  std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1,
		  [dPeriod](double dX){return dX + dPeriod;});
	  rModel.assignEventTimes(uEventTimes);

	  unsigned iTime(uEventTimes.size() - 1);
	  Slice uOption = rModel.cash(iTime, 1. + rCap.rate * dPeriod)
		  * rModel.discount(iTime, uEventTimes.back() + dPeriod);
	  Slice uCoupon;

	  while(iTime > 0)
	  {
		  uOption = min(uOption, 1.); // Seller = issuer = minimise
		  iTime--;
		  uOption.rollback(iTime);
		  uCoupon = liborRate(iTime, rModel, dPeriod) + dLiborSpread;
		  uCoupon = min(uCoupon, rCap.rate);
		  uCoupon *= rCap.period;
		  uCoupon *= rModel.discount(iTime, uEventTimes[iTime] + dPeriod);
		  uOption += uCoupon;
	  }

	  uOption *= rCap.notional;
	  return interpolate(uOption);
  }
  */

cfl::Slice liborRate(unsigned iTime, double dPeriod, cfl::InterestRateModel & rModel)
{
	return (1./rModel.discount(iTime, rModel.eventTimes()[iTime]+dPeriod) - 1.) / dPeriod;
}

cfl::MultiFunction prb::callableCappedFloater(const cfl::Data::CashFlow & rCap, 
					   double dLiborSpread, 
					   cfl::InterestRateModel & rModel)
{
	double dPeriod(rCap.period);
	std::vector<double> uEventTimes(rCap.numberOfPayments);
	uEventTimes.front() = rModel.initialTime();
	std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1,
		[dPeriod](double dX){return dX + dPeriod;});
	rModel.assignEventTimes(uEventTimes);

	unsigned iTime(uEventTimes.size()-1);
	Slice uOption = rModel.cash(iTime, 1.0);
	Slice uCoupon = min(liborRate(iTime, dPeriod, rModel) + dLiborSpread, rCap.rate)
			 * dPeriod;
	Slice uDiscount = rModel.discount(iTime, uEventTimes[iTime]+dPeriod);
	uOption += uCoupon;
	uOption *= uDiscount;

	while(iTime > 0)
	{
		uOption = min(uOption, rModel.cash(iTime, 1.0));
		iTime--;
		uOption.rollback(iTime);
		uDiscount = rModel.discount(iTime, uEventTimes[iTime]+dPeriod);
		uCoupon = min(liborRate(iTime, dPeriod, rModel) + dLiborSpread, rCap.rate)
			 * dPeriod;
		uOption += uCoupon * uDiscount;
	}

	uOption *= rCap.notional;
	return interpolate(uOption);
}

