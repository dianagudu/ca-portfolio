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

#include <boost/lexical_cast.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/unordered_map.hpp>

#include <fstream>
#include <iostream>
#include <random>
#include <string>

#include "src/instance.h"
#include "src/ca_greedy1.h"

int main(int argc, char *argv[]) {
  // get instance file and stats file as params
  if (argc != 3 && argc != 4) {
    std::cout << "[ERROR] Usage: " << argv[0] << " <instance_file> <stats_file> <mode>" << std::endl;
    std::cout << ".............. where mode (optional) is one of the following:" << std::endl;
    std::cout << ".............. 0: run all algorithms on the instance" << std::endl;
    std::cout << ".............. 1: (default) run all approximate algorithms on the instance (exclude cplex and rlps)" << std::endl;
    std::cout << ".............. 2: run all approximate algorithms on the instance and samples of the instance" << std::endl;
    std::cout << ".............. 3: run the randomized algorithms on the instance multiple times" << std::endl;
    return 1;
  }
  // read input arguments
  std::string instance_file(argv[1]);
  std::string stats_file(argv[2]);
  int mode;
  if (argc == 3) {
    mode = 1;
  } else {
    mode = std::atoi(argv[3]);
  }
  // load instance from file
  Instance *inst = new Instance(instance_file);
  if (!inst) {
    std::cout << "[ERROR] Could not create Instance!";
    return 2;
  }

  std::cout << inst->L() << ", " << inst->getBids().N() << ", "
            << inst->getAsks().V()[0] << std::endl;

  // create auction object
  CA* ca = new CAGreedy1(*inst);
  ca->run();
  std::cout << ca->getStats() << std::endl;

  delete inst;
  delete ca;

  return 0;
}
