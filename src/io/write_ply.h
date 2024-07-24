#pragma once
#include <string>
#include <vector>

#include "src/types/types.h"
namespace zview::io {

void writePly(std::string fn, const std::vector<types::Shape> &shapes);

}
