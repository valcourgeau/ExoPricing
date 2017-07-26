#include "Session4/Session4.hpp"

using namespace cfl;

class HistMin: public IResetValues
{
public:
	HistMin(const cfl::AssetModel & rModel) : m_rModel(rModel)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{
		return min(dBeforeReset, m_rModel.spot(iTime));
	}

private:
	const cfl::AssetModel & m_rModel;
};

cfl::PathDependent histMin(const std::vector<unsigned> rResetIndexes, 
						   const cfl::AssetModel & rModel)
{
	double dInitialValue =  std::numeric_limits<double>::max();
	return PathDependent(new HistMin(rModel), rResetIndexes, dInitialValue);
}


 cfl::MultiFunction prb::lookbackPut(const std::vector<double> & rResetTimes,
				 double dStrike, double dMaturity, 
				 cfl::AssetModel & rModel)
 {
	 PRECONDITION(dMaturity > rModel.initialTime());
	 PRECONDITION(rResetTimes.front() > rModel.initialTime());
	 PRECONDITION(dMaturity > rResetTimes.back());

	 std::vector<double> uEventTimes(1, rModel.initialTime());
	 uEventTimes.insert(uEventTimes.end(), rResetTimes.begin(), rResetTimes.end());
	 uEventTimes.insert(uEventTimes.end(), dMaturity);
	 rModel.assignEventTimes(uEventTimes);

	 std::vector<unsigned> uResetIndexes(rResetTimes.size(), 1);
	 std::transform(uResetIndexes.begin(), uResetIndexes.end()-1, uResetIndexes.begin()+1, [](unsigned iX){return iX+1;});
	 int iHistMin = rModel.addState(histMin(uResetIndexes, rModel));
	 int iTime = uEventTimes.size() - 1;

	 Slice uLookback = max(dStrike - rModel.state(iTime, iHistMin), 0.0);
	 uLookback.rollback(0);

	 return interpolate(uLookback, iHistMin);
 }


 /*
 class HistMin: public IResetValues
{
public:
	HistMin(const cfl::AssetModel & rModel) : m_rModel(rModel)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{
		min(dBeforeReset, m_rModel.spot(iTime));
	}

private:
	const cfl::AssetModel & m_rModel;
};

cfl::PathDependent histMin(const std::vector<unsigned> rResetIndexes, 
						   const cfl::AssetModel & rModel)
{
	double dInitialValue = std::numeric_limits<double>::max();
	return PathDependent(new HistMin(rModel), rResetIndexes, dInitialValue);
}





  cfl::MultiFunction prb::lookbackPut(const std::vector<double> & rResetTimes,
				 double dStrike, double dMaturity, 
				 cfl::AssetModel & rModel)
 {
	 PRECONDITION(rModel.initialTime() < rResetTimes.front());
	 PRECONDITION(rResetTimes.back() < dMaturity);
	 std::vector<double> uEventTimes(rResetTimes);
	 uEventTimes.insert(uEventTimes.begin(), rModel.initialTime());
	 uEventTimes.push_back(dMaturity);
	 rModel.assignEventTimes(uEventTimes);

	 std::vector<unsigned> uResetIndexes(rResetTimes.size());
	 uResetIndexes.front() = 1;
	 std::transform(uResetIndexes.begin(), uResetIndexes.end()-1,
		 uResetIndexes.begin() + 1, [](unsigned iX){return iX+1;});

	 unsigned iState = rModel.addState(histMin(uResetIndexes, rModel));
	 unsigned iTime(uEventTimes.size() - 1);
	 Slice uOption = max(dStrike - rModel.state(iTime, iState), 0.0);
	 uOption.rollback(0);

	 return interpolate(uOption, iState);
 }*/