#include "Homework3/Homework3.hpp"

using namespace cfl;

cfl::MultiFunction prb::americanPutOnFuture(double dNotional, 
					 double dBondMaturity, 
					 double dFutureMaturity, 
					 unsigned iFutureTimes, 
					 double dStrike,
					 cfl::InterestRateModel & rModel)
{
	/*PRECONDITION(rModel.initialTime() < dFutureMaturity);
	PRECONDITION(dFutureMaturity < dBondMaturity);
	double dDeltaT = (dFutureMaturity - rModel.initialTime()) / (double) iFutureTimes;
	std::vector<double> uEventTimes(iFutureTimes + 1);
	
	uEventTimes.front() = rModel.initialTime();
	std::transform(uEventTimes.begin(), uEventTimes.end()-1,
		uEventTimes.begin()+1, [dDeltaT](double dX){return dX + dDeltaT;});
	//uEventTimes.back() = dFutureMaturity;
	//ASSERT(std::abs(uEventTimes.back() - dFutureMaturity < c_dEps);
	rModel.assignEventTimes(uEventTimes);

	int iTime(uEventTimes.size() - 1);
	Slice uOption = rModel.cash(iTime, 0.0);
	Slice uFuture = dNotional * rModel.discount(iTime, dBondMaturity); 

	while(iTime > 0)
	{
		uOption = max(uOption, dStrike - uFuture);
		iTime--;
		uFuture /= rModel.discount(iTime, uEventTimes[iTime] + dDeltaT);
		uOption.rollback(iTime);
		uFuture.rollback(iTime);
	}

	*/
	return MultiFunction();
}

cfl::MultiFunction prb::americanPutOnFuture(double dNotional, 
					 double dBondMaturity, 
					 double dFutureMaturity, 
					 unsigned iFutureTimes, 
					 double dStrike,
					 cfl::InterestRateModel & rModel)
{
	PRECONDITION(dFutureMaturity < dBondMaturity);
	PRECONDITION(rModel.initialTime() < dFutureMaturity);
	double dPeriod((dFutureMaturity - rModel.initialTime()) / (double)iFutureTimes);
	std::vector<double> uEventTimes(iFutureTimes + 1);
	uEventTimes.front() = rModel.initialTime();
	std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1,
		[dPeriod](double dX){return dX + dPeriod;});
	rModel.assignEventTimes(uEventTimes);

	ASSERT(std::abs(dFutureMaturity - uEventTimes.back()) < c_dEps);

	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = rModel.cash(iTime, 0.0);
	Slice uFuture = rModel.discount(iTime, dBondMaturity);
	while(iTime > 0)
	{
		uOption = max(uOption, dStrike - uFuture);
		iTime--;
		uOption.rollback(iTime);
		uFuture /= rModel.discount(iTime, uEventTimes[iTime] + dPeriod);
	}

	uOption *= dNotional;

	return interpolate(uOption);

}