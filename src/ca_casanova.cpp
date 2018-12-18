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
      z(instance_.getAsks().N(), 0),
      maxSteps(instance_.getBids().N()),
      distribution_neighbor(0, instance_.getBids().N() - 1),
      distribution_wp(0.0, 1.0),
      distribution_np(0.0, 1.0) {
  // init sorted asks
  for (unsigned int j = 0; j < instance.getAsks().N(); ++j)
    asks_sorted.push_back(j);
  // sort asks ascendingly by density
  std::sort(ask_index.begin(), ask_index.end(),
            [this](unsigned int i, unsigned int j) -> bool {
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

    for (era = 0; era < maxSteps && bid_index.size() && ask_index.size();
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

// Allocates the given bid (if possible).
// @param i the bid index in the list of unallocated bids 'bids'
void CACasanova::insert(unsigned int i) {
  // look for a seller in the list of unallocated asks
  unsigned int j = 0;
  while (j < ask_index.size()) {
    // seller j can allocate resources to bidder i
    if (instance.canAllocate(bid_index[i], ask_index[j])) {
      x[bid_index[i]] = 1;
      z[ask_index[j]] = 1;
      y(bid_index[i], ask_index[j]) = 1;
      welfare += instance.getBids().V()[bid_index[i]] -
                 instance.getAsks().V()[ask_index[j]];
      ask_index.erase(ask_index.begin() + j);
      // update bid birthday
      birthday[bid_index[i]] = era;
      // remove from lists of 
      bid_index.erase(bid_index.begin() + i);
      return;
    }
    ++j;
  }
  // if no seller could be found, look for one that has already allocated
  // its bundle, but would gain more by switching to this bidder
  j = 0;
  while (j < instance.getAsks().N()) {
    if (z[asks_sorted[j]]) {  // already allocated
      if (instance.canAllocate(bid_index[i], asks_sorted[j])) {
        // check which bidder it already allocated goods to
        unsigned int alloc_i = 0;
        while (y(alloc_i, asks_sorted[j]) == 0)
          ++alloc_i;  // no other stopping condition since we know it exists!

        // change from alloc_i to i only if there is an increase in revenue
        if (instance.getBids().V()[alloc_i] <
            instance.getBids().V()[bid_index[i]]) {
          x[bid_index[i]] = 1;
          x[alloc_i] = 0;
          y(bid_index[i], asks_sorted[j]) = 1;
          y(alloc_i, asks_sorted[j]) = 0;
          welfare += instance.getBids().V()[bid_index[i]] -
                     instance.getBids().V()[alloc_i];
          // insert alloc_i in bid_index (unallocated bids) in the right place
          // according to average price
          unsigned int index = 0;
          while (index < bid_index.size() &&
                 tmp_bids.getAvgPrice()[bid_index[i]] >
                     tmp_bids.getAvgPrice()[alloc_i])
            ++index;
          bid_index.insert(bid_index.begin() + index, alloc_i);
          // update bid birthday
          birthday[bid_index[i]] = era;
          // once this bid was allocated, remove from list of unallocated bids
          bid_index.erase(bid_index.begin() + i);
          return;
        }
      }
    }
    ++j;
  }
}

void CACasanova::resetBetweenTries() {
  resetBase();  // includes recreating bid_index and ask_index
  welfare = 0.;
  z = std::vector<int>(instance.getAsks().N(), 0);
  // reset birthdays
  for (unsigned int i = 0; i < instance.getBids().N(); ++i) birthday[i] = 0;
  // sort bids descendingly by score / avg price
  std::sort(bid_index.begin(), bid_index.end(),
            [this](unsigned int i, unsigned int j) -> bool {
              return tmp_bids.getAvgPrice()[i] > tmp_bids.getAvgPrice()[j];
            });
  // sort asks ascendingly by density
  std::sort(ask_index.begin(), ask_index.end(),
            [this](unsigned int i, unsigned int j) -> bool {
              return tmp_asks.getDensity()[i] < tmp_asks.getDensity()[j];
            });
}

void CACasanova::resetAllocation() {
  resetBase();
  welfare = 0.;
  z = std::vector<int>(instance.getAsks().N(), 0);
  // reset birthdays
  for (unsigned int i = 0; i < instance.getBids().N(); ++i) birthday[i] = 0;
  // reset best welfare between computeAllocation calls
  best_welfare = 0.;
  best_x = std::vector<int>(instance.getBids().N(), 0);
  best_y = boost::numeric::ublas::zero_matrix<int>(instance.getBids().N(),
                                                   instance.getAsks().N());
}

bool CACasanova::noSideEffects() {
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

// calculates the age of a given bid based on its birthday and current era
// @param i the index of the bid
// @return age
int CACasanova::age(unsigned int i) { return era - birthday[i]; }
