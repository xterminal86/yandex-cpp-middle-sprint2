#pragma once

#include <tuple>

#ifdef DEBUG_LOGS
  #define DebugLog(format, ...)                   \
    do {                                          \
    std::print(format __VA_OPT__(,) __VA_ARGS__); \
    } while (false)
#else
  #define DebugLog(format, ...)
#endif

namespace stdx::details {

// Класс для хранения ошибки неуспешного сканирования
struct scan_error
{
  std::string message;
};

// =============================================================================

// Шаблонный класс для хранения результатов успешного сканирования
template <typename... Ts>
struct scan_result
{
  std::tuple<Ts...> holder;

  //
  // Restrict to be called only on lvalues (second & is a so-called 'ref
  // qualifier').
  // This will prevent, for example, the following from compiling:
  // auto& r = stdx::details::scan_result<int>{}.get<0>();
  //
  template <size_t Index>
  auto& get() & { return std::get<Index>(holder); }
};

} // namespace stdx::details
