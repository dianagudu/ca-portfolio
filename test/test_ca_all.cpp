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

#include "test/test_ca_all.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TestCAGeneric<TestCAGreedy1>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestCAGeneric<TestCAGreedy2>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestCAGeneric<TestCAGreedy3>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestCAGeneric<TestCAGreedy1S>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestCAGeneric<TestCAHill1>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestCAGeneric<TestCAHill1S>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestCAStochastic<TestCAHill2>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestCAStochastic<TestCAHill2S>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestCAStochastic<TestCASA>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestCAStochastic<TestCASAS>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestCAStochastic<TestCACasanova>);
#ifdef _CPLEX
CPPUNIT_TEST_SUITE_REGISTRATION(TestCAGeneric<TestCACplex>);
CPPUNIT_TEST_SUITE_REGISTRATION(TestCAGeneric<TestCACplexRLPS>);
#endif

TestCAGreedy1::TestCAGreedy1() { type = AuctionType::GREEDY1; }
TestCAGreedy2::TestCAGreedy2() { type = AuctionType::GREEDY2; }
TestCAGreedy3::TestCAGreedy3() { type = AuctionType::GREEDY3; }
TestCAGreedy1S::TestCAGreedy1S() { type = AuctionType::GREEDY1S; }
TestCAHill1::TestCAHill1() { type = AuctionType::HILL1; }
TestCAHill1S::TestCAHill1S() { type = AuctionType::HILL1S; }
TestCAHill2::TestCAHill2() { type = AuctionType::HILL2; }
TestCAHill2S::TestCAHill2S() { type = AuctionType::HILL2S; }
TestCASA::TestCASA() { type = AuctionType::SA; }
TestCASAS::TestCASAS() { type = AuctionType::SAS; }
TestCACasanova::TestCACasanova() { type = AuctionType::CASANOVA; }
TestCACplex::TestCACplex() { type = AuctionType::CPLEX; }
TestCACplexRLPS::TestCACplexRLPS() { type = AuctionType::RLPS; }