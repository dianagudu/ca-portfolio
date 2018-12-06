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

#include "src/runner.h"

#include <boost/unordered_map.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include "src/ca_factory.h"

void Runner::runAlgo(Instance instance, AuctionType type, std::string outfile,
                     std::string infile) {
  try {
    CA* ca = CAFactory::createAuction(instance, type);
    ca->run();
    // ca->printResults(type._to_string());
    auto stats = ca->getStats();
    writeStats(stats, type, outfile, infile);
    delete ca;
  } catch (std::invalid_argument& e) {
    std::cerr << "[WARNING] " << e.what() << std::endl;
  } catch (std::exception& e) {
    std::cerr << "[ERROR] " << e.what() << std::endl;
  }
}

void Runner::runMode(Instance instance, RunMode mode, std::string outfile,
                     std::string infile) {
  std::vector<AuctionType> algos;
  switch (mode) {
    case RunMode::ALL:
      for (auto type : AuctionType::_values()) algos.push_back(type);
      break;
    case RunMode::HEURISTICS:
      for (auto type : AuctionType::_values())
        if (type != +AuctionType::CPLEX && type != +AuctionType::RLPS)
          algos.push_back(type);
      break;
    case RunMode::RANDOM:
      algos.push_back(AuctionType::SA);
      algos.push_back(AuctionType::SAS);
      algos.push_back(AuctionType::CASANOVA);
      algos.push_back(AuctionType::CASANOVAS);
      break;
    case RunMode::SAMPLES:
      for (auto type : AuctionType::_values())
        if (type != +AuctionType::CPLEX && type != +AuctionType::RLPS)
          algos.push_back(type);
      break;
  }

  for (auto type : algos) {
    Runner::runAlgo(instance, type, outfile, infile);
  }
}

void Runner::run(InputParams params) {
  // loop over instance files and write the stats for one instance all at once
  for (auto infile : params.infiles) {
    Instance instance(infile);
    boost::unordered_map<std::string, Stats> stats;

    if (params.algo) {  // when specified, run a single algorithm
      runAlgo(instance, *params.algo, params.outfile, infile);
    } else if (params.mode) {  // when specified, run in given mode
      runMode(instance, *params.mode, params.outfile, infile);
    } else {  // defaults to HEURISTICS mode
      runMode(instance, RunMode::HEURISTICS, params.outfile, infile);
    }
  }
}

void Runner::writeStats(Stats stats, AuctionType type, std::string outfile,
                        std::string infile) {
  // set output mode: standard out or file
  std::ostream* osp = &std::cout;
  std::ofstream fout;
  if (outfile != "") {
    fout.open(outfile, std::ios::app);
    osp = &fout;
  }

  // write stats
  *osp << infile << "," << type << stats << std::endl;

  // close file
  if (outfile != "") {
    fout.close();
  }
}