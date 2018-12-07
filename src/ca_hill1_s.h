// --------------------------------------------------------------------------
// Copyright (C) Karlsruhe Institute of Technology, 2016
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

#ifndef CA_HILL1_S_H_
#define CA_HILL1_S_H_

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/unordered_map.hpp>

#include <vector>

#include "src/ca.h"

class CAHill1S : public CA {
 public:
  CAHill1S(Instance instance_);
  ~CAHill1S();
  void computeAllocation();

 private:
  void doGreedy();
  bool locallyImprove();

  double welfare = 0.;

  // temporary vars
  boost::numeric::ublas::matrix<int> _y;
  std::vector<int> _x;
  double _welfare = 0.;

  // critical index, or last allocated ask
  unsigned int critical_j;
};

#endif  // CA_HILL1_S_H_