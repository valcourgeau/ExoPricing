#include "Session3/Session3.hpp"

using namespace cfl;

cfl::MultiFunction prb::floor(const cfl::Data::CashFlow & rFloor,
			   cfl::InterestRateModel & rModel)
{
	double dPeriod(rFloor.period), dFloor( 1 + rFloor.rate * rFloor.period), dPayment;

	std::vector<double> uEventTimes(rFloor.numberOfPayments);
	uEventTimes.front() = rModel.initialTime();
	std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1, [&rFloor](double dX){return dX + rFloor.period;});

	rModel.assignEventTimes(uEventTimes);
	dPayment = uEventTimes.back() + dPeriod;

	int iTime(uEventTimes.size() - 1);
	Slice uOption = rModel.cash(iTime, 0.0);
	Slice uDiscount;

	while(iTime >0)
	{
		dPayment = uEventTimes[iTime] + dPeriod;
		uDiscount = rModel.discount(iTime, dPayment);
		uOption += max(dFloor * uDiscount - 1.0, 0.);
		iTime--;
		uOption.rollback(iTime);
	}

	uOption *= rFloor.notional;

	return interpolate(uOption);
}


cfl::Slice liborRate(unsigned iTime, double dPeriod, cfl::InterestRateModel & rModel)
{
	return (1./rModel.discount(iTime, rModel.eventTimes()[iTime] + dPeriod) - 1.) / dPeriod;
}


cfl::MultiFunction prb::floor(const cfl::Data::CashFlow & rFloor,
			   cfl::InterestRateModel & rModel)
{
	double dInitialTime(rModel.initialTime()), dPeriod(rFloor.period);
	std::vector<double> uEventTimes(rFloor.numberOfPayments);
	uEventTimes.front() = rModel.initialTime();

	std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1,
		[dInitialTime](double dX){return dX+dInitialTime;});
	rModel.assignEventTimes(uEventTimes);

	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = rModel.cash(iTime, 0.0);
	Slice uPayment = max(rFloor.rate - liborRate(iTime, dPeriod, rModel), 0.0);
	uPayment *= dPeriod;
	uPayment *= rModel.discount(iTime, uEventTimes[iTime] + dPeriod);
	uOption += uPayment;

	while(iTime > 0)
	{	
		iTime--;
		uOption.rollback(iTime);
		uPayment = max(rFloor.rate - liborRate(iTime, dPeriod, rModel), 0.0);
		uPayment *= dPeriod;
		uPayment *= rModel.discount(iTime, uEventTimes[iTime] + dPeriod);
		uOption += uPayment;
	}
	
	uOption *= rFloor.notional;

	return interpolate(uOption);

}