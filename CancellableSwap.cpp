#include "Session3/Session3.hpp"

using namespace cfl;
 

cfl::Slice floatInterest(unsigned iTime, double dPeriod, cfl::InterestRateModel & rModel)
{
	double dPaymentTime(rModel.eventTimes()[iTime] + dPeriod);
	return 1./rModel.discount(iTime, dPaymentTime) - 1.;
}

cfl::MultiFunction prb::cancelSwapArrears(const cfl::Data::Swap & rSwap,
				       cfl::InterestRateModel & rModel)
{
	double dPeriod(rSwap.period);
	std::vector<double> uEventTimes(rSwap.numberOfPayments + 1);
	uEventTimes.front() = rModel.initialTime();
	std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1,
		[dPeriod](double dX){return dX+dPeriod;});
	rModel.assignEventTimes(uEventTimes);
	int iTime(uEventTimes.size() - 1);
	double dFixed(rSwap.rate * rSwap.period);
	Slice uOption = rModel.cash(iTime, 0.0);

	while(iTime > 0)
	{
		uOption = max(uOption, 0.0);
		Slice uPayment = floatInterest(iTime, dPeriod, rModel) - dFixed;
		if(rSwap.payFloat)
		{
			uPayment *= -1.;
		}
		uOption += uPayment;
		iTime--;
		uOption.rollback(iTime);
	}

	uOption *= rSwap.notional;
	return interpolate(uOption);
}
/*
cfl::Slice liborRate(unsigned iTime, double dPeriod, cfl::InterestRateModel & rModel)
{
	return (1./ rModel.discount(iTime, rModel.eventTimes()[iTime] + dPeriod) - 1.) /dPeriod;
}

cfl::MultiFunction prb::cancelSwapArrears(const cfl::Data::Swap & rSwap,
				       cfl::InterestRateModel & rModel)
{
	double dPeriod(rSwap.period);
	std::vector<double> uEventTimes(rSwap.numberOfPayments);
	uEventTimes.front() = rModel.initialTime();
	std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1,
		[dPeriod](double dX){return dX + dPeriod;});
	
	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = rModel.cash(iTime, 0.0);
	Slice uPayment;
	while(iTime > 0)
	{
		uOption = max(uOption, 0.0);

		uPayment = liborRate(iTime, dPeriod, rModel) - rSwap.rate;
		uPayment *= rSwap.period;
		uOption += uPayment;
		if(rSwap.payFloat)
		{
			uOption *= 1.;
		}
		iTime--;
		uOption.rollback(iTime);
	}

	
	uOption *= rSwap.notional;

	return interpolate(uOption);
}*/