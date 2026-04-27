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
      std::println("{}", pr.error().message);
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
      std::println("{}", pr.error().message);
    }
  }
}

// Won't compile because of our defined concept.
//stdx::details::parse_value_with_format<int*>("123", "%d");
//stdx::details::parse_value_with_format<int&>("123", "%d");

// =============================================================================

template <typename T>
bool TestParseValueWithFormat(std::string_view test_name,
                              std::string_view value,
                              std::string_view fmt,
                              const T expected = T{})
{
  DebugLog("{}: ", test_name);
  auto result = stdx::details::parse_value_with_format<T>(value, fmt);
  if (!result)
  {
    DebugLog("{}\n", result.error().message);
    return false;
  }
  else
  {
    if constexpr (std::is_same_v<std::remove_cv_t<T>, float>)
    {
      EXPECT_FLOAT_EQ(expected, result.value());
    }
    else if constexpr (std::is_same_v<std::remove_cv_t<T>, double>)
    {
      EXPECT_DOUBLE_EQ(expected, result.value());
    }
    else if constexpr (std::is_same_v<std::remove_cv_t<T>, std::string>)
    {
      EXPECT_STRCASEEQ(expected.data(), result.value().data());
    }
    else
    {
      EXPECT_EQ(expected, result.value());
    }

    DebugLog("\n");
    return true;
  }
}

// =============================================================================

TEST(Debugging, ParseSourcesInt)
{
  ASSERT_TRUE(
    TestParseValueWithFormat<int32_t>("<int32_t>(2147483647)",
                                      "2147483647",
                                      "%d",
                                      2147483647)
  );
  ASSERT_TRUE(
    TestParseValueWithFormat<int32_t>("<int32_t>(-2147483648)",
                                      "-2147483648",
                                      "%d",
                                      -2147483648)
  );
  ASSERT_TRUE(
    TestParseValueWithFormat<int64_t>("<int64_t>(100500)",
                                      "100500",
                                      "%d",
                                      100500)
  );
  ASSERT_TRUE(
    TestParseValueWithFormat<int64_t>("<int64_t>(100500)",
                                      "100500",
                                      "",
                                      100500)
  );
  // ---------------------------------------------------------------------------
  ASSERT_FALSE( TestParseValueWithFormat<int>("<int>(abc)", "abc", "%d") );
  ASSERT_FALSE(
    TestParseValueWithFormat<int>("<int>(invalid specifier)", "abc", "%f")
  );
  ASSERT_FALSE(
    TestParseValueWithFormat<int8_t>("<int8_t>(128)", "128", "%d")
  );
  ASSERT_FALSE(
    TestParseValueWithFormat<int8_t>("<int8_t>(-129)", "-129", "%d")
  );
  ASSERT_FALSE(
    TestParseValueWithFormat<int16_t>("<int16_t>(65536)", "65536", "%d")
  );
  ASSERT_FALSE(
    TestParseValueWithFormat<int32_t>("<int32_t>(2147483648)",
                                      "2147483648",
                                      "%d")
  );
  ASSERT_FALSE(
    TestParseValueWithFormat<int32_t>("<int32_t>(-2147483649)",
                                      "-2147483649",
                                      "%d")
  );
  ASSERT_FALSE(
    TestParseValueWithFormat<int64_t>("<int64_t>(out of range)",
                                      "999999999999999999999999999",
                                      "%d")
  );
}

// =============================================================================

TEST(Debugging, ParseSourcesUInt)
{
  ASSERT_TRUE(
    TestParseValueWithFormat<uint16_t>("<uint16_t>({{}})", "65535", "", 65535)
  );
  ASSERT_TRUE(
    TestParseValueWithFormat<uint8_t>("<uint8_t>(128)", "128", "%u", 128)
  );
  // ---------------------------------------------------------------------------
  ASSERT_FALSE(
    TestParseValueWithFormat<unsigned int>("<uint>(abc)", "abc", "%u")
  );
  ASSERT_FALSE(
    TestParseValueWithFormat<unsigned int>("<uint>(%%f)", "abc", "%f")
  );
  ASSERT_FALSE(
    TestParseValueWithFormat<uint8_t>("<uint8_t>(-129)", "-129", "%u")
  );
  ASSERT_FALSE(
    TestParseValueWithFormat<uint16_t>("<uint16_t>(65536)", "65536", "%u")
  );
}

// =============================================================================

TEST(Debugging, ParseSourcesFloat)
{
  ASSERT_TRUE(
    TestParseValueWithFormat<float>("<float>(123)", "123", "%f", 123)
  );
  ASSERT_TRUE(
    TestParseValueWithFormat<float>("<float>(123.456)",
                                    "123.456",
                                    "%f",
                                    123.456)
  );
  ASSERT_TRUE(
    TestParseValueWithFormat<double>("<double>(123.456)",
                                     "123.456",
                                     "%f",
                                     123.456)
  );
  ASSERT_TRUE(
    TestParseValueWithFormat<double>("<double>({{}})",
                                     "123.456",
                                     "",
                                     123.456)
  );
  ASSERT_FALSE(
    TestParseValueWithFormat<float>("<float>(abc)", "abc", "%f")
  );
}

// =============================================================================

TEST(Debugging, ParseSourcesString)
{
  ASSERT_TRUE(
    TestParseValueWithFormat<std::string>("<std::string>(lolsus)",
                                          "lolsus",
                                          "%s",
                                          "lolsus")
  );
  ASSERT_TRUE(
    TestParseValueWithFormat<std::string_view>("<std::string_view>(lolsus)",
                                               "Fuck Bees",
                                               "%s",
                                               "Fuck Bees")
  );
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
    DebugLog("{}\n", result.error().message);
  }
  // ---------------------------------------------------------------------------
  {
    auto result = stdx::scan<int>(
      "I want to sum 42 and 3.14 numbers.",
      "I want to sum {} and {%f} numbers."
    );
    ASSERT_FALSE(result);
    DebugLog("{}\n", result.error().message);
  }
  // ---------------------------------------------------------------------------
  {
    auto result = stdx::scan<int, double>(
      "I want to sum 42 and 3.14 numbers.",
      "I want to sum {} and {%f} {} numbers."
    );
    ASSERT_FALSE(result);
    DebugLog("{}\n", result.error().message);
  }
}

// =============================================================================

TEST(ScanTest, SimpleTest)
{
  {
    auto result = stdx::scan("Nothing", "Nothing");
    ASSERT_FALSE(result);
    DebugLog("{}\n", result.error().message);
  }
  // ---------------------------------------------------------------------------
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
    EXPECT_EQ("lol", result.value().get<3>());
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
  EXPECT_EQ("bib", result.value().get<26>());
  EXPECT_EQ("bab", result.value().get<27>());
}

// =============================================================================

TEST(ScanTest, NegativeCases)
{
  {
    auto result = stdx::scan<int>("'this' is not an int", "{} is not an int");
    ASSERT_FALSE(result);
    DebugLog("{}\n", result.error().message);
  }
  // ---------------------------------------------------------------------------
  {
    auto result =
      stdx::scan<double>("invalid specifier 3.14", "invalid specifier {%p}");
    ASSERT_FALSE(result);
    DebugLog("{}\n", result.error().message);
  }
  // ---------------------------------------------------------------------------
  {
    auto result = stdx::scan<uint8_t>("overflow 300", "overflow {}");
    ASSERT_FALSE(result);
    DebugLog("{}\n", result.error().message);
  }
  // ---------------------------------------------------------------------------
  {
    auto result = stdx::scan<uint8_t>("wrong source 3.14", "wrong source {}");
    ASSERT_FALSE(result);
    DebugLog("{}\n", result.error().message);
  }
  // ---------------------------------------------------------------------------
  {
    //
    // Won't compile because of SupportedType concept.
    //

    //auto result = stdx::scan<bool>("unsupported type", "unsupported type");
    //auto result = stdx::scan<char>("unsupported type", "unsupported type");
    //
    // 'unsigned char' works (probably because it's the same as uint8_t).
    // 'char' doesn't though.
    //
    //auto result = stdx::scan<char*>("unsupported type", "unsupported type");
    //auto result = stdx::scan<void>("unsupported type", "unsupported type");
    //auto result = stdx::scan<void*>("unsupported type", "unsupported type");
    //auto result = stdx::scan<int&>("unsupported type", "unsupported type");
    //auto result = stdx::scan<int&&>("unsupported type", "unsupported type");
  }
}
