#pragma once
#include "src/types/types.h"
#include <vector>
#include <memory>
class ShapeInitVisitor;//fwddecl
class ShapeBuffer
{
    using BaseTypeVector = std::vector<types::Shape>;
    
    public:
    ShapeBuffer();

    void push(const types::Shape& s);
    BaseTypeVector::iterator begin();
	BaseTypeVector::const_iterator begin() const;
	BaseTypeVector::const_iterator cbegin() const;
	BaseTypeVector::iterator end();
	BaseTypeVector::const_iterator end() const;
	BaseTypeVector::const_iterator cend() const;
    private:
    BaseTypeVector buffer_;
    std::unique_ptr<ShapeInitVisitor> shape_visitor_p_;
};