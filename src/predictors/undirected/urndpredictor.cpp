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

#include <linkpred/predictors/undirected/urndpredictor.hpp>

namespace LinkPred {


template<typename Network, typename EdgeRndIt,
		typename ScoreRndIt, typename EdgeRndOutIt> void URNDPredictor<
		Network, EdgeRndIt, ScoreRndIt,
		EdgeRndOutIt>::predict(EdgeRndIt begin,
		EdgeRndIt end, ScoreRndIt scores) {

// Parallelism is dangerous here because of the random number generator
//#pragma omp parallel for if parallel
	for (auto it = begin; it < end; ++it) {
		*(scores + (it - begin)) = rng.getDouble(0.0, 1.0);
	}
}

template<typename Network, typename EdgeRndIt,
		typename ScoreRndIt, typename EdgeRndOutIt> double URNDPredictor<
		Network, EdgeRndIt, ScoreRndIt,
		EdgeRndOutIt>::score(Edge const & e) {
	return rng.getDouble(0.0, 1.0);
}

#define URNDPREDICTOR_CPP
#include "linkpred/instantiations.hpp"
#undef URNDPREDICTOR_CPP


} /* namespace LinkPred */
