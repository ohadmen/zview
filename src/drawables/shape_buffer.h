#pragma once
#include "src/types/types.h"
#include <vector>
#include <memory>
class ShapeInitVisitor;//fwddecl
class ShapeDrawVisitor;//fwddecl
class ShapeBuffer
{
    using BaseTypeVector = std::vector<types::Shape>;
    
    public:
    ShapeBuffer();
    ~ShapeBuffer();

    void push(const types::Shape& s);
    void draw();
    BaseTypeVector::iterator begin();
	BaseTypeVector::const_iterator begin() const;
	BaseTypeVector::const_iterator cbegin() const;
	BaseTypeVector::iterator end();
	BaseTypeVector::const_iterator end() const;
	BaseTypeVector::const_iterator cend() const;

    private:
    BaseTypeVector m_buffer;
    std::unique_ptr<ShapeInitVisitor> m_shape_init_visitor_p;
    std::unique_ptr<ShapeDrawVisitor> m_shape_draw_visitor_p;
};