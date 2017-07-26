#include "Session4/Session4.hpp"

using namespace cfl;  

class HistCoupon: public IResetValues
{
public:
	HistCoupon(const cfl::InterestRateModel & rModel, double dFactor, double dPeriod, double dNotional, double dFixedRate) 
		: m_rModel(rModel), m_dFactor(dFactor), m_dPeriod(dPeriod), m_dNotional(dNotional), m_dFixedRate(dFixedRate)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{
		Slice uDiscount = m_rModel.discount(iTime,
			m_rModel.eventTimes()[iTime] + m_dPeriod);
		return max(dBeforeReset + m_dNotional * m_dFactor
			 * (m_dFixedRate * m_dPeriod - (1./uDiscount - 1.0)), 0.0);
	}

private:
	const cfl::InterestRateModel & m_rModel;
	double m_dFactor, m_dPeriod, m_dNotional, m_dFixedRate;
};

cfl::PathDependent histCoupon(double dRate, double dFactor, double dPeriod, double dNotional, double dFixedRate, const std::vector<unsigned> rResetIndexes, 
						   const cfl::InterestRateModel & rModel)
{
	double dInitialCoupon(dNotional * dRate * dPeriod);
	return PathDependent(new HistCoupon(rModel, dFactor, dPeriod, dNotional, dFixedRate), rResetIndexes, dInitialCoupon);
}


 cfl::MultiFunction 
  prb::ratchetInverseFloater(const cfl::Data::CashFlow & rCashFlow, 
			double dInitialCouponRate, 
			double dLeverageFactor, 
			cfl::InterestRateModel & rModel)
 {
	 std::vector<double> uEventTimes(rCashFlow.numberOfPayments);
	 uEventTimes.front() = rModel.initialTime();
	 std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin() +1,
		 [&rCashFlow](double dX){return dX + rCashFlow.period;});
	 rModel.assignEventTimes(uEventTimes);

	 std::vector<unsigned> uResetIndexes(rCashFlow.numberOfPayments-1,1);
	 std::transform(uResetIndexes.begin(), uResetIndexes.end()-1, uResetIndexes.begin()+1,
		 [](unsigned iX){return iX + 1;});
	 
	 int iCoupon = rModel.addState(histCoupon(dInitialCouponRate, dLeverageFactor,
		 rCashFlow.rate,rCashFlow.notional,	rCashFlow.rate, uResetIndexes, rModel));
	 
	 int iTime(rModel.eventTimes().size()-1);
	 
	 double dMaturity = rModel.eventTimes().back() + rCashFlow.period;
	 Slice uDiscount = rModel.discount(iTime, dMaturity);
	 //Slice uOption = rModel.cash(iTime, 0.0);
	 
	 Slice uOption = rModel.state(iTime, iCoupon) * uDiscount;
	 
	 while(iTime >0)
	 {
		 
		 dMaturity -= rCashFlow.period;
		 iTime--;
		 uOption.rollback(iTime);
		 uDiscount = rModel.discount(iTime, dMaturity);
		 uOption += rModel.state(iTime, iCoupon) * uDiscount;
	 }
	 
	 return interpolate(uOption, iCoupon);
 }
 /*
 cfl::Slice liborRate(unsigned iTime, double dPeriod, cfl::InterestRateModel rModel)
 {
	 return (1. / rModel.discount(iTime, rModel.eventTimes()[iTime] + dPeriod) - 1.)/dPeriod;
 }
 
class HistRatchet: public IResetValues
{
public:
	HistRatchet(const cfl::Data::CashFlow & rCashFlow, 
							  double dLeverageFactor, 
							  const cfl::InterestRateModel & rModel) : 
	m_rModel(rModel), m_rCashFlow(rCashFlow), m_dLeverageFactor(dLeverageFactor)
	{};

	Slice resetValues(unsigned iTime, double dBeforeReset) const
	{
		return max(dBeforeReset + m_dLeverageFactor * m_rCashFlow.period * (m_rCashFlow.rate - liborRate(iTime, m_rCashFlow.period, m_rModel)), 0.0);
	}

private:
	const cfl::InterestRateModel & m_rModel;
	const cfl::Data::CashFlow & m_rCashFlow;
	double m_dLeverageFactor;
};

cfl::PathDependent histRatchet(const std::vector<unsigned> rResetIndexes,
							  const cfl::Data::CashFlow & rCashFlow, 
							  double dLeverageFactor, 
						      const cfl::InterestRateModel & rModel)
{
	double dInitialValue(rCashFlow.rate * rCashFlow.period);
	return PathDependent(new HistRatchet(rCashFlow, dLeverageFactor, rModel), rResetIndexes, dInitialValue);
}

 
cfl::MultiFunction 
  prb::ratchetInverseFloater(const cfl::Data::CashFlow & rCashFlow, 
			double dInitialCouponRate, 
			double dLeverageFactor, 
			cfl::InterestRateModel & rModel)
 {
	 std::vector<double> uEventTimes(1, rModel.initialTime());
	 std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1,
		 [&rCashFlow](double dX){return dX + rCashFlow.period;});
	 std::vector<unsigned> uResetIndexes(rCashFlow.numberOfPayments, 1);
	 std::transform(uResetIndexes.begin(), uResetIndexes.end()-1, uEventTimes.begin()+1,
		 [](unsigned iX){return iX + 1;});

	 unsigned iState = rModel.addState(histRatchet(uResetIndexes, rCashFlow, dLeverageFactor, rModel));
	 unsigned iTime(uEventTimes.size() - 1);
	 Slice uOption(rModel.cash(iTime, 0.0));
	 Slice uDiscount;

	 while(iTime > 0)
	 {
		 uDiscount = rModel.discount(iTime, uEventTimes[iTime] + rCashFlow.period);
		 uOption += rModel.state(iTime, iState) * uDiscount;
		 iTime--;
		 uOption.rollback(iTime);
	 }
	 uOption *= rCashFlow.notional;
	 return interpolate(uOption, iState);
 }*/