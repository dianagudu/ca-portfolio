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

#include "ca_cplex.h"

#include <iostream>
#include <random>
#include <string>

CACplex::CACplex(Instance instance_) : CA(instance_) {}

CACplex::~CACplex() {}

void CACplex::computeAllocation() {
  IloEnv env;
  try {
    // create optimization model
    IloModel model(env);
    // create rows and columns for model
    IloNumVarArray var(env);
    IloRangeArray con(env);
    // populate model
    populateByRow(model, var, con);
    // construct cplex object and extract the model
    IloCplex cplex(model);
    // turn output off
    cplex.setOut(env.getNullStream());
    // stop when solution is within 10% of the optimal value
    // cplex.setParam(IloCplex::EpGap, 0.1);
    // cplex.setParam(IloCplex::TiLim, 120);
    // emphasize optimality
    cplex.setParam(IloCplex::Param::Emphasis::MIP, 2);
    // solve the model
    cplex.solve();
    // get status and solution value
    // env.out() << "Solution status = " << cplex.getStatus() << std::endl;
    // env.out() << "Solution value  = " << cplex.getObjValue() << std::endl;
    // write model to file
    // cplex.exportModel("ca.cplex.mip.lp");
    // get values for optimal allocation
    IloNumArray vals(env);
    cplex.getValues(vals, var);
    unsigned int n = instance.getBids().N();
    unsigned int m = instance.getAsks().N();
    for (unsigned int i = 0; i < n; ++i) {
      x[i] = vals[i];
    }
    for (unsigned int i = 0; i < n; ++i) {
      for (unsigned int j = 0; j < m; ++j) {
        y(i, j) = vals[n + i * m + j];
      }
    }
  } catch (IloException& e) {
    std::cerr << "Concert exception caught: " << e << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception caught" << std::endl;
  }

  // terminate Concert application => destroy Ilo environment
  env.end();
}

void CACplex::populateByRow(IloModel model, IloNumVarArray var,
                            IloRangeArray c) {
  unsigned int n = instance.getBids().N();
  unsigned int m = instance.getAsks().N();
  unsigned int l = instance.L();

  IloEnv env = model.getEnv();

  for (unsigned int i = 0; i < n; ++i) {
    var.add(IloNumVar(env, 0.0, 1.0, ILOINT));
    var[i].setName((std::string("x") + std::to_string(i)).c_str());
  }

  for (unsigned int i = 0; i < n; ++i) {
    for (unsigned int j = 0; j < m; ++j) {
      var.add(IloNumVar(env, 0.0, 1.0, ILOINT));
      unsigned int index = n + i * m + j;
      var[index].setName((std::string("y") + std::to_string(i) +
                          std::string("_") + std::to_string(j))
                             .c_str());
    }
  }

  IloExpr expr = IloExpr(env);

  for (unsigned int i = 0; i < n; ++i) {
    expr += instance.getBids().V()[i] * var[i];
    for (unsigned int j = 0; j < m; ++j) {
      unsigned int index = n + i * m + j;
      expr -= instance.getAsks().V()[j] * var[index];
    }
  }

  model.add(IloMaximize(env, expr));

  unsigned int nc = 0;
  for (unsigned int j = 0; j < m; ++j, ++nc) {
    IloExpr constr1 = IloExpr(env);
    for (unsigned int i = 0; i < n; ++i) {
      constr1 += var[n + i * m + j];
    }
    c.add(constr1 <= 1);
    c[nc].setName((std::string("pj") + std::to_string(j)).c_str());
  }

  for (unsigned int i = 0; i < n; ++i, ++nc) {
    IloExpr constr2 = IloExpr(env);
    for (unsigned int j = 0; j < m; ++j) {
      constr2 += var[n + i * m + j];
    }
    constr2 -= var[i];
    c.add(constr2 == 0);
    c[nc].setName((std::string("pi") + std::to_string(i)).c_str());
  }

  for (unsigned int i = 0; i < n; ++i) {
    for (unsigned int k = 0; k < l; ++k) {
      IloExpr constr3 = IloExpr(env);
      constr3 += int(instance.getBids().Q()(i, k)) * var[i];
      for (unsigned int j = 0; j < m; ++j) {
        constr3 -= int(instance.getAsks().Q()(j, k)) * var[n + i * m + j];
      }
      c.add(constr3 <= 0);
      c[nc].setName((std::string("q") + std::to_string(i) + std::string("_") +
                     std::to_string(k))
                        .c_str());
      ++nc;
    }
  }

  model.add(c);
}
