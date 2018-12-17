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

#ifndef SRC_CA_CPLEX_RLPS_H_
#define SRC_CA_CPLEX_RLPS_H_

#include <ilcplex/ilocplex.h>
#include <boost/unordered_map.hpp>
#include <vector>

#include "src/ca.h"

class CACplexRLPS : public CA {
 protected:
  std::vector<double> xi;
  std::vector<double> xj;

 public:
  CACplexRLPS(Instance instance_);
  ~CACplexRLPS();

  bool noSideEffects();
  void resetAllocation();

 private:
  void computeAllocation();
  void populateByRow(IloModel model, IloNumVarArray x, IloRangeArray c);
};

#endif  // SRC_CA_CPLEX_RLPS_H_
