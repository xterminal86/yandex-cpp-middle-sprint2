#pragma once

#include "parse.hpp"
#include "types.hpp"

namespace stdx {

template <typename... Ts>
std::expected<
  details::scan_result<Ts...>, details::scan_error
>
scan(std::string_view input, std::string_view format)
{
  // parse_sources
  details::ParseResult pr = details::parse_sources(input, format);
  if (!pr)
  {
    return std::unexpected(pr.error());
  }

  details::PairSS res = pr.value();

  size_t pack_size = sizeof...(Ts);
  size_t fmt_size  = res.first.size();

  if (pack_size != fmt_size)
  {
    std::string message =
      std::vformat("Source ({}) and destination ({}) count mismatch!",
                   std::make_format_args(pack_size, fmt_size));

    return std::unexpected(details::scan_error(message));
  }

  details::scan_result<Ts...> result;

  // aggregate parse_value_with_format

  // TODO:

  // return scan_result object

  return result;
}

} // namespace stdx
