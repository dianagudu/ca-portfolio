// --------------------------------------------------------------------------
// Copyright (C) Karlsruhe Institute of Technology, 2017
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// --------------------------------------------------------------------------

#include "ca_greedy1.h"

#include <boost/lexical_cast.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

CAGreedy1::CAGreedy1(Instance instance_): CA(instance_, RelevanceMode::UNIFORM) {}

CAGreedy1::~CAGreedy1() {}

void CAGreedy1::computeAllocation() {
  unsigned int n = instance.getBids().N();
  unsigned int m = instance.getAsks().N();
  unsigned int l = instance.L();

  // reset variables if new allocation must be calculated
  x = std::vector<int>(n, 0);
  y = boost::numeric::ublas::zero_matrix<int>(n, m);

  // vectors of bid and ask indices => they must be sorted by density
  std::vector<int> bids;
  std::vector<int> asks;
  for (unsigned int i = 0; i < n; ++i) {
    bids.push_back(i);
  }
  for (unsigned int j = 0; j < m; ++j) {
    asks.push_back(j);
  }

  // sort bids descendingly by density
  std::sort(bids.begin(), bids.end(), [this] (unsigned int i, unsigned int j) -> bool {
      return tmp_bids.getDensity()[i] > tmp_bids.getDensity()[j];
      });
  // sort asks ascendingly by density
  std::sort(asks.begin(), asks.end(), [this] (unsigned int i, unsigned int j) -> bool {
      return tmp_asks.getDensity()[i] < tmp_asks.getDensity()[j];
      });

  unsigned int i = 0; unsigned int j = 0;

  while (i < n && j < m) {
    unsigned int k = 0;
    while (k < l && instance.getBids().Q()(bids[i], k) <= instance.getAsks().Q()(asks[j], k)) {
      ++k;
    }
    // seller j can allocate resources to bidder i
    if (k == l && instance.getBids().V()[bids[i]] >= instance.getAsks().V()[asks[j]]) {
      x[bids[i]] = 1;
      y(bids[i], asks[j]) = 1;
      ++i;
    }
    ++j;
  }
}

