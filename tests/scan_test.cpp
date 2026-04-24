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
  // ---------------------------------------------------------------------------
  {
    DebugLog("<int>(abc):\n");
    auto result = stdx::details::parse_value_with_format<int>("abc", "%d");
    ASSERT_FALSE(result);
    DebugLog("parse int: '%s'\n", result.error().message.data());

    DebugLog("<int8_t>(128):\n");
    result = stdx::details::parse_value_with_format<int8_t>("128", "%d");
    ASSERT_FALSE(result);
    DebugLog("parse int8 (out of range): %s\n", result.error().message.data());

    DebugLog("<int8_t>(-129):\n");
    result = stdx::details::parse_value_with_format<int8_t>("-129", "%d");
    ASSERT_FALSE(result);
    DebugLog("parse int8 (out of range): %s\n", result.error().message.data());

    DebugLog("<uint8_t>(-1):\n");
    result = stdx::details::parse_value_with_format<uint8_t>("-1", "%d");
    ASSERT_FALSE(result);
    DebugLog("parse int8 (out of range): %s\n", result.error().message.data());

    DebugLog("<uint8_t>(256):\n");
    result = stdx::details::parse_value_with_format<uint8_t>("256", "%u");
    ASSERT_FALSE(result);
    DebugLog("parse int8 (out of range): %s\n", result.error().message.data());

    DebugLog("<uint8_t>(-1):\n");
    result = stdx::details::parse_value_with_format<uint8_t>("-1", "%u");
    ASSERT_FALSE(result);
    DebugLog("parse int8 (out of range): %s\n", result.error().message.data());

    DebugLog("<int16_t>(65536):\n");
    result = stdx::details::parse_value_with_format<int16_t>("65536", "%d");
    ASSERT_FALSE(result);
    DebugLog("parse int8 (out of range): %s\n", result.error().message.data());

    DebugLog("<int32_t>(2147483648):\n");
    result = stdx::details::parse_value_with_format<int32_t>("2147483648", "%d");
    ASSERT_FALSE(result);
    DebugLog("parse int8 (out of range): %s\n", result.error().message.data());

    DebugLog("<int32_t>(2147483647):\n");
    result = stdx::details::parse_value_with_format<int32_t>("2147483647", "%d");
    ASSERT_TRUE(result);

    DebugLog("<int32_t>(-2147483649):\n");
    result = stdx::details::parse_value_with_format<int32_t>("-2147483649", "%d");
    ASSERT_FALSE(result);
    DebugLog("parse int8 (out of range): %s\n", result.error().message.data());

    DebugLog("<int32_t>(-2147483648):\n");
    result = stdx::details::parse_value_with_format<int32_t>("-2147483648", "%d");
    ASSERT_TRUE(result);

    DebugLog("<std::string>('abcdef'):\n");
    result = stdx::details::parse_value_with_format<std::string>("abcdef", "%s");
    ASSERT_TRUE(result);

    //DebugLog("<std::string_view>('abcdef'):\n");
    //result = stdx::details::parse_value_with_format<std::string_view>("abcdef", "%s");
    //ASSERT_TRUE(result);

    // Won't compile because of our defined concept.
    //stdx::details::parse_value_with_format<int*>("123", "%d");
    //stdx::details::parse_value_with_format<int&>("123", "%d");
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
  // ---------------------------------------------------------------------------
  {
    auto result = stdx::scan(
      "I want to sum 42 and 3.14 numbers.",
      "I want to sum {} and {%f} numbers."
    );
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
}

// =============================================================================

TEST(ScanTest, SimpleTest)
{
  {
    auto result = stdx::scan<int, double>(
      "I want to sum 42 and 3.14 numbers.",
      "I want to sum {} and {%f} numbers."
    );
    ASSERT_TRUE(result);
  }
  // ---------------------------------------------------------------------------
  {
    auto result = stdx::scan(
      "No conversion specifiers here.",
      "No conversion specifiers here."
    );
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
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
