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

#include <iostream>

#include "src/instance.h"

Instance::Instance(const BidSet &_bids, const BidSet &_asks)
    : bids(_bids), asks(_asks) {}

Instance::Instance(const Instance &copy) : bids(copy.bids), asks(copy.asks) {}

Instance::Instance(std::string filename) {
  YAML::Node inst = YAML::LoadFile(filename);
  // std::cout << inst["params"] << std::endl;
  bids = BidSet::fromYAML(inst["bids"]);
  asks = BidSet::fromYAML(inst["asks"]);
  assert(bids.L() == asks.L());
}

Instance Instance::sample(double sampling_ratio) {
  return Instance(bids.sample(sampling_ratio), asks.sample(sampling_ratio));
}

bool Instance::canAllocate(int bidder, int seller) {
  // no allocation possible if bid value is less than the asked value
  if (bids.V()[bidder] < asks.V()[seller]) return false;

  // no allocation possible if requested quantities are not at least matched
  for (unsigned int k = 0; k < L(); ++k)
    if (bids.Q()(bidder, k) > asks.Q()(seller, k)) return false;

  // all requirements are matched => bidder and seller _can_ trade
  return true;
}