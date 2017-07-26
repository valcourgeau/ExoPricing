#include "SampleExam2/SampleExam2.hpp"

using namespace cfl;  

cfl::MultiFunction prb::resetCouponPutBond(const cfl::Data::CashFlow & rBond,  
					double dResetCouponRate,
					double dRedemptionPrice,
					cfl::InterestRateModel & rModel)
  {
	  PRECONDITION(rBond.rate > dResetCouponRate);
	  PRECONDITION(dRedemptionPrice < 1); // as given in the script
	  double dPeriod(rBond.period);
	  std::vector<double> uEventTimes(rBond.numberOfPayments);
	  uEventTimes.front() = rModel.initialTime();
	  std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1,
		  [dPeriod](double dX){return dX + dPeriod;});
	  rModel.assignEventTimes(uEventTimes);

	  unsigned iTime(uEventTimes.size() - 1);
	  
	  std::vector<Slice> uOptionArray(2);

	  uOptionArray.front() = rModel.cash(iTime, 1.0 + rBond.rate * rBond.period);
	  uOptionArray.front() = rModel.discount(iTime, uEventTimes[iTime] + dPeriod);
	  uOptionArray.back() = rModel.cash(iTime, 1.0 + dResetCouponRate* rBond.period);
	  uOptionArray.back() = rModel.discount(iTime, uEventTimes[iTime] + dPeriod);

	  iTime--;
      uOptionArray.front().rollback(iTime);
	  uOptionArray.back().rollback(iTime);

	  while(iTime > 0)
	  {
		  // What about redeeming?
		  uOptionArray.back() = max(uOptionArray.back(), dRedemptionPrice);
		  // Issuer wants to reduce price: is it better to redeem 
		  // the not already redeemed option?
		  uOptionArray.front() = min(uOptionArray.front(), uOptionArray.back());

		  uOptionArray.front() += rBond.rate * dPeriod;
		  uOptionArray.back() += dResetCouponRate * dPeriod;

		  iTime--;
		  uOptionArray.front().rollback(iTime);
		  uOptionArray.back().rollback(iTime);
	  }


	  uOptionArray.front() *= rBond.notional;
	  uOptionArray.back() *= rBond.notional;
	  return interpolate(uOptionArray.front());
  }

/*
cfl::MultiFunction prb::resetCouponPutBond(const cfl::Data::CashFlow & rBond,  
					double dResetCouponRate,
					double dRedemptionPrice,
					cfl::InterestRateModel & rModel)
 {
	 PRECONDITION(dRedemptionPrice < 1.);
	 PRECONDITION(dResetCouponRate < rBond.rate);
	 double dPeriod(rBond.period);
	 std::vector<double> uEventTimes(rBond.numberOfPayments);
	 uEventTimes.front() = rModel.initialTime();
	 std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1,
		 [dPeriod](double dX){return dX + dPeriod;});
	 rModel.assignEventTimes(uEventTimes);

	 unsigned iTime(uEventTimes.size() - 1);
	 double dTime(uEventTimes[iTime] + dPeriod);
	 Slice uDiscount = rModel.discount(iTime, dTime);
	 Slice uNotReset = rModel.cash(iTime, 1. + rBond.rate * dPeriod) * uDiscount;
	 Slice uReset = rModel.cash(iTime, 1. + dResetCouponRate * dPeriod) * uDiscount;
	

	 while(iTime > 0)
	 {
		//dTime = uEventTimes[iTime] + dPeriod;
		//uDiscount = rModel.discount(iTime, dTime);
		uReset = max(uReset, dRedemptionPrice);
		uNotReset = min(uReset, uNotReset);
		uNotReset += rBond.period*rBond.rate;
		uReset += dResetCouponRate*rBond.rate;
		iTime--;
		uNotReset.rollback(iTime);
		uReset.rollback(iTime);
	 }

	 uNotReset *= rBond.notional;
	 uReset *= rBond.notional;

	 return interpolate(uNotReset);
}*/