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

#include "ca_hill2_s.h"

CAHill2S::CAHill2S(Instance instance_)
    : CA(instance_),
      z(instance_.getAsks().N(), 0),
      distribution_neighbor(0, instance_.getAsks().N() - 1) {}

CAHill2S::~CAHill2S() {}

void CAHill2S::computeAllocation() {
  // seed mersenne_twister_engine with rd()
  std::random_device rd;
  generator.seed(rd());

  generateInitialSolution();
  while (locallyImprove())
    ;
}

// the neighbors are selected by flipping z bits, i.e. which sellers
// allocate resources
bool CAHill2S::locallyImprove() {
  unsigned int n = instance.getBids().N();
  unsigned int m = instance.getAsks().N();

  // update vars
  _y = y;
  _x = x;
  _z = z;
  _welfare = welfare;

  // randomly select one ask
  unsigned int j = distribution_neighbor(generator);
  if (_z[j] == 0) {
    // if z_j==0, try to find a bid to match from sorted bids
    unsigned int i = 0;
    while (i < n) {
      // check if bidder i has already allocated its resources
      if (_x[bid_index[i]] == 0) {
        // seller j can allocate resources to bidder bid_index[i]
        if (instance.canAllocate(bid_index[i], j)) {
          _x[bid_index[i]] = 1;
          _y(bid_index[i], j) = 1;
          _z[j] = 1;
          _welfare +=
              instance.getBids().V()[bid_index[i]] - instance.getAsks().V()[j];
          break;
        }
      }
      ++i;
    }
  }
  if (_welfare > welfare) {
    x = _x;
    y = _y;
    z = _z;
    welfare = _welfare;
    num_neighbors = 0;
    return true;
  }
  if (num_neighbors < m) {
    ++num_neighbors;
    return true;
  }
  return false;
}

void CAHill2S::generateInitialSolution() {
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
  // compute greedy1s solution
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
      ++j;
    }
    ++i;
  }
}

void CAHill2S::resetAllocation() {
  resetBase();
  welfare = 0.;
  num_neighbors = 0;
  z = std::vector<int>(instance.getAsks().N(), 0);
}

bool CAHill2S::noSideEffects() {
  if (welfare) return false;
  if (num_neighbors) return false;
  for (unsigned int j = 0; j < instance.getAsks().N(); ++j)
    if (z[j]) return false;
  return noSideEffectsBase();
}