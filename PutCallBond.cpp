#include "Homework3/Homework3.hpp"

using namespace cfl;

/*
cfl::Slice couponBond(unsigned iTime, const cfl::Data::CashFlow & rCouponBond,
				 cfl::InterestRateModel & rModel)
{
	Slice uBond = rModel.cash(iTime, 0.0);
	double dPaymentTime = rModel.eventTimes()[iTime];
	for(unsigned iI = 0; iI < rCouponBond.numberOfPayments; iI++)
	{
		dPaymentTime += rCouponBond.period;
		uBond += rModel.discount(iTime, dPaymentTime);
	}

	uBond *= (rCouponBond.period * rCouponBond.rate);
	uBond += rModel.discount(iTime, dPaymentTime);
	uBond *= rCouponBond.notional;

	return uBond;
}

cfl::MultiFunction prb::putCallBond(const cfl::Data::CashFlow & rCouponBond,
				 double dRedemptionPrice,
				 double dRepurchasePrice, 
				 cfl::InterestRateModel & rModel)
{
	PRECONDITION(dRedemptionPrice < 1);
	PRECONDITION(dRepurchasePrice > 1);

	double dPeriod(rCouponBond.period);
	bool bTerminated(false);
	std::vector<double> uEventTimes(rCouponBond.numberOfPayments);
	uEventTimes.front() = rModel.initialTime();
	std::transform(uEventTimes.begin(), uEventTimes.end()-1, 
		uEventTimes.begin()+1, [dPeriod](double dX){return dX + dPeriod;});
	rModel.assignEventTimes(uEventTimes);

	int iTime(uEventTimes.size() - 1);
	double dCoupon = rCouponBond.rate * rCouponBond.period;
	Slice uDiscount = rModel.discount(iTime, rModel.eventTimes()[iTime] + dPeriod);
	Slice uOption = uDiscount * (1. + dCoupon);

	while(iTime > 0)
	{
		uOption = min(uOption, dRepurchasePrice);
		uOption = max(uOption, dRedemptionPrice);
		
		uDiscount = rModel.discount(iTime, rModel.eventTimes()[iTime] + dPeriod);	
		
		// WHY DON'T WE DISCOUNT?
		uOption += dCoupon;

		iTime--;
		uOption.rollback(iTime);
	}
	
	uOption *= rCouponBond.notional;

	return interpolate(uOption);
}*/

cfl::MultiFunction prb::putCallBond(const cfl::Data::CashFlow & rCouponBond,
				 double dRedemptionPrice,
				 double dRepurchasePrice, 
				 cfl::InterestRateModel & rModel)
{
	//PRECONDITION(dRedemptionPrice <1.);
	//PRECONDITION(dRedemptionPrice > 1.);
	double dPeriod(rCouponBond.period);
	std::vector<double> uEventTimes(rCouponBond.numberOfPayments + 1);
	uEventTimes.front() = rModel.initialTime();
	std::transform(uEventTimes.begin(), uEventTimes.end()-1, uEventTimes.begin()+1,
		[dPeriod](double dX){return dX +  dPeriod;});
	rModel.assignEventTimes(uEventTimes);

	unsigned iTime(uEventTimes.size() - 1);
	Slice uOption = rModel.cash(iTime, 1.0);
	uOption += (rCouponBond.period * rCouponBond.rate);
	iTime--;
	uOption.rollback(iTime);

	while(iTime > 0)
	{
		uOption = min(dRepurchasePrice, uOption);
		uOption = max(dRedemptionPrice, uOption);
		uOption += (rCouponBond.period * rCouponBond.rate);
		iTime--;
		uOption.rollback(iTime);
	}

	uOption *= rCouponBond.notional;
	return interpolate(uOption);
}
