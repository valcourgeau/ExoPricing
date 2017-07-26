#include "Homework1/Homework1.hpp"



using namespace cfl;

class ForwardPrice : public cfl::IFunction
{
public:
	ForwardPrice(const cfl::Data::CashFlow & rBond, 
				  const cfl::Function & rDiscount, 
				  double dInitialTime, bool bClean) : 
	m_uBond(rBond), m_dInitialTime(dInitialTime), m_bClean(bClean), m_uDiscount(rDiscount)
	{}

	double operator()(double dT) const
	{
		PRECONDITION(belongs(dT));
		int index(std::floor((dT- m_dInitialTime) / m_uBond.period) + 1);
		double dPrice(0.0);
		
		for(unsigned int iI = index; iI < m_uBond.numberOfPayments; iI++)
		{
			dPrice += m_uDiscount(m_dInitialTime + iI * m_uBond.period);
		}
		dPrice *= m_uBond.rate * m_uBond.period;

		dPrice += m_uDiscount(m_dInitialTime + m_uBond.period * m_uBond.numberOfPayments);
		dPrice /= m_uDiscount(dT);

		// Compute clean price
		if(m_bClean)
		{
			double dAccrudedTime(dT - (m_dInitialTime + m_uBond.period * (index - 1)));
			ASSERT(dAccrudedTime < m_uBond.period);
			ASSERT(dAccrudedTime >= 0);
			dPrice -= m_uBond.rate*dAccrudedTime;
		}

		dPrice *= m_uBond.notional;

		return dPrice;
	}

	bool belongs(double dT) const
	{
		return (dT >= m_dInitialTime && m_uDiscount.belongs(dT) &&
			dT <= (m_dInitialTime + m_uBond.numberOfPayments * m_uBond.period));
	}

private:
	cfl::Data::CashFlow  m_uBond;
	cfl::Function m_uDiscount;
	double m_dInitialTime;
	bool m_bClean;
};

  cfl::Function prb::forwardCouponBond(const cfl::Data::CashFlow & rBond, 
				  const cfl::Function & rDiscount, 
				  double dInitialTime, bool bClean)
  {
	  return cfl::Function(new ForwardPrice(rBond, rDiscount, dInitialTime, bClean));
  }


  cfl::Function prb::forwardCouponBond(const cfl::Data::CashFlow & rBond, 
				  const cfl::Function & rDiscount, 
				  double dInitialTime, bool bClean)
  {
  }
  /*

  class ForwardPrice2 : public cfl::IFunction
{
public:
	ForwardPrice2(const cfl::Data::CashFlow & rBond, 
				  const cfl::Function & rDiscount, 
				  double dInitialTime, bool bClean) : 
	m_uBond(rBond), m_dInitialTime(dInitialTime), m_bClean(bClean), m_uDiscount(rDiscount)
	{}

	double operator()(double dT) const
	{
		PRECONDITION(belongs(dT));
		unsigned iIndex(std::floor((dT - m_dInitialTime) / m_uBond.period) + 1);
		double dPrice(0.0);

		for(unsigned iI = iIndex; iI <= m_uBond.numberOfPayments; iI++)
		{
			dPrice += m_uDiscount(m_dInitialTime + iI * m_uBond.period);
		}

		dPrice *= (m_uBond.rate * m_uBond.period);
		dPrice += m_uDiscount(m_dInitialTime + m_uBond.period * m_uBond.numberOfPayments);
		dPrice /= m_uDiscount(dT);

		if(m_bClean)
		{
			dPrice -= dPrice - m_uBond.rate * (dT - (m_dInitialTime + (iIndex-1) * m_uBond.period));
		}
		dPrice *= m_uBond.notional;
		return dPrice;
	}

	bool belongs(double dT) const
	{
		return (dT <= m_dInitialTime + m_uBond.period * m_uBond.numberOfPayments)
			&& (dT >= m_dInitialTime) && m_uDiscount.belongs(dT);
	}

private:
	cfl::Data::CashFlow m_uBond;
	cfl::Function m_uDiscount;
	double m_dInitialTime;
	bool m_bClean;
};

  */