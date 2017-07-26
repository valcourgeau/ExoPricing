#include "SampleExam1/SampleExam1.hpp"

using namespace cfl;    



class HistSpot: public IResetValues
{
public:
  HistSpot(const cfl::AssetModel & rModel) 
    :m_rModel(rModel)
  {};
		
  Slice resetValues(unsigned iTime, double dBeforeReset) const 
  {
      return m_rModel.spot(iTime);
  }

private:
  const cfl::AssetModel & m_rModel;
  double m_dInitialStrike;
};

cfl::PathDependent 
histSpot(double dInitialValue, const std::vector<unsigned> & rResetIndexes, 
	 const AssetModel & rModel) 
{
  return PathDependent(new HistSpot(rModel), rResetIndexes, dInitialValue); 
}


cfl::MultiFunction prb::clique(double dMaturity, 
			    const std::vector<double> & rAverageTimes, 
			    const std::vector<double> & rResetTimes, 
			    double dInitialStrike,  
				cfl::AssetModel & rModel)
  {
	  PRECONDITION(rAverageTimes.back() < dMaturity);
	  PRECONDITION(rResetTimes.back() < rAverageTimes.back());

	  std::vector<double> uEventTimes(2 + rAverageTimes.size() + rResetTimes.size());
	  uEventTimes.front() = rModel.initialTime();
	  uEventTimes.back() = dMaturity;
	  std::vector<double>::iterator itEnd = 
		  std::set_union(rAverageTimes.begin(), rAverageTimes.end(), 
		  rResetTimes.begin(), rResetTimes.end(), uEventTimes.begin() + 1);
	  uEventTimes.resize(itEnd - uEventTimes.begin() + 1);
	  rModel.assignEventTimes(uEventTimes);

	  std::vector<unsigned> uResetIndexes(rResetTimes.size(), 0);
	  for(unsigned iI = 0; iI < uResetIndexes.size(); iI++)
	  {
		  uResetIndexes[iI] = std::lower_bound(uEventTimes.begin(), 
			  uEventTimes.end(), rResetTimes[iI]) - uEventTimes.begin();
	  }
 
	  unsigned iState = rModel.addState(histSpot(dInitialStrike, uResetIndexes, rModel));
	  unsigned iTime(uEventTimes.size() - 1);
	  Slice uOption = rModel.cash(iTime, 0.0);
	  iTime--;
      uOption.rollback(iTime);

	  while(iTime > 0)
	  {
		  if(std::binary_search(rAverageTimes.begin(), rAverageTimes.end(),
			  rModel.eventTimes()[iTime]))
		  {
			  uOption += max(rModel.spot(iTime) -  rModel.state(iTime, iState), 0.0)
				   * rModel.discount(iTime, dMaturity);
		  }
		  iTime--;
		  uOption.rollback(iTime);
	  }
	  uOption /= (double)rAverageTimes.size();


	  return interpolate(uOption, iState);
  }



class HistStrike: public IResetValues
{
public:
	HistStrike(const cfl::AssetModel & rModel) : 
		m_rModel(rModel)
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

cfl::MultiFunction prb::clique(double dMaturity, 
			    const std::vector<double> & rAverageTimes, 
			    const std::vector<double> & rResetTimes, 
			    double dInitialStrike,  
				cfl::AssetModel & rModel)
{
	PRECONDITION(rAverageTimes.back() < dMaturity);
	PRECONDITION(rResetTimes.back() < rAverageTimes.back());
	std::vector<double> uEventTimes(1 + rAverageTimes.size() + rResetTimes.size());
	uEventTimes.front() = rModel.initialTime();
	std::vector<double>::iterator itEnd = std::set_union(rAverageTimes.begin(),
		rAverageTimes.end(), rResetTimes.begin(), rResetTimes.end(), uEventTimes.begin()+1);
	uEventTimes.resize(itEnd - uEventTimes.begin());
	rModel.assignEventTimes(uEventTimes);

	std::vector<unsigned> uResetIndexes(rResetTimes.size());
	for(unsigned iI = 0; iI < rResetTimes.size(); iI++)
	{
		uResetIndexes[iI] = std::lower_bound(uEventTimes.begin(), uEventTimes.end(),
			rResetTimes[iI]) - uEventTimes.begin();
	}

	unsigned iState = rModel.addState(histStrike(uResetIndexes, dInitialStrike, rModel));

	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = rModel.cash(iTime, 0.0);
	
	while(iTime > 0)
	{
		if(std::binary_search(rAverageTimes.begin(), rAverageTimes.end(), uEventTimes[iTime]))
		{
			uOption += max(rModel.spot(iTime) - rModel.state(iTime, iState), 0.0)
				* rModel.discount(iTime, dMaturity);
		}
		iTime--;
		uOption.rollback(iTime);
	}

	uOption /= (double)rAverageTimes.size();

	return interpolate(uOption, iState);
}