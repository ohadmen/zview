#pragma once

namespace zview {
template <typename To, typename From>
To recast(From x) {
  // NOLINTNEXTLINE
  static_assert(sizeof(To) == sizeof(From));
  // NOLINTNEXTLINE
  return reinterpret_cast<To>(x);
}
}  // namespace zview
