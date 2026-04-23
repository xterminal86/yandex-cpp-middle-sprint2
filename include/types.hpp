#pragma once

#include <tuple>

#ifdef DEBUG_LOGS
  #define DebugLog(format, ...)    \
    do {                           \
    printf(format, ##__VA_ARGS__); \
    fflush(stdout);                \
    } while (false)
#else
  #define DebugLog(format, ...)
#endif

namespace stdx::details {

// Класс для хранения ошибки неуспешного сканирования
struct scan_error
{
  // TODO: add "error code" enum class?
  std::string message;
};

// =============================================================================

// Шаблонный класс для хранения результатов успешного сканирования
template <typename... Ts>
struct scan_result
{
  std::tuple<Ts...> holder;
};

} // namespace stdx::details
