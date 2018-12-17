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

#include "test/test_ca.h"

#include <cppunit/TestAssert.h>

#include "src/ca_factory.h"

void TestCA::testNoOversell(void) {
  mTestObj->run();
  auto y = mTestObj->getAllocation();
  for (unsigned int k = 0; k < l; ++k) {
    for (unsigned int j = 0; j < m; ++j) {
      unsigned int sold_jk = 0;
      for (unsigned int i = 0; i < n; ++i) {
        sold_jk += y(i, j) * instance->getBids().Q()(i, k);
      }
      CPPUNIT_ASSERT(sold_jk <= instance->getAsks().Q()(j, k));
    }
  }
  std::cout << "[" << type << "] No overselling" << std::endl;
}

void TestCA::testBudgetBalance(void) {
  mTestObj->run();
  double balance = 0.;
  for (auto& p : mTestObj->getPricingBuyers()) {
    balance += p.second;
  }
  for (auto& p : mTestObj->getPricingSellers()) {
    balance -= p.second;
  }
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0., balance, 1.e-4);
  std::cout << "[" << type << "] Budget-balance" << std::endl;
}

void TestCA::testIndividualRationality(void) {
  mTestObj->run();
  auto price_buyer = mTestObj->getPricingBuyers();
  for (unsigned int i = 0; i < n; ++i) {
    // check that the utility of winning buyers is positive
    if (price_buyer[i] > 0) {
      CPPUNIT_ASSERT_MESSAGE(
          "buyer utility = " +
              std::to_string(instance->getBids().V()[i] - price_buyer[i]),
          instance->getBids().V()[i] - price_buyer[i] >= 0.);
    }
  }
  auto price_seller = mTestObj->getPricingSellers();
  auto y = mTestObj->getAllocation();
  for (unsigned int j = 0; j < m; ++j) {
    // check that the utility of winning sellers is positive
    if (price_seller[j] > 0) {
      double utility = price_seller[j];
      for (unsigned int i = 0; i < n; ++i) {
        utility -= instance->getAsks().V()[j] * y(i, j);
      }
      CPPUNIT_ASSERT_MESSAGE("seller utility = " + std::to_string(utility),
                             utility >= 0.);
    }
  }
  std::cout << "[" << type << "] Individual rationality" << std::endl;
}

void TestCA::testSingleMindedSellers(void) {
  mTestObj->run();
  auto y = mTestObj->getAllocation();
  for (unsigned int j = 0; j < m; ++j) {
    int xj = 0;
    for (unsigned int i = 0; i < n; ++i) {
      xj += y(i, j);
    }
    CPPUNIT_ASSERT(xj <= 1);
  }
  std::cout << "[" << type << "] Single minded sellers" << std::endl;
}

void TestCA::testResetAllocation(void) {
  mTestObj->run();
  mTestObj->resetAllocation();
  CPPUNIT_ASSERT(mTestObj->noSideEffects());

  std::cout << "[" << type << "] No side-effects between runs" << std::endl;
}

void TestCA::testDeterministic(void) {
  mTestObj->run();
  auto y1 = mTestObj->getAllocation();
  mTestObj->run();
  auto y2 = mTestObj->getAllocation();
  for (unsigned int j = 0; j < m; ++j) {
    for (unsigned int i = 0; i < n; ++i) {
      CPPUNIT_ASSERT(y1(i, j) == y2(i, j));
    }
  }
  std::cout << "[" << type << "] Deterministic allocation" << std::endl;
}

void TestCA::setUp(void) {
  // init instance
  instance = new Instance("/tmp/dataset1");
  n = instance->getBids().N();
  m = instance->getAsks().N();
  l = instance->L();

  // init auction object of a certain type
  mTestObj = CAFactory::createAuction(*instance, type);
}

void TestCA::tearDown(void) { delete mTestObj; }

TestCA::TestCA() : type(AuctionType::GREEDY1) {}

TestCA::~TestCA() { delete instance; }
