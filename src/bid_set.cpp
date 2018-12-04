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

#include "src/bid_set.h"

BidSet::BidSet(const std::vector<double> &v_v,
               const boost::numeric::ublas::matrix<int> &m_q)
    : values(v_v), quantities(m_q) {}

BidSet::BidSet(const BidSet &copy)
    : values(copy.values), quantities(copy.quantities) {}

BidSet BidSet::fromYAML(YAML::Node bidset) {
  auto values = bidset["values"].as<std::vector<double>>();

  std::vector<unsigned int> v_q;
  for (auto row : bidset["quantities"]) {
    auto v_row = row.as<std::vector<unsigned int>>();
    v_q.insert(v_q.end(), v_row.begin(), v_row.end());
  }

  auto nrows = bidset["quantities"].size();
  auto ncols = v_q.size() / nrows;

  boost::numeric::ublas::matrix<unsigned int> quantities(nrows, ncols);
  std::copy(v_q.begin(), v_q.end(), quantities.data().begin());

  return BidSet(values, quantities);
}

boost::unordered_map<unsigned int, double> BidSet::computeAvgPrices() {
  boost::unordered_map<unsigned int, double> avg_price;
  for (unsigned int i = 0; i < N(); ++i) {
    unsigned int q_i = 0;
    for (unsigned int k = 0; k < L(); ++k) {
      q_i += quantities(i, k);
    }
    avg_price[i] = values[i] / q_i;
  }
  return avg_price;
}

boost::unordered_map<unsigned int, double> BidSet::computeDensities() {
    return computeDensities(std::vector<double>(L(), 1.));
}

boost::unordered_map<unsigned int, double> BidSet::computeDensities(
    std::vector<double> f) {
  boost::unordered_map<unsigned int, double> density;
  for (unsigned int i = 0; i < N(); ++i) {
    double m_i = 0;
    for (unsigned int k = 0; k < L(); ++k) {
      m_i += quantities(i, k) * f[k];
    }
    density[i] = values[i] / std::sqrt(m_i);
  }
  return density;
}

std::vector<unsigned int> BidSet::computeQPerResource() {
  std::vector<unsigned int> qpr(L(), 0);
  for (unsigned int i = 0; i < N(); ++i) {
    for (unsigned int k = 0; k < L(); ++k) {
      qpr[k] += quantities(i, k);
    }
  }
  return qpr;
}