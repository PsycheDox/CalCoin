#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-CAL");
    BOOST_CHECK(GetBoolArg("-CAL"));
    BOOST_CHECK(GetBoolArg("-CAL", false));
    BOOST_CHECK(GetBoolArg("-CAL", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-CALo"));
    BOOST_CHECK(!GetBoolArg("-CALo", false));
    BOOST_CHECK(GetBoolArg("-CALo", true));

    ResetArgs("-CAL=0");
    BOOST_CHECK(!GetBoolArg("-CAL"));
    BOOST_CHECK(!GetBoolArg("-CAL", false));
    BOOST_CHECK(!GetBoolArg("-CAL", true));

    ResetArgs("-CAL=1");
    BOOST_CHECK(GetBoolArg("-CAL"));
    BOOST_CHECK(GetBoolArg("-CAL", false));
    BOOST_CHECK(GetBoolArg("-CAL", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noCAL");
    BOOST_CHECK(!GetBoolArg("-CAL"));
    BOOST_CHECK(!GetBoolArg("-CAL", false));
    BOOST_CHECK(!GetBoolArg("-CAL", true));

    ResetArgs("-noCAL=1");
    BOOST_CHECK(!GetBoolArg("-CAL"));
    BOOST_CHECK(!GetBoolArg("-CAL", false));
    BOOST_CHECK(!GetBoolArg("-CAL", true));

    ResetArgs("-CAL -noCAL");  // -CAL should win
    BOOST_CHECK(GetBoolArg("-CAL"));
    BOOST_CHECK(GetBoolArg("-CAL", false));
    BOOST_CHECK(GetBoolArg("-CAL", true));

    ResetArgs("-CAL=1 -noCAL=1");  // -CAL should win
    BOOST_CHECK(GetBoolArg("-CAL"));
    BOOST_CHECK(GetBoolArg("-CAL", false));
    BOOST_CHECK(GetBoolArg("-CAL", true));

    ResetArgs("-CAL=0 -noCAL=0");  // -CAL should win
    BOOST_CHECK(!GetBoolArg("-CAL"));
    BOOST_CHECK(!GetBoolArg("-CAL", false));
    BOOST_CHECK(!GetBoolArg("-CAL", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--CAL=1");
    BOOST_CHECK(GetBoolArg("-CAL"));
    BOOST_CHECK(GetBoolArg("-CAL", false));
    BOOST_CHECK(GetBoolArg("-CAL", true));

    ResetArgs("--noCAL=1");
    BOOST_CHECK(!GetBoolArg("-CAL"));
    BOOST_CHECK(!GetBoolArg("-CAL", false));
    BOOST_CHECK(!GetBoolArg("-CAL", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CAL", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CAL", "eleven"), "eleven");

    ResetArgs("-CAL -bar");
    BOOST_CHECK_EQUAL(GetArg("-CAL", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CAL", "eleven"), "");

    ResetArgs("-CAL=");
    BOOST_CHECK_EQUAL(GetArg("-CAL", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CAL", "eleven"), "");

    ResetArgs("-CAL=11");
    BOOST_CHECK_EQUAL(GetArg("-CAL", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-CAL", "eleven"), "11");

    ResetArgs("-CAL=eleven");
    BOOST_CHECK_EQUAL(GetArg("-CAL", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-CAL", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CAL", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-CAL", 0), 0);

    ResetArgs("-CAL -bar");
    BOOST_CHECK_EQUAL(GetArg("-CAL", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-CAL=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-CAL", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-CAL=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-CAL", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--CAL");
    BOOST_CHECK_EQUAL(GetBoolArg("-CAL"), true);

    ResetArgs("--CAL=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-CAL", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noCAL");
    BOOST_CHECK(!GetBoolArg("-CAL"));
    BOOST_CHECK(!GetBoolArg("-CAL", true));
    BOOST_CHECK(!GetBoolArg("-CAL", false));

    ResetArgs("-noCAL=1");
    BOOST_CHECK(!GetBoolArg("-CAL"));
    BOOST_CHECK(!GetBoolArg("-CAL", true));
    BOOST_CHECK(!GetBoolArg("-CAL", false));

    ResetArgs("-noCAL=0");
    BOOST_CHECK(GetBoolArg("-CAL"));
    BOOST_CHECK(GetBoolArg("-CAL", true));
    BOOST_CHECK(GetBoolArg("-CAL", false));

    ResetArgs("-CAL --noCAL");
    BOOST_CHECK(GetBoolArg("-CAL"));

    ResetArgs("-noCAL -CAL"); // CAL always wins:
    BOOST_CHECK(GetBoolArg("-CAL"));
}

BOOST_AUTO_TEST_SUITE_END()
