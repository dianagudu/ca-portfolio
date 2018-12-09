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

#ifndef SRC_CA_FACTORY_H_
#define SRC_CA_FACTORY_H_

#include "src/ca.h"
#include "src/ca_greedy1.h"
#include "src/ca_greedy1_s.h"
#include "src/ca_greedy2.h"
#include "src/ca_greedy3.h"
#include "src/ca_hill1.h"
#include "src/ca_hill1_s.h"
#include "src/ca_hill2.h"
#include "src/ca_hill2_s.h"
#include "src/ca_sa.h"
#include "src/ca_sa_s.h"
#include "src/helper.h"

class CAFactory {
 public:
  static CA* createAuction(Instance instance, AuctionType type) {
    switch (type) {
      case AuctionType::GREEDY1:
        return new CAGreedy1(instance);
      case AuctionType::GREEDY2:
        return new CAGreedy2(instance);
      case AuctionType::GREEDY3:
        return new CAGreedy3(instance);
      case AuctionType::GREEDY1S:
        return new CAGreedy1S(instance);
      case AuctionType::HILL1:
        return new CAHill1(instance);
      case AuctionType::HILL1S:
        return new CAHill1S(instance);
      case AuctionType::HILL2:
        return new CAHill2(instance);
      case AuctionType::HILL2S:
        return new CAHill2S(instance);
      case AuctionType::SA:
        return new CASA(instance);
      case AuctionType::SAS:
        return new CASAS(instance);
      case AuctionType::CASANOVA:
        // return new CACasanova(instance);
      case AuctionType::CASANOVAS:
        // return new CACasanovaS(instance);
#ifdef _CPLEX
      case AuctionType::CPLEX:
        // return new CACplex(instance);
      case AuctionType::RLPS:
        // return new CARlps(instance);
#endif
      default:
        throw std::invalid_argument(std::string(type._to_string()) +
                                    ": algorithm not implemented!");
    }
  }
};

#endif  // SRC_CA_FACTORY_H_
