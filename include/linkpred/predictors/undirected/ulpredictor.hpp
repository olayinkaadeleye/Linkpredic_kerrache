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
 * @ingroup Predictors
 * @brief Contains the interface of a link predictor.
 */

#ifndef LPredictor_HPP_
#define LPredictor_HPP_

#include "LinkPredConfig.hpp"
#include "linkpred/core/unetwork/unetwork.hpp"
#include "linkpred/core/ds/lmapqueue.hpp"
#ifdef LINKPRED_WITH_OPENMP
#include <omp.h>
#endif
#ifdef LINKPRED_WITH_MPI
#include <mpi.h>
#include "linkpred/utils/miscutils.hpp"
#endif
#include <memory>
#include <map>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <queue>
#include <set>

namespace LinkPred {

/**
 * @brief The interface of a link predictor in an undirected network.
 * @tparam NetworkT The network type.
 * @tparam EdgeRndItT A random iterator type used to iterate on edges.
 * @tparam ScoreRndItT A random iterator type used to iterate on scores.
 * @param EdgeRndOutItT A random output iterator to write edges.
 */
template<typename NetworkT = UNetwork<>,
		typename EdgeRndItT = typename std::vector<typename NetworkT::Edge>::const_iterator,
		typename ScoreRndItT = typename std::vector<double>::iterator,
		typename EdgeRndOutItT = typename std::vector<typename NetworkT::Edge>::iterator> class ULPredictor {
public:
	using Network = NetworkT; /**< The network type. */
	using EdgeRndIt = EdgeRndItT; /**< A random iterator type used to iterate on edges. */
	using ScoreRndIt = ScoreRndItT; /**< A random iterator type used to iterate on scores. */
	using EdgeRndOutIt = EdgeRndOutItT; /**< A random output iterator to write edges. */
	using NodeID = typename Network::NodeID; /**< The node IDs type. */
	using Edge = typename Network::Edge; /**< The edges type. */

protected:
	std::shared_ptr<Network const> net; /**< The network. */
	std::string name; /**< The name of the predictor. */
#ifdef LINKPRED_WITH_OPENMP
	bool parallel = false; /**< Enable/disable shared-memory parallelism. */
#endif
#ifdef LINKPRED_WITH_MPI
	// TODO add distributed attribute to all link predictors and update top methods.
	bool distributed = false; /**< Enable/disable distributed parallelism. */
	MPI_Comm comm = MPI_COMM_WORLD; /**< The MPI communicator. */
#endif

	/**
	 * Find the top scored edges when the score is a monotone function of the degrees.
	 */
	std::size_t topDegMonotone(std::size_t l, EdgeRndOutIt eit,
			ScoreRndIt sit) {

		// Copy and sort degrees
		std::vector<std::pair<NodeID, std::size_t>> deg;
		deg.resize(net->getNbNodes());
		for (auto it = net->nodesDegBegin(); it < net->nodesDegEnd(); ++it) {
			deg[it - net->nodesDegBegin()] = *it;
		}
		std::sort(deg.begin(), deg.end(),
				[](auto a, auto b) {return a.second > b.second;});

		std::size_t nbNodes = deg.size();
		std::set<std::pair<std::size_t, std::size_t>> inq;

		struct Comparator {
			bool operator ()(
					std::pair<double, std::pair<std::size_t, std::size_t>> const & left,
					std::pair<double, std::pair<std::size_t, std::size_t>> const & right) {
				return left.first < right.first;
			}
		};

		std::priority_queue<
				std::pair<double, std::pair<std::size_t, std::size_t>>,
				std::vector<
						std::pair<double, std::pair<std::size_t, std::size_t>>>,
				Comparator> pq;
		pq.push(
				std::make_pair(
						this->score(net->makeEdge(deg[0].first, deg[1].first)),
						std::make_pair(0, 1)));
		inq.insert(std::make_pair(0, 1));
		std::size_t cpt = 0;
		while (cpt < l && !pq.empty()) {
			auto el = pq.top();
			pq.pop();
//			inq.erase(el.second); // Cannot erase here, otherwise we'll have repeated visits
			auto ii = deg[el.second.first].first;
			auto jj = deg[el.second.second].first;
			if (!net->isEdge(ii, jj)) {
				*eit = net->makeEdge(ii, jj);
				++eit;
				*sit = el.first;
				++sit;
				cpt++;
				if (cpt == l) {
					break;
				}
			}
			std::size_t i = el.second.first;
			std::size_t j = el.second.second;
			if (j < nbNodes - 1) {
				auto e = std::make_pair(i, j + 1);
				if (inq.count(e) == 0) {
					pq.push(
							std::make_pair(
									this->score(
											net->makeEdge(deg[i].first,
													deg[j + 1].first)), e));
					inq.insert(e);
				}
			}
			if (i < nbNodes - 1 && i + 1 < j) {
				auto e = std::make_pair(i + 1, j);
				if (inq.count(e) == 0) {
					pq.push(
							std::make_pair(
									this->score(
											net->makeEdge(deg[i + 1].first,
													deg[j].first)),
									std::make_pair(i + 1, j)));
					inq.insert(e);
				}
			}
		}

		return cpt;
	}

public:
	/**
	 * Constructor.
	 * @param net The network.
	 */
	ULPredictor(std::shared_ptr<Network const> net) :
			net(net) {
	}

	/**
	 * Copy constructor.
	 * @param that The object to copy.
	 */
	ULPredictor(ULPredictor const & that) = default;

	/**
	 * Copy assignment operator.
	 * @param that The object to copy.
	 */
	ULPredictor & operator =(ULPredictor const & that) = default;

	/**
	 * Move constructor.
	 * @param that The object to move.
	 */
	ULPredictor(ULPredictor && that) = default;

	/**
	 * Move assignment operator.
	 * @param that The object to move.
	 */
	ULPredictor & operator =(ULPredictor && that) = default;

#ifdef LINKPRED_WITH_OPENMP
	/**
	 * @return Whether shared memory parallelism is enabled.
	 */
	bool isParallel() const {
		return parallel;
	}

	/**
	 * Enable/disable shared memory parallelism.
	 * @param parallel True to enable parallelism, false to disable it.
	 */
	void setParallel(bool parallel) {
		this->parallel = parallel;
	}
#endif

#ifdef LINKPRED_WITH_MPI
	/**
	 * @return Whether distributed memory parallelism is enabled.
	 */
	bool isDistributed() const {
		return distributed;
	}

	/**
	 * Enable/disable distributed memory parallelism.
	 * @param distributed True to enable distributed memory parallelism, false to disable it.
	 */
	void setDistributed(bool distributed) {
		this->distributed = distributed;
	}

	/**
	 * @return The size of the local portion of non-existing links that will be predicted by the current processor.
	 */
	std::size_t localSize() {
		int nbProcs;
		int procID;
		if (distributed) {
			MPI_Comm_size(comm, &nbProcs);
			MPI_Comm_rank(comm, &procID);
		} else {
			nbProcs = 1;
			procID = 0;
		}

		std::size_t l = net->getNbNonEdges() / nbProcs;
		if (static_cast<std::size_t>(procID) < net->getNbNonEdges() % nbProcs) {
			l++;
		}
		return l;
	}
#endif

	/**
	 * Initialize the solver.
	 */
	virtual void init() = 0;

	/**
	 * Learning.
	 */
	virtual void learn() = 0;

	/**
	 * Predict links.
	 * @param begin Iterator to the first edge to be predicted.
	 * @param end end Iterator to one past the last edge to be predicted.
	 * @param scores Random output iterator to store the scores.
	 */
	virtual void predict(EdgeRndIt begin, EdgeRndIt end, ScoreRndIt scores) {
#ifdef LINKPRED_WITH_OPENMP
#pragma omp parallel for if (parallel)
#endif
		for (auto it = begin; it < end; ++it) {
			*(scores + (it - begin)) = this->score(*it);
		}

	}

	/**
	 * Predict score for all negative (non-existing) links in the network.
	 * @param scores Random output iterator to store the scores.
	 * @return A pair of iterators begin and end to the range of non-existing links predicted by the method.
	 */
	virtual std::pair<typename Network::NonEdgeIt, typename Network::NonEdgeIt> predictNeg(
			ScoreRndIt scores) {
#ifdef LINKPRED_WITH_MPI
		int nbProcs;
		int procID;
		if (distributed) {
			MPI_Comm_size(comm, &nbProcs);
			MPI_Comm_rank(comm, &procID);
		} else {
			nbProcs = 1;
			procID = 0;
		}

		auto begin = net->nonEdgesBegin()
				+ (net->getNbNonEdges() / nbProcs) * procID
				+ std::min(static_cast<std::size_t>(procID),
						net->getNbNonEdges() % nbProcs);
		auto end = net->nonEdgesBegin()
				+ (net->getNbNonEdges() / nbProcs) * (procID + 1)
				+ std::min(static_cast<std::size_t>(procID + 1),
						net->getNbNonEdges() % nbProcs);
#else
		auto begin = net->nonEdgesBegin();
		auto end = net->nonEdgesEnd();
#endif

#ifdef LINKPRED_WITH_OPENMP
#pragma omp parallel for if (parallel)
#endif
		for (auto it = begin; it < end; ++it) {
			*(scores + (it - begin)) = this->score(*it);
		}

		return std::make_pair(begin, end);
	}

	/**
	 * Compute the score of a single edge.
	 * @param e The edge.
	 * @return The score of e.
	 */
	virtual double score(Edge const & e) = 0;

	/**
	 * Finds the k negative edges with the top score. Ties are broken randomly.
	 * @param k The number of edges to find.
	 * @param eit An output iterator where the edges are written.
	 * @param sit An output iterator where the scores are written. The scores are written in the same order as the edges.
	 * @return The number of negative edges inserted. It is the minimum between k and the number of negative edges in the network.
	 */
	virtual std::size_t top(std::size_t k, EdgeRndOutIt eit, ScoreRndIt sit) {
		std::vector<LMapQueue<Edge, double>> mqs;

#ifdef LINKPRED_WITH_OPENMP
#pragma omp parallel if (parallel)
		{
#pragma omp critical(initLMapQueueArray)
			{
#endif
		mqs.push_back(LMapQueue<Edge, double>(k));
#ifdef LINKPRED_WITH_OPENMP
			}
		}
#endif

		// Computing loop start, end and step in case of distributed processing
#ifdef LINKPRED_WITH_MPI
		int nbProcs;
		int procID;
		if (distributed) {
			MPI_Comm_size(comm, &nbProcs);
			MPI_Comm_rank(comm, &procID);
		} else {
			nbProcs = 1;
			procID = 0;
		}
		auto start = net->nonEdgesBegin() + procID;
		auto end = net->nonEdgesEnd();
		auto step = nbProcs;
#else
		auto start = net->nonEdgesBegin();
		auto end = net->nonEdgesEnd();
		auto step = 1;
#endif

#ifdef LINKPRED_WITH_OPENMP
#pragma omp parallel for if (parallel) schedule(dynamic) // We need a dynamic schedule because iteration vary greatly in duration
#endif
		for (auto eit = start; eit < end; eit += step) {
#ifdef LINKPRED_WITH_OPENMP
			int ind = omp_get_thread_num();
#else
			int ind = 0;
#endif
			mqs[ind].push(*eit, this->score(*eit));
		}

#ifdef LINKPRED_WITH_OPENMP
		// Now we merge results obtained by all threads
		if (parallel) {
			LMapQueue<Edge, double>::parMerge(mqs.begin(), mqs.end());
		}
#endif

#ifdef LINKPRED_WITH_MPI
		if (distributed) {
			// Now we merge over all processes
			Utils::merge(mqs[0], comm);
		}
#endif

		for (auto it = mqs[0].begin(); it != mqs[0].end(); ++it) {
			*eit = it->first;
			++eit;
			*sit = it->second;
			++sit;
		}
		return mqs[0].size();
	}

	/**
	 * @return The network.
	 */
	auto getNet() const {
		return net;
	}

	/**
	 * @return The name of the predictor.
	 */
	const std::string& getName() const {
		return name;
	}

	/**
	 * Set the name of the predictor.
	 * @param name The new name of the predictor.
	 */
	void setName(const std::string& name) {
		this->name = name;
	}

#ifdef LINKPRED_WITH_MPI
	/**
	 * @return The MPI communicator.
	 */
	MPI_Comm getComm() const {
		return comm;
	}

	/**
	 * Set the MPI communicator.
	 * @param comm The new MPI communicator.
	 */
	void setComm(MPI_Comm const & comm) {
		this->comm = comm;
	}
#endif

	/**
	 * Destructor.
	 */
	virtual ~ULPredictor() = default;

};

}
/* namespace LinkPred */

#endif /* LPredictor_HPP_ */
