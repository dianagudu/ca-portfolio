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

#ifndef SRC_BID_SET_H_
#define SRC_BID_SET_H_

#include <yaml-cpp/yaml.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/unordered_map.hpp>
#include <vector>

class BidSet {
 protected:
  std::vector<double> values;
  boost::numeric::ublas::matrix<unsigned int> quantities;

 public:
  BidSet(const std::vector<double> &v_v,
         const boost::numeric::ublas::matrix<int> &m_q);  // generic constructor
  BidSet(const BidSet &copy);                             // copy constructor
  BidSet() {}                                             // default constructor
  static BidSet fromYAML(YAML::Node bidset);
  BidSet sample(double sampling_ratio);

  inline unsigned int N() const { return quantities.size1(); }
  inline unsigned int L() const { return quantities.size2(); }
  inline const auto &V() const { return values; }
  inline const auto &Q() const { return quantities; }

  boost::unordered_map<unsigned int, double> computeAvgPrices() const;
  boost::unordered_map<unsigned int, double> computeDensities() const;
  boost::unordered_map<unsigned int, double> computeDensities(
      std::vector<double> f) const;
  std::vector<unsigned int> computeQPerResource() const;
};

#endif  // SRC_BID_SET_H_