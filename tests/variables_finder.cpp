#define BOOST_TEST_MODULE variables_finder
#include <boost/test/included/unit_test.hpp>
#include "exprtest.hpp"

VARFINDERTEST(variable_finder1, "x", (std::set<std::string>{"x"}))
VARFINDERTEST(variable_finder2, "x + y * z", (std::set<std::string>{"x", "y", "z"}))
VARFINDERTEST(variable_finder3, "t * t + 2 * t + 1", (std::set<std::string>{"t"}))
VARFINDERTEST(variable_finder4, "t * e * rafik + 1", (std::set<std::string>{"t", "rafik"}))