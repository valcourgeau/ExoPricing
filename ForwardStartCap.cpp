#include "Session4/Session4.hpp"

using namespace cfl;  

cfl::Slice swapRate(unsigned iTime, double dPeriod, unsigned iPeriods, const cfl::InterestRateModel & rModel)
{
	Slice uFixed = rModel.cash(iTime, 0.0);
	double dTime = rModel.eventTimes()[iTime];

	for(unsigned iI=0; iI < iPeriods;iI++)
	{
		dTime += dPeriod;
		uFixed += rModel.discount(iTime, dTime);
	}
	uFixed *= dPeriod;
	Slice uFloat = 1.0 - rModel.discount(iTime, dTime);
	Slice uRate = (uFloat / uFixed);

	return(uRate);
}

class HistSwapRate: public IResetValues
{
public:
	HistSwapRate(const cfl::InterestRateModel & rModel, int iPeriods, double dPeriod) : m_rModel(rModel), m_iPeriods(iPeriods), m_dPeriod(dPeriod)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{
		return swapRate(iTime, m_dPeriod, m_iPeriods, m_rModel);
	}

private:
	const cfl::InterestRateModel & m_rModel;
	int m_iPeriods;
	double m_dPeriod;
};

cfl::PathDependent histSwapRate(const std::vector<unsigned> rResetIndexes,
							  int iPeriods, double dPeriod,
						      const cfl::InterestRateModel & rModel)
{
	double dInitialValue = std::numeric_limits<double>::max();
	return PathDependent(new HistSwapRate(rModel, iPeriods, dPeriod), rResetIndexes, dInitialValue);
}



cfl::MultiFunction prb::forwardStartCap(double dIssueTime, double
				     dNotional, double dPeriod,
				     unsigned iNumberOfPayments, 
				     cfl::InterestRateModel & rModel)
  {
	  PRECONDITION(rModel.initialTime() < dIssueTime);
	  std::vector<double> uEventTimes(iNumberOfPayments + 1);
	  uEventTimes.front() = rModel.initialTime();
	  uEventTimes[1] = dIssueTime;
	  std::transform(uEventTimes.begin()+1, uEventTimes.end()-1, uEventTimes.begin()+2,
		  [dPeriod](double dX){return dX + dPeriod;});
	  rModel.assignEventTimes(uEventTimes);

	  int iTime(uEventTimes.size()-1);
	  std::vector<unsigned> uResetIndexes(1, 1);
	  int iHistSwapRate = rModel.addState(histSwapRate(uResetIndexes,iNumberOfPayments, dPeriod, rModel));
	  Slice uDiscount = rModel.discount(iTime, uEventTimes.back() + dPeriod);
	  // LIBOR * deltaT - SwapRate * deltaT
	  Slice swapRateDeltaT = rModel.state(iTime, iHistSwapRate) * dPeriod;
	  Slice uOption = uDiscount * max(1./uDiscount - 1.0 - swapRateDeltaT, 0.0);
	  while(iTime > 1)
	  {
		  iTime--;
		  uOption.rollback(iTime);
		  uDiscount = rModel.discount(iTime, uEventTimes[iTime]+ dPeriod);
		  swapRateDeltaT = rModel.state(iTime, iHistSwapRate) * dPeriod;
		  uOption += uDiscount * max(1./uDiscount - 1.0 - swapRateDeltaT, 0.0);

	  }
	  // Very last period: nothing happens
	  uOption *= dNotional;
	  uOption.rollback(0);

	  return interpolate(uOption, iHistSwapRate);
  }

/*
class HistSwap: public IResetValues
{
public:
	HistSwap(double dPeriod, unsigned iNumberOfPayments, const cfl::InterestRateModel & rModel) : 
		m_rModel(rModel), m_dPeriod(dPeriod), m_iPeriod(iNumberOfPayments)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{
		return swapRate(iTime, m_dPeriod, m_iPeriod, m_rModel);
	}

private:
	const cfl::InterestRateModel & m_rModel;
	double m_dPeriod;
	unsigned m_iPeriod;
};

cfl::PathDependent histSwap(const std::vector<unsigned> rResetIndexes,
							  double dPeriod,
							unsigned iNumberOfPayments, 
						   double dInitialValue,
						   const cfl::InterestRateModel & rModel)
{
	return PathDependent(new HistSwap(dPeriod, iNumberOfPayments, rModel), rResetIndexes, dInitialValue);
}

cfl::MultiFunction prb::forwardStartCap(double dIssueTime, double
				     dNotional, double dPeriod,
				     unsigned iNumberOfPayments, 
				     cfl::InterestRateModel & rModel)
{
	PRECONDITION(rModel.initialTime() < dIssueTime);
	std::vector<double> uEventTimes(iNumberOfPayments + 1);
	uEventTimes.front() = rModel.initialTime();
	uEventTimes[1] = dIssueTime;
	std::transform(uEventTimes.begin()+1, uEventTimes.end()-1,
		uEventTimes.begin()+2, [dPeriod](double dX){return dX + dPeriod;});
	ASSERT(std::abs(uEventTimes.back() - dPeriod * iNumberOfPayments - dIssueTime) < c_dEps);
	rModel.assignEventTimes(uEventTimes);

	std::vector<unsigned> uResetIndexes(1,1);
	
	double dInitialValue = 0.0;
	unsigned iState = rModel.addState(histSwap(uResetIndexes, dPeriod, 
		iNumberOfPayments, dInitialValue, rModel));

	unsigned iTime(uEventTimes.size() - 1);
	Slice uRate = rModel.state(iTime, iState);
	Slice uPayment, uOption = rModel.cash(iTime, 0.0);
	iTime--;
	uOption.rollback(iTime);
	
	while(iTime > 1)
	{
		uPayment = max((1. / rModel.discount(iTime, uEventTimes[iTime] + dPeriod) - 1.) / dPeriod
		- uRate, 0.0);
		uPayment *= rModel.discount(iTime, uEventTimes[iTime] + dPeriod);
		uPayment *= dPeriod;
		uOption += max(uPayment, 0.0);
		iTime--;
		uOption.rollback(iTime);
	}

	// Forward part
	iTime--;
	uOption.rollback(iTime);
	ASSERT(iTime == 0);
	uOption *= dNotional;

	return interpolate(uOption, iState);
}*/