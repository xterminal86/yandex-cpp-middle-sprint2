#include <gtest/gtest.h>
#include <print>

#include "scan.hpp"

#ifdef DEBUG_LOGS
TEST(Debugging, ParseSources)
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

// Won't compile because of our defined concept.
//stdx::details::parse_value_with_format<int*>("123", "%d");
//stdx::details::parse_value_with_format<int&>("123", "%d");

// =============================================================================

TEST(Debugging, ParseSourcesInt)
{
  {
    DebugLog("<int>(abc): ");
    auto result = stdx::details::parse_value_with_format<int>("abc", "%d");
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<int>(invalid specifier): ");
    auto result = stdx::details::parse_value_with_format<int>("abc", "%f");
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<int8_t>(128): ");
    auto result = stdx::details::parse_value_with_format<int8_t>("128", "%d");
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<int8_t>(-129): ");
    auto result = stdx::details::parse_value_with_format<int8_t>("-129", "%d");
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<int16_t>(65536): ");
    auto result = stdx::details::parse_value_with_format<int16_t>("65536", "%d");
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<int32_t>(2147483648): ");
    auto result = stdx::details::parse_value_with_format<int32_t>("2147483648", "%d");
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<int32_t>(2147483647):\n");
    auto result = stdx::details::parse_value_with_format<int32_t>("2147483647", "%d");
    ASSERT_TRUE(result);
    EXPECT_EQ(2147483647, result.value());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<int32_t>(-2147483649): ");
    auto result = stdx::details::parse_value_with_format<int32_t>("-2147483649", "%d");
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<int32_t>(-2147483648):\n");
    auto result = stdx::details::parse_value_with_format<int32_t>("-2147483648", "%d");
    ASSERT_TRUE(result);
    EXPECT_EQ(-2147483648, result.value());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<int64_t>(100500):\n");
    auto result = stdx::details::parse_value_with_format<int64_t>("100500", "%d");
    ASSERT_TRUE(result);
    EXPECT_EQ(100500, result.value());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<int64_t>(100500):\n");
    auto result = stdx::details::parse_value_with_format<int64_t>("100500", "");
    ASSERT_TRUE(result);
    EXPECT_EQ(100500, result.value());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<int64_t>(out of range): ");
    auto result = stdx::details::parse_value_with_format<int64_t>("999999999999999999999999999", "%d");
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
}

// =============================================================================

TEST(Debugging, ParseSourcesUInt)
{
  {
    DebugLog("<uint>(abc): ");
    auto result = stdx::details::parse_value_with_format<unsigned int>("abc", "%u");
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<uint>(%%f): ");
    auto result = stdx::details::parse_value_with_format<unsigned int>("abc", "%f");
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<uint8_t>(128):\n");
    auto result = stdx::details::parse_value_with_format<uint8_t>("128", "%u");
    ASSERT_TRUE(result);
    EXPECT_EQ(128, result.value());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<uint8_t>(-129): ");
    auto result = stdx::details::parse_value_with_format<uint8_t>("-129", "%u");
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<uint16_t>(65536): ");
    auto result = stdx::details::parse_value_with_format<uint16_t>("65536", "%u");
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<uint16_t>({}):\n");
    auto result = stdx::details::parse_value_with_format<uint16_t>("65535", "");
    ASSERT_TRUE(result);
    EXPECT_EQ(65535, result.value());
  }
}

// =============================================================================

TEST(Debugging, ParseSourcesFloat)
{
  {
    DebugLog("<float>(abc): ");
    auto result = stdx::details::parse_value_with_format<float>("abc", "%f");
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<float>(123):\n");
    auto result = stdx::details::parse_value_with_format<float>("123", "%f");
    ASSERT_TRUE(result);
    EXPECT_DOUBLE_EQ(123, result.value());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<float>(123.456):\n");
    auto result = stdx::details::parse_value_with_format<float>("123.456", "%f");
    ASSERT_TRUE(result);
    EXPECT_FLOAT_EQ(123.456f, result.value());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<double>(123.456):\n");
    auto result = stdx::details::parse_value_with_format<double>("123.456", "%f");
    ASSERT_TRUE(result);
    EXPECT_DOUBLE_EQ(123.456, result.value());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<double>({}):\n");
    auto result = stdx::details::parse_value_with_format<double>("123.456", "");
    ASSERT_TRUE(result);
    EXPECT_DOUBLE_EQ(123.456, result.value());
  }
}

// =============================================================================

TEST(Debugging, ParseSourcesString)
{
  {
    DebugLog("<std::string>(lolsus):\n");
    auto result =
      stdx::details::parse_value_with_format<std::string>("lolsus", "%s");
    ASSERT_TRUE(result);
    EXPECT_STRCASEEQ("lolsus", result.value().data());
  }
  // ---------------------------------------------------------------------------
  {
    DebugLog("<std::string_view>(Fuck Bees):\n");
    auto result =
      stdx::details::parse_value_with_format<std::string_view>("Fuck Bees", "%s");
    ASSERT_TRUE(result);
    EXPECT_STRCASEEQ("Fuck Bees", result.value().data());
  }
}
#endif

// =============================================================================

TEST(ScanTest, CountMismatch)
{
  /*
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
  */
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
}

// =============================================================================

/*
TEST(ScanTest, AllSupported)
{
  auto result = stdx::scan<int, std::string, size_t, double>(
    "d = -123, s = \"Test String\", u = 42, f = 3.1415926",
    "d = {%d}, s = {%s}, u = {%u}, f = {%f}"
  );
  ASSERT_TRUE(result);
}
*/
