// --------------------------------------------------------------------------
// Copyright (C) Karlsruhe Institute of Technology, 2015
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

#ifndef TEST_TEST_CA_H_
#define TEST_TEST_CA_H_

#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "src/ca.h"
#include "src/helper.h"
#include "src/instance.h"

class TestCA : public CppUnit::TestFixture {
 public:
  TestCA();
  ~TestCA();
  void setUp(void);
  void tearDown(void);

 protected:
  // check if sellers sell only the amounts they have
  void testNoOversell(void);
  // check if the mechanism is budget-balanced
  void testBudgetBalance(void);
  // check if the mechanism is individually rational
  void testIndividualRationality(void);
  // check single-mindedness of sellers
  void testSingleMindedSellers(void);
  // check side-effects between allocation runs
  void testResetAllocation(void);
  // check same results for deterministic algorithms
  void testDeterministic(void);

 protected:
  unsigned int n;
  unsigned int m;
  unsigned int l;
  Instance* instance;
  AuctionType type;
  CA* mTestObj;
};

#endif  // TEST_TEST_CA_H_
