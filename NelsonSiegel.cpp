#include "Session1/Session1.hpp"

using namespace cfl;


class DiscountNelsonSiegel : public cfl::IFunction
{
public:
	DiscountNelsonSiegel(double dZ1, double dZ2, 
		double dLambda, double dInitialTime) : m_dZ1(dZ1), m_dZ2(dZ2), m_dLambda(dLambda), m_dInitialTime(dInitialTime)
	{}

	double operator()(double dT) const
	{
		PRECONDITION(belongs(dT));
		double dDNS(m_dZ1 + m_dZ2 * std::exp(- m_dLambda * (dT-m_dInitialTime)));
		return std::exp(- dDNS * (dT - m_dInitialTime));
	}

	bool belongs(double dT) const
	{
		return (dT >= m_dInitialTime);
	}

private:
	double m_dInitialTime, m_dZ1, m_dZ2, m_dLambda;
};

cfl::Function 
  prb::discountNelsonSiegel(double dZ1, double dZ2, 
		       double dLambda, double dInitialTime)
  {
	  return cfl::Function(new DiscountNelsonSiegel(dZ1, dZ2, dLambda, dInitialTime));
  }


/*
class DiscountNelsonSiegel2 : public cfl::IFunction
{
public:
	DiscountNelsonSiegel2(double dZ1, double dZ2, 
		       double dLambda, double dInitialTime) : 
	m_dZ1(dZ1), m_dZ2(dZ2), m_dLambda(dLambda), m_dInitialTime(dInitialTime)
	{}

	double operator()(double dT) const
	{
		PRECONDITION(belongs(dT));
		return(std::exp(-(m_dZ1 + m_dZ2 * cfl::Data::assetShape(m_dLambda, m_dInitialTime)(dT)) * (dT - m_dInitialTime)));
	}

	bool belongs(double dT) const
	{
		return dT >= m_dInitialTime;
	}

	private:
	double m_dInitialTime, m_dZ1, m_dZ2, m_dLambda;
};
	cfl::Function 
  prb::discountNelsonSiegel(double dZ1, double dZ2, 
		       double dLambda, double dInitialTime)
  {
	  return cfl::Function(new DiscountNelsonSiegel2(dZ1, dZ2, dLambda, dInitialTime));
	}*/