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

#ifndef SRC_STATS_H_
#define SRC_STATS_H_

#include <iostream>

class Stats {
 private:
    double time_wdp;        // execution time for the WDP (allocation) in ms
    double welfare;         // calculated social welfare
    int num_goods_traded;   // number of allocated goods
    int num_winners;        // auction winners including buyers and sellers
    double mean_utility;    // average utility of auction winners
    double stddev_utility;  // standard deviation of utility of auciton winners
    double avg_unit_price;  // average price per unit of goods traded
 public:
    Stats():
          time_wdp(0.)
        , welfare(0.)
        , num_goods_traded(0)
        , num_winners(0)
        , mean_utility(0.)
        , stddev_utility(0.)
        , avg_unit_price(0.)
    {}
    ~Stats() {}
    // getters
    const double getTimeWdp() const { return time_wdp; }
    const double getWelfare() const { return welfare; }
    const int getNumGoodsTraded() const { return num_goods_traded; }
    const int getNumWinners() const { return num_winners; }
    const double getMeanUtility() const { return mean_utility; }
    const double getStddevUtility() const { return stddev_utility; }
    const double getAvgUnitPrice() const { return avg_unit_price; }
    // setters
    void setTimeWdp(double f_time_wdp) { time_wdp = f_time_wdp; }
    void setWelfare(double f_welfare) { welfare = f_welfare; }
    void setNumGoodsTraded(int i_num_goods_traded) { num_goods_traded = i_num_goods_traded; }
    void setNumWinners(int i_num_winners) { num_winners = i_num_winners; }
    void setMeanUtility(double f_mean_utility) { mean_utility = f_mean_utility; }
    void setStddevUtility(double f_stddev_utility) { stddev_utility = f_stddev_utility; }
    void setAvgUnitPrice(double f_avg_unit_price) {avg_unit_price = f_avg_unit_price; }

    // print formatted stats
    void printFriendly(std::ostream& out, std::string mechanism_name) {
        out << "=======\t"<< mechanism_name << " ========" << std::endl;
        out << "time wdp       = " << time_wdp << std::endl;
        out << "welfare        = " << welfare << std::endl;
        out << "num goods      = " << num_goods_traded << std::endl;
        out << "avg utility    = " << mean_utility << std::endl;
        out << "stddev utility = " << stddev_utility << std::endl;
        out << "avg price      = " << avg_unit_price << std::endl;
        out << "=============================" << std::endl;
    }

    // print comma separated stats
    friend std::ostream& operator<<(std::ostream& out, const Stats& s) {
        return out
            << "," << s.time_wdp
            << "," << s.welfare
            << "," << s.num_goods_traded
            << "," << s.num_winners
            << "," << s.mean_utility
            << "," << s.stddev_utility
            << "," << s.avg_unit_price
            ;
    }
};

#endif  // SRC_STATS_H_
