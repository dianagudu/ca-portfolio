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
    : CA(instance_), z(instance_.getAsks().N(), 0),
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

  // update vars
  _y = y;
  _x = x;
  _z = z;
  _welfare = welfare;

  // randomly select one bid
  unsigned int i =  distribution_neighbor(generator);
  if (_x[i] == 0) {
    // x_i==0, try to find an ask to match from sorted asks
    unsigned int j = 0;
    while (j < m) {
      // check if seller j has already allocated its resources
      if (_z[ask_index[j]] == 0) {
        // seller ask_index[j] can allocate resources to bidder i
        if (instance.canAllocate(i, ask_index[j])) {
          _x[i] = 1;
          _y(i, ask_index[j]) = 1;
          _z[ask_index[j]] = 1;
          _welfare +=
              instance.getBids().V()[i] - instance.getAsks().V()[ask_index[j]];
          break;
        }
      }
      ++j;
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
  // reset variables if new allocation must be calculated
  x = std::vector<int>(instance.getBids().N(), 0);
  z = std::vector<int>(instance.getAsks().N(), 0);
  y = boost::numeric::ublas::zero_matrix<int>(instance.getBids().N(),
                                              instance.getAsks().N());

  bid_index = std::vector<int>();
  ask_index = std::vector<int>();
  for (unsigned int i = 0; i < instance.getBids().N(); ++i) {
    bid_index.push_back(i);
  }
  for (unsigned int j = 0; j < instance.getAsks().N(); ++j) {
    ask_index.push_back(j);
  }

  // initialise all prices to 0
  for (unsigned int i = 0; i < instance.getBids().N(); ++i) {
    price_buyer[i] = 0.;
  }
  for (unsigned int j = 0; j < instance.getAsks().N(); ++j) {
    price_seller[j] = 0.;
  }
}