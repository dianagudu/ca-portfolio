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

#ifndef TEST_TEST_CA_GENERIC_H_
#define TEST_TEST_CA_GENERIC_H_

#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "src/helper.h"
#include "test/test_ca.h"

template <class A>
class TestCAGeneric : public A {
  CPPUNIT_TEST_SUITE(TestCAGeneric<A>);
  CPPUNIT_TEST(testNoOversell);
  CPPUNIT_TEST(testBudgetBalance);
  CPPUNIT_TEST(testIndividualRationality);
  CPPUNIT_TEST(testSingleMindedSellers);
  CPPUNIT_TEST(testDeterministic);
  CPPUNIT_TEST(testResetAllocation);
  CPPUNIT_TEST_SUITE_END();
};

template <class A>
class TestCAStochastic : public A {
  CPPUNIT_TEST_SUITE(TestCAStochastic<A>);
  CPPUNIT_TEST(testNoOversell);
  CPPUNIT_TEST(testBudgetBalance);
  CPPUNIT_TEST(testIndividualRationality);
  CPPUNIT_TEST(testSingleMindedSellers);
  CPPUNIT_TEST(testResetAllocation);
  CPPUNIT_TEST_SUITE_END();
};

class TestCAGreedy1 : public TestCA {
 public:
  TestCAGreedy1();
};

class TestCAGreedy2 : public TestCA {
 public:
  TestCAGreedy2();
};

class TestCAGreedy3 : public TestCA {
 public:
  TestCAGreedy3();
};

class TestCAGreedy1S : public TestCA {
 public:
  TestCAGreedy1S();
};

class TestCAHill1 : public TestCA {
 public:
  TestCAHill1();
};

class TestCAHill1S : public TestCA {
 public:
  TestCAHill1S();
};

class TestCAHill2 : public TestCA {
 public:
  TestCAHill2();
};

class TestCAHill2S : public TestCA {
 public:
  TestCAHill2S();
};

class TestCASA : public TestCA {
 public:
  TestCASA();
};

class TestCASAS : public TestCA {
 public:
  TestCASAS();
};

class TestCACplex : public TestCA {
 public:
  TestCACplex();
};

class TestCACplexRLPS : public TestCA {
 public:
  TestCACplexRLPS();
};

#endif  // TEST_TEST_CA_GENERIC_H_