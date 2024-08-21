#pragma once
#include <string>
#include <vector>

#include "zview/types/types.h"

namespace zview::io {
std::vector<types::Shape> read_ply(const std::string &fn);
}
