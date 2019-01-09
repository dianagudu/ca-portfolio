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

#include "ca_sa.h"

CASA::CASA(Instance instance_)
    : CA(instance_),
      z(instance_.getAsks().N(), 0),
      distribution_neighbor(0, instance_.getBids().N() - 1),
      distribution_ap(0.0, 1.0) {}

CASA::~CASA() {}

void CASA::computeAllocation() {
  // seed mersenne_twister_engine with rd()
  std::random_device rd;
  generator.seed(rd());

  generateInitialSolution();

  double T = T_max;
  bool frozen = false;
  unsigned int num_frozen_temps = 0;
  while (T > T_min && !frozen) {
    frozen = true;
    for (unsigned int iter = 0; iter < niter; ++iter) {
      Neighbor neigh = neighbor();
      if (neigh.found && acceptanceProbability(neigh.welfare, T) >
                             distribution_ap(generator)) {
        welfare = neigh.welfare;
        // flip neighbor bid and ask
        x[neigh.bid] = 1 - x[neigh.bid];
        z[neigh.ask] = 1 - z[neigh.ask];
        y(neigh.bid, neigh.ask) = 1 - y(neigh.bid, neigh.ask);
        frozen = false;
        num_frozen_temps = 0;
      }
    }
    T *= alpha;
    if (frozen) ++num_frozen_temps;
    // only stop when the system is frozen for 3 consecutive temperatures
    if (num_frozen_temps < 3) frozen = false;
  }
}

double CASA::acceptanceProbability(double new_welfare, double T) {
  return exp((new_welfare - welfare) / T);
}

Neighbor CASA::neighbor() {
  unsigned int m = instance.getAsks().N();
  // TODO: change x, y, z only if solution is accepted
  // how: compute welfare difference and find which bid and ask have to be
  // flipped
  Neighbor neigh;
  neigh.welfare = welfare;
  neigh.found = false;

  // randomly select one bid
  unsigned int i = distribution_neighbor(generator);
  if (x[i]) {  // if x_i==1 set it to 0
    neigh.welfare -= instance.getBids().V()[i];
    for (unsigned int j = 0; j < m; ++j) {
      if (y(i, j)) {
        neigh.welfare += instance.getAsks().V()[j];
        neigh.bid = i;
        neigh.ask = j;
        neigh.found = true;
        break;
      }
    }
  } else {  // x_i==0, try to find an ask to match from sorted asks
    for (unsigned int j = 0; j < m; ++j) {
      // check if seller ask_index[j] can allocate resources to bidder i
      if (!z[ask_index[j]] && instance.canAllocate(i, ask_index[j])) {
        neigh.welfare +=
            instance.getBids().V()[i] - instance.getAsks().V()[ask_index[j]];
        neigh.bid = i;
        neigh.ask = ask_index[j];
        neigh.found = true;
        break;
      }
    }
  }
  return neigh;
}

void CASA::generateInitialSolution() {
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

  // starting temperature is the maximum possible welfare increase TODO: is this
  // correct? T_max = instance.getBids().V()[bid_index[0]] -
  //         instance.getAsks().V()[ask_index[0]];
  auto bid_values = instance.getBids().V();
  auto ask_values = instance.getAsks().V();
  T_max = *(std::max_element(bid_values.begin(), bid_values.end())) -
          *(std::min_element(ask_values.begin(), ask_values.end()));
  // return;
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

void CASA::resetAllocation() {
  resetBase();
  welfare = 0.;
  z = std::vector<int>(instance.getAsks().N(), 0);
}

bool CASA::noSideEffects() {
  if (welfare) return false;
  for (unsigned int j = 0; j < instance.getAsks().N(); ++j)
    if (z[j]) return false;
  return noSideEffectsBase();
}