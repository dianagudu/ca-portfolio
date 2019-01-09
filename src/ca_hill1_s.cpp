// --------------------------------------------------------------------------
// Copyright (C) Karlsruhe Institute of Technology, 2016
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

#include "ca_hill1_s.h"

CAHill1S::CAHill1S(Instance instance_) : CA(instance_) {}

CAHill1S::~CAHill1S() {}

void CAHill1S::computeAllocation() {
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

  // compute initial solution
  welfare = neighbor();
  best_ask_index = ask_index;

  // gradient descent
  while (locallyImprove())
    ;

  // compute solution based on best ordering
  unsigned int i = 0;
  unsigned int j = 0;
  ask_index = best_ask_index;
  welfare = 0;
  while (i < instance.getBids().N() && j < instance.getAsks().N()) {
    // seller ask_index[j] can allocate resources to bidder bid_index[i]
    if (instance.canAllocate(bid_index[i], ask_index[j])) {
      x[bid_index[i]] = 1;
      y(bid_index[i], ask_index[j]) = 1;
      welfare += instance.getBids().V()[bid_index[i]] -
                 instance.getAsks().V()[ask_index[j]];
      ++j;
    }
    ++i;
  }
}

bool CAHill1S::locallyImprove() {
  unsigned int j = critical_j + 1;
  while (j < instance.getAsks().N()) {
    // get the neighbor by changing the order of one request
    // moving ask j to front
    std::rotate(ask_index.begin(), ask_index.begin() + j,
                ask_index.begin() + j + 1);
    // get welfare of neighbor
    double new_welfare = neighbor();
    // check improvement
    if (new_welfare > welfare) {
      best_ask_index = ask_index;
      welfare = new_welfare;
      return true;
    }
    ++j;
  }
  return false;
}

double CAHill1S::neighbor() {
  double new_welfare = 0.;
  unsigned int i = 0;
  unsigned int j = 0;
  critical_j = 0;
  while (i < instance.getBids().N() && j < instance.getAsks().N()) {
    // seller ask_index[j] can allocate resources to bidder bid_index[i]
    if (instance.canAllocate(bid_index[i], ask_index[j])) {
      new_welfare += instance.getBids().V()[bid_index[i]] -
                     instance.getAsks().V()[ask_index[j]];
      critical_j = j;
      ++j;
    }
    ++i;
  }
  return new_welfare;
}
