#include "Session3/Session3.hpp"

using namespace cfl;
 
cfl::Slice swapRate(unsigned iTime, double dPeriod, unsigned iPeriods, const cfl::InterestRateModel & rModel)
{
	Slice uFixed = rModel.cash(iTime, 0.0);
	double dTime = rModel.eventTimes()[iTime];

	for(unsigned iI=0; iI < iPeriods;iI++)
	{
		dTime += dPeriod;
		uFixed += rModel.discount(iTime, dTime);
	}
	uFixed *= dPeriod;
	Slice uFloat = 1.0 - rModel.discount(iTime, dTime);
	Slice uRate = (uFloat / uFixed);

	return(uRate);
}


cfl::MultiFunction prb::capOnSwapRate(const cfl::Data::CashFlow & rCap, 
				   double dSwapPeriod, unsigned iSwapPayments, 
				   cfl::InterestRateModel & rModel)
 {
	 double dDeltaT(rCap.period);
	 std::vector<double> uEventTimes(rCap.numberOfPayments);
	 uEventTimes.front() = rModel.initialTime();
	 std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1,
		 [dDeltaT](double dX){return dX +dDeltaT;});
	 rModel.assignEventTimes(uEventTimes);

	 int iTime(uEventTimes.size() -1);

	 Slice uDiscount = rModel.discount(iTime, rModel.eventTimes()[iTime] + dDeltaT);
	 Slice uOption = uDiscount * max(swapRate(iTime, dSwapPeriod, iSwapPayments, rModel)-rCap.rate, 0.0);
	 while(iTime > 0)
	 {
		 iTime--;
		 uOption.rollback(iTime);
		 uDiscount = rModel.discount(iTime, rModel.eventTimes()[iTime] + dDeltaT);
		 uOption += uDiscount * max(swapRate(iTime, dSwapPeriod, iSwapPayments, rModel)-rCap.rate, 0.0);
		 
	 }
	 uOption *= (rCap.notional * dDeltaT);
	 return interpolate(uOption);
 }

/*
cfl::Slice swapRate(unsigned iTime, double dSwapPeriod, unsigned iSwapPayments, 
				   cfl::InterestRateModel & rModel)
{
	Slice dSwapRate = rModel.cash(iTime, 0.0);
	double dTime(rModel.eventTimes()[iTime]);
	for(unsigned iI = 0; iI < iSwapPayments; iI++)
	{
		dTime += dSwapPeriod;
		dSwapRate += rModel.discount(iTime, dTime);		
	}

	dSwapRate *= dSwapPeriod;
	ASSERT(std::abs(rModel.eventTimes()[iTime] + dSwapPeriod * iSwapPayments - dTime) < c_dEps);
	return (1. - rModel.discount(iTime, dTime)) / dSwapRate;
}

cfl::MultiFunction prb::capOnSwapRate(const cfl::Data::CashFlow & rCap, 
				   double dSwapPeriod, unsigned iSwapPayments, 
				   cfl::InterestRateModel & rModel)
{
	double dPeriod(rCap.period);
	std::vector<double> uEventTimes(rCap.numberOfPayments);
	uEventTimes.front() = rModel.initialTime();
	std::transform(uEventTimes.begin(), uEventTimes.end() -1, uEventTimes.begin()+1,
		[dPeriod](double dX){return dX + dPeriod;});
	rModel.assignEventTimes(uEventTimes);

	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = rModel.cash(iTime, 0.0);
	Slice uPayment = max(swapRate(iTime, dSwapPeriod, iSwapPayments, rModel) - rCap.rate, 0.0);
	uPayment *= rCap.period;
    uOption += uPayment;
	iTime--;
	uOption.rollback(iTime);

	while(iTime > 0)
	{
		uPayment = max(swapRate(iTime, dSwapPeriod, iSwapPayments, rModel) - rCap.rate, 0.0);
		uPayment *= rCap.period;
		uOption += uPayment * rModel.discount(iTime, uEventTimes[iTime] + dPeriod);
		iTime--;
		uOption.rollback(iTime);
	}

	uOption *= rCap.notional;

	return interpolate(uOption);
}*/