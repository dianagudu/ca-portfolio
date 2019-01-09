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

#include "ca_hill1.h"

CAHill1::CAHill1(Instance instance_) : CA(instance_) {}

CAHill1::~CAHill1() {}

void CAHill1::computeAllocation() {
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
  best_bid_index = bid_index;

  // gradient descent
  while (locallyImprove())
    ;

  // compute solution based on best ordering
  bid_index = best_bid_index;
  unsigned int i = 0;
  unsigned int j = 0;
  welfare = 0;
  while (i < instance.getBids().N() && j < instance.getAsks().N()) {
    // seller ask_index[j] can allocate resources to bidder bid_index[i]
    if (instance.canAllocate(bid_index[i], ask_index[j])) {
      x[bid_index[i]] = 1;
      y(bid_index[i], ask_index[j]) = 1;
      welfare += instance.getBids().V()[bid_index[i]] -
                 instance.getAsks().V()[ask_index[j]];
      ++i;
    }
    ++j;
  }
}

bool CAHill1::locallyImprove() {
  unsigned int i = critical_i + 1;
  while (i < instance.getBids().N()) {
    // get the neighbor by changing the order of one request
    // moving bid i to front
    std::rotate(bid_index.begin(), bid_index.begin() + i,
                bid_index.begin() + i + 1);
    // get welfare of neighbor
    double new_welfare = neighbor();
    // check improvement
    if (new_welfare > welfare) {
      best_bid_index = bid_index;
      welfare = new_welfare;
      return true;
    }
    ++i;
  }
  return false;
}

double CAHill1::neighbor() {
  double new_welfare = 0.;
  unsigned int i = 0;
  unsigned int j = 0;
  critical_i = 0;
  while (i < instance.getBids().N() && j < instance.getAsks().N()) {
    // seller ask_index[j] can allocate resources to bidder bid_index[i]
    if (instance.canAllocate(bid_index[i], ask_index[j])) {
      new_welfare += instance.getBids().V()[bid_index[i]] -
                  instance.getAsks().V()[ask_index[j]];
      critical_i = i;
      ++i;
    }
    ++j;
  }
  return new_welfare;
}
