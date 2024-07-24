#include "src/drawables/shape_buffer.h"

#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <variant>

#include "src/drawables/shape_draw_visitor.h"
#include "src/drawables/shape_init_visitor.h"
#include "src/io/write_ply.h"
namespace zview {

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

std::uint32_t randomInt() {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist6(1000, 9999);

  return std::uint32_t(dist6(rng));
}
const std::string getName(const types::Shape &s) {
  return std::visit([](const auto &v) { return v.getName(); }, s);
}
std::uint32_t ShapeBuffer::push(const types::Shape &s) {
  // check that there is no other shape with the same name
  static_assert(std::is_copy_constructible_v<types::Shape>);

  const std::string s_name = getName(s);
  for (const auto &shape : m_buffer) {
    if (getName(shape.second) == s_name) {
      std::cout << "shape with name " << s_name << " already exists"
                << std::endl;
      types::Shape s_renamed{s};
      std::visit(
          [](auto &v) {
            v.setName(v.getName() + "_" + std::to_string(randomInt()));
          },
          s_renamed);
      return push(s_renamed);
    }
  }
  m_buffer.insert({m_next_key, s});

  auto ret = std::visit(*m_shape_init_visitor_p.get(), m_buffer.at(m_next_key));
  if (!ret) {
    m_buffer.erase(m_next_key);
    std::cout << "could not init shape" << std::endl;
    return 0;
  }
  return m_next_key++;
}

void ShapeBuffer::draw(
    const float *tform,
    const std::function<void(const std::pair<std::uint32_t, types::Shape> &)>
        &preDrawFunction) const {
  const auto &draw_func = m_shape_draw_visitor_p.get();
  auto func = [&tform, &draw_func](const auto &v) {
    draw_func->operator()(v, tform);
  };
  for (const auto &s : m_buffer) {
    bool enabled =
        std::visit([](const auto &v) { return v.enabled(); }, s.second);
    if (!enabled) {
      continue;
    }
    preDrawFunction(s);
    std::visit(func, s.second);
  }
}

types::Bbox3d ShapeBuffer::getBbox(std::vector<std::uint32_t> keys) const {
  if (keys.empty()) {
    keys.reserve(m_buffer.size());
    for (const auto &s : m_buffer) {
      keys.push_back(s.first);
    }
  }
  types::Bbox3d bbox;
  for (const auto &s : keys) {
    bool enabled =
        std::visit([](const auto &v) { return v.enabled(); }, m_buffer.at(s));
    if (!enabled) {
      continue;
    }
    const auto this_bbox =
        std::visit([](const auto &v) { return v.getBbox(); }, m_buffer.at(s));
    bbox |= this_bbox;
  }
  return bbox;
}
std::size_t ShapeBuffer::size() const { return m_buffer.size(); }

std::optional<types::Vector3> ShapeBuffer::get3dLocation(
    const std::uint32_t &object_key, const std::uint32_t &prim_index,
    const std::array<types::Vector3, 2> &ray) const {
  auto it = m_buffer.find(object_key);
  if (it == m_buffer.end()) {
    return std::nullopt;
  }
  return std::visit(
      [prim_index, ray](const auto &v) {
        return v.get3dLocation(prim_index, ray);
      },
      it->second);
}
void ShapeBuffer::writeBufferToFile(const std::string &f) const {
  std::vector<types::Shape> data;
  for (const auto &s : m_buffer) {
    data.push_back(s.second);
  }
  io::writePly(f, data);
}

bool &ShapeBuffer::shapeVisibility(const std::uint32_t &object_key) {
  auto it = m_buffer.find(object_key);
  if (it == m_buffer.end()) {
    throw std::runtime_error("object key not found");
  }
  return std::visit([](auto &v) -> bool & { return v.enabled(); }, it->second);
}
}  // namespace zview
