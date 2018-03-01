/**
 * @file nesterov_momentum_sgd_test.cpp
 * @author Sourabh Varshney
 *
 * Test file for NesterovMomentumSGD (Stochastic gradient descent with
 * nesterov momentum updates).
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#include <mlpack/core.hpp>
#include <mlpack/core/optimizers/sgd/sgd.hpp>
#include <mlpack/core/optimizers/sgd/update_policies/gradient_clipping.hpp>
#include <mlpack/core/optimizers/sgd/update_policies/nesterov_momentum_update.hpp>
#include <mlpack/core/optimizers/problems/generalized_rosenbrock_function.hpp>
#include <mlpack/core/optimizers/problems/sgd_test_function.hpp>

#include <boost/test/unit_test.hpp>
#include "test_tools.hpp"

using namespace std;
using namespace arma;
using namespace mlpack;
using namespace mlpack::optimization;
using namespace mlpack::optimization::test;

BOOST_AUTO_TEST_SUITE(NesterovMomentumSGDTest);

BOOST_AUTO_TEST_CASE(NesterovMomentumSGDSpeedUpTestFunction)
{
  SGDTestFunction f;
  NesterovMomentumUpdate nesterovMomentumUpdate();
  NesterovMomentumSGD s(0.0003, 1, 2500000, 1e-9, true,
                        nesterovMomentumUpdate);

  arma::mat coordinates = f.GetInitialPoint();
  double result = s.Optimize(f, coordinates);

  BOOST_REQUIRE_CLOSE(result, -1.0, 0.15);
  BOOST_REQUIRE_SMALL(coordinates[0], 1e-3);
  BOOST_REQUIRE_SMALL(coordinates[1], 1e-7);
  BOOST_REQUIRE_SMALL(coordinates[2], 1e-7);

  // Compare with SGD with vanilla update.
  SGDTestFunction f1;
  StandardSGD s1(0.0003, 1, 2500000, 1e-9, true);

  arma::mat coordinates1 = f.GetInitialPoint();
  double result1 = s1.Optimize(f1, coordinates1);

  // Result doesn't converge in 2500000 iterations.
  BOOST_REQUIRE_GT(result1 + 1.0, 0.05);
  BOOST_REQUIRE_GE(coordinates1[0], 1e-3);
  BOOST_REQUIRE_SMALL(coordinates1[1], 1e-7);
  BOOST_REQUIRE_SMALL(coordinates1[2], 1e-7);

  BOOST_REQUIRE_LE(result, result1);
}

BOOST_AUTO_TEST_CASE(GeneralizedRosenbrockTest)
{
  // Loop over several variants.
  for (size_t i = 10; i < 50; i += 5)
  {
    // Create the generalized Rosenbrock function.
    GeneralizedRosenbrockFunction f(i);
    NesterovMomentumUpdate nesterovMomentumUpdate();
    NesterovMomentumSGD s(0.0008, 1, 0, 1e-15, true, nesterovMomentumUpdate);

    arma::mat coordinates = f.GetInitialPoint();
    double result = s.Optimize(f, coordinates);

    BOOST_REQUIRE_SMALL(result, 1e-4);
    for (size_t j = 0; j < i; ++j)
      BOOST_REQUIRE_CLOSE(coordinates[j], (double) 1.0, 1e-3);
  }
}

BOOST_AUTO_TEST_SUITE_END();
