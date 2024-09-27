#include "zview/drawables/shape_buffer.h"

#include <iostream>
#include <random>
#include <regex>
#include <string>
#include <utility>
#include <variant>

#include "zview/drawables/shape_draw_visitor.h"
#include "zview/drawables/shape_init_visitor.h"
#include "zview/drawables/shape_update_visitor.h"
#include "zview/io/write_ply.h"
namespace zview {

ShapeBuffer::ShapeBuffer()
    : m_shape_init_visitor_p{std::make_unique<ShapeInitVisitor>()},
      m_shape_draw_visitor_p{std::make_unique<ShapeDrawVisitor>()},
      m_shape_update_visitor_p{std::make_unique<ShapeUpdateVisitor>()} {}

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

const std::string getName(const types::Shape &s) {
  return std::visit([](const auto &v) { return v.getName(); }, s);
}
std::uint32_t ShapeBuffer::emplace(types::Shape &&s) {
  const std::string s_name = getName(s);
  const auto key = getKey(s_name);

  if (key == 0) {
    // new shape - add it to the buffer, init it

    m_buffer.insert({m_next_key, std::move(s)});

    auto ret =
        std::visit(*m_shape_init_visitor_p.get(), m_buffer.at(m_next_key));
    if (ret) {
      // add it to string2key dict
      m_string2key.insert({s_name, m_next_key});
      return m_next_key++;
    } else {
      m_buffer.erase(m_next_key);
      std::cout << "could not init shape" << std::endl;
      return 0;
    }
  } else {
    // shape already there, update it
    auto &cur_shape = m_buffer.at(key);
    const bool ok =
        std::visit(*m_shape_update_visitor_p.get(), cur_shape, std::move(s));
    if (!ok) {
      std::cout << "could not update shape" << std::endl;
    }
  }
  return 0;
}

void ShapeBuffer::draw(
    const std::optional<MVPmat>& mvp,
    const std::function<void(const std::pair<std::uint32_t, types::Shape> &)>
        &preDrawFunction) const {
  const auto &draw_func = m_shape_draw_visitor_p.get();
  auto func = [&mvp, &draw_func](const auto &v) {
    draw_func->operator()(v, mvp);
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
  if (keys.empty()) {
    return types::Bbox3d{{0, 0, 0}, {0, 0, 0}};
  }
  types::Vector3 mmin(std::numeric_limits<float>::max(),
                      std::numeric_limits<float>::max(),
                      std::numeric_limits<float>::max());
  types::Vector3 mmax(std::numeric_limits<float>::lowest(),
                      std::numeric_limits<float>::lowest(),
                      std::numeric_limits<float>::lowest());

  for (const auto &s : keys) {
    bool enabled =
        std::visit([](const auto &v) { return v.enabled(); }, m_buffer.at(s));
    if (!enabled) {
      continue;
    }
    const auto this_bbox =
        std::visit([](const auto &v) { return v.getBbox(); }, m_buffer.at(s));
    mmin = mmin.cwiseMin(this_bbox.min());
    mmax = mmax.cwiseMax(this_bbox.max());
  }
  return types::Bbox3d{mmin, mmax};
}
std::size_t ShapeBuffer::size() const { return m_buffer.size(); }

std::optional<types::Vector3> ShapeBuffer::get3dLocation(
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
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
  std::vector<types::Shape> data{};
  for (const auto &s : m_buffer) {
    data.push_back(s.second);
  }
  io::writePly(f, data);
}

bool &ShapeBuffer::shapeVisibility(const std::uint32_t &object_key) {
  auto it = m_buffer.find(object_key);
  if (it == m_buffer.end()) {
    // object key not found

    return m_dummy_bool;
  }
  return std::visit([](auto &v) -> bool & { return v.enabled(); }, it->second);
}
std::uint32_t ShapeBuffer::getKey(const ::std::string &name) {
  auto it = m_string2key.find(name);
  if (it == m_string2key.end()) {
    return 0;
  }
  return it->second;
}
void ShapeBuffer::erase(const std::uint32_t &key) {
  const std::string s_name = getName(m_buffer.at(key));
  m_string2key.erase(s_name);
  m_buffer.erase(key);
}
std::vector<std::uint32_t> ShapeBuffer::getKeys(const ::std::string &name) {
  // iterate over all key, and add all the keys that contains the name
  std::vector<std::uint32_t> ret{};
  std::regex regex{name};
  for (const auto &s : m_string2key) {
    const auto &key_name = s.first;
    // use regular expression to find the name in inside the key name
    if (std::regex_search(key_name, regex)) {
      ret.push_back(s.second);
    }
  }
  return ret;
}
ShapeBuffer::~ShapeBuffer() = default;
}  // namespace zview
