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

#include "ca_hill2.h"

CAHill2::CAHill2(Instance instance_)
    : CA(instance_),
      z(instance_.getAsks().N(), 0),
      distribution_neighbor(0, instance_.getBids().N() - 1) {}

CAHill2::~CAHill2() {}

void CAHill2::computeAllocation() {
  // seed mersenne_twister_engine with rd()
  std::random_device rd;
  generator.seed(rd());

  generateInitialSolution();
  while (locallyImprove())
    ;
}

bool CAHill2::locallyImprove() {
  unsigned int n = instance.getBids().N();
  unsigned int m = instance.getAsks().N();

  Neighbor neigh;
  neigh.welfare = welfare;
  neigh.found = false;

  // randomly select one bid
  unsigned int i = distribution_neighbor(generator);
  if (x[i] == 0) {
    // x_i==0, try to find an ask to match from sorted asks
    unsigned int j = 0;
    while (j < m) {
      // check if seller j has already allocated its resources
      if (z[ask_index[j]] == 0) {
        // seller ask_index[j] can allocate resources to bidder i
        if (instance.canAllocate(i, ask_index[j])) {
          neigh.welfare +=
              instance.getBids().V()[i] - instance.getAsks().V()[ask_index[j]];
          neigh.bid = i;
          neigh.ask = ask_index[j];
          neigh.found = true;
          break;
        }
      }
      ++j;
    }
  }

  if (neigh.found && neigh.welfare > welfare) {
    // update allocation
    x[neigh.bid] = 1;
    z[neigh.ask] = 1;
    y(neigh.bid, neigh.ask) = 1;
    welfare = neigh.welfare;
    num_neighbors = 0;
    return true;
  }
  if (num_neighbors < n) {
    ++num_neighbors;
    return true;
  }
  return false;
}

void CAHill2::generateInitialSolution() {
  unsigned int n = instance.getBids().N();
  unsigned int m = instance.getAsks().N();

  // sort bids descendingly by density
  std::sort(bid_index.begin(), bid_index.end(),
            [this](unsigned int i, unsigned int j) -> bool {
              return tmp_bids.getDensity()[i] > tmp_bids.getDensity()[j];
            });
  // sort asks ascendingly by density
  std::sort(ask_index.begin(), ask_index.end(),
            [this](unsigned int i, unsigned int j) -> bool {
              return tmp_asks.getDensity()[i] < tmp_asks.getDensity()[j];
            });
  return;
  // compute greedy1 solution
  unsigned int i = 0;
  unsigned int j = 0;
  welfare = 0.;
  while (i < n && j < m) {
    // seller ask_index[j] can allocate resources to bidder bid_index[i]
    if (instance.canAllocate(bid_index[i], ask_index[j])) {
      x[bid_index[i]] = 1;
      z[ask_index[j]] = 1;
      y(bid_index[i], ask_index[j]) = 1;
      welfare += instance.getBids().V()[bid_index[i]] -
                 instance.getAsks().V()[ask_index[j]];
      ++i;
    }
    ++j;
  }
}

void CAHill2::resetAllocation() {
  resetBase();
  welfare = 0.;
  num_neighbors = 0;
  z = std::vector<int>(instance.getAsks().N(), 0);
}

bool CAHill2::noSideEffects() {
  if (welfare) return false;
  if (num_neighbors) return false;
  for (unsigned int j = 0; j < instance.getAsks().N(); ++j)
    if (z[j]) return false;
  return noSideEffectsBase();
}