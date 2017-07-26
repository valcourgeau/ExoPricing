#include "Homework4/Homework4.hpp"

using namespace cfl;

cfl::Slice swap(unsigned iTime, double
					  dNotional, double dPeriod,
					  unsigned iNumberOfPayments,
					  bool bPayFloat, 
					  const Slice & rRate,
		   cfl::InterestRateModel & rModel)
{
	Slice uCash = rModel.cash(iTime, 0.0);
	double dPaymentTime(rModel.eventTimes()[iTime]);
	for(unsigned iI =0; iI < iNumberOfPayments; iI++)
	{
		dPaymentTime += dPeriod;
		uCash += rModel.discount(iTime, dPaymentTime);
	}

	uCash *= (dPeriod * rRate);
	uCash += rModel.discount(iTime, dPaymentTime);
	uCash *= dNotional;

	uCash -= dNotional;
	if(!bPayFloat)
	{
		uCash *= -1.0;
	}

	return uCash;
}

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
	HistSwapRate(const cfl::InterestRateModel & rModel, double dIssueTime, double dMaturity, double dPeriod, unsigned iNumberOfPayments) : 
		m_rModel(rModel), m_dIssueTime(dIssueTime), m_dMaturity(dMaturity), m_dPeriod(dPeriod), m_iNumberOfPayments(iNumberOfPayments)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{
		return swapRate(iTime, m_dPeriod, m_iNumberOfPayments, m_rModel);
	}

private:
	const cfl::InterestRateModel & m_rModel;
	double m_dIssueTime, m_dMaturity, m_dPeriod;
	unsigned m_iNumberOfPayments;
};

cfl::PathDependent histSwapRate(const std::vector<unsigned> rResetIndexes, 
						   const cfl::InterestRateModel & rModel, double dInitialValue, double dIssueTime, double dMaturity, double dPeriod, unsigned iNumberOfPayments)
{
	return PathDependent(new HistSwapRate(rModel, dIssueTime, dMaturity, dPeriod, iNumberOfPayments), rResetIndexes, dInitialValue);
}


cfl::MultiFunction prb::forwardStartSwaption(double dIssueTime, double
					  dNotional, double dPeriod,
					  unsigned iNumberOfPayments,
					  bool bPayFloat, double dMaturity,  
					  cfl::InterestRateModel & rModel)
  {
	  std::vector<double> uEventTimes(1, rModel.initialTime());
	  uEventTimes.push_back(dIssueTime);
	  uEventTimes.push_back(dMaturity);
	  rModel.assignEventTimes(uEventTimes);

	  std::vector<unsigned> uResetIndexes(1,1);
	  double dInitialValue(0.0);
	  unsigned iSwapRate = rModel.addState(histSwapRate(uResetIndexes, rModel, dInitialValue, dIssueTime, dMaturity, dPeriod, iNumberOfPayments));
	  unsigned iTime(uEventTimes.size() - 1);
	  
	  Slice uSwap = swap(iTime, dNotional, dPeriod, iNumberOfPayments, bPayFloat, rModel.state(iTime, iSwapRate), rModel);
	  uSwap = max(uSwap, 0.0);
	  uSwap.rollback(0);
	
	  return interpolate(uSwap, iSwapRate);
  }


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


cfl::Slice swap(unsigned iTime, Slice dRate,
				double dPeriod, double dNotional,
					  unsigned iNumberOfPayments,
					  bool bPayFloat, double dMaturity,
		   cfl::InterestRateModel & rModel)
{
	Slice uCash = rModel.cash(iTime, 0.0);
	double dPaymentTime(rModel.eventTimes()[iTime]);
	for(unsigned iI =0; iI < iNumberOfPayments; iI++)
	{
		dPaymentTime += dPeriod;
		uCash += rModel.discount(iTime, dPaymentTime);
	}

	uCash *= (dPeriod * dRate);
	uCash += rModel.discount(iTime, dPaymentTime);
	uCash *= dNotional;

	uCash -= dNotional;
	if(bPayFloat)
	{
		uCash *= -1.0;
	}

	return uCash;
}

class HistSwapRate: public IResetValues
{
public:
	HistSwapRate(const cfl::InterestRateModel & rModel, int iPeriods, double dPeriod) : 
		m_rModel(rModel), m_iPeriods(iPeriods), m_dPeriod(dPeriod)
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
	double dInitialValue = 0.0;
	return PathDependent(new HistSwapRate(rModel, iPeriods, dPeriod), rResetIndexes, dInitialValue);
}


cfl::MultiFunction prb::forwardStartSwaption(double dIssueTime, double
					  dNotional, double dPeriod,
					  unsigned iNumberOfPayments,
					  bool bPayFloat, double dMaturity,  
					  cfl::InterestRateModel & rModel)
{
	PRECONDITION(rModel.initialTime < dIssueTime);
	PRECONDITION(dIssueTime < dMaturity);
	std::vector<double> uEventTimes(1, rModel.initialTime());
	uEventTimes.push_back(dIssueTime);
	uEventTimes.push_back(dMaturity);
	rModel.assignEventTimes(uEventTimes);

	std::vector<unsigned> uResetIndexes(1,1);

	unsigned iState = rModel.addState(histSwapRate(uResetIndexes,iNumberOfPayments, dPeriod, rModel));
	unsigned iTime(uEventTimes.size() - 1);

	Slice uOption = max(swap(iTime, rModel.state(iTime, iState), dPeriod, dNotional, iNumberOfPayments, bPayFloat, dMaturity, rModel), 0.0);
	uOption.rollback(0);

	return interpolate(uOption, iState);
}