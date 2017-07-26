#include "SampleExam1/SampleExam1.hpp"
#include <assert.h>

using namespace cfl;  

cfl::Slice 
couponBond(unsigned iTime, const cfl::Data::CashFlow & rBond, 
	   const InterestRateModel & rModel) 
{
  Slice uCashFlow = rModel.cash(iTime, 0.);
  double dTime = rModel.eventTimes()[iTime];
  for (unsigned iI=0; iI<rBond.numberOfPayments; iI++) {
    dTime += rBond.period;
    uCashFlow += rModel.discount(iTime, dTime);
  }
  uCashFlow *= (rBond.rate*rBond.period);
  uCashFlow += rModel.discount(iTime, dTime);
  uCashFlow *= rBond.notional;
  return Slice();
}


cfl::MultiFunction 
  prb::futureOnCheapToDeliver(double dFutureMaturity, 
			 unsigned iFutureTimes, 
			 const std::vector<cfl::Data::CashFlow> & rBonds,
			 cfl::InterestRateModel & rModel)
  {
	  PRECONDITION(rModel.initialTime() < dFutureMaturity);
	  std::vector<double> uEventTimes(iFutureTimes + 1);
	  uEventTimes.front() = rModel.initialTime();
	  
	  double dPeriod((dFutureMaturity - rModel.initialTime()) / (double)iFutureTimes);
	  std::transform(uEventTimes.begin(), uEventTimes.end()-2, uEventTimes.begin()+1,
		  [dPeriod](double dX){return dX + dPeriod;});
	  rModel.assignEventTimes(uEventTimes);
	  ASSERT(std::abs(dFutureMaturity - uEventTimes.back()) < c_dEps);
	  
	 
	  
	  unsigned iTime(uEventTimes.size() - 1);
	  Slice uOption = rModel.cash(iTime, std::numeric_limits<double>::max());
	  
	  for(cfl::Data::CashFlow bond : rBonds)
	  {
		  //uEventTimes.back() = dFutureMaturity + bond.period * bond.numberOfPayments;
		  
		  uOption = min(uOption, couponBond(iTime, bond, rModel));
	  }
	  uOption = max(uOption, 0.0);
	  iTime--;
	  uOption.rollback(iTime);

	  while(iTime > 0)
	  {
		  uOption /= rModel.discount(iTime, rModel.eventTimes()[iTime] + dPeriod);
		  iTime--;
		  uOption.rollback(iTime);
	  }
	  
	  
	  return MultiFunction();
  }

/*
cfl::MultiFunction 
  prb::futureOnCheapToDeliver(double dFutureMaturity, 
			 unsigned iFutureTimes, 
			 const std::vector<cfl::Data::CashFlow> & rBonds,
			 cfl::InterestRateModel & rModel)
{
	PRECONDITION(rModel.initialTime() < dFutureMaturity);
	double dPeriod((dFutureMaturity - rModel.initialTime())/(double)iFutureTimes);
	std::vector<double> uEventTimes(iFutureTimes + 1);
	uEventTimes.front() = rModel.initialTime();
	std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1,
		[dPeriod](double dX){return dX + dPeriod;});
	rModel.assignEventTimes(uEventTimes);
	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = rModel.cash(iTime, std::numeric_limits<double>::max());
	for(cfl::Data::CashFlow bond : rBonds)
	{
		uOption = min(couponBond(iTime, bond, rModel), uOption);
	}
	//  discount * forward  = 1.

	while(iTime > 0)
	{
		iTime--;
		uOption.rollback(iTime);
		uOption /= rModel.discount(iTime, uEventTimes[iTime] + dPeriod);
	}

	return interpolate(uOption);
}*/