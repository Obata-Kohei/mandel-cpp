#ifndef TYPES_HPP
#define TYPES_HPP

#include <complex>
#include <boost/multiprecision/mpfr.hpp>
#include <boost/multiprecision/mpc.hpp>

using Float = boost::multiprecision::mpfr_float;
//using Float = double;
using Complex = boost::multiprecision::mpc_complex;
//using Complex = std::complex<double>;

#endif  // TYPES_HPP