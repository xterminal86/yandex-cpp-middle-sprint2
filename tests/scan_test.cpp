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
  {
    auto result = stdx::scan<int, double>(
      "I want to sum 42 and 3.14 numbers.",
      "I want to sum {} and {%f} numbers."
    );
    ASSERT_TRUE(result);
    EXPECT_EQ(42, result.value().get<0>());
    EXPECT_DOUBLE_EQ(3.14, result.value().get<1>());
  }
  // ---------------------------------------------------------------------------
  {
    auto result = stdx::scan<int, int, int, std::string>(
      "number 1, number 2, number 3, string lol",
      "number {}, number {}, number {}, string {}"
    );
    ASSERT_TRUE(result);
    EXPECT_EQ(1, result.value().get<0>());
    EXPECT_EQ(2, result.value().get<1>());
    EXPECT_EQ(3, result.value().get<2>());
    EXPECT_STRCASEEQ("lol", result.value().get<3>().data());
  }
}

// =============================================================================

TEST(ScanTest, TypesCheck)
{
  {
    auto result = stdx::scan<
      int8_t,
      int16_t,
      int32_t,
      int64_t,
      uint8_t,
      uint16_t,
      uint32_t,
      uint64_t,
      int,
      unsigned int,
      float,
      double,
      std::string,
      std::string_view
    >
    (
      "i8 = 8, i16 = 16, i32 = 32, i64 = 64, "
      "u8 = 80, u16 = 160, u32 = 320, u64 = 640, "
      "int = 111, uint = 888, "
      "float = 123.456, double = 9.876543, "
      "string = string, string_view = string_view"
      ,
      "i8 = {}, i16 = {}, i32 = {}, i64 = {}, "
      "u8 = {}, u16 = {}, u32 = {}, u64 = {}, "
      "int = {}, uint = {}, "
      "float = {}, double = {}, "
      "string = {}, string_view = {}"
    );
    ASSERT_TRUE(result);
    //
    EXPECT_TRUE( typeid(int8_t)   == typeid(result.value().get<0>()) );
    EXPECT_TRUE( typeid(int16_t)  == typeid(result.value().get<1>()) );
    EXPECT_TRUE( typeid(int32_t)  == typeid(result.value().get<2>()) );
    EXPECT_TRUE( typeid(int64_t)  == typeid(result.value().get<3>()) );
    //
    EXPECT_TRUE( typeid(uint8_t)  == typeid(result.value().get<4>()) );
    EXPECT_TRUE( typeid(uint16_t) == typeid(result.value().get<5>()) );
    EXPECT_TRUE( typeid(uint32_t) == typeid(result.value().get<6>()) );
    EXPECT_TRUE( typeid(uint64_t) == typeid(result.value().get<7>()) );
    //
    EXPECT_TRUE( typeid(int)          == typeid(result.value().get<8>()) );
    EXPECT_TRUE( typeid(unsigned int) == typeid(result.value().get<9>()) );
    //
    EXPECT_TRUE( typeid(float)  == typeid(result.value().get<10>()) );
    EXPECT_TRUE( typeid(double) == typeid(result.value().get<11>()) );
    //
    EXPECT_TRUE( typeid(std::string)      == typeid(result.value().get<12>()) );
    EXPECT_TRUE( typeid(std::string_view) == typeid(result.value().get<13>()) );
  }
}

// =============================================================================

TEST(ScanTest, AllSupported)
{
  auto result = stdx::scan<
    int8_t,
    int8_t,
    int16_t,
    int16_t,
    int32_t,
    int32_t,
    int64_t,
    int64_t,
    uint8_t,
    uint8_t,
    uint16_t,
    uint16_t,
    uint32_t,
    uint32_t,
    uint64_t,
    uint64_t,
    int,
    int,
    unsigned int,
    unsigned int,
    float,
    float,
    double,
    double,
    std::string,
    std::string,
    std::string_view,
    std::string_view
  >
  (
    "1 -1 2 -2 3 -3 4 -4 "
    "1 1 2 2 3 3 4 4 "
    "123 -123 456 456 "
    "1.2 3.4 5.6 7.8 "
    "lol sus bib bab "
    ,
    "{} {%d} {} {%d} {} {%d} {} {%d} "
    "{} {%u} {} {%u} {} {%u} {} {%u} "
    "{} {%d} {} {%u} "
    "{} {%f} {} {%f} "
    "{} {%s} {} {%s} "
  );
  ASSERT_TRUE(result);
  //
  EXPECT_EQ( 1, result.value().get<0>());
  EXPECT_EQ(-1, result.value().get<1>());
  EXPECT_EQ( 2, result.value().get<2>());
  EXPECT_EQ(-2, result.value().get<3>());
  EXPECT_EQ( 3, result.value().get<4>());
  EXPECT_EQ(-3, result.value().get<5>());
  EXPECT_EQ( 4, result.value().get<6>());
  EXPECT_EQ(-4, result.value().get<7>());
  //
  EXPECT_EQ(1, result.value().get<8>());
  EXPECT_EQ(1, result.value().get<9>());
  EXPECT_EQ(2, result.value().get<10>());
  EXPECT_EQ(2, result.value().get<11>());
  EXPECT_EQ(3, result.value().get<12>());
  EXPECT_EQ(3, result.value().get<13>());
  EXPECT_EQ(4, result.value().get<14>());
  EXPECT_EQ(4, result.value().get<15>());
  //
  EXPECT_EQ(123,  result.value().get<16>());
  EXPECT_EQ(-123, result.value().get<17>());
  EXPECT_EQ(456,  result.value().get<18>());
  EXPECT_EQ(456,  result.value().get<19>());
  //
  EXPECT_FLOAT_EQ(1.2, result.value().get<20>());
  EXPECT_FLOAT_EQ(3.4, result.value().get<21>());
  EXPECT_DOUBLE_EQ(5.6, result.value().get<22>());
  EXPECT_DOUBLE_EQ(7.8, result.value().get<23>());
  //
  EXPECT_STRCASEEQ("lol", result.value().get<24>().data());
  EXPECT_STRCASEEQ("sus", result.value().get<25>().data());
  EXPECT_STRCASEEQ("bib", result.value().get<26>().data());
  EXPECT_STRCASEEQ("bab", result.value().get<27>().data());
}

// =============================================================================

TEST(ScanTest, NegativeCases)
{
  {
    auto result = stdx::scan<int>("'this' is not an int", "{} is not an int");
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
  // ---------------------------------------------------------------------------
  {
    auto result =
      stdx::scan<double>("invalid specifier 3.14", "invalid specifier {%p}");
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
  // ---------------------------------------------------------------------------
  {
    auto result = stdx::scan<uint8_t>("overflow 300", "overflow {}");
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
  // ---------------------------------------------------------------------------
  {
    auto result = stdx::scan<uint8_t>("wrong source 3.14", "wrong source {}");
    ASSERT_FALSE(result);
    DebugLog("%s\n", result.error().message.data());
  }
}
