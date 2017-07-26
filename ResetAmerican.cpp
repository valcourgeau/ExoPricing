#include "Session4/Session4.hpp"

using namespace cfl;


class HistSpot: public IResetValues
{
public:
	HistSpot(const cfl::AssetModel & rModel) : m_rModel(rModel)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{
		return m_rModel.spot(iTime);
	}

private:
	const cfl::AssetModel & m_rModel;
};

cfl::PathDependent histSpot(double dInitialStrike, const std::vector<unsigned> rResetIndexes, 
						   const cfl::AssetModel & rModel)
{
	return PathDependent(new HistSpot(rModel), rResetIndexes, dInitialStrike);
}


cfl::MultiFunction prb::resetAmericanPut(double dInitialStrike,
				      const std::vector<double> & rResetTimes, 
				      const std::vector<double> & rExerciseTimes,
				      cfl::AssetModel & rModel)
{
	PRECONDITION(rResetTimes.back() < rExerciseTimes.back());
	std::vector<double> uEventTimes(1 + rResetTimes.size() + rExerciseTimes.size());
	uEventTimes.front() = rModel.initialTime();
	std::vector<double>::iterator itEnd = std::set_union(rResetTimes.begin(), rResetTimes.end(),
		rExerciseTimes.begin(), rExerciseTimes.end(), uEventTimes.begin() + 1);
	uEventTimes.resize(itEnd - uEventTimes.begin());
	rModel.assignEventTimes(uEventTimes);

	std::vector<unsigned> uResetIndexes(rResetTimes.size());
	for(unsigned iI = 0; iI < rResetTimes.size(); iI++)
	{
		uResetIndexes[iI] = std::lower_bound(uEventTimes.begin(), uEventTimes.end(),
			rResetTimes[iI]) - uEventTimes.begin();
	}
	
	int iTime(uEventTimes.size() -1);
	int iHistSpot = rModel.addState(histSpot(dInitialStrike, uResetIndexes, rModel));
	Slice uOption = rModel.cash(iTime, 0.0);
	
	while(iTime > 0)
	{
		if(std::binary_search(rExerciseTimes.begin(), rExerciseTimes.end(), uEventTimes[iTime]))
		{
			uOption = max(uOption, rModel.state(iTime, iHistSpot) - rModel.spot(iTime));
		}
		iTime --;
		uOption.rollback(iTime);
	}
	
	return interpolate(uOption, iHistSpot);
}


/*
class HistStrike: public IResetValues
{
public:
	HistStrike(const cfl::AssetModel & rModel) : m_rModel(rModel)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{
		return m_rModel.spot(iTime);
	}

private:
	const cfl::AssetModel & m_rModel;
};

cfl::PathDependent histStrike(const std::vector<unsigned> rResetIndexes, 
						   double dInitialStrike,
						   const cfl::AssetModel & rModel)
{
	return PathDependent(new HistStrike(rModel), rResetIndexes, dInitialStrike);
}

cfl::MultiFunction prb::resetAmericanPut(double dInitialStrike,
				      const std::vector<double> & rResetTimes, 
				      const std::vector<double> & rExerciseTimes,
				      cfl::AssetModel & rModel)
{
	PRECONDITION(rExerciseTimes.back() > rResetTimes.back());
	std::vector<double> uEventTimes(1 + rResetTimes.size() + rExerciseTimes.size());
	uEventTimes.front() = rModel.initialTime();
	std::vector<double>::iterator itEnd =
		std::set_union(rResetTimes.begin(), rResetTimes.end(),
		rExerciseTimes.begin(), rExerciseTimes.end(),
		uEventTimes.begin()+1);
	uEventTimes.resize(itEnd - uEventTimes.begin());
	rModel.assignEventTimes(uEventTimes);

	std::vector<unsigned> uResetIndexes(rResetTimes.size());
	for(unsigned iI=0; iI < rResetTimes.size(); iI++)
	{
		uResetIndexes[iI] = std::lower_bound(uEventTimes.begin(),
			uEventTimes.end(), rResetTimes[iI]) - uEventTimes.begin();
	}

	unsigned iState = rModel.addState(HistStrike(uResetIndexes, dInitialStrike, rModel));
	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = max(rModel.state(iTime, iState) - rModel.spot(iTime), 0.0);
	iTime--;
    uOption.rollback(iTime);

	while(iTime > 0)
	{
		if(std::binary_search(rExerciseTimes.begin(), rExerciseTimes.end(), uEventTimes[iTime]))
		{
			uOption = max(rModel.state(iTime, iState) - rModel.spot(iTime), uOption);
		}
		iTime--;
		uOption.rollback(iTime);
	}

	return interpolate(uOption, iState);
}*/