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

#ifndef CA_HILL2_H_
#define CA_HILL2_H_

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/unordered_map.hpp>

#include <vector>

#include "src/ca.h"

class CAHill2 : public CA {
 public:
  CAHill2(Instance instance_);
  ~CAHill2();
  
 private:
  void computeAllocation();
  void resetAllocation();
  void generateInitialSolution();
  bool locallyImprove();

  std::vector<int> z;  // same as x, but for sellers
  double welfare = 0.;

  // temporary vars
  boost::numeric::ublas::matrix<int> _y;
  std::vector<int> _x;
  std::vector<int> _z;
  double _welfare = 0.;

  // maximum number of neighbors
  unsigned int num_neighbors = 0;
};

#endif  // CA_HILL2_H_