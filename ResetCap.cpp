#include "SampleExam2/SampleExam2.hpp"

using namespace cfl;   


cfl::Slice liborRate(unsigned iTime, const cfl::InterestRateModel & rModel, double dPeriod)
{
	return (1./rModel.discount(iTime, rModel.eventTimes()[iTime] + dPeriod) - 1.0)/dPeriod;
}

class HistLiborPlusSpread: public IResetValues
{
public:
	HistLiborPlusSpread(const cfl::InterestRateModel & rModel, double dSpread, double dPeriod) : 
		m_rModel(rModel), m_dSpread(dSpread), m_dPeriod(dPeriod)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{
		return liborRate(iTime, m_rModel, m_dPeriod) + m_dSpread;
	}

private:
	const cfl::InterestRateModel & m_rModel;
	double m_dSpread, m_dPeriod;
};

cfl::PathDependent histSwapRate(const std::vector<unsigned> rResetIndexes,
							  double dSpread, double dPeriod, double dInitialValue,
						      const cfl::InterestRateModel & rModel)
{
	return PathDependent(new HistLiborPlusSpread(rModel, dSpread, dPeriod), rResetIndexes, dInitialValue);
}



cfl::MultiFunction prb::resetCap(const cfl::Data::CashFlow & rCap, 
			      double dSpread, 
			      const std::vector<double> & rResetTimes,
			      cfl::InterestRateModel & rModel)
  {
	  double dPeriod(rCap.period);
	  std::vector<double> uCapTimes(rCap.numberOfPayments, rModel.initialTime()),
		  uEventTimes(rCap.numberOfPayments + rResetTimes.size());

	  std::transform(uCapTimes.begin(), uCapTimes.end()-1, uCapTimes.begin()+1,
		  [dPeriod](double dX){return dX + dPeriod;});

	  std::vector<double>::iterator itEnd = std::set_union(uCapTimes.begin(), uCapTimes.end(),
		  rResetTimes.begin(), rResetTimes.end(), uEventTimes.begin());
	  uEventTimes.resize(itEnd - uEventTimes.begin());
	  rModel.assignEventTimes(uEventTimes);

	  std::vector<unsigned> uResetIndexes(rResetTimes.size(), 0);
	  for(unsigned iI = 0; iI < rResetTimes.size(); iI++)
	  {
		  uResetIndexes[iI] = std::lower_bound(uEventTimes.begin(), uEventTimes.end(),
			  rResetTimes[iI]) - uEventTimes.begin();
	  }

	  unsigned iReset = rModel.addState(histSwapRate(uResetIndexes, dSpread, dPeriod, rCap.rate,
		  rModel));
	  unsigned iTime(uEventTimes.size()-1);
	  double dTime(uEventTimes.back());
	  Slice uDiscount = rModel.discount(iTime, dTime + dPeriod);
	  Slice uOption = max(liborRate(iTime, rModel, dPeriod) - rModel.state(iTime, iReset), 0.0)
		  * dPeriod * uDiscount;
	  iTime--;
	  uOption.rollback(iTime);
	  ASSERT(uCapTimes.back() >= rResetTimes.back());
	  while(iTime > 0)
	  {
		  dTime = uEventTimes[iTime];
		  if(std::binary_search(uCapTimes.begin(), uCapTimes.end(), dTime))
		  {
			  uDiscount = rModel.discount(iTime, dTime + dPeriod);
			  uOption += max(liborRate(iTime, rModel, dPeriod) - rModel.state(iTime, iReset), 0.0)
			  * dPeriod * uDiscount;
		  }
		  iTime--;
		  uOption.rollback(iTime);
	  }

	  uOption *= rCap.notional;
	  return interpolate(uOption, iReset);
  }

/*

cfl::Slice liborRate(unsigned iTime, double dPeriod, cfl::InterestRateModel & rModel)
{
	return (1./rModel.discount(iTime, rModel.eventTimes()[iTime] + dPeriod) - 1.)/dPeriod;
}


class HistLiborPlusSpread: public IResetValues
{
public:
	HistLiborPlusSpread(const cfl::InterestRateModel & rModel, double dPeriod, double dSpread) :
		m_rModel(rModel), m_dPeriod(dPeriod), m_dLiborSpread(dSpread)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{
		return liborRate(iTime, m_dPeriod, m_rModel) + m_dLiborSpread;
	}

private:
	const cfl::InterestRateModel & m_rModel;
	double m_dPeriod, m_dLiborSpread;
};

cfl::PathDependent histLiborPlusSpread(const std::vector<unsigned> rResetIndexes,
							  double dPeriod, double dSpread, double dInitialValue,
						      const cfl::InterestRateModel & rModel)
{
	return PathDependent(new HistLiborPlusSpread(rModel, dPeriod, dSpread), rResetIndexes, dInitialValue);
}


cfl::MultiFunction prb::resetCap(const cfl::Data::CashFlow & rCap, 
			      double dSpread, 
			      const std::vector<double> & rResetTimes,
			      cfl::InterestRateModel & rModel)
{
	PRECONDITION(rModel.initialTime() < rResetTimes.front());
	double dPeriod(rCap.period);
	std::vector<double> uPaymentTimes(rCap.numberOfPayments), uEventTimes(rCap.numberOfPayments + rResetTimes.size());
	uPaymentTimes.front() = rModel.initialTime();
	std::transform(uPaymentTimes.begin(), uPaymentTimes.end()-1, uPaymentTimes.begin()+1,
		[dPeriod](double dX){return dX + dPeriod;});
	std::vector<double>::iterator itEnd = std::set_union(uPaymentTimes.begin(),
		uPaymentTimes.end(), rResetTimes.begin(), rResetTimes.end(),
		uEventTimes.begin());
	uEventTimes.resize(itEnd - uEventTimes.begin());
	rModel.assignEventTimes(uEventTimes);

	std::vector<unsigned> uResetIndexes(rResetTimes.size(), 0);
	for(unsigned iI = 0; iI < rResetTimes.size(); iI++)
	{
		uResetIndexes[iI] = std::lower_bound(uEventTimes.begin(), uEventTimes.end(),
			rResetTimes[iI]) - uEventTimes.begin();
	}

	unsigned iState = rModel.addState(histLiborPlusSpread(uResetIndexes, dPeriod, dSpread, rCap.rate, rModel));
	unsigned iTime(uEventTimes.size()-1);
	Slice uOption = rModel.cash(iTime, 0.0), uDiscount, uPayment;
	double dTime;
	while(iTime > 0)
	{
		dTime = uEventTimes[iTime];
		if(std::binary_search(rResetTimes.begin(), rResetTimes.end(), dTime))
		{
			uDiscount = rModel.discount(iTime, dTime + dPeriod);
			uPayment = max(liborRate(iTime, dPeriod, rModel) - rModel.state(iTime, iState), 0.0);
			uPayment *= dPeriod;
			uPayment *= uDiscount;
			uOption += uPayment;
		}
		iTime--;
		uOption.rollback(iTime);
	}

	uOption *= rCap.notional;
	return interpolate(uOption, iState);
}*/