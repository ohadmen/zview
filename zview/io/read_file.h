#pragma once
#include <string>
#include <vector>

#include "zview/types/types.h"

namespace zview::io {
std::vector<types::Shape> read_file(const std::string &fn);
}
