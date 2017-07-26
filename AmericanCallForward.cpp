#include "Homework2/Homework2.hpp"

using namespace cfl;

Slice longForward(double dForwardPrice, double dTimeToMaturity,
				  unsigned iTime, cfl::AssetModel & rModel)
{
	double dMaturity(rModel.initialTime() + dTimeToMaturity);
	Slice uForward = rModel.discount(iTime, dTimeToMaturity)
		 * (rModel.forward(iTime, dTimeToMaturity) - dForwardPrice);

	return uForward;
}

cfl::MultiFunction 
  prb::americanCallOnForward(double dForwardPrice, 
			double dTimeToMaturity,
			const std::vector<double> & rExerciseTimes,
			cfl::AssetModel & rModel)
{
	PRECONDITION(rExerciseTimes.front() > rModel.initialTime());
	std::vector<double> uEventTimes(1, rModel.initialTime());
	uEventTimes.insert(uEventTimes.end(), rExerciseTimes.begin(), rExerciseTimes.end());
	rModel.assignEventTimes(uEventTimes);

	int iTime(uEventTimes.size() - 1);
	Slice uAmerican = rModel.cash(iTime, 0.0);
	
	while(iTime > 0)
	{
		uAmerican = max(longForward(dForwardPrice, dTimeToMaturity, iTime, rModel),
			uAmerican);
		iTime--;
		uAmerican.rollback(iTime);
	}

	return interpolate(uAmerican);
}

/*
cfl::MultiFunction 
  prb::americanCallOnForward(double dForwardPrice, 
			double dTimeToMaturity,
			const std::vector<double> & rExerciseTimes,
			cfl::AssetModel & rModel)
{
	PRECONDITION(std::equal(rExerciseTimes.begin(), rExerciseTimes.end()-1,
		rExerciseTimes.begin() + 1, std::greater<double>()));
	std::vector<double> uEventTimes(rExerciseTimes);
	uEventTimes.insert(uEventTimes.begin(), rModel.initialTime());
	rModel.assignEventTimes(rExerciseTimes);

	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = rModel.cash(iTime, 0.0), uDiscount;

	while(iTime > 0)
	{
		uDiscount = rModel.discount(iTime, uEventTimes[iTime] + dTimeToMaturity);
		uOption = max(uOption,uDiscount*(rModel.forward(iTime, uEventTimes[iTime] + dTimeToMaturity) - dForwardPrice));
		iTime--;
		uOption.rollback(iTime);
	}

	return interpolate(uOption);
}
*/