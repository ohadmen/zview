#pragma once
#include "src/types/types.h"
#include <algorithm>
#include <fstream>
namespace io
{
        void writePly(std::string fn, const std::vector<types::Shape>& shapes);

}