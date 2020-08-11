// Copyright (c) 2019 by Robert Bosch GmbH. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "source/example_component/example_base_class.hpp"
#include "test.hpp"

using namespace ::testing;
using ::testing::Return;

namespace example
{
namespace test
{
/// @req IOX_SWRS_112, IOX_SWRS_200
/// @brief Test goal: "This test suite verifies that the BaseClass function is verified"
/// @pre describe what needs to be done in setup()
/// @post describe what needs to be done in teardown()
/// @note name of the Testfixture should match to the Class you want to test
template <typename T>
class ExampleBaseClass_test : public Test
{
  public:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

/// @note name of the Testcase shall describe the test case in detail to avoid additional comments
TEST_F(ExampleBaseClass_test, FirstUnitTestCase)
{
    ExampleBaseClass<T> testInstance(1, 2);

    testinstance.simplerMethod();

    EXPECT_THAT(testInstance.m_memberVariable, Eq(100u));
}

} // namespace test
} // namespace example