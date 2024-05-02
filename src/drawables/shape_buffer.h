#pragma once
#include "src/drawables/picking_texture.h"
#include "src/types/types.h"
#include <memory>
#include <vector>
#include <optional>
namespace zview{
class ShapeInitVisitor; // fwddecl
class ShapeDrawVisitor; // fwddecl


class ShapeBuffer {
  using BaseTypeVector = std::vector<types::Shape>;

public:
  ShapeBuffer();
  ~ShapeBuffer();

  void push(const types::Shape &s);

  void draw(const float* tform, std::uint32_t object_index)const;
  
  std::optional<types::Vector3> get3dLocation(const std::uint32_t& object_index,const std::uint32_t& prim_index, const std::array<types::Vector3,2>& ray)const;
  types::Bbox3d getBbox() const;
  BaseTypeVector::iterator begin();
  BaseTypeVector::const_iterator begin() const;
  BaseTypeVector::const_iterator cbegin() const;
  BaseTypeVector::iterator end();
  BaseTypeVector::const_iterator end() const;
  BaseTypeVector::const_iterator cend() const;
  std::size_t size() const;
  void writeBufferToFile(const std::string &f) const;

private:
  BaseTypeVector m_buffer;
  
  

  std::unique_ptr<ShapeInitVisitor> m_shape_init_visitor_p;
  std::unique_ptr<ShapeDrawVisitor> m_shape_draw_visitor_p;
  
};
} // namespace zview