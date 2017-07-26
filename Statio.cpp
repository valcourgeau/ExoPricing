#include "Homework1/Homework1.hpp"
#include <math.h>
#include <numeric>

using namespace cfl;
using namespace cfl::Data;

 cfl::Function prb::volatilityFitBlack(const std::vector<double> & rMaturities, 
				   const std::vector<double> & rVolatilities, 
				   double dLambda, double dInitialTime)
 {
	 PRECONDITION(rMaturities.size() == rVolatilities.size());
	 PRECONDITION(rMaturities.size() > 0);
	 PRECONDITION(dLambda > 0);
	 PRECONDITION(rMaturities.front() > dInitialTime);

	 std::vector<double> uVols(rMaturities.size());
	 Function uShape = volatility(1.,dLambda,dInitialTime) * assetShape(dLambda,dInitialTime);
	 
	 std::transform(rMaturities.begin(), rMaturities.end(), uVols.begin(), uShape);

	 double dCov = std::inner_product(rVolatilities.begin(),rVolatilities.end(),
		 uVols.begin(),0.);
	 double dVar = std::inner_product(uVols.begin(),uVols.end(), uVols.begin(),0.); 
	 double dKappa = dCov / dVar;
	 uShape *= dKappa;

	 return uShape;
 }


  cfl::Function prb::volatilityFitBlack(const std::vector<double> & rMaturities, 
				   const std::vector<double> & rVolatilities, 
				   double dLambda, double dInitialTime)
 {
	 std::vector<double> uVol(rVolatilities.size()), uAssetShape(rVolatilities.size());
	 std::transform(rMaturities.begin(), rMaturities.end(), uVol.begin(), 
		 cfl::Data::volatility(1.0, dLambda, dInitialTime));
	 std::transform(rMaturities.begin(), rMaturities.end(), uAssetShape.begin(), 
		 cfl::Data::assetShape(dLambda, dInitialTime));
	 std::transform(uAssetShape.begin(), uAssetShape.end(), uVol.begin(),
		 uVol.begin(), std::multiplies<double>());

	 double dVar = std::inner_product(uVol.begin(), uVol.end(), uVol.begin(), 0.0);
	 double dCov = std::inner_product(uVol.begin(), uVol.end(), rVolatilities.begin(), 0.0);

	 return dCov / dVar * cfl::Data::assetShape(dLambda, dInitialTime) 
		 * cfl::Data::volatility(1.0, dLambda, dInitialTime);
 }