#include "Homework4/Homework4.hpp"

using namespace cfl;

class HistMax: public IResetValues
{
public:
	HistMax(const cfl::AssetModel & rModel) : m_rModel(rModel)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{
		return max(dBeforeReset, m_rModel.spot(iTime));
	}

private:
	const cfl::AssetModel & m_rModel;
};

cfl::PathDependent histMax(const std::vector<unsigned> rResetIndexes, 
						   const cfl::AssetModel & rModel)
{
	double dInitialValue =  - std::numeric_limits<double>::max();
	return PathDependent(new HistMax(rModel), rResetIndexes, dInitialValue);
}

cfl::MultiFunction prb::lookbackCall(const std::vector<double> & rResetTimes,
				  double dStrike, double dMaturity, 
				  cfl::AssetModel & rModel)
  {
	  PRECONDITION(rModel.initialTime() < rResetTimes.front());
	  PRECONDITION(rResetTimes.back() < dMaturity);

	  std::vector<double> uEventTimes(1, rModel.initialTime());
	  uEventTimes.insert(uEventTimes.end(), rResetTimes.begin(), rResetTimes.end());
	  uEventTimes.insert(uEventTimes.end(), dMaturity);
	  rModel.assignEventTimes(uEventTimes);

	  std::vector<unsigned> uResetIndexes(rResetTimes.size(),1);
	  std::transform(uResetIndexes.begin(),uResetIndexes.end()-1, uResetIndexes.begin()+1, [](unsigned iX){return iX+1;});

	  unsigned iHistMax = rModel.addState(histMax(uResetIndexes, rModel));

	  int iTime(uEventTimes.size()-1);
	  Slice uOption(max(rModel.state(iTime, iHistMax) - dStrike, 0.0));
	  uOption.rollback(0);

	  return interpolate(uOption, iHistMax);
  }



class HistMax: public IResetValues
{
public:
	HistMax(const cfl::AssetModel & rModel) : m_rModel(rModel)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{
		return max(dBeforeReset, m_rModel.spot(iTime));
	}

private:
	const cfl::AssetModel & m_rModel;
};

cfl::PathDependent histMax(const std::vector<unsigned> rResetIndexes, 
						   const cfl::AssetModel & rModel)
{
	double dInitialValue =  - std::numeric_limits<double>::max();
	return PathDependent(new HistMax(rModel), rResetIndexes, dInitialValue);
}

cfl::MultiFunction prb::lookbackCall(const std::vector<double> & rResetTimes,
				  double dStrike, double dMaturity, 
				  cfl::AssetModel & rModel)
{
	PRECONDITION(rModel.initialTime() < rResetTimes.front());
	PRECONDITION(rResetTimes.back() < dMaturity);

	std::vector<double> uEventTimes(rResetTimes);
	uEventTimes.insert(uEventTimes.begin(), rModel.initialTime());
	uEventTimes.push_back(dMaturity);
	rModel.assignEventTimes(uEventTimes);

	std::vector<unsigned> uResetIndexes(rResetTimes.size(),1);
	std::transform(uResetIndexes.begin(), uResetIndexes.end()-1, uResetIndexes.begin()+1,
		[](unsigned iX){return iX+1;});
	unsigned iState = rModel.addState(histMax(uResetIndexes, rModel));

	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = max(rModel.state(iTime, iState) - dStrike, 0.0);
	uOption.rollback(0);

	return interpolate(uOption, iState);
}