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

#include "src/ca.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <fstream>
#include <iostream>

using Time = boost::posix_time::ptime;
using TimeDuration = boost::posix_time::time_duration;

// init f_a and f_b attributes to 1 by default
CA::CA(Instance _instance)
    : instance(_instance),
      tmp_bids(BidSetAux(instance.getBids())),
      tmp_asks(BidSetAux(instance.getAsks())),
      x(_instance.getBids().N(), 0),
      y(_instance.getBids().N(), _instance.getAsks().N()) {
  for (unsigned int i = 0; i < _instance.getBids().N(); ++i) {
    bid_index.push_back(i);
  }
  for (unsigned int j = 0; j < _instance.getAsks().N(); ++j) {
    ask_index.push_back(j);
  }
}

CA::CA(Instance _instance, RelevanceMode mode)
    : instance(_instance),
      x(_instance.getBids().N(), 0),
      y(_instance.getBids().N(), _instance.getAsks().N()) {
  std::vector<double> f_b, f_a;
  switch (mode) {
    case RelevanceMode::UNIFORM: {
      f_b = std::vector<double>(instance.L(), 1.);
      f_a = std::vector<double>(instance.L(), 1.);
      break;
    }
    case RelevanceMode::SCARCITY: {
      auto capacity = instance.getAsks().computeQPerResource();
      auto demand = instance.getBids().computeQPerResource();
      for (unsigned int k = 0; k < instance.L(); ++k) {
        f_b.push_back(1. / capacity[k]);
        f_a.push_back(1. / demand[k]);
      }
      break;
    }
    case RelevanceMode::RELATIVE_SCARCITY: {
      auto capacity = instance.getAsks().computeQPerResource();
      auto demand = instance.getBids().computeQPerResource();
      for (unsigned int k = 0; k < instance.L(); ++k) {
        f_b.push_back(std::abs((demand[k] - capacity[k]) * 1. / demand[k]));
        f_a.push_back(std::abs((demand[k] - capacity[k]) * 1. / capacity[k]));
      }
      break;
    }
    default:
      break;
  }
  tmp_bids = BidSetAux(instance.getBids(), f_b);
  tmp_asks = BidSetAux(instance.getAsks(), f_a);
  for (unsigned int i = 0; i < _instance.getBids().N(); ++i) {
    bid_index.push_back(i);
  }
  for (unsigned int j = 0; j < _instance.getAsks().N(); ++j) {
    ask_index.push_back(j);
  }
}

void CA::run() {
  resetAllocation();
  // solve WDP and measure time
  Time t0(boost::posix_time::microsec_clock::local_time());
  computeAllocation();
  Time t1(boost::posix_time::microsec_clock::local_time());
  long usec = (t1 - t0).total_microseconds();
  // compute pricing and stats
  computeKPricing(0.5);
  computeStatistics();
  stats.setTimeWdp(usec / 1000.);
}

void CA::resetAllocation() {
  // reset variables if new allocation must be calculated
  x = std::vector<int>(instance.getBids().N(), 0);
  y = boost::numeric::ublas::zero_matrix<int>(instance.getBids().N(),
                                              instance.getAsks().N());

  for (unsigned int i = 0; i < instance.getBids().N(); ++i) {
    bid_index.push_back(i);
  }
  for (unsigned int j = 0; j < instance.getAsks().N(); ++j) {
    ask_index.push_back(j);
  }

  // initialise all prices to 0
  for (unsigned int i = 0; i < instance.getBids().N(); ++i) {
    price_buyer[i] = 0.;
  }
  for (unsigned int j = 0; j < instance.getAsks().N(); ++j) {
    price_seller[j] = 0.;
  }
}

void CA::computeKPricing(double kappa) {
  // compute prices
  for (unsigned int i = 0; i < instance.getBids().N(); ++i) {
    for (unsigned int j = 0; j < instance.getAsks().N(); ++j) {
      if (y(i, j) > 0) {
        price_buyer[i] = instance.getAsks().V()[j] * kappa +
                         instance.getBids().V()[i] * (1 - kappa);
        price_seller[j] = price_buyer[i];
        break;
      }
    }
  }
}

void CA::computeStatistics() {
  // return welfare / (n + m);
  // only consider winners in calculations
  double welfare = 0.;
  unsigned int num_goods_traded = 0;
  unsigned int num_winners = 0;
  double mean_utility = 0.;
  double stddev_utility = 0.;
  double avg_unit_price = 0.;

  // ========================================================== //
  // compute social welfare, numer of winners and number of units of goods
  // traded buyers
  for (unsigned int i = 0; i < instance.getBids().N(); ++i) {
    if (x[i] > 0) {
      welfare += (instance.getBids().V()[i] - price_buyer[i]);
      ++num_winners;
      for (unsigned int k = 0; k < instance.L(); ++k) {
        num_goods_traded += instance.getBids().Q()(i, k);
      }
      // sellers
      for (unsigned int j = 0; j < instance.getAsks().N(); ++j) {
        if (y(i, j) > 0) {
          welfare += (price_seller[j] - instance.getAsks().V()[j]);
          ++num_winners;
          break;
        }
      }
    }
  }
  // ========================================================== //
  if (num_winners != 0) {
    // compute average and stddev for utility
    mean_utility = welfare / num_winners;
    // buyers
    for (unsigned int i = 0; i < instance.getBids().N(); ++i) {
      if (x[i] > 0) {
        stddev_utility +=
            (instance.getBids().V()[i] - price_buyer[i] - mean_utility) *
            (instance.getBids().V()[i] - price_buyer[i] - mean_utility);
        // sellers
        for (unsigned int j = 0; j < instance.getAsks().N(); ++j) {
          if (y(i, j) > 0) {
            stddev_utility +=
                (price_seller[j] - instance.getAsks().V()[j] - mean_utility) *
                (price_seller[j] - instance.getAsks().V()[j] - mean_utility);
            break;
          }
        }
      }
    }
    stddev_utility = std::sqrt(stddev_utility / num_winners);
    // ========================================================== //
    // compute average price
    for (unsigned int i = 0; i < instance.getBids().N(); ++i) {
      avg_unit_price += price_buyer[i];
    }
    avg_unit_price /= num_goods_traded;
  }
  // ========================================================== //
  // set fields in stats object
  stats.setWelfare(welfare);
  stats.setNumGoodsTraded(num_goods_traded);
  stats.setNumWinners(num_winners);
  stats.setMeanUtility(mean_utility);
  stats.setStddevUtility(stddev_utility);
  stats.setAvgUnitPrice(avg_unit_price);
}

void CA::printResults(std::string mechanism_name) {
  // print summary to standard output
  stats.printFriendly(std::cout, mechanism_name);
}
