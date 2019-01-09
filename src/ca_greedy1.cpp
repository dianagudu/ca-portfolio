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

CAGreedy1::CAGreedy1(Instance instance_)
    : CA(instance_, RelevanceMode::UNIFORM) {}

CAGreedy1::~CAGreedy1() {}

void CAGreedy1::computeAllocation() {
  unsigned int n = instance.getBids().N();
  unsigned int m = instance.getAsks().N();

  // sort bids descendingly by density
  std::sort(bid_index.begin(), bid_index.end(),
            [&](unsigned int i, unsigned int j) -> bool {
              return tmp_bids.getDensity()[i] > tmp_bids.getDensity()[j];
            });
  // sort asks ascendingly by density
  std::sort(ask_index.begin(), ask_index.end(),
            [&](unsigned int i, unsigned int j) -> bool {
              return tmp_asks.getDensity()[i] < tmp_asks.getDensity()[j];
            });

  unsigned int i = 0;
  unsigned int j = 0;

  while (i < n && j < m) {
    // seller ask_index[j] can allocate resources to bidder bid_index[i]
    if (instance.canAllocate(bid_index[i], ask_index[j])) {
      x[bid_index[i]] = 1;
      y(bid_index[i], ask_index[j]) = 1;
      ++i;
    }
    ++j;
  }
}
