#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "zview/types/types.h"
namespace zview::io {

void writePly(std::string fn, const std::vector<types::Shape> &shapes);
void writePly(std::string fn,
              const std::unordered_map<std::uint32_t, types::Shape> &shapes);

}  // namespace zview::io
