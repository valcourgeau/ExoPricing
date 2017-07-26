#include "SampleExam2/SampleExam2.hpp"

using namespace cfl;

  cfl::MultiFunction prb::boost(double dNotional, double dLowerBarrier, 
			   double dUpperBarrier,  
			   const std::vector<double> & rBarrierTimes,
			   cfl::AssetModel & rModel)
  {
	  PRECONDITION(rModel.initialTime() < rBarrierTimes.front());
	  std::vector<double> uEventTimes(1, rModel.initialTime());
	  uEventTimes.insert(uEventTimes.end(), rBarrierTimes.begin(), rBarrierTimes.end());
	  rModel.assignEventTimes(uEventTimes);

	  unsigned iTime(uEventTimes.size() - 1);
	  Slice uOption = rModel.cash(iTime, 1.0), uInd;
	  iTime--;
	  uOption.rollback(iTime);

	  while(iTime > 0)
	  {
		  // uInd is 1 when we are out of the bounds
		  uInd = (1 -indicator(dUpperBarrier, rModel.spot(iTime)) * indicator(rModel.spot(iTime),dLowerBarrier));
		  
		  // in this case we replace uOption by an indicator at this timestep (times percentage)
		  uOption =  uInd * (1.0 - uOption)
			  * (double)(iTime-1) / ((double)rBarrierTimes.size());
		  // No discount since we receive the cash at this particular timestep if necessary
		  // 1 - uInd1 * uInd2 computes the payoff with sharp inequalities
		  iTime--;
		  uOption.rollback(iTime);
	  }
	
	 uOption *= dNotional;
	  return interpolate(uOption);
  }
  /*
    cfl::MultiFunction prb::boost(double dNotional, double dLowerBarrier, 
			   double dUpperBarrier,  
			   const std::vector<double> & rBarrierTimes,
			   cfl::AssetModel & rModel)
  {
	  PRECONDITION(rModel.initialTime() < rBarrierTimes.front());
	  PRECONDITION(dLowerBarrier < dUpperBarrier);
	  std::vector<double> uEventTimes(rBarrierTimes);
	  uEventTimes.insert(uEventTimes.begin(),  rModel.initialTime());
	  rModel.assignEventTimes(uEventTimes);

	  unsigned iTime(uEventTimes.size() - 1);
	  Slice uOption = rModel.cash(iTime, 1.0);
	  Slice uInd;
	  iTime--;
	  uOption.rollback(iTime);

	  while(iTime > 0)
	  {
		  //uDiscount = rModel.discount(iTime, uEventTimes.back());
		  uInd = indicator(rModel.spot(iTime), dLowerBarrier) * indicator(rModel.spot(iTime), dUpperBarrier);
		  // No need to discount because we receive the money at the first opportunity
		  uOption += (1. - uInd) * (((double)iTime-1)/(double)rBarrierTimes.size() - uOption); // Replace uOption!
		  iTime--;
		  uOption.rollback(iTime);
	  }

	  uOption *= dNotional;

	  return interpolate(uOption);
	}*/