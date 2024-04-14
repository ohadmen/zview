#pragma once
#include "src/types/types.h"
class ShapeInitVisitor
{
    

public:
    ShapeInitVisitor()=default;
    int operator()(const types::Pcl &obj);
    int operator()(const types::Edges &obj);
    int operator()(const types::Mesh &obj);
};
