#pragma once
#include <vector>
#include "src/types/types.h"


namespace io
{
    std::vector <types::Shape> read_ply(const std::string& fn);
}
