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

#include "ca_sa_s.h"

CASAS::CASAS(Instance instance_)
    : CA(instance_),
      z(instance_.getAsks().N(), 0),
      distribution_neighbor(0, instance_.getAsks().N() - 1),
      distribution_ap(0.0, 1.0) {}

CASAS::~CASAS() {}

void CASAS::computeAllocation() {
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
      T *= alpha;
    }
  }
}

double CASAS::acceptanceProbability(double T) {
  return exp((_welfare - welfare) / T);
}

void CASAS::neighbor() {
  unsigned int n = instance.getBids().N();

  // update vars
  _y = y;
  _x = x;
  _z = z;
  _welfare = welfare;

  // randomly select one ask
  unsigned int j = distribution_neighbor(generator);
  if (_z[j]) {  // if z_j==1 set it to 0
    _welfare += instance.getAsks().V()[j];
    _z[j] = 0;
    for (unsigned int i = 0; i < n; ++i) {
      if (_y(i, j)) {
        _welfare -= instance.getBids().V()[i];
        _y(i, j) = 0;
        _x[i] = 0;
        break;
      }
    }
  } else {  // z_j==0, try to find a match in the sorted bids
    for (unsigned int i = 0; i < n; ++i) {
      // check if seller j can allocate resources to bidder bid_index[i]
      if (!_x[bid_index[i]] && instance.canAllocate(bid_index[i], j)) {
        _x[bid_index[i]] = 1;
        _y(bid_index[i], j) = 1;
        _z[j] = 1;
        _welfare +=
            instance.getBids().V()[bid_index[i]] - instance.getAsks().V()[j];
        break;
      }
    }
  }
}

void CASAS::generateInitialSolution() {
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
  // T_max = instance.getBids().V()[bid_index[0]] -
  //         instance.getAsks().V()[ask_index[0]];
  auto bid_values = instance.getBids().V();
  auto ask_values = instance.getAsks().V();
  T_max = *(std::max_element(bid_values.begin(), bid_values.end())) -
          *(std::min_element(ask_values.begin(), ask_values.end()));
  // return;
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

void CASAS::resetAllocation() {
  resetBase();
  welfare = 0.;
  z = std::vector<int>(instance.getAsks().N(), 0);
}

bool CASAS::noSideEffects() {
  if (welfare) return false;
  for (unsigned int j = 0; j < instance.getAsks().N(); ++j)
    if (z[j]) return false;
  return noSideEffectsBase();
}