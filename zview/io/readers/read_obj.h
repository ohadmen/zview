#pragma once
#include <vector>
#include "zview/common/common_types.h"


namespace io
{
    std::vector<types::Shape> readObj(const std::string& fn);
}
