#include "src/drawables/shape_init_visitor.h"

#include <GL/glew.h>  // Initialize with glewInit()
#include <GLFW/glfw3.h>
namespace zview {

bool ShapeInitVisitor::operator()(types::Pcl &obj) const {
  obj.initShader("point");
  const auto verts = obj.v();
  glGenVertexArrays(1, &obj.vao());
  glGenBuffers(1, &obj.vbo());

  glBindVertexArray(obj.vao());
  glBindBuffer(GL_ARRAY_BUFFER, obj.vbo());
  glBufferData(
      GL_ARRAY_BUFFER,
      static_cast<std::int64_t>(verts.size() * sizeof(types::VertData)),
      verts.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
      1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 4 * sizeof(float),
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<void *>(12));  // 3 * sizeof(float)
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  bool ok = glGetError() == GL_NO_ERROR;
  return ok;
}
bool ShapeInitVisitor::operator()(types::Edges &obj) const {
  obj.initShader("edges");

  const auto indices = obj.e();
  const auto verts = obj.v();
  glGenVertexArrays(1, &obj.vao());
  glGenBuffers(1, &obj.vbo());
  glGenBuffers(1, &obj.ebo());
  glBindVertexArray(obj.vao());
  glBindBuffer(GL_ARRAY_BUFFER, obj.vbo());
  glBufferData(
      GL_ARRAY_BUFFER,
      static_cast<std::int64_t>(verts.size() * sizeof(types::VertData)),
      verts.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.ebo());
  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      static_cast<std::int64_t>(indices.size() * sizeof(types::EdgeIndx)),
      indices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
      1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 4 * sizeof(float),
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<void *>(12));  // 3 * sizeof(float)
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  bool ok = glGetError() == GL_NO_ERROR;
  return ok;
}
bool ShapeInitVisitor::operator()(types::Mesh &obj) const {
  if (!obj.initShader("mesh")) {
    return false;
  }

  const auto indices = obj.f();
  const auto verts = obj.v();

  glGenVertexArrays(1, &obj.vao());

  glGenBuffers(1, &obj.vbo());
  glGenBuffers(1, &obj.ebo());
  glBindVertexArray(obj.vao());
  glBindBuffer(GL_ARRAY_BUFFER, obj.vbo());
  glBufferData(
      GL_ARRAY_BUFFER,
      static_cast<std::int64_t>(verts.size() * sizeof(types::VertData)),
      verts.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.ebo());

  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      static_cast<std::int64_t>(indices.size() * sizeof(types::FaceIndx)),
      indices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<void *>(0));

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
      1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 4 * sizeof(float),
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<void *>(12));  // 3 * sizeof(float)
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  const auto err_num = glGetError();

  return err_num == GL_NO_ERROR;
}
}  // namespace zview
