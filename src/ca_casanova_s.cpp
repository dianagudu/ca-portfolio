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
      z(instance_.getAsks().N(), 0),
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
            [this](unsigned int i, unsigned int j) -> bool {
              return tmp_bids.getDensity()[i] > tmp_bids.getDensity()[j];
            });  // init sorted asks
  for (unsigned int j = 0; j < instance.getAsks().N(); ++j)
    asks_sorted.push_back(j);
  // sort asks ascendingly by density
  std::sort(asks_sorted.begin(), asks_sorted.end(),
            [this](unsigned int i, unsigned int j) -> bool {
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
            insert(0);
          } else {
            insert(1);
          }
        }
      }
    }
    if (welfare > best_welfare) {
      best_welfare = welfare;
      best_y = y;
      best_x = x;
    }
  }
  y = best_y;
  x = best_x;
  welfare = best_welfare;
}

// Allocates the given ask (if possible).
// @param j the ask index in the list of unallocated asks 'asks'
void CACasanovaS::insert(unsigned int j) {
  // look for a bidder in the list of unallocated bids
  unsigned int i = 0;
  while (i < bid_index.size()) {
    // seller j can allocate resources to bidder i
    if (instance.canAllocate(bid_index[i], ask_index[j])) {
      x[bid_index[i]] = 1;
      z[ask_index[j]] = 1;
      y(bid_index[i], ask_index[j]) = 1;
      welfare += instance.getBids().V()[bid_index[i]] -
                 instance.getAsks().V()[ask_index[j]];
      last_improved_era = era;
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
  i = 0;
  while (i < instance.getBids().N()) {
    if (x[bids_sorted[i]] && instance.canAllocate(bids_sorted[i], ask_index[j])) {
      // check which seller already allocated its goods to this bidder
      unsigned int alloc_j = 0;
      while (y(bids_sorted[i], alloc_j) == 0) ++alloc_j;

      // change from alloc_j to ask_index[j] only if there is an increase in revenue
      if (instance.getAsks().V()[alloc_j] >
          instance.getAsks().V()[ask_index[j]]) {
        z[ask_index[j]] = 1;
        z[alloc_j] = 0;
        y(bids_sorted[i], ask_index[j]) = 1;
        y(bids_sorted[i], alloc_j) = 0;
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
  x = std::vector<int>(instance.getBids().N(), 0);
  z = std::vector<int>(instance.getAsks().N(), 0);
  y = boost::numeric::ublas::zero_matrix<int>(instance.getBids().N(),
                                              instance.getAsks().N());
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
  for (unsigned int j = 0; j < instance.getAsks().N(); ++j) birthday[j] = 0;
}

void CACasanovaS::resetAllocation() {
  resetBase();
  welfare = 0.;
  z = std::vector<int>(instance.getAsks().N(), 0);
  // reset birthdays
  for (unsigned int j = 0; j < instance.getAsks().N(); ++j) birthday[j] = 0;
  // reset best welfare between computeAllocation calls
  best_welfare = 0.;
  best_x = std::vector<int>(instance.getBids().N(), 0);
  best_y = boost::numeric::ublas::zero_matrix<int>(instance.getBids().N(),
                                                   instance.getAsks().N());
}

bool CACasanovaS::noSideEffects() {
  // side effects from casanova-related variables
  if (welfare) return false;
  for (unsigned int j = 0; j < instance.getAsks().N(); ++j)
    if (z[j]) return false;
  for (unsigned int i = 0; i < instance.getBids().N(); ++i)
    if (birthday[i]) return false;

  if (best_welfare) return false;
  for (unsigned int i = 0; i < instance.getBids().N(); ++i)
    if (best_x[i]) return false;
  for (unsigned int i = 0; i < instance.getBids().N(); ++i)
    for (unsigned int j = 0; j < instance.getAsks().N(); ++j)
      if (best_y(i, j)) return false;

  return noSideEffectsBase();
}

// calculates the age of a given ask based on its birthday and current era
// @param j the index of the ask
// @return age
int CACasanovaS::age(unsigned int j) { return era - birthday[j]; }
