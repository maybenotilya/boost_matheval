#include "matheval.hpp"

#include <boost/math/constants/constants.hpp>

#include <string>



#define EXPRTEST(casename, expr, expected)                             \
BOOST_AUTO_TEST_CASE( casename )                                       \
{                                                                      \
    std::string const s = expr;                                        \
    std::map<std::string, double> st;                                  \
    double result;                                                     \
    BOOST_CHECK_NO_THROW(result = matheval::parse(s, st));             \
    BOOST_CHECK_CLOSE(result, (expected),                              \
                      std::numeric_limits<double>::epsilon());         \
}

#define SYMEXPRTEST(casename, expr, st, expected)                      \
BOOST_AUTO_TEST_CASE( casename )                                       \
{                                                                      \
    std::string const s = expr;                                        \
    double result;                                                     \
    BOOST_CHECK_NO_THROW(result = matheval::parse(s, st));             \
    BOOST_CHECK_CLOSE(result, (expected),                              \
                      std::numeric_limits<double>::epsilon());         \
}

#define VARFINDERTEST(casename, expr, expected)                        \
BOOST_AUTO_TEST_CASE( casename )                                       \
{                                                                      \
    std::string s = expr;                                              \
    matheval::Parser parser;                                           \
    parser.parse(s);                                                   \
    std::set<std::string> result;                                      \
    BOOST_CHECK_NO_THROW(result = parser.find_variables());            \
    BOOST_CHECK_EQUAL(result == (expected), true);                     \
}