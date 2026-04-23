#include <gtest/gtest.h>
#include <print>

#include "scan.hpp"

#ifdef DEBUG_LOGS
TEST(ScanTest, Debugging)
{
  using namespace stdx::details;

  {
    ParseResult pr;

    pr = stdx::details::parse_sources(
      "I want to sum 42 and 3.14 numbers.",
      "I want to sum {} and {%f} numbers."
    );

    ASSERT_TRUE(pr);
  }
  // ---------------------------------------------------------------------------
  // NOTE: these probably shouldn't work, but since we're just extracting
  // values I guess it's not a big deal.
  {
    ParseResult pr;

    pr = stdx::details::parse_sources(
      "I want to sum  42 and 3.14 numbers.",
      "I want to sum {} and {%f} numbers."
    );

    //ASSERT_FALSE(pr);

    if (!pr)
    {
      std::println("{}", pr.error().message.data());
    }
  }
  // ---------------------------------------------------------------------------
  {
    ParseResult pr;

    pr = stdx::details::parse_sources(
      "I want to sum 42 and 3.14 numbers.",
      "I want to sum{} and {%f} numbers."
    );

    //ASSERT_FALSE(pr);

    if (!pr)
    {
      std::println("{}", pr.error().message.data());
    }
  }
}
#endif

// =============================================================================

TEST(ScanTest, CountMismatch)
{
  {
    auto result = stdx::scan<int, double, std::string>(
        "I want to sum 42 and 3.14 numbers.",
        "I want to sum {} and {%f} numbers."
    );
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
  // ---------------------------------------------------------------------------
  {
    auto result = stdx::scan<int>(
        "I want to sum 42 and 3.14 numbers.",
        "I want to sum {} and {%f} numbers."
    );
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
  // ---------------------------------------------------------------------------
  {
    auto result = stdx::scan<int, double>(
        "I want to sum 42 and 3.14 numbers.",
        "I want to sum {} and {%f} {} numbers."
    );
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
}

// =============================================================================

TEST(ScanTest, SimpleTest)
{
  auto result = stdx::scan<int, double>(
    "I want to sum 42 and 3.14 numbers.",
    "I want to sum {} and {%f} numbers."
  );
  ASSERT_TRUE(result);
}

// =============================================================================

TEST(ScanTest, AllSupported)
{
  auto result = stdx::scan<int, std::string, size_t, double>(
    "d = -123, s = \"Test String\", u = 42, f = 3.1415926",
    "d = {%d}, s = {%s}, u = {%u}, f = {%f}"
  );
  ASSERT_TRUE(result);
}
