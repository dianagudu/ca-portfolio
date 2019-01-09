// --------------------------------------------------------------------------
// Copyright (C) Karlsruhe Institute of Technology, 2018
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

#ifndef SRC_BID_SET_AUX_H_
#define SRC_BID_SET_AUX_H_

#include <boost/unordered_map.hpp>
#include <vector>

#include "src/bid_set.h"

class BidSetAux {
 protected:
  std::vector<double> f;                                 // relevance factors
  boost::unordered_map<unsigned int, double> avg_price;  // average prices
  boost::unordered_map<unsigned int, double> density;    // densities

 public:
  BidSetAux() {}
  BidSetAux(BidSet bidset);
  BidSetAux(BidSet bidset, std::vector<double> _f);

  inline boost::unordered_map<unsigned int, double> &getDensity() { return density; }
  inline boost::unordered_map<unsigned int, double> &getAvgPrice() { return avg_price; }
};

#endif  // SRC_BID_SET_AUX_H_