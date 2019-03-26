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
                     std::string infile, double sampling_ratio) {
  try {
    CA* ca = CAFactory::createAuction(instance, type);
    if (!ca)
      throw std::invalid_argument(
          std::string("Something went wrong when creating auction of type ") +
          type._to_string());
    unsigned int nruns = 1;
    if (isStochastic(type)) nruns = 10;
    for (unsigned int run = 0; run < nruns; ++run) {
      ca->run();
      // ca->printResults(type._to_string());
      auto stats = ca->getStats();
      writeStats(stats, type, outfile, infile, sampling_ratio);
    }
    delete ca;
  } catch (std::invalid_argument& e) {
    std::cerr << "[WARNING] " << e.what() << std::endl;
  } catch (std::exception& e) {
    std::cerr << "[ERROR] " << e.what() << std::endl;
  }
}

void Runner::runMode(Instance instance, RunMode mode, std::string outfile,
                     std::string infile) {
  switch (mode) {
    case RunMode::ALL:
      for (auto type : AuctionType::_values())
        Runner::runAlgo(instance, type, outfile, infile, 1.0);
      break;
    case RunMode::HEURISTICS:
      for (auto type : AuctionType::_values())
        if (type != +AuctionType::CPLEX && type != +AuctionType::RLPS)
          Runner::runAlgo(instance, type, outfile, infile, 1.0);
      break;
    case RunMode::SAMPLES:
      {
        double sampling_ratios[] = {0.05, 0.1,  0.15, 0.2,  0.25, 0.3,  0.35,
                                    0.4,  0.45, 0.5,  0.55, 0.6,  0.65, 0.7,
                                    0.75, 0.8,  0.85, 0.9,  0.95};
        for (double sampling_ratio : sampling_ratios) {
          Instance probe = instance.sample(sampling_ratio);
          for (auto type : AuctionType::_values())
            if (type != +AuctionType::CPLEX && type != +AuctionType::RLPS)
              Runner::runAlgo(probe, type, outfile, infile,
                              sampling_ratio);
        }
      }
      break;
    case RunMode::RANDOM:
      for (auto type : AuctionType::_values())
        if (isStochastic(type))
          Runner::runAlgo(instance, type, outfile, infile, 1.0);
      break;
  }
}

void Runner::run(InputParams params) {
  // loop over instance files and write the stats for one instance all at once
  for (auto infile : params.infiles) {
    Instance instance(infile);
    boost::unordered_map<std::string, Stats> stats;

    if (params.algo) {  // when specified, run a single algorithm
      runAlgo(instance, *params.algo, params.outfile, infile, 1.0);
    } else if (params.mode) {  // when specified, run in given mode
      runMode(instance, *params.mode, params.outfile, infile);
    } else {  // defaults to HEURISTICS mode
      runMode(instance, RunMode::HEURISTICS, params.outfile, infile);
    }
  }
}

void Runner::writeStats(Stats stats, AuctionType type, std::string outfile,
                        std::string infile, double sampling_ratio) {
  // set output mode: standard out or file
  std::ostream* osp = &std::cout;
  std::ofstream fout;
  if (outfile != "") {
    fout.open(outfile, std::ios::app);
    osp = &fout;
  }

  // write stats
  if (sampling_ratio == 1.0)
    *osp << infile << "," << type << stats << std::endl;
  else
    *osp << sampling_ratio << "," << infile << "," << type << stats
         << std::endl;

  // close file
  if (outfile != "") {
    fout.close();
  }
}
