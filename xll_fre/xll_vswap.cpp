// vswap.cpp
#include "fre_vswap.h"
#include "xll_fre.h"

using namespace xll;

//!!! implement vswap::variance
AddIn xai_vswap(
	Function(XLL_DOUBLE, "xll_vswap", "XLL.VSWAP")
	.Arguments({
		Arg(XLL_DOUBLE, "f", "is the forward price."),
		Arg(XLL_FPX, "k", "is an array of strikes."),
		Arg(XLL_FPX, "p", "is an array of put prices."),
		Arg(XLL_FPX, "c", "is an array of call prices."),
		})
		.Category(CATEGORY)
	.FunctionHelp("Return the price of a variance swap.")
);

double WINAPI xll_vswap(double f_, _FPX* k_, _FPX* p_, _FPX* c_)
{
#pragma XLLEXPORT
	try {
		ensure(size(*k_) == size(*p_));
		ensure(size(*k_) == size(*c_));
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
	}
		
	return fre::vswap::variance(f_, size(*k_), k_->array, p_->array, c_->array);
}