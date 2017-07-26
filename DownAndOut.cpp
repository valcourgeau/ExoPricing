#include "Session2/Session2.hpp"

using namespace cfl;

cfl::MultiFunction prb::downAndOutCall(double dBarrier, 
				    const std::vector<double> & rBarrierTimes,
				    double dStrike, double dMaturity, 
				    cfl::AssetModel & rModel)
{
	//PRECONDITION(rBarrierTimes.front() > rModel.initialTime());
	//PRECONDITION(rBarrierTimes.back() < dMaturity);
	//PRECONDITION(std::equal(rBarrierTimes.begin()+1, rBarrierTimes.end(),
	//	rBarrierTimes.begin(), std::greater<double>()));

	std::vector<double> uTimes(1, rModel.initialTime());
	uTimes.insert(uTimes.end(), rBarrierTimes.begin(), rBarrierTimes.end());
	uTimes.insert(uTimes.end(), dMaturity);
	rModel.assignEventTimes(uTimes);

	int iRemaining(uTimes.size() - 1);
	Slice uOption = max(rModel.spot(iRemaining) - dStrike, 0.0);
	iRemaining--;
	uOption.rollback(iRemaining);
	while(iRemaining > 0)
	{
		uOption *= indicator(rModel.spot(iRemaining), dBarrier);
		iRemaining--;
		uOption.rollback(iRemaining);
	}

	return interpolate(uOption);
}

/*
cfl::MultiFunction prb::downAndOutCall(double dBarrier, 
				    const std::vector<double> & rBarrierTimes,
				    double dStrike, double dMaturity, 
				    cfl::AssetModel & rModel)
{
	PRECONDITION(rBarrierTimes.front() > rModel.initialTime());
	PRECONDITION(dMaturity > rBarrierTimes.back());

	std::vector<double> uEventTimes(rBarrierTimes);
	uEventTimes.insert(uEventTimes.begin(), rModel.initialTime());
	uEventTimes.insert(uEventTimes.end(), dMaturity);
	rModel.assignEventTimes(uEventTimes);

	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = max(rModel.spot(iTime) - dStrike, 0.0);
	iTime--;
	uOption.rollback(iTime);

	while(iTime > 0)
	{
		uOption *= indicator(rModel.spot(iTime), dBarrier);
		iTime--;
		uOption.rollback(iTime);
	}

	return interpolate(uOption);
}*/