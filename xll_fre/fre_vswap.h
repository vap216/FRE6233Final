// vswap.h - Variance swap pricing
// σ^2 = (1/n) sum_{i = 0}^{n-1} (R_i)^2 where R_i = (S_{i+1} - S_i)/S_i is the realized return over [t_i, t_{i+1}]
//     = E[f(S_n)] for f(x) = ???
#pragma once
#include <algorithm>
#include <vector>
#include "../xll/xll/ensure.h"

namespace fre::vswap {

	// Piecewise linear curve determined by (x_i, y_i) for i = 0, ..., n-1
	class pwlinear {
		std::vector<double> x;
		std::vector<double> y;
		// x[i] <= x_ < x[i+1]
		size_t index(double x_) const
		{
			return std::lower_bound(x.begin(), x.end(), x_) - x.begin();
		}
	public:
		pwlinear(size_t n, const double* x_, const double* y_)
			: x(x_, x_ + n), y(y_, y_ + n)
		{
			ensure(n >= 2);
			ensure(std::is_sorted(x.begin(), x.end()));
		}
		// value at x
		double value(double x_) const
		{
			size_t i = index(x_);
			if (i == x.size() - 1) {
				ensure(x_ > x.back());
				--i; // use last two points
			}
			double m = (y[i + 1] - y[i]) / (x[i + 1] - x[i]);

			return y[i] + m * (x_ - x[i]);
		}

		// first derivative at x
		double derivative(double x_) const
		{
			size_t i = index(x_);
			if (i == x.size() - 1) {
				ensure(x_ > x.back());
				--i; // use last two points
			}
			double m = (y[i + 1] - y[i]) / (x[i + 1] - x[i]);
			return m; // derivative returns the slope of the piecewise line
		}

		// second derivative at x[1], ..., x[n-2]
		std::vector<double> delta()
		{
			std::vector<double> deltas;
			deltas.push_back(0.0); // 0 at beginning of array
			for (int i = 0; i < x.size() - 2; i++) {
				double delta_val = derivative(x[i + 1]) - derivative(x[i]); // delta function pushes back the magnitude of the slope change at given points
				deltas.push_back(delta_val); 
			}
			deltas.push_back(0.0); // 0 at end of array
			return deltas; 
		}
	};

	// par variance given strikes, put, and call prices
	// use put prices for strikes < forward and call prices for strikes >= forward
	double variance(double f, size_t n, const double* k, const double* p, double* c)
	{	
		double var_swap = 0; // value of variance swap
		double* v = new double[n]; // initialize v(x) function to get y vector for pwlinear curve
		for (size_t i = 0; i < n; i++) {
			v[i] = -2 * log(k[i] / f) + 2 * (k[i] - f) / f; // populate v(x) function
		}
		pwlinear func(n,k,v); // instantiate piecewise linear curve
		delete[] v;
		for (size_t i = 0; i < n; i++) {
			if (k[i] < f) { 
				var_swap = var_swap + p[i] * func.delta()[i]; // if strike is less than forward, multiply put price by the delta function and add to total var_swap 
			}
			else {
				var_swap = var_swap + c[i] * func.delta()[i]; // if strike is greater than or equal to forward, multiply call price by the delta function and add to total var_swap
			}
		}
		return var_swap; // final var_swap value
	}

} // namespace fre::vswap
