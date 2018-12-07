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

#include "src/helper.h"

#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include <iomanip>
#include <iostream>

void usage(char* program_name, boost::program_options::options_description desc) {
  std::cout << "Usage: " << program_name << " [-m MODE] [-o OUTFILE] [-i] INFILE(s)" << std::endl
            << "   or: " << program_name << " [-a ALGO] [-o OUTFILE] [-i] INFILE(s)" << std::endl
            << std::endl << "Run algorithm portfolio on auction instance(s) stored in INFILE(s)."
            << std::endl << "By default, the portfolio is run in HEURISTICS mode, and stats are"
            << std::endl << "printed to standard out."
            << std::endl;

  std::cout << std::endl << desc << std::endl;

  std::cout << std::endl << "Valid MODE values are:" << std::endl;

  for (auto mode : RunMode::_values()) {
    std::cout << "\t" << std::left << std::setw(10)
              << mode._to_string() << ": "
              << mode_descriptions[mode] << std::endl;
  }

  std::cout << std::endl
            << "Valid ALGO values are:"
            << std::endl;
  for (auto type : AuctionType::_values()) {
    std::cout << "\t" << std::left << std::setw(10)
              << type._to_string() << ": "
              << algo_descriptions[type] << std::endl;
  }
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v) {
  if (!v.empty()) {
    out << '[';
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
    out << "\b\b]";
  }
  return out;
}

// Function used to check that 'opt1' and 'opt2' are not specified
// at the same time.
void conflicting_options(const boost::program_options::variables_map& vm,
                         const char* opt1, const char* opt2) {
  if (vm.count(opt1) && !vm[opt1].defaulted() && vm.count(opt2) &&
      !vm[opt2].defaulted())
    throw std::logic_error(std::string("conflicting options '") + opt1 +
                           "' and '" + opt2 + "'.");
}

boost::optional<InputParams> parse(int argc, char* argv[]) {
  try {
    InputParams params;
    std::string mode;
    std::string algo;

    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "show this help message")
        ("mode,m", po::value<std::string>(&mode)->
                   default_value(std::string("HEURISTICS"))->
                   value_name("MODE"),
                   "run portfolio in given mode")
        ("algo,a", po::value<std::string>(&algo)->
                   value_name("ALGO"),
                   "run only specified algorithm")
        ("out,o", po::value<std::string>(&params.outfile)->
                  value_name("OUTFILE"),
                  "output file to store runtime stats")
        ("in,i", po::value<std::vector<std::string>>(&params.infiles)->
                 value_name("INFILE(s)"),
                 "input files, one per auction instance")
    ;
    po::positional_options_description p;
    p.add("in", -1);
    po::variables_map vm;
    po::store(
        po::command_line_parser(argc, argv).options(desc).positional(p).run(),
        vm);
    po::notify(vm);

    conflicting_options(vm, "mode", "algo");

    if (vm.count("help")) {
      usage(argv[0], desc);
      return {};
    }

    if (!vm.count("in")) {
      throw std::logic_error(std::string("missing INFILE argument"));
    }

    if (vm.count("mode")) {
      // validate run mode
      if (!RunMode::_is_valid_nocase(mode.c_str()))
        throw std::invalid_argument(std::string("mode ") + mode + " invalid.");
    }

    if (vm.count("algo")) {
      // validate algorithm in algo mode
      if (!AuctionType::_is_valid_nocase(algo.c_str()))
        throw std::invalid_argument(std::string("algorithm ") + algo +
                                    " invalid.");
    }

    params.mode = RunMode::_from_string_nocase_nothrow(mode.c_str());
    params.algo = AuctionType::_from_string_nocase_nothrow(algo.c_str());

    return params;
  } catch (std::exception& e) {
    std::cerr << argv[0] << ": " << e.what() << std::endl;
    std::cerr << "Try '" << argv[0] << " --help' for more information." << std::endl;
    return {};
  }
}
