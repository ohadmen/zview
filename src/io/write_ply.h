#pragma once
#include "src/types/types.h"
#include <string>
#include <vector>
namespace zview::io {

void writePly(std::string fn, const std::vector<types::Shape> &shapes);

}
