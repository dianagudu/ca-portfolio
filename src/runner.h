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

#ifndef SRC_RUNNER_H_
#define SRC_RUNNER_H_

#include <iostream>
#include <string>

#include "src/helper.h"
#include "src/instance.h"
#include "src/stats.h"

class Runner {
 public:
  static void run(InputParams params);

 private:
  static void runAlgo(Instance instance, AuctionType type, std::string outfile,
                      std::string infile);
  static void runMode(Instance instance, RunMode mode, std::string outfile,
                      std::string infile);
  static void writeStats(Stats stats, AuctionType type, std::string outfile,
                         std::string infile);
};

#endif  // SRC_RUNNER_H_