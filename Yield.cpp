#include "Session1/Session1.hpp"

using namespace cfl;

class Yield : public cfl::IFunction
{
public:
	Yield(const cfl::Function & rDiscount, double dInitialTime) : 
		m_uDiscount(rDiscount), m_dInitialTime(dInitialTime)
	{}

	double operator()(double dT) const
	{
		PRECONDITION(belongs(dT));
		double dYield;

		if(dT < m_dInitialTime + c_dEps)
		{
			dYield = (1. - m_uDiscount(m_dInitialTime + c_dEps)) / c_dEps;
		} else
		{
			dYield = -std::log(m_uDiscount(dT)) / (dT-m_dInitialTime);
		}

		return dYield;
	}

	bool belongs(double dT) const
	{
		return m_uDiscount.belongs(dT);
	}

private:
	cfl::Function m_uDiscount;
	double m_dInitialTime;
};


 cfl::Function prb::yield(const cfl::Function & rDiscount, double dInitialTime)
 {
	 return cfl::Function(new Yield(rDiscount, dInitialTime));
 }
 


 /*
class Yield : public cfl::IFunction
{
public:
	Yield(const cfl::Function & rDiscount, double dInitialTime) : 
		m_uDiscount(rDiscount), m_dInitialTime(dInitialTime)
	{}

	double operator()(double dT) const
	{
		double dYield(0.0);
		PRECONDITION(belongs(dT));
		if(dT < m_dInitialTime + c_dEps)
		{
			dYield = (1. - m_uDiscount(m_dInitialTime + c_dEps)) / c_dEps;
		} else
		{
			dYield = -std::log(m_uDiscount(dT)) / (dT-m_dInitialTime);
		}
		return dYield;
	}

	bool belongs(double dT) const
	{
		return m_uDiscount.belongs(dT);
	}

private:
	cfl::Function m_uDiscount;
	double m_dInitialTime;
};

 cfl::Function prb::yield(const cfl::Function & rDiscount, double dInitialTime)
 {
 }*/