#include "Homework3/Homework3.hpp"
#include <iostream>
#include <stdio.h>

using namespace cfl;



 cfl::MultiFunction prb::collar(const cfl::Data::CashFlow & rCap, 
			    double dFloorRate, 
			    cfl::InterestRateModel & rModel)
  {
	  PRECONDITION(rCap.rate > dFloorRate);

	  double dPeriod(rCap.period);
	std::vector<double> uEventTimes(rCap.numberOfPayments);
	uEventTimes.front() = rModel.initialTime();
	std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin() + 1,
		[dPeriod](double dX){return dX+dPeriod;});
	rModel.assignEventTimes(uEventTimes);

	int iTime(uEventTimes.size() -1);
	Slice uOption = rModel.cash(iTime, 0.0), uCap, uFloor;
	Slice uPaymentTime;

	while(iTime > 0)
	{
		uPaymentTime = 1./rModel.discount(iTime, rModel.eventTimes()[iTime] + dPeriod) - 1.;
		Slice uDiscount = rModel.discount(iTime, rModel.eventTimes()[iTime] + dPeriod);
		uPaymentTime = 1./uDiscount - 1.;
		uOption += uDiscount * max(uPaymentTime - rCap.rate*dPeriod, 0.0);
		uOption -= uDiscount * max(dFloorRate*dPeriod - uPaymentTime, 0.0);

		iTime--;
		uOption.rollback(iTime);
	}

	uOption *= rCap.notional;
	return interpolate(uOption);
  }

 /*
 cfl::Slice liborRate(unsigned iTime, double dPeriod, cfl::InterestRateModel & rModel)
 {
	 return (1./rModel.discount(iTime, rModel.eventTimes()[iTime] + dPeriod) - 1.)/dPeriod;
 }
 
 cfl::MultiFunction prb::collar(const cfl::Data::CashFlow & rCap, 
			    double dFloorRate, 
			    cfl::InterestRateModel & rModel)
 {
	 PRECONDITION(rCap.rate > dFloorRate);
	 double dPeriod(rCap.period);
	 std::vector<double> uEventTimes(rCap.numberOfPayments);
	 uEventTimes.front() = rModel.initialTime();
	 std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1,
		 [dPeriod](double dX){return dX+dPeriod;});

	 unsigned iTime(uEventTimes.size() - 1);
	 Slice uOption = rModel.cash(iTime, 0.0);
	 Slice uPayment, uInd, uTemp;

	 while(iTime > 0)
	 {
		uTemp = liborRate(iTime, dPeriod, rModel);

		// Because F < C only one of them will be non zero
		uPayment = max(uTemp - rCap.rate, 0.0);
		uPayment -= max(dFloorRate - uTemp, 0.0);
		uOption += uPayment * rModel.discount(iTime, uEventTimes[iTime] + dPeriod);
		iTime--;
		uOption.rollback(iTime);
	 }

	 uOption *= (rCap.notional * dPeriod);

	 return interpolate(uOption);
 }
 */