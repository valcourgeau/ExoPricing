#include "SampleExam1/SampleExam1.hpp"

using namespace cfl;

class ForwardPrice : public cfl::IFunction
{
public:
	ForwardPrice(double dSpot, 
			std::vector<double> & rFixedDividendsTimes, 
			std::vector<double> & rFixedDividends, 
			const cfl::Function & rDiscount, 
			double dInitialTime) : 
	m_uTimes(rFixedDividendsTimes), m_uFixedDividends(rFixedDividends),
	 m_dSpot(dSpot), m_uDiscount(rDiscount)
	{
		// PRECONDITION(m_uTimes.size() == rFixedDividendsTimes.size());
		m_uTimes.insert(m_uTimes.begin(), dInitialTime);
	}

	double operator()(double dT) const
	{
		PRECONDITION(belongs(dT));
		double dPrice(m_dSpot/m_uDiscount(dT));
		unsigned iTimeIndex(1);
		std::vector<double>::iterator itTime = std::upper_bound(m_uTimes.begin(), m_uTimes.end(), dT);
		
		/*// We want to include the last dividend if we are at its delivery time
		if(std::binary_search(m_uTimes.begin()+1, m_uTimes.end(), dT))
		{
			itTime += 1;
		}
		*/
		for(std::vector<double>::iterator it = (m_uTimes.begin()+1); it < itTime; it++)
		{
			dPrice -= m_uFixedDividends[iTimeIndex] / m_uDiscount(dT - *it + m_uTimes.front());
			iTimeIndex++;
		}
		

		return dPrice;
	}

	bool belongs(double dT) const
	{
		// check if t_0 <= dT <= t_m
		return (dT >= m_uTimes.front() && m_uDiscount.belongs(dT) &&
			(dT <= m_uTimes.back()));
	}

private:
	cfl::Function m_uDiscount;
	std::vector<double> & m_uTimes, m_uFixedDividends;
	double m_dInitialTime, m_dSpot;
};



cfl::Function 
  prb::forwardStockDividends(double dSpot, 
			std::vector<double> & rFixedDividendsTimes, 
			std::vector<double> & rFixedDividends, 
			const cfl::Function & rDiscount, 
			double dInitialTime)
  {
	  PRECONDITION(rFixedDividendsTimes.front() > dInitialTime);
	  PRECONDITION(rFixedDividends.size(), rFixedDividendsTimes.size());
	  PRECONDITION(std::equal(rFixedDividendsTimes.begin()+1, rFixedDividendsTimes.end(),
		  rFixedDividendsTimes.begin(), std::greater<double>()));

	  return Function(new ForwardPrice(dSpot, rFixedDividendsTimes, rFixedDividends, rDiscount, dInitialTime));
  }


class ForwardPrice : public cfl::IFunction
{
public:
	ForwardPrice(double dSpot, std::vector<double> & rFixedDividendsTimes, 
			std::vector<double> & rFixedDividends, 
			const cfl::Function & rDiscount, 
			double dInitialTime) : 
	m_uTimes(rFixedDividendsTimes), m_dInitialTime(dInitialTime), m_uDiscount(rDiscount), m_uFixedDividends(rFixedDividends), m_dSpot(dSpot)
	{
		PRECONDITION(m_uTimes.size() == m_uFixedDividends.size());
		//m_uTimes.insert(m_uTimes.begin(), dInitialTime);
	}

	double operator()(double dT) const
	{
		PRECONDITION(belongs(dT));
		double dPrice(m_dSpot);

		unsigned iNumberDivToBePaid = std::upper_bound(m_uFixedDividends.begin(),
			m_uFixedDividends.end(), dT) - m_uFixedDividends.begin();
		for(unsigned iI = iNumberDivToBePaid;  iI < m_uFixedDividends.size(); iI++)
		{
			dPrice -= m_uFixedDividends[iI] * m_uDiscount(m_uTimes[iI]);
		}

		dPrice /= m_uDiscount(m_uTimes.back());
		return dPrice;
	}

	bool belongs(double dT) const
	{
		// check if t_0 <= dT <= t_m
		return m_uDiscount.belongs(dT) && (dT >= m_dInitialTime) 
			&& (dT <= m_uTimes.back());
	}

private:
	cfl::Function m_uDiscount;
	std::vector<double> & m_uTimes, m_uFixedDividends;
	double m_dInitialTime, m_dSpot;
};



cfl::Function 
  prb::forwardStockDividends(double dSpot, 
			std::vector<double> & rFixedDividendsTimes, 
			std::vector<double> & rFixedDividends, 
			const cfl::Function & rDiscount, 
			double dInitialTime)
  {
	  return Function( new ForwardPrice(dSpot, rFixedDividendsTimes, rFixedDividends, rDiscount, dInitialTime));
}