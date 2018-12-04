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

#ifndef SRC_CA_H_
#define SRC_CA_H_

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/unordered_map.hpp>

#include <iostream>
#include <vector>

#include "src/bid_set_aux.h"
#include "src/instance.h"
#include "src/stats.h"

enum class RelevanceMode { UNIFORM, SCARCITY, RELATIVE_SCARCITY };

class CA {
 protected:
  // problem instance (a set of bids and asks)
  Instance instance;

  // auxiliary structs
  BidSetAux tmp_bids;
  BidSetAux tmp_asks;

  // output of allocation and pricing
  std::vector<int> x;                    // xi
  boost::numeric::ublas::matrix<int> y;  // yij
  boost::unordered_map<int, double> price_buyer;
  boost::unordered_map<int, double> price_seller;

  // statistics
  Stats stats;

 public:
  CA(Instance _instance);
  CA(Instance _instance, RelevanceMode mode);
  virtual ~CA(){};

  const boost::numeric::ublas::matrix<int> &getAllocation() { return y; }
  const boost::unordered_map<int, double> &getPricingBuyers() { return price_buyer; }
  const boost::unordered_map<int, double> &getPricingSellers() { return price_seller; }
  const Stats &getStats() { return stats; }

  void run();
  void printResults(std::string mechanism_name, std::string solution_file);

 protected:
  void resetFinalPrices();
  void computeStatistics();
  virtual void computeAllocation() = 0;  // WDP to be overwritten for each
                                         // implemented mechanism
  virtual void computeKPricing(double kappa);
};

#endif  // SRC_CA_H_
