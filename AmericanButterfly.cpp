#include "Session2/Session2.hpp"

using namespace cfl;

cfl::MultiFunction 
  prb::americanButterfly(double dPutStrike, double dCallStrike, 
		    const std::vector<double> & rExerciseTimes,
		    cfl::AssetModel & rModel)
{
	PRECONDITION(dPutStrike, dCallStrike);
	PRECONDITION(rExerciseTimes.front() >= rModel.initialTime());
	std::vector<double> uTimes(1, rModel.initialTime());
	uTimes.insert(uTimes.end(), rExerciseTimes.begin(), rExerciseTimes.end());
	rModel.assignEventTimes(uTimes);

	int iTimes(uTimes.size() - 1);
	Slice uOption = rModel.cash(iTimes, 0.0);

	while(iTimes > 0)
	{
		Slice uSpot(rModel.spot(iTimes));
		uOption = max(max(uSpot-dCallStrike,dPutStrike - uSpot), uOption);
		iTimes--;
		uOption.rollback(iTimes);
	}

	return interpolate(uOption);
	
}
/*
cfl::MultiFunction 
  prb::americanButterfly(double dPutStrike, double dCallStrike, 
		    const std::vector<double> & rExerciseTimes,
		    cfl::AssetModel & rModel)
{
	PRECONDITION(rModel.initialTime() < rExerciseTimes.front());
	PRECONDITION(dCallStrike > dPutStrike);
	PRECONDITION(std::equal(rExerciseTimes.begin(), rExerciseTimes.end()-1,
		rExerciseTimes.begin()+1, std::multiplies<double>()));

	std::vector<double> uEventTimes(rExerciseTimes);
	uEventTimes.insert(uEventTimes.begin(), rModel.initialTime());
	rModel.assignEventTimes(uEventTimes);

    unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = rModel.cash(iTime, 0.0);

	while(iTime > 0)
	{
		uOption += max(max(rModel.spot(iTime) - dCallStrike, dPutStrike - rModel.spot(iTime)), 0.0);
		iTime--;
		uOption.rollback(iTime);
	}

	return interpolate(uOption);
}*/