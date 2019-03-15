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

#include "ca_casanova.h"

CACasanova::CACasanova(Instance instance_)
    : CA(instance_),
      maxSteps(instance_.getBids().N()),
      theta(instance_.getBids().N() / 4),
      distribution_neighbor(0, instance_.getBids().N() - 1),
      distribution_wp(0.0, 1.0),
      distribution_np(0.0, 1.0) {
  // init sorted bids
  for (unsigned int i = 0; i < instance.getBids().N(); ++i)
    bids_sorted.push_back(i);
  // sort bids descendingly by score (average price)
  std::sort(bids_sorted.begin(), bids_sorted.end(),
            [&](unsigned int i, unsigned int j) -> bool {
              return tmp_bids.getAvgPrice()[i] > tmp_bids.getAvgPrice()[j];
            });
  // init sorted asks
  for (unsigned int j = 0; j < instance.getAsks().N(); ++j)
    asks_sorted.push_back(j);
  // sort asks ascendingly by density
  std::sort(asks_sorted.begin(), asks_sorted.end(),
            [&](unsigned int i, unsigned int j) -> bool {
              return tmp_asks.getDensity()[i] < tmp_asks.getDensity()[j];
            });
}

CACasanova::~CACasanova() {}

void CACasanova::computeAllocation() {
  // reset random generator
  std::random_device rd;
  generator.seed(rd());

  for (unsigned int tries = 0; tries < maxTries; ++tries) {
    resetBetweenTries();

    for (era = 0, last_improved_era = 0;
         era < maxSteps && bid_index.size() && ask_index.size() &&
         (era < theta || era - last_improved_era < theta / 2);
         ++era) {
      if (distribution_wp(generator) < wp) {
        // allocate a random bid
        unsigned int i = distribution_neighbor(generator) % bid_index.size();
        insert(i);
      } else {
        if (bid_index.size() == 1 || age(bid_index[0]) > age(bid_index[1])) {
          insert(0);
        } else {
          if (distribution_np(generator) < np) {
            insert(1);
          } else {
            insert(0);
          }
        }
      }
    }
    if (welfare > best_welfare) {
      best_welfare = welfare;
      best_allocated_asks = allocated_asks;
    }
  }

  welfare = best_welfare;
  for (auto it : best_allocated_asks) {
    x[it.second] = 1;
    y(it.second, it.first) = 1;
  }
}

// Allocates the given bid (if possible).
// @param i the bid index in the list of unallocated bids 'bids'
void CACasanova::insert(unsigned int i) {
  // look for a seller in the list of unallocated asks
  unsigned int j = 0;
  while (j < ask_index.size()) {
    // seller j can allocate resources to bidder i
    if (instance.canAllocate(bid_index[i], ask_index[j])) {
      welfare += instance.getBids().V()[bid_index[i]] -
                 instance.getAsks().V()[ask_index[j]];
      last_improved_era = era;
      allocated_asks[ask_index[j]] = bid_index[i];
      ask_index.erase(ask_index.begin() + j);
      // update bid birthday
      birthday[bid_index[i]] = era;
      // remove from lists of
      bid_index.erase(bid_index.begin() + i);
      return;
    }
    ++j;
  }
  // return;
  // if no seller could be found, look for one that has already allocated
  // its bundle, but would gain more by switching to this bidder
  for (auto it : allocated_asks) {
    j = it.first;
    if (instance.canAllocate(bid_index[i], j)) {
      // check which bidder it already allocated goods to
      unsigned int alloc_i = it.second;

      // change from alloc_i to bid_index[i] only if there is an increase in
      // revenue
      if (instance.getBids().V()[alloc_i] <
          instance.getBids().V()[bid_index[i]]) {
        allocated_asks[j] = bid_index[i];
        welfare += instance.getBids().V()[bid_index[i]] -
                   instance.getBids().V()[alloc_i];
        last_improved_era = era;
        // update bid birthday
        birthday[bid_index[i]] = era;
        // once this bid was allocated, remove from list of unallocated bids
        bid_index.erase(bid_index.begin() + i);
        // insert alloc_i in bid_index (unallocated bids) in the right place
        // according to average price
        unsigned int index = 0;
        while (index < bid_index.size() &&
               tmp_bids.getAvgPrice()[bid_index[index]] >
                   tmp_bids.getAvgPrice()[alloc_i])
          ++index;
        bid_index.insert(bid_index.begin() + index, alloc_i);
        return;
      }
    }
  }
}

void CACasanova::resetBetweenTries() {
  allocated_asks.clear();
  bid_index = bids_sorted;
  ask_index = asks_sorted;
  welfare = 0.;

  // initialise all prices to 0
  for (unsigned int i = 0; i < instance.getBids().N(); ++i) {
    price_buyer[i] = 0.;
  }
  for (unsigned int j = 0; j < instance.getAsks().N(); ++j) {
    price_seller[j] = 0.;
  }

  stats = Stats();

  // reset birthdays
  birthday = std::vector<int>(instance.getBids().N(), -1);
}

void CACasanova::resetAllocation() {
  resetBase();
  allocated_asks.clear();
  welfare = 0.;
  // reset birthdays
  birthday = std::vector<int>(instance.getBids().N(), -1);
  // reset best welfare between computeAllocation calls
  best_welfare = 0.;
  best_allocated_asks.clear();
}

bool CACasanova::noSideEffects() {
  // side effects from casanova-related variables
  if (welfare) return false;
  if (!allocated_asks.empty()) return false;
  for (unsigned int i = 0; i < instance.getBids().N(); ++i)
    if (birthday[i] >= 0) return false;

  if (best_welfare) return false;
  if (!best_allocated_asks.empty()) return false;

  return noSideEffectsBase();
}

// calculates the age of a given bid based on its birthday and current era
// @param i the index of the bid
// @return age
int CACasanova::age(unsigned int i) { return era - birthday[i]; }
