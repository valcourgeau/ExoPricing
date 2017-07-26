#include "Session2/Session2.hpp"

using namespace cfl;


cfl::MultiFunction prb::corridor(double dNotional, double dLowerBarrier, 
			      double dUpperBarrier,  
			      const std::vector<double> & rBarrierTimes,
			      cfl::AssetModel & rModel)
{
	PRECONDITION(rBarrierTimes.front() >= rModel.initialTime());

	std::vector<double> uEventTimes(1, rModel.initialTime());
	double uMaturity(rBarrierTimes.back());
	uEventTimes.insert(uEventTimes.end(), rBarrierTimes.begin(), rBarrierTimes.end());
	int iTime(uEventTimes.size()-1);
	rModel.assignEventTimes(uEventTimes);
	Slice uOption = rModel.cash(iTime, 0.0);
	
	while(iTime > 0)
	{
		uOption += (indicator(rModel.spot(iTime), dLowerBarrier) * 
			indicator(dUpperBarrier, rModel.spot(iTime))) *
			rModel.discount(iTime, uMaturity);
		iTime--;
		uOption.rollback(iTime);
	}
	uOption *= (dNotional / (double)rBarrierTimes.size());

	return interpolate(uOption);
}
/*
cfl::MultiFunction prb::corridor(double dNotional, double dLowerBarrier, 
			      double dUpperBarrier,  
			      const std::vector<double> & rBarrierTimes,
			      cfl::AssetModel & rModel)
{
	PRECONDITION(rModel.initialTime() < rBarrierTimes.front());
	std::vector<double> uEventTimes(rBarrierTimes);
	uEventTimes.insert(uEventTimes.begin(), rModel.initialTime());
	rModel.assignEventTimes(uEventTimes);

	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = rModel.cash(iTime, 0.0);
	Slice uInd;
	while(iTime > 0)
	{
		uInd = (indicator(rModel.spot(iTime), dLowerBarrier) 
		* indicator(dUpperBarrier, rModel.spot(iTime)));
		uInd *= rModel.discount(iTime, uEventTimes.back());
	    uOption += uInd;
		iTime--;
		uOption.rollback(iTime);
	}

	uOption /= (double)rBarrierTimes.size();
	uOption *= dNotional;
	return interpolate(uOption);
}*/