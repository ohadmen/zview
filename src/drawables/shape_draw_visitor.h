#pragma once
#include "src/types/types.h"
struct ShapeDrawVisitor
{
    void operator()(const types::Pcl &obj);
    void operator()(const types::Edges &obj);
    void operator()(const types::Mesh &obj);
};
