#include "Session3/Session3.hpp"

using namespace cfl;


cfl::MultiFunction prb::putOnFRA(double dFixedRate, double dPeriod, 
			      double dNotional, double dMaturity,
			      cfl::InterestRateModel & rModel)
{
	std::vector<double> uEventTimes(1, rModel.initialTime());
	uEventTimes.push_back(dMaturity);

	rModel.assignEventTimes(uEventTimes);
	int iTime(uEventTimes.size() - 1);
	double dFixedPayment = 1.0 + dFixedRate * dPeriod;

	Slice uOption = max(1.0 - dFixedPayment * rModel.discount(iTime, dMaturity + dPeriod), 0.0);
	iTime--;
	uOption.rollback(iTime);
	uOption *= dNotional;
	return interpolate(uOption);
}
/*
cfl::MultiFunction prb::putOnFRA(double dFixedRate, double dPeriod, 
			      double dNotional, double dMaturity,
			      cfl::InterestRateModel & rModel)
{
	PRECONDITION(rModel.initialTime() < dMaturity);
	std::vector<double> uEventTimes(1, rModel.initialTime());
	uEventTimes.push_back(dMaturity);
	//uEventTimes.push_back(dMaturity + dPeriod);
	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = max(1./rModel.discount(iTime, dMaturity + dPeriod) 
		- (1. + dFixedRate * dPeriod), 0.0);
	uOption *= rModel.discount(iTime, dMaturity + dPeriod);
	// Could have implementd both at oncem but this is easier to see what the steps are.
	iTime--;
	uOption.rollback(iTime);

	return interpolate(uOption);
}*/