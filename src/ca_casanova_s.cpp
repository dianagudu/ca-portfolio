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

#include "ca_casanova_s.h"

CACasanovaS::CACasanovaS(Instance instance_)
    : CA(instance_),
      maxSteps(instance_.getAsks().N()),
      theta(instance_.getAsks().N()/4),
      distribution_neighbor(0, instance_.getAsks().N() - 1),
      distribution_wp(0.0, 1.0),
      distribution_np(0.0, 1.0) {
  // init sorted bids
  for (unsigned int i = 0; i < instance.getBids().N(); ++i)
    bids_sorted.push_back(i);
  // sort bids descendingly by density
  std::sort(bids_sorted.begin(), bids_sorted.end(),
            [&](unsigned int i, unsigned int j) -> bool {
              return tmp_bids.getDensity()[i] > tmp_bids.getDensity()[j];
            });  // init sorted asks
  for (unsigned int j = 0; j < instance.getAsks().N(); ++j)
    asks_sorted.push_back(j);
  // sort asks ascendingly by score (average price)
  std::sort(asks_sorted.begin(), asks_sorted.end(),
            [&](unsigned int i, unsigned int j) -> bool {
              return tmp_asks.getAvgPrice()[i] < tmp_asks.getAvgPrice()[j];
            });
}

CACasanovaS::~CACasanovaS() {}

void CACasanovaS::computeAllocation() {
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
        // allocate a random ask
        unsigned int j = distribution_neighbor(generator) % ask_index.size();
        insert(j);
      } else {
        if (ask_index.size() == 1 || age(ask_index[0]) > age(ask_index[1])) {
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
      best_allocated_bids = allocated_bids;
    }
  }
  
  welfare = best_welfare;
  for (auto it : best_allocated_bids) {
    x[it.first] = 1;
    y(it.first, it.second) = 1;
  }
}

// Allocates the given ask (if possible).
// @param j the ask index in the list of unallocated asks 'asks'
void CACasanovaS::insert(unsigned int j) {
  // look for a bidder in the list of unallocated bids
  unsigned int i = 0;
  while (i < bid_index.size()) {
    // seller j can allocate resources to bidder i
    if (instance.canAllocate(bid_index[i], ask_index[j])) {
      welfare += instance.getBids().V()[bid_index[i]] -
                 instance.getAsks().V()[ask_index[j]];
      last_improved_era = era;
      allocated_bids[bid_index[i]] = ask_index[j];
      bid_index.erase(bid_index.begin() + i);
      // update ask birthday
      birthday[ask_index[j]] = era;
      // once this ask was allocated, remove from list of unallocated asks
      ask_index.erase(ask_index.begin() + j);
      return;
    }
    ++i;
  }
  // return;
  // if no bidder could be found, look for one that has already been allocated
  // a bundle, but would gain more by switching to this seller
  for (auto it : allocated_bids) {
    i = it.first;
    if (instance.canAllocate(i, ask_index[j])) {
      // check which seller already allocated its goods to this bidder
      unsigned int alloc_j = it.second;

      // change from alloc_j to ask_index[j] only if there is an increase in revenue
      if (instance.getAsks().V()[alloc_j] >
          instance.getAsks().V()[ask_index[j]]) {
        allocated_bids[i] = ask_index[j];
        welfare += instance.getAsks().V()[alloc_j] -
                   instance.getAsks().V()[ask_index[j]];
        last_improved_era = era;
        // update ask birthday
        birthday[ask_index[j]] = era;
        // once this ask was allocated, remove from list of unallocated asks
        ask_index.erase(ask_index.begin() + j);
        // insert alloc_j in ask_index (unallocated asks) in the right place
        // according to average price
        unsigned int index = 0;
        while (index < ask_index.size() &&
               tmp_asks.getAvgPrice()[ask_index[index]] <
                   tmp_asks.getAvgPrice()[alloc_j])
          ++index;
        ask_index.insert(ask_index.begin() + index, alloc_j);
        return;
      }
    }
    ++i;
  }
}

void CACasanovaS::resetBetweenTries() {
  allocated_bids.clear();
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
  birthday = std::vector<int>(instance.getAsks().N(), -1);
}

void CACasanovaS::resetAllocation() {
  resetBase();
  allocated_bids.clear();
  welfare = 0.;
  // reset birthdays
  birthday = std::vector<int>(instance.getAsks().N(), -1);
  // reset best welfare between computeAllocation calls
  best_welfare = 0.;
  best_allocated_bids.clear();
}

bool CACasanovaS::noSideEffects() {
  // side effects from casanova-related variables
  if (welfare) return false;
  if (!allocated_bids.empty()) return false;
  for (unsigned int i = 0; i < instance.getBids().N(); ++i)
    if (birthday[i] >= 0) return false;

  if (best_welfare) return false;
  if (!best_allocated_bids.empty()) return false;

  return noSideEffectsBase();
}

// calculates the age of a given ask based on its birthday and current era
// @param j the index of the ask
// @return age
int CACasanovaS::age(unsigned int j) { return era - birthday[j]; }
