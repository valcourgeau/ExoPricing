#include "Homework2/Homework2.hpp"

using namespace cfl;


 cfl::MultiFunction prb::straddle(double dStrike, double dMaturity, 
			      cfl::AssetModel & rModel)
{
	std::vector<double> uEventTimes(1, rModel.initialTime());
	uEventTimes.push_back(dMaturity);
	rModel.assignEventTimes(uEventTimes);

	Slice uSpot = rModel.spot(dMaturity);
	Slice uStraddle = max(uSpot-dStrike, dStrike - uSpot);
	uStraddle.rollback(0);

	return interpolate(uStraddle);
}
 /*
  cfl::MultiFunction prb::straddle(double dStrike, double dMaturity, 
			      cfl::AssetModel & rModel)
{
	PRECONDITION(dMaturity > rModel.initialTime());
	std::vector<double> uEventTimes(1, rModel.initialTime());
	uEventTimes.push_back(dMaturity);
	rModel.assignEventTimes(uEventTimes);


	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = max(rModel.spot(iTime) - dStrike, 0.0);
	uOption += max(dStrike - rModel.spot(iTime), 0.0);

    iTime--;
	uOption.rollback(iTime);
	ASSERT(iTime == 0);
 }*/