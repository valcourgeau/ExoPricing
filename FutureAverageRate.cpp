#include "Homework4/Homework4.hpp"
#include <assert.h>

using namespace cfl;
    


class HistLibor: public IResetValues
{
public:
	HistLibor(const cfl::InterestRateModel & rModel, double dPeriod) : 
		m_rModel(rModel),  m_dPeriod(dPeriod)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{
		std::vector<double> uEventTimes(m_rModel.eventTimes());
		int iNumber(std::lower_bound(uEventTimes.begin()+1, uEventTimes.end(), uEventTimes[iTime]) - (uEventTimes.begin()+1));
		Slice uLibor((1./m_rModel.discount(iTime, uEventTimes[iTime] + m_dPeriod) - 1.) / m_dPeriod);
		return (dBeforeReset * iNumber + uLibor) / (double)(iNumber + 1.);
	}

private:
	const cfl::InterestRateModel & m_rModel;
	double m_dPeriod;
};

cfl::PathDependent histLibor(const std::vector<unsigned> rResetIndexes, 
						   const cfl::InterestRateModel & rModel, double dInitialValue, double dPeriod)
{
	return PathDependent(new HistLibor(rModel, dPeriod), rResetIndexes, dInitialValue);
}


cfl::MultiFunction 
  prb::futureOnAverageRate(double dMaturity, unsigned iNumberOfTimes, 
		      cfl::InterestRateModel & rModel)
  {
	  PRECONDITION(dMaturity > rModel.initialTime());
	  double dPeriod((dMaturity - rModel.initialTime()) / (double)iNumberOfTimes);
	  std::vector<double> uEventTimes(iNumberOfTimes + 1);
	  uEventTimes.front() = rModel.initialTime();
	  std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1,  [dPeriod](double dX){return dX + dPeriod;});
	  assert(std::abs(dMaturity - uEventTimes.back()) < c_dEps);
	  rModel.assignEventTimes(uEventTimes);

	  std::vector<unsigned> uResetIndexes(iNumberOfTimes, 1);
	  std::transform(uResetIndexes.begin(), uResetIndexes.end()-1,
		  uResetIndexes.begin() + 1, [](unsigned iX){return iX + 1;});
	  
	  double dInitialValue(0.0);
	  unsigned iTime(uEventTimes.size() - 1);
	  unsigned iLibor = rModel.addState(histLibor(uResetIndexes, rModel, dInitialValue, dPeriod));

	  Slice uOption(1.0 - rModel.state(iTime, iLibor));
	  while(iTime > 0)
	  {
		  iTime--;
		  uOption.rollback(iTime);
		  uOption /= rModel.discount(iTime, uEventTimes[iTime] + dPeriod);
	  }
	  return interpolate(uOption, iLibor);
  }


/*

class HistSumLibor: public IResetValues
{
public:
	HistSumLibor(const cfl::InterestRateModel & rModel, double dPeriod) : 
		m_rModel(rModel), m_dPeriod(dPeriod)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{
		return dBeforeReset + liborRate(iTime, m_dPeriod, m_rModel);
	}

private:
	const cfl::InterestRateModel & m_rModel;
	double m_dPeriod;
};

cfl::PathDependent histSwapRate(const std::vector<unsigned> rResetIndexes,
							  int iPeriods, double dPeriod,
						      const cfl::InterestRateModel & rModel)
{
	double dInitialValue = 0.0;
	return PathDependent(new HistSumLibor(rModel, dPeriod), rResetIndexes, dInitialValue);
}

cfl::Slice liborRate(unsigned iTime, double dPeriod, cfl::InterestRateModel rModel)
{
	return (1./rModel.discount(iTime, rModel.eventTimes()[iTime] + dPeriod) - 1.)/dPeriod;
}

cfl::MultiFunction 
  prb::futureOnAverageRate(double dMaturity, unsigned iNumberOfTimes, 
		      cfl::InterestRateModel & rModel)
{
	PRECONDITION(rModel.initialTime() < dMaturity);
	double dPeriod((dMaturity - rModel.initialTime()) / (double)iNumberOfTimes);
	std::vector<double> uEventTimes(iNumberOfTimes + 1);
	uEventTimes.front = rModel.initialTime();
	std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1,
		[dPeriod](double dX){return dX + dPeriod;});
	rModel.assignEventTimes(uEventTimes);
	ASSERT(std::abs(uEventTimes.back() - dMaturity) < c_dEps);

	std::vector<unsigned> uResetIndexes(iNumberOfTimes,1);
	std::transform(uResetIndexes.begin(), uResetIndexes.end()-1, uResetIndexes.begin()+1,
		[dPeriod](unsigned iX){return iX+dPeriod;});

	unsigned iState = rModel.addState(histLibor(uResetIndexes, rModel, dPeriod));
	unsigned iTime(uEventTimes.size()-1);

	Slice uFuture = rModel.cash(iTime, 1.);
	Slice uOption = 1. - rModel.state(iTime, iState) / (double)iNumberOfTimes;
	
	while(iTime > 0)
	{
		iTime--;
		uOption.rollback(iTime);
		uOption /= rModel.discount(iTime, uEventTimes[iTime] + dPeriod);		
	}

	return interpolate(uOption, iState);
}/*