#include "Homework3/Homework3.hpp"

using namespace cfl;


cfl::Slice 
couponBondTeacher(unsigned iTime, const Data::CashFlow & rBond, 
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
  return uCashFlow;
}
	
cfl::Slice 
swapTeacher(unsigned iTime, const Data::Swap & rSwap, 
     const InterestRateModel & rModel) 
{
  //assume first that we receive fixed and pay float
  Slice uSwap = couponBondTeacher(iTime, rSwap, rModel)-rSwap.notional;
  if (!rSwap.payFloat) { //if we pay fixed
    uSwap *= -1;
  }
  return uSwap;
}


cfl::Slice swap(unsigned iTime, const cfl::Data::Swap & rSwap, 
		   cfl::InterestRateModel & rModel)
{
	Slice uCash = rModel.cash(iTime, 0.0);
	double dPaymentTime(rModel.eventTimes()[iTime]);
	for(unsigned iI =0; iI < rSwap.numberOfPayments; iI++)
	{
		dPaymentTime += rSwap.period;
		uCash += rModel.discount(iTime, dPaymentTime);
	}

	uCash *= (rSwap.period * rSwap.rate);
	uCash += rModel.discount(iTime, dPaymentTime);
	uCash *= rSwap.notional;

	uCash -= rSwap.notional;
	if(!rSwap.payFloat)
	{
		uCash *= -1.0;
	}

	return uCash;
}

cfl::MultiFunction 
  prb::americanSwaption(const cfl::Data::Swap & rSwap, 
		   const std::vector<double> & rExerciseTimes, 
		   cfl::InterestRateModel & rModel)
  {
	  PRECONDITION(rModel.initialTime() < rExerciseTimes.front());

	  double dPeriod(rSwap.period), dRate(rSwap.rate);
	  std::vector<double> uEventTimes(1, rModel.initialTime());
	  uEventTimes.insert(uEventTimes.end(), rExerciseTimes.begin(), rExerciseTimes.end());
	  rModel.assignEventTimes(uEventTimes);
	  int iTime(uEventTimes.size() - 1);

	  double dPaymentTime(uEventTimes.back() + dPeriod);
	  Slice uOption = rModel.cash(iTime, 0.0);

	  while(iTime > 0)
	  {
		  uOption = max(uOption, swap(iTime, rSwap, rModel));
		  iTime--;
		  uOption.rollback(iTime);
	  }

	  return interpolate(uOption);
  }


/*
cfl::MultiFunction 
  prb::americanSwaption(const cfl::Data::Swap & rSwap, 
		   const std::vector<double> & rExerciseTimes, 
		   cfl::InterestRateModel & rModel)
{
	PRECONDITION(rModel.initialTime() < rExerciseTimes.front());
	std::vector<double> uEventTimes(rExerciseTimes);
	uEventTimes.insert(uEventTimes.begin(), rModel.initialTime());

	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = rModel.cash(iTime, 0.0);

	while(iTime > 0)
	{
		uOption = max(uOption, swap(iTime, rSwap, rModel));
		iTime--;
		uOption.rollback(iTime);
	}

	return interpolate(uOption);
}*/