#include "Homework2/Homework2.hpp"

using namespace cfl;


cfl::MultiFunction 
  prb::downAndRebate(double dLowerBarrier, double dNotional, 
		const std::vector<double> & rBarrierTimes,
		cfl::AssetModel & rModel)
{
	PRECONDITION(rBarrierTimes.front() > rModel.initialTime());
	std::vector<double> uEventTimes(1, rModel.initialTime());

	uEventTimes.insert(uEventTimes.end(), uEventTimes.begin(), uEventTimes.end());
	rModel.assignEventTimes(uEventTimes);

	int iIndex(uEventTimes.size() - 1);
	Slice uDown = rModel.cash(iIndex, 0.0);
	while(iIndex > 0)
	{
		uDown += (dNotional - uDown) *  indicator(dLowerBarrier, rModel.spot(iIndex));
		iIndex--;
		uDown.rollback(iIndex);
	}

	return interpolate(uDown);
}

cfl::MultiFunction 
  prb::downAndRebate(double dLowerBarrier, double dNotional, 
		const std::vector<double> & rBarrierTimes,
		cfl::AssetModel & rModel)
{
	PRECONDITION(rModel.initialTime() < rBarrierTimes.front());
	std::vector<double> uEventTimes(rBarrierTimes);
	uEventTimes.insert(uEventTimes.begin(), rModel.initialTime());
	rModel.assignEventTimes(uEventTimes);

	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = rModel.cash(iTime, 0.0);

	while(iTime > 0)
	{
		uOption += indicator(rModel.spot(iTime), dLowerBarrier) * (1. - uOption);
		iTime--;
		uOption.rollback(iTime);
	}

	return interpolate(uOption);
}