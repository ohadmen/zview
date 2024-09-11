#pragma once
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "zview/types/types.h"
namespace zview {
struct ShapeInitVisitor;    // fwddecl
struct ShapeDrawVisitor;    // fwddecl
struct ShapeUpdateVisitor;  // fwddecl

class ShapeBuffer {
  using BaseTypeVector = std::unordered_map<std::uint32_t, types::Shape>;

 public:
  ShapeBuffer();
  // copy constructor
  ShapeBuffer(const ShapeBuffer& other) = delete;
  // copy assignment
  ShapeBuffer& operator=(const ShapeBuffer& other) = delete;
  // move constructor
  ShapeBuffer(ShapeBuffer&& other) = delete;
  // move assignment
  ShapeBuffer& operator=(ShapeBuffer&& other) = delete;

  // destructor
  ~ShapeBuffer();

  /*
   * @brief push a shape to the buffer
   * @param s the shape to push
   * @return the key of the pushed shape
   */
  std::uint32_t emplace(types::Shape&& s);

  /*
   * @brief remove a shape from the buffer
   * @param key the key of the shape to remove
   */
  void erase(const std::uint32_t& key);

  //get the object key from the name if exists, otherwise return 0
  std::uint32_t getKey(const ::std::string& name);

  //get the object key from the name if exists, otherwise the children keys, otherwise return empty vector
  std::vector<std::uint32_t> getKeys(const ::std::string& name);
  /*
   * @brief draw all shapes in the shape buffer
   * @param tform the transformation matrix
   * @param preDrawFunction a function that is called before drawing each shape,
   * recieving the shape key and the shape object
   */
  void draw(
      const float* tform,
      const std::function<void(const std::pair<std::uint32_t, types::Shape>&)>&
          preDrawFunction = []([[maybe_unused]] const auto& s) {}) const;

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
  std::unordered_map<std::string, std::uint32_t> m_string2key;
  std::uint32_t m_next_key{1000};
  bool m_dummy_bool{false};//in case of trying to change visibility of a non existing object

  std::unique_ptr<ShapeInitVisitor> m_shape_init_visitor_p;
  std::unique_ptr<ShapeDrawVisitor> m_shape_draw_visitor_p;
  std::unique_ptr<ShapeUpdateVisitor> m_shape_update_visitor_p;
};
}  // namespace zview
