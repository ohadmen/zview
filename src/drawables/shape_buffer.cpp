#include "shape_buffer.h"
#include  <variant>
#include "src/drawables/shape_init_visitor.h"
#include "src/drawables/shape_draw_visitor.h"


ShapeBuffer::ShapeBuffer():
m_shape_init_visitor_p{std::make_unique<ShapeInitVisitor>()},
m_shape_draw_visitor_p{std::make_unique<ShapeDrawVisitor>()}{}
ShapeBuffer::~ShapeBuffer()=default;

ShapeBuffer::BaseTypeVector::iterator ShapeBuffer::begin() { return m_buffer.begin(); }
ShapeBuffer::BaseTypeVector::const_iterator ShapeBuffer::begin() const { return m_buffer.begin(); }
ShapeBuffer::BaseTypeVector::const_iterator ShapeBuffer::cbegin() const { return begin(); }
ShapeBuffer::BaseTypeVector::iterator ShapeBuffer::end() { return m_buffer.end(); }
ShapeBuffer::BaseTypeVector::const_iterator ShapeBuffer::end() const { return m_buffer.end(); }
ShapeBuffer::BaseTypeVector::const_iterator ShapeBuffer::cend() const { return end(); }


    void ShapeBuffer::push(const types::Shape& s)
    {
        
        m_buffer.push_back(s);
        

        std::visit(*m_shape_init_visitor_p.get(),m_buffer.back());
    }
    void ShapeBuffer::draw()
    {
        for(const auto& s:m_buffer)
        {
            std::visit(*m_shape_draw_visitor_p.get(),s);
        }
    }
