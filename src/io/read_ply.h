#pragma once
#include "src/types/types.h"
#include <vector>

namespace zview::io {
std::vector<types::Shape> read_ply(const std::string &fn);
}
