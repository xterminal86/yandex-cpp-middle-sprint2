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
concept IsInt = std::is_same_v<std::remove_cv_t<T>, int>
             or std::is_same_v<std::remove_cv_t<T>, int8_t>
             or std::is_same_v<std::remove_cv_t<T>, int16_t>
             or std::is_same_v<std::remove_cv_t<T>, int32_t>
             or std::is_same_v<std::remove_cv_t<T>, int64_t>;

template <typename T>
concept IsUInt = std::is_same_v<std::remove_cv_t<T>, unsigned int>
              or std::is_same_v<std::remove_cv_t<T>, uint8_t>
              or std::is_same_v<std::remove_cv_t<T>, uint16_t>
              or std::is_same_v<std::remove_cv_t<T>, uint32_t>
              or std::is_same_v<std::remove_cv_t<T>, uint64_t>;

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

template <SupportedType T>
requires std::is_integral_v<T> and (std::is_signed_v<T> or std::is_unsigned_v<T>)
std::expected<T, scan_error> parse_int(std::string_view in)
{
  T result;

  const char* first = in.data();
  const char* last  = in.data() + in.size();

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

template <SupportedType T>
requires std::is_floating_point_v<T>
std::expected<T, scan_error> parse_float(std::string_view in)
{
  T result;

  const char* first = in.data();
  const char* last  = in.data() + in.size();

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

// Функция для парсинга значения с учетом спецификатора формата
template <SupportedType T>
std::expected<T, scan_error>
parse_value_with_format(std::string_view input, std::string_view fmt)
{
  if constexpr (std::is_integral_v<T> and std::is_signed_v<T>)
  {
    if (fmt == "%d" or fmt.empty())
    {
      auto r = parse_int<T>(input);
      if (not r)
      {
        return std::unexpected(r.error());
      }

      return r.value();
    }
    else
    {
      return std::unexpected(scan_error(kErrInvalidFormat));
    }
  }
  else if constexpr (std::is_integral_v<T> and std::is_unsigned_v<T>)
  {
    if (fmt == "%u" or fmt.empty())
    {
      auto r = parse_int<T>(input);
      if (not r)
      {
        return std::unexpected(r.error());
      }

      return r.value();
    }
    else
    {
      return std::unexpected(scan_error(kErrInvalidFormat));
    }
  }
  else if constexpr (std::is_floating_point_v<T>)
  {
    if (fmt == "%f" or fmt.empty())
    {
      auto r = parse_float<T>(input);
      if (not r)
      {
        return std::unexpected(r.error());
      }

      return r.value();
    }
    else
    {
      return std::unexpected(scan_error(kErrInvalidFormat));
    }
  }
  else if constexpr (std::is_same_v<T, std::string>)
  {
    if (fmt == "%s" or fmt.empty())
    {
      return std::string(input);
    }
    else
    {
      return std::unexpected(scan_error(kErrInvalidFormat));
    }
  }
  else if constexpr (std::is_same_v<T, std::string_view>)
  {
    if (fmt == "%s" or fmt.empty())
    {
      return input;
    }
    else
    {
      return std::unexpected(scan_error(kErrInvalidFormat));
    }
  }
  else
  {
    static_assert("Unsupported type");
  }

  return std::unexpected(scan_error("Reached the end without returning anything"));
}

// =============================================================================

using StringV = std::vector<std::string>;
using PairSS  = std::pair<StringV, StringV>; // ( format_parts, input_parts )

using ParseResult = std::expected<PairSS, scan_error>;

// Функция для проверки корректности входных данных и выделения из обеих строк
// интересующих данных для парсинга.
ParseResult parse_sources(std::string_view input, std::string_view format)
{
#ifdef DEBUG_LOGS
  static const std::string ruler(80, '=');

  DebugLog("%s\n", ruler.data());
  DebugLog("input  = '%s'\n", input.data());
  DebugLog("format = '%s'\n", format.data());
  DebugLog("%s\n", ruler.data());
#endif

  constexpr std::string_view err_format_mismatch =
    "Unformatted text in input and format string are different";

  //
  // NOTE: заменил всё на std::string, т.к. мне не нравится, что format_parts и
  // input_parts здесь локальные переменные, которые наполняются временными
  // объектами. Получится, что к моменту выхода из данной функции они будут уже
  // невалидны как и их содержимое. К тому же происхождение данных внутри этих
  // контейнеров будет неочевидным (т.е. что это, условно, ссылки на временные
  // объекты, созданные где-то в глубине какой-то функции) и придётся лезть в
  // эту функцию, чтобы это увидеть. Возможно тут сработает copy elision, но я
  // не считаю это аргументом в пользу подобного говнокода.
  // Плюс, мне не нравится сам подход, когда в параметры нужно передавать
  // литералы, т.к. это снижает защиту "от дурака", т.к. string_view это,
  // как говорится, по сути не более чем glorified pointer.
  //
  std::vector<std::string> format_parts;  // Части формата между {}
  std::vector<std::string> input_parts;
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
      std::string between = std::string(format.substr(start, open - start));
      auto pos = input.find(between);
      if (input.size() < between.size() || pos == std::string_view::npos)
      {
        return std::unexpected(scan_error{std::string(err_format_mismatch)});
      }

      if (start != 0)
      {
        input_parts.emplace_back(std::string(input.substr(0, pos)));
      }

      input = input.substr(pos + between.size());
    }

    // Сохраняем спецификатор формата (то, что между {})
    std::string contents = std::string(
      format.substr(open + 1, close - open - 1)
    );

    format_parts.push_back(contents);

    start = close + 1;
  }

  // Проверяем оставшийся текст после последней }
  if (start < format.size())
  {
    std::string remaining_format = std::string(format.substr(start));
    auto pos = input.find(remaining_format);
    if (input.size() < remaining_format.size() || pos == std::string_view::npos)
    {
      return std::unexpected(scan_error{std::string(err_format_mismatch)});
    }

    input_parts.emplace_back(
      std::string(input.substr(0, pos))
    );

    input = input.substr(pos + remaining_format.size());
  }
  else
  {
    input_parts.emplace_back(input);
  }

#ifdef DEBUG_LOGS
  DebugLog("input parts = %lu, format parts = %lu\n",
           input_parts.size(), format_parts.size());

  for (size_t i = 0; i < format_parts.size(); i++)
  {
    DebugLog("ip[%lu] = '%s', fp[%lu] = '%s'\n",
             i, input_parts[i].data(), i, format_parts[i].data());
  }

  DebugLog("\n");
#endif

  return std::pair{format_parts, input_parts};
}

} // namespace stdx::details
