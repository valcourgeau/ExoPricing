#include "SampleExam3/SampleExam3.hpp"

using namespace cfl;

cfl::MultiFunction  
  prb::compoundCallAmericanPut(double dMaturity,
			  double dCompoundCallStrike, 
			  double dPutStrike, 
			  const std::vector<double> & rExerciseTimes, 
			  cfl::AssetModel & rModel)
  {
	  PRECONDITION(rModel.initialTime() < dMaturity);
	  PRECONDITION(dMaturity < rExerciseTimes.front());
	  std::vector<double> uEventTimes(1, rModel.initialTime());
	  uEventTimes.push_back(dMaturity);
	  uEventTimes.insert(uEventTimes.end(), rExerciseTimes.begin(), rExerciseTimes.end());
	  rModel.assignEventTimes(uEventTimes);

	  unsigned iTime(uEventTimes.size() - 1);
	  Slice uPut(max(dPutStrike - rModel.spot(iTime), 0.0));
	  iTime--;
	  uPut.rollback(iTime);

	  while(iTime > 1)
	  {
		  uPut = max(uPut, max(dPutStrike - rModel.spot(iTime), 0.0));
		  iTime--;
		  uPut.rollback(iTime);
	  }
	  uPut = max(uPut - dCompoundCallStrike, 0.0);
	  iTime--;
	  uPut.rollback(iTime);
	  return interpolate(uPut);
  }
/*
cfl::MultiFunction  
  prb::compoundCallAmericanPut(double dMaturity,
			  double dCompoundCallStrike, 
			  double dPutStrike, 
			  const std::vector<double> & rExerciseTimes, 
			  cfl::AssetModel & rModel)
{
	PRECONDITION(rModel.initialTime() < dMaturity);
	PRECONDITION(dMaturity < rExerciseTimes.front());
	std::vector<double> uEventTimes(rExerciseTimes);
	uEventTimes.insert(uEventTimes.begin(), dMaturity);
	uEventTimes.insert(uEventTimes.begin(), rModel.initialTime());
	rModel.assignEventTimes(uEventTimes);

	unsigned iTime(uEventTimes.size() - 1);
	Slice uPut = max(dPutStrike - rModel.spot(iTime), 0.0);
	iTime--;
	uPut.rollback(iTime);

	while(iTime > 1)
	{
		uPut = max(uPut, dPutStrike - rModel.spot(iTime));
		iTime--;
		uPut.rollback(iTime);
	}

	Slice uOption = max(uPut - dCompoundCallStrike, 0.0);
	iTime--;
	ASSERT(iTime == 0);
	uOption.rollback(0);

	return interpolate(uOption);
}*/