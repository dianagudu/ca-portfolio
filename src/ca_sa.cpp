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
  while (T > T_min && !frozen) {
    frozen = true;
    for (unsigned int iter = 0; iter < niter; ++iter) {
      neighbor();
      if (acceptanceProbability(T) > distribution_ap(generator)) {
        x = _x;
        y = _y;
        z = _z;
        welfare = _welfare;
        frozen = false;
      }
    }
    T *= alpha;
  }
}

double CASA::acceptanceProbability(double T) {
  return exp((_welfare - welfare) / T);
}

void CASA::neighbor() {
  unsigned int m = instance.getAsks().N();

  // update vars
  _y = y;
  _x = x;
  _z = z;
  _welfare = welfare;

  // randomly select one bid
  unsigned int i = distribution_neighbor(generator);
  if (_x[i]) {  // if x_i==1 set it to 0
    _welfare -= instance.getBids().V()[i];
    _x[i] = 0;
    for (unsigned int j = 0; j < m; ++j) {
      if (_y(i, j)) {
        _welfare += instance.getAsks().V()[j];
        _y(i, j) = 0;
        _z[j] = 0;
        break;
      }
    }
  } else {  // x_i==0, try to find an ask to match from sorted asks
    for (unsigned int j = 0; j < m; ++j) {
      // check if seller ask_index[j] can allocate resources to bidder i
      if (!_z[ask_index[j]] && instance.canAllocate(i, ask_index[j])) {
        _x[i] = 1;
        _y(i, ask_index[j]) = 1;
        _z[ask_index[j]] = 1;
        _welfare +=
            instance.getBids().V()[i] - instance.getAsks().V()[ask_index[j]];
        break;
      }
    }
  }
}

void CASA::generateInitialSolution() {
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

  // starting temperature is the maximum possible welfare increase
  T_max = instance.getBids().V()[bid_index[0]] -
          instance.getAsks().V()[ask_index[0]];
  //return;
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