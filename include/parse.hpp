#pragma once

#include <expected>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <cstdlib>
#include <cerrno>

#include "types.hpp"

namespace stdx::details {

const std::string kErrOutOfRange      = "Result is out of range";
const std::string kErrFailedToConvert = "Failed to convert";
const std::string kErrInvalidType     = "Invalid type";
const std::string kErrInvalidFormat   = "Invalid format specifier";

template <typename T>
concept IsInt = std::signed_integral<T>
        and not std::is_same_v<std::remove_cv_t<T>, char>;

template <typename T>
concept IsUInt = std::unsigned_integral<T>;

template <typename T>
concept IsFloat = std::is_same_v<std::remove_cv_t<T>, float>;

template <typename T>
concept IsDouble = std::is_same_v<std::remove_cv_t<T>, double>;

template <typename T>
concept IsString = std::is_same_v<std::remove_cv_t<T>, std::string>
                or std::is_same_v<std::remove_cv_t<T>, std::string_view>;

template <typename T>
concept SupportedType = IsInt<T>
                     or IsUInt<T>
                     or IsFloat<T>
                     or IsDouble<T>
                     or IsString<T>;

// =============================================================================

template <typename T>
requires IsInt<T> or IsUInt<T> or IsFloat<T> or IsDouble<T>
[[nodiscard]]
std::expected<T, scan_error> parse_value(std::string_view input)
{
  //DebugLog("{}\n", __PRETTY_FUNCTION__);

  T result;

  const char* first = input.data();
  const char* last  = input.data() + input.size();

  auto [ptr, ec] = std::from_chars(first, last, result);

  if (ec == std::errc() and ptr != last)
  {
    return std::unexpected(scan_error(kErrFailedToConvert));
  }
  else if (ec == std::errc::invalid_argument)
  {
    return std::unexpected(scan_error(kErrInvalidType));
  }
  else if (ec == std::errc::result_out_of_range)
  {
    return std::unexpected(scan_error(kErrOutOfRange));
  }

  return result;
}

// =============================================================================

template <typename T>
requires IsString<T>
[[nodiscard]]
std::expected<T, scan_error> parse_value(std::string_view input)
{
  //DebugLog("{}\n", __PRETTY_FUNCTION__);

  if constexpr (std::is_same_v<std::remove_cv_t<T>, std::string>)
  {
    static std::string tmp;
    tmp = input;
    return tmp;
  }
  else if constexpr (std::is_same_v<std::remove_cv_t<T>, std::string_view>)
  {
    return input;
  }

  return std::unexpected(scan_error("Failed to parse value as string"));
}

// =============================================================================

// Функция для парсинга значения с учетом спецификатора формата
template <SupportedType T>
std::expected<T, scan_error>
parse_value_with_format(std::string_view input, std::string_view fmt)
{
  //DebugLog("{}\n", __PRETTY_FUNCTION__);

  if (fmt == "%s" or fmt == "%d" or fmt == "%u" or fmt == "%f" or fmt.empty())
  {
    return parse_value<T>(input);
  }
  else
  {
    return std::unexpected(scan_error(kErrInvalidFormat));
  }

  return std::unexpected(scan_error("Failed to parse value with format"));
}

// =============================================================================

using StringV = std::vector<std::string_view>;
using PairSS  = std::pair<StringV, StringV>; // ( format_parts, input_parts )

using ParseResult = std::expected<PairSS, scan_error>;

// Функция для проверки корректности входных данных и выделения из обеих строк
// интересующих данных для парсинга.
ParseResult parse_sources(std::string_view input, std::string_view format)
{
#ifdef DEBUG_LOGS
  static const std::string ruler(80, '=');

  DebugLog("{}\n", ruler);
  DebugLog("input  = '{}'\n", input);
  DebugLog("format = '{}'\n", format);
  DebugLog("{}\n", ruler);
#endif

  constexpr std::string_view err_format_mismatch =
    "Unformatted text in input and format string are different";

  std::vector<std::string_view> format_parts;  // Части формата между {}
  std::vector<std::string_view> input_parts;
  size_t start = 0;
  while (true)
  {
    size_t open = format.find('{', start);
    if (open == std::string_view::npos)
    {
      break;
    }

    size_t close = format.find('}', open);
    if (close == std::string_view::npos)
    {
      break;
    }

    // Если между предыдущей } и текущей { есть текст,
    // проверяем его наличие во входной строке
    if (open > start)
    {
      std::string_view between = format.substr(start, open - start);
      auto pos = input.find(between);
      if (input.size() < between.size() || pos == std::string_view::npos)
      {
        return std::unexpected(scan_error{std::string(err_format_mismatch)});
      }

      if (start != 0)
      {
        input_parts.emplace_back(input.substr(0, pos));
      }

      input = input.substr(pos + between.size());
    }

    // Сохраняем спецификатор формата (то, что между {})
    format_parts.push_back(format.substr(open + 1, close - open - 1));

    start = close + 1;
  }

  // Проверяем оставшийся текст после последней }
  if (start < format.size())
  {
    std::string_view remaining_format = format.substr(start);
    auto pos = input.find(remaining_format);
    if (input.size() < remaining_format.size() || pos == std::string_view::npos)
    {
      return std::unexpected(scan_error{std::string(err_format_mismatch)});
    }

    input_parts.emplace_back(input.substr(0, pos));

    input = input.substr(pos + remaining_format.size());
  }
  else
  {
    input_parts.emplace_back(input);
  }

#ifdef DEBUG_LOGS
  DebugLog("input parts = {}, format parts = {}\n",
           input_parts.size(), format_parts.size());

  for (size_t i = 0; i < format_parts.size(); i++)
  {
    DebugLog("ip[{}] = '{}', fp[{}] = '{}'\n",
             i, input_parts[i], i, format_parts[i]);
  }

  DebugLog("\n");
#endif

  return std::pair{format_parts, input_parts};
}

} // namespace stdx::details
