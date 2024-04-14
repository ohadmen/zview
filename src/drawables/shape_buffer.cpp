#include "shape_buffer.h"
#include  <variant>


ShapeBuffer::BaseTypeVector::iterator ShapeBuffer::begin() { return buffer_.begin(); }
ShapeBuffer::BaseTypeVector::const_iterator ShapeBuffer::begin() const { return buffer_.begin(); }
ShapeBuffer::BaseTypeVector::const_iterator ShapeBuffer::cbegin() const { return begin(); }
ShapeBuffer::BaseTypeVector::iterator ShapeBuffer::end() { return buffer_.end(); }
ShapeBuffer::BaseTypeVector::const_iterator ShapeBuffer::end() const { return buffer_.end(); }
ShapeBuffer::BaseTypeVector::const_iterator ShapeBuffer::cend() const { return end(); }


    void ShapeBuffer::push(const types::Shape& s)
    {
        buffer_.push_back(s);
        std::visit(*shape_visitor_p_,s);
    }
