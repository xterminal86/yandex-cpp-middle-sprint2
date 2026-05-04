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
  //DebugLog("{}\n", __PRETTY_FUNCTION__);

  // parse_sources
  details::ParseResult pr = details::parse_sources(input, format);
  if (!pr)
  {
    return std::unexpected(pr.error());
  }

  const details::PairSS& res = pr.value();

  size_t pack_size = sizeof...(Ts);
  size_t fmt_size  = res.first.size();

  if (pack_size != fmt_size)
  {
    std::string message =
      std::vformat("Source ({}) and destination ({}) count mismatch!",
                   std::make_format_args(pack_size, fmt_size));

    return std::unexpected(details::scan_error(message));
  }

  if (fmt_size == 0)
  {
    return std::unexpected(
      details::scan_error("No conversion specifiers found")
    );
  }

  details::scan_result<std::remove_cv_t<Ts>...> result;

  bool success = true;
  details::scan_error error;

  const details::StringV& format_parts = res.first;
  const details::StringV& input_parts  = res.second;

  // satanic black magic
  [&]<size_t... Is>(std::index_sequence<Is...>)
  {
    auto parse_one = [&]<size_t I>()
    {
      if (!success)
      {
        return;
      }

      auto parse_result = details::parse_value_with_format<
        std::tuple_element_t<I, std::tuple<std::remove_cv_t<Ts>...>>
      >(input_parts[I], format_parts[I]);

      if (parse_result)
      {
        std::get<I>(result.holder) = *parse_result;
      }
      else
      {
        success = false;
        error = parse_result.error();
      }
    };

    (parse_one.template operator()<Is>(), ...);
  }(std::index_sequence_for<Ts...>{});

  // return scan_result object
  if (success)
  {
    return details::scan_result<Ts...>{result.holder};
  }
  else
  {
    return std::unexpected(error);
  }
}

} // namespace stdx
