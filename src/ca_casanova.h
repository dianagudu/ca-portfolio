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

#ifndef CA_CASANOVA_H_
#define CA_CASANOVA_H_

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/unordered_map.hpp>

#include <random>
#include <vector>

#include "src/ca.h"

class CACasanova : public CA {
 public:
  CACasanova(Instance instance_);
  ~CACasanova();

  bool noSideEffects();
  void resetAllocation();

 private:
  void computeAllocation();
  void resetBetweenTries();
  inline int age(unsigned int i);
  void insert(unsigned int i);

  std::vector<int> z;
  std::vector<int> bids_sorted;
  std::vector<int> asks_sorted;
  double welfare = 0.;

  boost::unordered_map<int, int> birthday;
  unsigned int era = 0;
  unsigned int maxSteps;

  const double wp = 0.15;  // walk probability
  const double np = 0.5;   // novelty probability
  const unsigned int maxTries = 10;

  // soft restart strategy: if at least theta steps have occured since reinit,
  // but no improvement within the last theta/2 steps
  unsigned int theta;
  unsigned int last_improved_era;

  // variables for random number generation
  std::mt19937_64 generator;
  std::uniform_int_distribution<> distribution_neighbor;
  std::uniform_real_distribution<> distribution_wp;
  std::uniform_real_distribution<> distribution_np;

  // the best solution
  boost::numeric::ublas::matrix<int> best_y;
  std::vector<int> best_x;
  double best_welfare = 0.;
};

#endif  // CA_CASANOVA_H_
