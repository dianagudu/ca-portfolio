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

#ifndef SRC_INSTANCE_H_
#define SRC_INSTANCE_H_

#include "src/bid_set.h"

class Instance {
 protected:
  BidSet bids;
  BidSet asks;

 public:
  Instance(const BidSet &_bids, const BidSet &_asks);  // generic constructor
  Instance(const Instance &copy);                      // copy constructor
  Instance(std::string filename);  // creates instance from input file
  ~Instance(){};

  bool canAllocate(int bidder, int seller);

  inline unsigned int L() { return bids.L(); }
  const BidSet &getBids() { return bids; }
  const BidSet &getAsks() { return asks; }
};

#endif  // SRC_INSTANCE_H_