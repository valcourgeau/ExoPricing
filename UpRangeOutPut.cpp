#include "SampleExam1/SampleExam1.hpp"

using namespace cfl;

cfl::MultiFunction prb::upRangeOutPut(double dUpperBarrier, unsigned iOutTimes, 
				   const std::vector<double> & rBarrierTimes,
				   double dStrike, double dMaturity, 
				   cfl::AssetModel & rModel)
{
	PRECONDITION(rModel.initialTime() < rBarrierTimes.front());
	PRECONDITION(rBarrierTimes.back() < dMaturity);
	PRECONDITION(iOutTimes < rBarrierTimes.size());

	std::vector<double> uEventTimes(1, rModel.initialTime());
	uEventTimes.insert(uEventTimes.begin()+1, rBarrierTimes.begin(), rBarrierTimes.end());
	uEventTimes.insert(uEventTimes.end(), dMaturity);
	rModel.assignEventTimes(uEventTimes);

	unsigned iTime(uEventTimes.size() - 1);

	Slice uPut(max(dStrike - rModel.spot(iTime), 0.0));
	std::vector<Slice> uOption(iOutTimes, uPut);	

	// Go backward from maturity:
	iTime--;
	for(unsigned iI =0; iI < uOption.size(); iI++)
	{
		uOption[iI].rollback(iTime);
	}
	
	while(iTime > 0)
	{
		Slice uInd = indicator(rModel.spot(iTime), dUpperBarrier);
		for(unsigned iI =0; iI < (uOption.size()-1); iI++)
		{
			uOption[iI] += uInd * (uOption[iI+1] - uOption[iI]);
		}

		uOption.back() *= (1-uInd);

		iTime--;
		for(unsigned iI =0; iI < uOption.size(); iI++)
		{
			uOption[iI].rollback(iTime);
		}
	}

	return interpolate(uOption.front());
}


/*
cfl::MultiFunction prb::upRangeOutPut(double dUpperBarrier, unsigned iOutTimes, 
				   const std::vector<double> & rBarrierTimes,
				   double dStrike, double dMaturity, 
				   cfl::AssetModel & rModel)
{
	PRECONDITION(rModel.initialTime() < rBarrierTimes.front());
	PRECONDITION(rBarrierTimes.back() < dMaturity);
	std::vector<double> uEventTimes(rBarrierTimes);
	uEventTimes.insert(uEventTimes.begin(), rModel.initialTime());
	uEventTimes.push_back(dMaturity);
	rModel.assignEventTimes(uEventTimes);

	unsigned iTime(uEventTimes.size() - 1);
	Slice uPut = max(dStrike - rModel.spot(iTime), 0.0);
	std::vector<Slice> uOption(iOutTimes, uPut);
	iTime--;
	for(unsigned iI = 0; iI < (iOutTimes-1);iI++)
	{
		uOption[iI].rollback(iTime);
	}

	Slice uInd = indicator(rModel.spot(iTime), dUpperBarrier);
	while(iTime > 0)
	{
		for(unsigned iI = 0; iI < (iOutTimes-1);iI++)
		{
			uOption[iI] += uInd * (uOption[iI+1] - uOption[iI]);
		}
		// Final one gets cancelled if above barrier
		uOption.back() *= (1. - uInd);
		iTime--;
		for(unsigned iI = 0; iI < (iOutTimes-1);iI++)
		{
			uOption[iI].rollback(iTime);
		}
	}

	return interpolate(uOption.front());
}*/