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

#include <linkpred.hpp>
#include <iostream>  
#include <vector>
#include <memory>

using namespace LinkPred;

int main(int argc, char*argv[]) {
	if (argc != 2) {
		std::cerr << "Bad arguments\nUsage: " << argv[0] << " net" << std::endl;
		exit(1);
	}
	std::string netFileName(argv[1]);
	std::cout << "Reading network..." << std::endl;
	auto net = UNetwork<>::read(netFileName, false, true);
	auto start = std::chrono::steady_clock::now();
	std::cout << "Creating test set..." << std::endl;
	auto testData = NetworkManipulator<>::createTestData(net, 0.1, 0.0, false,
			true, 0, true, 0, FN, TN, 777, true);
	testData.lock();
	auto predictor = std::make_shared<UADAPredictor<>>(testData.getObsNet());
	PredResults<> pr(testData, predictor);
	pr.compPosScores();
	pr.compNegScores();
	std::cout << "Score of positive links in the test set (false negatives):"
			<< std::endl;
	for (auto it = pr.posBegin(); it != pr.posEnd(); ++it) {
		std::cout << *it << std::endl;
	}
	std::cout << "Score of negative links in the test set (true negatives):"
			<< std::endl;
	for (auto it = pr.negBegin(); it != pr.negEnd(); ++it) {
		std::cout << *it << std::endl;
	}
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cerr << "#Time: "
			<< std::chrono::duration<double, std::milli>(diff).count() << " ms"
			<< std::endl;
	return 0;
}

