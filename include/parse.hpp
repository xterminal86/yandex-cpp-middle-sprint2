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

#define CHECK_LIMITS(type)                                  \
  do {                                                      \
    if (typeid(T) == typeid(type))                          \
    {                                                       \
      if (result < std::numeric_limits<type>::min() or      \
          result > std::numeric_limits<type>::max())        \
      {                                                     \
        return std::unexpected(scan_error(kErrOutOfRange)); \
      }                                                     \
    }                                                       \
  } while (false)

template <IsInt T>
std::expected<T, scan_error> parse_value(std::string_view input)
{
  std::expected<T, scan_error> ret;

  errno = 0;
  char* end;
  long long result = std::strtoll(input.data(), &end, 0);
  if (end == input.data())
  {
    return std::unexpected(scan_error(kErrFailedToConvert));
  }

  if (errno == ERANGE)
  {
    return std::unexpected(scan_error(kErrOutOfRange));
  }

  CHECK_LIMITS(int8_t);
  CHECK_LIMITS(int16_t);
  CHECK_LIMITS(int32_t);

  ret = result;

  return ret;
}

// =============================================================================

template <IsUInt T>
std::expected<T, scan_error> parse_value(std::string_view input)
{
  std::expected<T, scan_error> ret;

  errno = 0;
  char* end;
  unsigned long long result = std::strtoull(input.data(), &end, 0);
  if (end == input.data())
  {
    return std::unexpected(scan_error(kErrFailedToConvert));
  }

  if (errno == ERANGE)
  {
    return std::unexpected(scan_error(kErrOutOfRange));
  }

  CHECK_LIMITS(uint8_t);
  CHECK_LIMITS(uint16_t);
  CHECK_LIMITS(uint32_t);

  ret = result;

  return ret;
}

// =============================================================================

template <IsFloat T>
std::expected<T, scan_error> parse_value(std::string_view input)
{
  errno = 0;
  char* end;
  T result = std::strtof(input.data(), &end);
  if (end == input.data())
  {
    return std::unexpected(scan_error(kErrFailedToConvert));
  }

  if (errno == ERANGE)
  {
    return std::unexpected(scan_error(kErrOutOfRange));
  }

  return result;
}

// =============================================================================

template <IsDouble T>
std::expected<T, scan_error> parse_value(std::string_view input)
{
  errno = 0;
  char* end;
  T result = std::strtod(input.data(), &end);
  if (end == input.data())
  {
    return std::unexpected(scan_error(kErrFailedToConvert));
  }

  if (errno == ERANGE)
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
  std::expected<T, scan_error> pr;

  if (fmt == "%d")
  {
    DebugLog("Parsing integer...\n");

    if      (typeid(T) == typeid(int))     pr = parse_value<int>(input);
    else if (typeid(T) == typeid(int8_t))  pr = parse_value<int8_t>(input);
    else if (typeid(T) == typeid(int16_t)) pr = parse_value<int16_t>(input);
    else if (typeid(T) == typeid(int32_t)) pr = parse_value<int32_t>(input);
    else if (typeid(T) == typeid(int64_t)) pr = parse_value<int64_t>(input);
    else return std::unexpected(scan_error(kErrInvalidType));
  }
  else if (fmt == "%u")
  {
    DebugLog("Parsing unsigned integer...\n");

    if      (typeid(T) == typeid(uint))     pr = parse_value<uint>(input);
    else if (typeid(T) == typeid(uint8_t))  pr = parse_value<uint8_t>(input);
    else if (typeid(T) == typeid(uint16_t)) pr = parse_value<uint16_t>(input);
    else if (typeid(T) == typeid(uint32_t)) pr = parse_value<uint32_t>(input);
    else if (typeid(T) == typeid(uint64_t)) pr = parse_value<uint64_t>(input);
    else return std::unexpected(scan_error(kErrInvalidType));
  }
  else if (fmt == "%f")
  {
    DebugLog("Parsing floating point value...\n");

    if      (typeid(T) == typeid(float))  pr = parse_value<float>(input);
    else if (typeid(T) == typeid(double)) pr = parse_value<double>(input);
    else return std::unexpected(scan_error(kErrInvalidType));
  }
  else if (fmt == "%s")
  {
    if (typeid(T) == typeid(std::string))
    {
      DebugLog("Parsing std::string...\n");
    }
    else if (typeid(T) == typeid(std::string_view))
    {
      DebugLog("Parsing std::string_view...\n");
    }

    // TODO:
    /*
    if (typeid(T) == typeid(std::string))
    {
      pr = parse_value<std::string>(input);
    }
    else if (typeid(T) == typeid(std::string_view))
    {
      pr = parse_value<std::string_view>(input);
    }
    else return std::unexpected(scan_error(kErrInvalidType));
    */
  }
  else
  {
    std::string message =
      std::vformat("Invalid format specifier '{}'",
                   std::make_format_args(fmt));

    return std::unexpected(scan_error(message));
  }

  /*
  if (fmt == "%d" or fmt == "%s" or fmt == "%u" or fmt == "%f" or fmt.empty())
  {
    auto pr = parse_value<T>(input);
    if (pr)
    {
      return pr.value();
    }
    else
    {
      return std::unexpected(pr.error());
    }
  }
  else
  {
    std::string message =
      std::vformat("Invalid format specifier '{}'",
                   std::make_format_args(fmt));

    return std::unexpected(scan_error(message));
  }
  */

  return pr;
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
