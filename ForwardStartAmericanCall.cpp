#include "Homework4/Homework4.hpp"

using namespace cfl;

class HistForward: public IResetValues
{
public:
	HistForward(const cfl::AssetModel & rModel, double dMaturity) : m_rModel(rModel), m_dMaturity(dMaturity)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{
		
		PRECONDITION(m_rModel.eventTimes()[iTime] <= m_dMaturity);
		return m_rModel.forward(iTime, m_dMaturity);
	}

private:
	const cfl::AssetModel & m_rModel;
	double m_dMaturity;
};

cfl::PathDependent histForward(const std::vector<unsigned> rResetIndexes, 
						   const cfl::AssetModel & rModel, double dInitialValue, double dMaturity)
{
	return PathDependent(new HistForward(rModel, dMaturity), rResetIndexes, dInitialValue);
}


cfl::MultiFunction 
  prb::forwardStartAmericanCall(double dIssueTime,
			   const std::vector<double> & rExerciseTimes,
			   cfl::AssetModel & rModel)
  {
	  PRECONDITION(rModel.initialTime() < dIssueTime);
	  PRECONDITION(dIssueTime < rExerciseTimes.front());
	  std::vector<double> uEventTimes(1, rModel.initialTime());
	  uEventTimes.push_back(dIssueTime);
	  uEventTimes.insert(uEventTimes.end(), rExerciseTimes.begin(), rExerciseTimes.end());
	  rModel.assignEventTimes(uEventTimes);

	  std::vector<unsigned> uResetIndexes(1,1);
	  
	  double dMaturity(uEventTimes.back()), dInitialValue(0.0);
	  unsigned iForward = rModel.addState(histForward(uResetIndexes, rModel, dInitialValue, uEventTimes.back()));
	  unsigned iTime(uEventTimes.size() -1);
	  Slice uOption = rModel.cash(iTime, 0.0);

	  while(iTime > 1) // BECAUSE WE DONT WANT THE FIRST POINT (HAS NOTHING TO DO WITH OPTION!)
	  {
		  uOption = max(uOption, rModel.spot(iTime) - rModel.state(iTime, iForward));
		  iTime--;
		  uOption.rollback(iTime);
	  }
	  uOption.rollback(0);
	  return interpolate(uOption, iForward);
  }