/*
 * This file is part of LinkPred.
 *
 * LinkPred: A high performance library for link prediction in complex networks.
 * Copyright (C) 2017  by Said Kerrache.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * \file
 * @ingroup ML
 * @brief Contains the implementation of a logistic regression optimization problem.
 */

#ifndef LOGREGCG_HPP_
#define LOGREGCG_HPP_

#include "linkpred/numerical/cg/cgdescent.hpp"
#include "linkpred/utils/randomgen.hpp"
#include "linkpred/numerical/linear/vec.hpp"
#include <vector>

namespace LinkPred {

/**
 * @brief Logistic regression optimization problem.
 * @tparam InRndIt Input (features) iterator type.
 * @tparam OutRndIt Output (class) iterator type.
 */
template<typename InRndIt = typename std::vector<Vec>::iterator,
		typename OutRndIt = typename std::vector<bool>::iterator> class LogRegCG: public CG::CGDProblem {

protected:
	InRndIt trInBegin; /**< Iterator to the first example features (input). */
	InRndIt trInEnd; /**< Iterator to one-past-the-last example features (input). */
	OutRndIt trOutBegin; /**< Iterator to the first example class (output). */
	OutRndIt trOutEnd; /**< Iterator to one-past-the-last example class (output). */
	double lambda; /**< Regularization coefficient. */
	long int seed; /**< Seed of the random number generator. */
	RandomGen rng; /**< A random number generator. */
	std::size_t m = 0; /**< The number of features. */

public:

	/**
	 * Constructor.
	 * @param trInBegin Iterator to the first example features (input).
	 * @param trInEnd Iterator to one-past-the-last example features (input).
	 * @param trOutBegin Iterator to the first example class (output).
	 * @param trOutEnd Iterator to one-past-the-last example class (output).
	 * @param lambda Regularization coefficient.
	 * @param seed The random number generator's seed.
	 */
	LogRegCG(InRndIt trInBegin, InRndIt trInEnd, OutRndIt trOutBegin,
			OutRndIt trOutEnd, double lambda, long int seed) :
			trInBegin(trInBegin), trInEnd(trInEnd), trOutBegin(trOutBegin), trOutEnd(
					trOutEnd), lambda(lambda), seed(seed), rng(seed) {
	}

	/**
	 * Deleted copy constructor.
	 * @param that The object to copy.
	 */
	LogRegCG(const LogRegCG & that) = delete;

	/**
	 * Deleted copy assignment operator.
	 * @param that The object to copy.
	 */
	LogRegCG& operator=(const LogRegCG & that) = delete;

	/**
	 * Deleted move constructor.
	 * @param that The object to move.
	 */
	LogRegCG(LogRegCG && that) = delete;

	/**
	 * Deleted move assignment operator.
	 * @param that The object to move.
	 */
	LogRegCG& operator=(LogRegCG && that) = delete;

	/**
	 * Initializes theta.
	 * @param theta The variables.
	 * @param n The size.
	 */
	virtual void init(double * theta, CG::INT n);

	/**
	 * Objective function.
	 * @param theta The variables.
	 * @param n The size.
	 * @return The value of the objective.
	 */
	virtual double f(double * theta, CG::INT n);

	/**
	 * Gradient.
	 * @param grad_f The gradient (output).
	 * @param theta The variables.
	 * @param n The size.
	 */
	virtual void grad(double * grad_f, double * theta, CG::INT n);

	/**
	 * Compute objective and gradient at the same time. This is a default
	 * implementation that should be overloaded if needed.
	 * @param grad_f The gradient (output).
	 * @param theta The variables.
	 * @param n The size.
	 * @return The value of the objective.
	 */
	virtual double fgrad(double * grad_f, double * theta, CG::INT n);

	/**
	 * Finalize the solution.
	 * @param theta The variables.
	 * @param n The size.
	 */
	virtual void finalize(double * theta, CG::INT n) {
	}

	/**
	 * @return The regularization coefficient.
	 */
	double getLambda() const {
		return lambda;
	}

	/**
	 * Set the regularization coefficient.
	 * @param lambda The new regularization coefficient.
	 */
	void setLambda(double lambda) {
		this->lambda = lambda;
	}

	/**
	 * Destructor.
	 */
	virtual ~LogRegCG() = default;

};

} /* namespace LinkPred */

#endif /* LOGREGCG_HPP_ */
