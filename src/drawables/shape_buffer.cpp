#include "shape_buffer.h"
#include "src/drawables/shape_draw_visitor.h"
#include "src/drawables/shape_init_visitor.h"
#include "src/io/write_ply.h"
#include <iostream>
#include <variant>
namespace zview{

ShapeBuffer::ShapeBuffer()
    : m_shape_init_visitor_p{std::make_unique<ShapeInitVisitor>()},
      m_shape_draw_visitor_p{std::make_unique<ShapeDrawVisitor>()} {}
ShapeBuffer::~ShapeBuffer() = default;

ShapeBuffer::BaseTypeVector::iterator ShapeBuffer::begin() {
  return m_buffer.begin();
}
ShapeBuffer::BaseTypeVector::const_iterator ShapeBuffer::begin() const {
  return m_buffer.begin();
}
ShapeBuffer::BaseTypeVector::const_iterator ShapeBuffer::cbegin() const {
  return begin();
}
ShapeBuffer::BaseTypeVector::iterator ShapeBuffer::end() {
  return m_buffer.end();
}
ShapeBuffer::BaseTypeVector::const_iterator ShapeBuffer::end() const {
  return m_buffer.end();
}
ShapeBuffer::BaseTypeVector::const_iterator ShapeBuffer::cend() const {
  return end();
}

void ShapeBuffer::push(const types::Shape &s) {

  m_buffer.push_back(s);

  auto ret = std::visit(*m_shape_init_visitor_p.get(), m_buffer.back());
  if (!ret) {
    m_buffer.pop_back();
    std::cout << "could not init shape" << std::endl;
  }
}
void ShapeBuffer::draw(const float* tform,
                       std::uint32_t object_index) const {
  const auto &draw_func = m_shape_draw_visitor_p.get();
  auto func = [&tform, &draw_func](const auto &v) {
    draw_func->operator()(v, tform);
  };
  std::visit(func, m_buffer[object_index]);
}

types::Bbox3d ShapeBuffer::getBbox() const {
  types::Bbox3d bbox;
  for (const auto &s : m_buffer) {

    const auto this_bbox =
        std::visit([](const auto &v) { return v.getBbox(); }, s);
    bbox |= this_bbox;
  }
  return bbox;
}
std::size_t ShapeBuffer::size() const { return m_buffer.size(); }

std::optional<types::Vector3>
ShapeBuffer::get3dLocation(const std::uint32_t &object_index,
                           const std::uint32_t &prim_index,
                           const std::array<types::Vector3, 2> &ray) const {
  if (object_index >= m_buffer.size()) {
    return std::nullopt;
  }
  return std::visit(
      [prim_index, ray](const auto &v) {
        return v.get3dLocation(prim_index, ray);
      },
      m_buffer[object_index]);
}
void ShapeBuffer::writeBufferToFile(const std::string &f) const {
  io::writePly(f, m_buffer);
}
} // namespace zview