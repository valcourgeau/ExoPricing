#include "Homework2/Homework2.hpp"

using namespace cfl;

cfl::MultiFunction 
  prb::upAndInAmericanPut(double dBarrier, 
		     const std::vector<double> & rBarrierTimes, 
		     double dStrike, 
		     const std::vector<double> & rExerciseTimes, 
		     cfl::AssetModel & rModel)
{
	PRECONDITION(rExerciseTimes.front() > rModel.initialTime());
	PRECONDITION(rExerciseTimes.front() > rBarrierTimes.front());
	PRECONDITION(rBarrierTimes.back() < rExerciseTimes.back());

	std::vector<double> uEventTimes(1 + rBarrierTimes.size() + rExerciseTimes.size());
	uEventTimes.front() = rModel.initialTime();
	std::vector<double>::iterator itEnd = std::set_union(rBarrierTimes.begin(),
		rBarrierTimes.end(), rExerciseTimes.begin(), rExerciseTimes.end(),
		uEventTimes.begin() + 1);
	
	rModel.assignEventTimes(uEventTimes);

	int iIndex(uEventTimes.size()-1);
	Slice uSpot;
	Slice uPut = rModel.cash(iIndex, 0.0);
	Slice uOption = rModel.cash(iIndex, 0.0);
	while(iIndex > 0)
	{
		uSpot = rModel.spot(iIndex);
		double dTime = uEventTimes[iIndex];
		if(std::binary_search(rExerciseTimes.begin(), rExerciseTimes.end(), dTime))
		{
			uPut = max(uPut, dStrike - uSpot);
		}
		if(std::binary_search(rBarrierTimes.begin(), rBarrierTimes.end(), dTime))
		{
			uOption += indicator(uSpot, dBarrier) * (uPut - uOption);
		}
		
		iIndex--;
		uPut.rollback(iIndex);
		uOption.rollback(iIndex);
	}

	return interpolate(uOption);
}


// UP AND IN AMERICAN CALL
/*
cfl::MultiFunction 
  prb::upAndInAmericanPut(double dBarrier, 
		     const std::vector<double> & rBarrierTimes, 
		     double dStrike, 
		     const std::vector<double> & rExerciseTimes, 
		     cfl::AssetModel & rModel)
{
	PRECONDITION(rBarrierTimes.front() < rExerciseTimes.front());
	PRECONDITION(rBarrierTimes.back() < rExerciseTimes.back());
	PRECONDITION(rModel.initialTime() < rBarrierTimes.front());

	std::vector<double> uEventTimes(1 + rExerciseTimes.size() + rBarrierTimes.size());
	uEventTimes.front() = rModel.initialTime();
	std::vector<double>::iterator itEnd 
		= std::set_union(rExerciseTimes.begin(), rExerciseTimes.end(),
		rBarrierTimes.begin(), rBarrierTimes.end(), uEventTimes.begin()+1);
	uEventTimes.resize(itEnd - uEventTimes.begin());

	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = rModel.cash(iTime, 0.0);
	Slice uCall = rModel.cash(iTime, 0.0);
	Slice uSpot;
	double dTime;
	while(iTime > 0)
	{
		dTime = uEventTimes[iTime];
		uSpot = rModel.spot(iTime);
		if(std::binary_search(rExerciseTimes.begin(), rExerciseTimes.end(), dTime))
		{
			uCall = max(uCall, uSpot - dStrike);
		}
		if(std::binary_search(rBarrierTimes.begin(), rBarrierTimes.end(), dTime))
		{
			uCall += indicator(uSpot, dBarrier) * (uCall - uOption);
		}
		iTime--;
		uCall.rollback(iTime);
		uOption.rollback(iTime);
	}

	return interpolate(uOption);
}*/