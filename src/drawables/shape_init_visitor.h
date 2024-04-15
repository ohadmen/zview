#pragma once
#include "src/types/types.h"
struct ShapeInitVisitor
{
    void operator()(types::Pcl &obj);
    void operator()(types::Edges &obj);
    void operator()(types::Mesh &obj);
};
