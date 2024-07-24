#pragma once
#include <memory>
#include <optional>
#include <vector>

#include "src/drawables/picking_texture.h"
#include "src/types/types.h"
namespace zview {
class ShapeInitVisitor;  // fwddecl
class ShapeDrawVisitor;  // fwddecl

class ShapeBuffer {
  using BaseTypeVector = std::unordered_map<std::uint32_t, types::Shape>;

 public:
  ShapeBuffer();
  ~ShapeBuffer();

  /*
   * @brief push a shape to the buffer
   * @param s the shape to push
   * @return the key of the pushed shape
   */
  std::uint32_t push(const types::Shape& s);

  /*
   * @brief draw all shapes in the shape buffer
   * @param tform the transformation matrix
   * @param preDrawFunction a function that is called before drawing each shape,
   * recieving the shape key and the shape object
   */
  void draw(
      const float* tform,
      const std::function<void(const std::pair<std::uint32_t, types::Shape>&)>&
          preDrawFunction = [](const auto& s) {}) const;

  std::optional<types::Vector3> get3dLocation(
      const std::uint32_t& object_key, const std::uint32_t& prim_index,
      const std::array<types::Vector3, 2>& ray) const;
  types::Bbox3d getBbox(std::vector<std::uint32_t> keys) const;
  BaseTypeVector::iterator begin();
  BaseTypeVector::const_iterator begin() const;
  BaseTypeVector::const_iterator cbegin() const;
  BaseTypeVector::iterator end();
  BaseTypeVector::const_iterator end() const;
  BaseTypeVector::const_iterator cend() const;
  std::size_t size() const;
  bool& shapeVisibility(const std::uint32_t& object_key);

  void writeBufferToFile(const std::string& f) const;

 private:
  BaseTypeVector m_buffer;
  std::uint32_t m_next_key{1000};

  std::unique_ptr<ShapeInitVisitor> m_shape_init_visitor_p;
  std::unique_ptr<ShapeDrawVisitor> m_shape_draw_visitor_p;
};
}  // namespace zview