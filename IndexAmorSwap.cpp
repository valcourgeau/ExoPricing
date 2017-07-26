#include "SampleExam3/SampleExam3.hpp"

using namespace cfl;  
  
cfl::Slice liborRate(unsigned iTime, const cfl::InterestRateModel & rModel, double dPeriod)
{
	return (1./rModel.discount(iTime, rModel.eventTimes()[iTime] + dPeriod) - 1.)/dPeriod;
}


class HistNotional: public IResetValues
{
public:
	HistNotional(const cfl::InterestRateModel & rModel, double dPeriod, double (*fAmortizing)(double)) : 
		m_rModel(rModel),  m_dPeriod(dPeriod), m_f(fAmortizing)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{

		return dBeforeReset * liborRate(iTime, m_rModel, m_dPeriod).apply(m_f);
	}

private:
	double (*m_f)(double);
	const cfl::InterestRateModel & m_rModel;
	double m_dPeriod;
};

cfl::PathDependent histNotional(const std::vector<unsigned> rResetIndexes, 
						   const cfl::InterestRateModel & rModel, double dInitialValue, double dPeriod, double (*fAmortizing)(double))
{
	return PathDependent(new HistNotional(rModel, dPeriod, fAmortizing), rResetIndexes, dInitialValue);
}



cfl::MultiFunction prb::indexAmortizingSwap(const cfl::Data::Swap & rSwap,
					 double (*fAmortizing)(double), 
					 double dLowerThreshold, 
					 cfl::InterestRateModel & rModel)
  {
	  double dPeriod(rSwap.period);
	  std::vector<double> uEventTimes(rSwap.numberOfPayments);
	  std::vector<unsigned> uResetIndexes(rSwap.numberOfPayments-1, 1);
	  uEventTimes.front() = rModel.initialTime();
	  std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1,
		  [dPeriod](double dX){return dX + dPeriod;});
	  rModel.assignEventTimes(uEventTimes);

	  std::transform(uResetIndexes.begin(), uResetIndexes.end(), uResetIndexes.begin(),
		  [](unsigned iX){return iX + 1;});
	  
	  unsigned iTime(uEventTimes.size() - 1);
	  unsigned iState = rModel.addState(histNotional(uResetIndexes,rModel, 1.0, dPeriod, fAmortizing));
	  
	  Slice uNotional = rModel.state(iTime, iState);
	  Slice uTemp = liborRate(iTime, rModel, dPeriod) - rSwap.rate;
	  uTemp *= (uNotional * dPeriod);
	  uTemp *= rModel.discount(iTime, uEventTimes[iTime] + dPeriod);
	  
	  Slice uOption = rModel.cash(iTime, 0.0); // because cancelled
	  uOption += uTemp;
	 
	  while(iTime > 0)
	  {
		  uOption *= indicator(uNotional, dLowerThreshold);
	      iTime--;
	      uOption.rollback(iTime);
	  
		  uNotional = rModel.state(iTime, iState);
		  uTemp = liborRate(iTime, rModel, dPeriod) - rSwap.rate;
	      uTemp *= (uNotional * dPeriod);
		  uTemp *= rModel.discount(iTime, uEventTimes[iTime] + dPeriod);
		  uOption += uTemp;
	  }

	  if (rSwap.payFloat) { //if we pay fixed
			uOption *= -1;
	   }
	  uOption *= rSwap.notional;
	  return interpolate(uOption, iState);
  }


cfl::Slice liborRate(unsigned iTime, double dPeriod, const cfl::InterestRateModel & rModel)
{
	return (1./rModel.discount(iTime, rModel.eventTimes()[iTime] + dPeriod) - 1.)/dPeriod;
}

class HistSwapRate: public IResetValues
{
public:
	HistSwapRate(const cfl::InterestRateModel & rModel, double (*fAmortizing)(double), double dPeriod) :
		m_rModel(rModel), m_f(fAmortizing), m_dPeriod(dPeriod)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{
		return dBeforeReset * liborRate(iTime, m_dPeriod, m_rModel).apply(m_f);
	}

private:
	const cfl::InterestRateModel & m_rModel;
	double (*m_f)(double);
	double m_dPeriod;
};

cfl::PathDependent histSwapRate(const std::vector<unsigned> rResetIndexes,
							  double (*fAmortizing)(double), double dPeriod, double dInitialValue,
						      const cfl::InterestRateModel & rModel)
{
	return PathDependent(new HistSwapRate(rModel, fAmortizing, dPeriod), rResetIndexes, dInitialValue);
}

cfl::MultiFunction prb::indexAmortizingSwap(const cfl::Data::Swap & rSwap,
					 double (*fAmortizing)(double), 
					 double dLowerThreshold, 
					 cfl::InterestRateModel & rModel)
{
	double dPeriod(rSwap.period);
	std::vector<double> uEventTimes(rSwap.numberOfPayments, rModel.initialTime());
	std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1,
		[dPeriod](double dX){return dX + dPeriod;});
	rModel.assignEventTimes(uEventTimes);

	std::vector<unsigned> uResetIndexes(rSwap.numberOfPayments, 1);
	std::transform(uResetIndexes.begin(), uResetIndexes.end()-1, uResetIndexes.begin()+1,
		[](unsigned iX){return iX + 1;});

	double dInitialValue(1.0);
	unsigned iState = rModel.addState(histSwapRate(uResetIndexes, fAmortizing, dPeriod, dInitialValue, rModel));
	unsigned iTime(uEventTimes.size() - 1);

	Slice uOption = rModel.cash(iTime, 0.0), uPayment, uNotional;
	while(iTime > 0)
	{
		uNotional = rModel.state(iTime, iState);
		uOption += indicator(dLowerThreshold, uNotional) * (rModel.cash(iTime, 0.0) - uOption);
		uPayment = liborRate(iTime, dPeriod, rModel) - rSwap.rate;
		uPayment *= dPeriod;
		uPayment *= uNotional;
		uPayment *= rModel.discount(iTime, uEventTimes[iTime] + dPeriod);

		uOption += uPayment;
		iTime--;
		uOption.rollback(iTime);
	}

	if(rSwap.payFloat)
	{
		uOption *= -1.;
	}
	uOption *= rSwap.notional;

	return interpolate(uOption, iState);

}