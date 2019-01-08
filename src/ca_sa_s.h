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

#ifndef CA_SA_S_H_
#define CA_SA_S_H_

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/unordered_map.hpp>

#include <random>
#include <vector>

#include "src/ca.h"

class CASAS : public CA {
 public:
  CASAS(Instance instance_);
  ~CASAS();

  bool noSideEffects();
  void resetAllocation();

 private:
  void computeAllocation();
  void generateInitialSolution();
  void neighbor();
  double acceptanceProbability(double T);

  std::vector<int> z;  // same as x, but for sellers
  double welfare = 0.;

  // temporary vars
  boost::numeric::ublas::matrix<int> _y;
  std::vector<int> _x;
  std::vector<int> _z;
  double _welfare = 0.;

  // SA-specific params
  double T_max;
  const double T_min = 0.00001;
  const double alpha = 0.9;
  const unsigned int niter = 20;

  // variables for random number generation
  std::mt19937_64 generator;
  std::uniform_int_distribution<> distribution_neighbor;
  std::uniform_real_distribution<> distribution_ap;
};

#endif  // CA_SA_S_H_
