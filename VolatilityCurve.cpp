#include "SampleExam3/SampleExam3.hpp"

using namespace cfl;

cfl::Function 
  prb::volatilityLinearInterpOfVar(const std::vector<double> & rMaturities, 
			      const std::vector<double> & rVolatilities, 
			      double dInitialTime)
  {
	  PRECONDITION(dInitialTime < rMaturities.front());
	  PRECONDITION(rMaturities.size() == rVolatilities.size());

	  std::vector<double> uVolSq(rVolatilities), 
		  uDiffTimes(rMaturities);
	  uVolSq.insert(uVolSq.begin(), rVolatilities.front());
	  std::transform(uVolSq.begin(), uVolSq.end(), uVolSq.begin(),
		  [](double dX){return dX*dX;});

	  uDiffTimes.insert(uDiffTimes.begin(), dInitialTime); // to have constant vol on [t_0;t_1]
	  std::vector<double>  uTimes(uDiffTimes); // Saves the times for lin interpolation
	  std::transform(uDiffTimes.begin(), uDiffTimes.end(), uDiffTimes.begin(),
		  [dInitialTime](double dX){return dX-dInitialTime;});
	  ASSERT(std::abs(uDiffTimes.front()) < c_dEps);

	  cfl::Interp uLinear = NInterp::linear();

	  // element-wise multiplication to get Volatility curve
	  std::transform(uDiffTimes.begin(), uDiffTimes.end(),
		  uVolSq.begin(), uVolSq.begin(), std::multiplies<double>());
	  double dTest(rVolatilities[0] * rVolatilities[0] * (rMaturities[0] - dInitialTime));
	  ASSERT(std::abs(uVolSq[1] - (rVolatilities[0] * rVolatilities[0] * (rMaturities[0] - dInitialTime))) < c_dEps);

	  return uLinear.interpolate(uTimes.begin(), uTimes.end(), uVolSq.begin());
  }

/*
class VolatilityFromVariance : public cfl::IFunction
{
public:
	VolatilityFromVariance(cfl::Function & rVariance, double dInitialTime) : 
		m_uVariance(rVariance), m_dInitialTime(dInitialTime)
	{}

	double operator()(double dT) const
	{
		PRECONDITION(belongs(dT));
		return std::sqrt(m_uVariance(dT) / (dT - m_dInitialTime));
	}

	bool belongs(double dT) const
	{
		return (dT > m_dInitialTime);
	}

private:
	cfl::Function & m_uVariance;
	double m_dInitialTime,;
};

cfl::Function 
  prb::volatilityLinearInterpOfVar(const std::vector<double> & rMaturities, 
			      const std::vector<double> & rVolatilities, 
			      double dInitialTime)
{
	std::vector<double> uMaturities(rMaturities);
	uMaturities.insert(uMaturities.begin(), dInitialTime);
	std::vector<double> uVol(rVolatilities.size() + 1, 0.0);
	std::transform(uVol.begin(), uVol.end(), uVol.begin(),
		[](double dX){return dX*dX;});
	std::transform(uMaturities.begin()+1, uMaturities.end(), uVol.begin()+1,
		uVol.begin()+1,[dInitialTime](double dT, double dX){return (dT-dInitialTime)*dX;});

	cfl::Interp uLinear = NInterp::linear();
	cfl::Function uLog = uLinear.interpolate(uMaturities.begin(), uMaturities.end(), 
		uVol.begin());

	return Function(new VolatilityFromVariance(uLog, dInitialTime));
}*/