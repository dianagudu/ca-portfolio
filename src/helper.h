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

#ifndef SRC_HELPER_H_
#define SRC_HELPER_H_

#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include "src/enum.h"

BETTER_ENUM(RelevanceMode, int,
  UNIFORM = 1,
  SCARCITY,
  RELATIVE_SCARCITY
)

BETTER_ENUM(AuctionType, int,
  GREEDY1 = 1,
  GREEDY2,
  GREEDY3,
  GREEDY1S,
  HILL1,
  HILL1S,
  HILL2,
  HILL2S,
  SA,
  SAS,
  CASANOVA,
  CASANOVAS,
  CPLEX,
  RLPS
)

BETTER_ENUM(RunMode, int,
  ALL = 0,
  HEURISTICS,
  SAMPLES
)

constexpr const char* describe_algorithms(AuctionType type) {
  switch (type) {
    case AuctionType::GREEDY1: return "greedy algorihm";
    case AuctionType::GREEDY2: return "greedy algorihm with scarcity-based relevance factors";
    case AuctionType::GREEDY3: return "greedy algorihm with relative scarcity-based relevance factors";
    case AuctionType::GREEDY1S: return "greedy algorihm with focus on sellers";
    case AuctionType::HILL1: return "hill climbing algorihm";
    case AuctionType::HILL1S: return "hill climbing algorihm with focus on sellers";
    case AuctionType::HILL2: return "hill climbing algorihm";
    case AuctionType::HILL2S: return "hill climbing algorihm with focus on sellers";
    case AuctionType::SA: return "simulated annealing algorithm";
    case AuctionType::SAS: return "simulated annealing algorithm with focus on sellers";
    case AuctionType::CASANOVA: return "Casanova algorithm (stochastic local search)";
    case AuctionType::CASANOVAS: return "Casanova algorithm (stochastic local search) with focus on sellers";
    case AuctionType::CPLEX: return "optimal algorithm using CPLEX library to solve MILP";
    case AuctionType::RLPS: return "heuristic based on relaxed linear program (requires CPLEX library)";
    default: return "invalid auction type";
  }
}

constexpr const char* describe_run_modes(RunMode mode) {
  switch (mode) {
    case RunMode::ALL: return "run all algorithms";
    case RunMode::HEURISTICS: return "run all heuristic algorithms (exclude CPLEX and RLPS from all)";
    case RunMode::SAMPLES: return "run all heuristic algorithms on instance and samples";
    default: return "invalid mode";
  }
}

constexpr auto algo_descriptions = better_enums::make_map(describe_algorithms);
constexpr auto mode_descriptions = better_enums::make_map(describe_run_modes);


typedef struct _InputParams_ {
    better_enums::optional<RunMode> mode;
    better_enums::optional<AuctionType> algo;
    std::string outfile;
    std::vector<std::string> infiles;
} InputParams;

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v);

void conflicting_options(const boost::program_options::variables_map& vm,
                         const char* opt1, const char* opt2);

// prints program usage
void usage(char* program_name, boost::program_options::options_description);

// parses command line arguments
boost::optional<InputParams> parse(int argc, char* argv[]);

// whether an algorihtm is stochastic => will be run multiple times
bool isStochastic(AuctionType type);

#endif  // SRC_HELPER_H_
