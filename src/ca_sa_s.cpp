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

#include <boost/lexical_cast.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>
#include <string>
#include <random>
#include <math.h>
#include <fstream>
#include <cstdlib>

using Time = boost::posix_time::ptime;
using TimeDuration = boost::posix_time::time_duration;

CASAS::CASAS(
      int i_n
    , int i_m
    , int i_nres
    , const std::vector<double>& v_b
    , const std::vector<double>& v_a
    , const boost::numeric::ublas::matrix<int>&    m_r
    , const boost::numeric::ublas::matrix<int>&    m_s
    ): CA(i_n, i_m, i_nres, v_b, v_a, m_r, m_s) {}

CASAS::~CASAS() {}

void CASAS::computeAllocation ()
{
  srand(time(NULL));
  generateInitialSolution();

  while(T > T_min) {
    neighbor();
    ap = acceptanceProbability();
    if (ap > (((double) rand() / (RAND_MAX)))) {
      x = _x; y = _y; z = _z; welfare = _welfare;
      if (welfare > best_welfare) {
        best_welfare = welfare;
        best_y = y;
        best_x = x;
      }
    }
    T *= alpha;
  }
  y = _y = best_y;
  x = _x = best_x;
  z = _z;
}

double CASAS::acceptanceProbability() {
  if (welfare == 0) return 2.;
  double ap = exp((_welfare-welfare)/abs(welfare)/T);
  return ap;
}

void CASAS::neighbor() {
  _y = y;
  _x = x;
  _z = z;
  _welfare = welfare;
  int j = rand() % m;
  if (_z[j]) {  // if z_j==1 set it to 0
    _welfare += a[j];
    _z[j] = 0;
    for (int i = 0; i < n; ++i) {
      if (_y(i,j)) {
        _welfare -= b[i];
        _y(i,j) = 0;
        _x[i] = 0;
        break;
      }
    }
  } else {  // z_j==0, set it to 1
    int i = 0;
    while (i < n) {
      // check if seller j has already allocated its resources
      if (_x[bids[i]] == 0) { 
        int k = 0;
        while (k < nres && r(bids[i], k) <= s(j, k)) {
          ++k;
        }
        // seller asks[j] can allocate resources to bidder i
        if (k == nres && b[bids[i]] >= a[j]) {
          _x[bids[i]] = 1;
          _y(bids[i], j) = 1;
          _z[j] = 1;
          _welfare += b[bids[i]] - a[j];
          break;
        }
      }
      ++i;
    }
  }
}

void CASAS::generateInitialSolution() {
  // init y 
  y = boost::numeric::ublas::zero_matrix<int>(n,m);
  x = std::vector<int>(n, 0);
  z = std::vector<int>(n, 0);
  _y = boost::numeric::ublas::zero_matrix<int>(n,m);
  _x = std::vector<int>(n, 0);
  _z = std::vector<int>(n, 0);
  best_y = boost::numeric::ublas::zero_matrix<int>(n,m);
  best_x = std::vector<int>(n, 0);

  // vectors of bid and ask indices => they must be sorted by density
  for (int i = 0; i < n; ++i) {
    bids.push_back(i);
  }
  for (int j = 0; j < m; ++j) {
    asks.push_back(j);
  }

  calculateAttributesFk(1);
  calculateDensities();
  calculateAveragePrices();

  // sort bids descendingly by density
  std::sort(bids.begin(), bids.end(), [this] (int i, int j) -> bool {
    return bid_density[i] > bid_density[j];
    });
  // sort asks ascendingly by density
  std::sort(asks.begin(), asks.end(), [this] (int i, int j) -> bool {
    return ask_density[i] < ask_density[j];
    });

  //compute greedy1 solution
  int i = 0; int j = 0;
  while (i < n && j < m) {
    int k = 0;
    while (k < nres && r(bids[i], k) <= s(asks[j], k)) {
      ++k;
    }
    // seller j can allocate resources to bidder i
    if (k == nres && b[bids[i]] >= a[asks[j]]) {
      x[bids[i]] = 1;
      z[asks[j]] = 1;
      y(bids[i], asks[j]) = 1;
      welfare += b[bids[i]] - a[asks[j]];
      ++j;
    }
    ++i;
  }
}
