#include "Session2/Session2.hpp"

using namespace cfl;

cfl::MultiFunction 
  prb::callOnForwardPrice(double dStrike, double dCallMaturity, 
		     double dForwardMaturity,
		     cfl::AssetModel & rModel)
{
	PRECONDITION(rModel.initialTime() < dCallMaturity);
	PRECONDITION(dCallMaturity < dForwardMaturity);


	std::vector<double> uEventTimes(1, rModel.initialTime());
	uEventTimes.push_back(dCallMaturity);
	rModel.assignEventTimes(uEventTimes);

	int iEventTime(1);
	Slice uOption(max(rModel.forward(iEventTime, dForwardMaturity) - dStrike, 0.0));
	uOption.rollback(0);
	return interpolate(uOption);
}
/*
cfl::MultiFunction 
  prb::callOnForwardPrice(double dStrike, double dCallMaturity, 
		     double dForwardMaturity,
		     cfl::AssetModel & rModel)
{
	PRECONDITION(dCallMaturity < dForwardMaturity);
	PRECONDITION(rModel.initialTime() < dCallMaturity);

	std::vector<double> uEventTimes(1, rModel.initialTime());
	uEventTimes.push_back(dCallMaturity);
	//uEventTimes.push_back(dForwardMaturity);
	rModel.assignEventTimes(uEventTimes);

	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption =  max(rModel.forward(iTime, dForwardMaturity) - dStrike, 0.0);
	iTime--;
	uOption.rollback(iTime);
	ASSERT(iTime == 0);
	return interpolate(uOption);
}*/