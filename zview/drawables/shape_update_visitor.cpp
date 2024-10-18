#include "zview/drawables/shape_update_visitor.h"

#include <GL/glew.h>  // Initialize with glewInit()
#include <GLFW/glfw3.h>
namespace zview {
bool pclSwitch(std::uint32_t vao, std::uint32_t vbo,
               std::vector<types::VertData> &dst,
               std::vector<types::VertData> &&src) {
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // get pointer
  const auto old_size = dst.size();
  dst = std::move(src);
  if (old_size < dst.size()) {
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<std::int64_t>(dst.size() * sizeof(types::VertData)),
        dst.data(), GL_STATIC_DRAW);
  } else {
    auto ptr = static_cast<types::VertData *>(
        glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
    // now copy data into memory
    std::copy(dst.begin(), dst.end(), ptr);

    glUnmapBuffer(GL_ARRAY_BUFFER);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  bool ok = glGetError() == GL_NO_ERROR;
  return ok;
}
bool ShapeUpdateVisitor::operator()(types::Pcl &obj,
                                    types::Pcl &&newObj) const {
  return pclSwitch(obj.vao(), obj.vbo(), obj.v(), std::move(newObj.v()));
}
bool ShapeUpdateVisitor::operator()(types::Edges &obj,
                                    types::Edges &&newObj) const {
  bool ok = pclSwitch(obj.vao(), obj.vbo(), obj.v(), std::move(newObj.v()));
  if (!ok) {
    return false;
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.ebo());
  const auto old_size = obj.e().size();
  obj.e() = newObj.e();
  if (old_size < obj.e().size()) {
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<std::int64_t>(obj.e().size() * sizeof(types::EdgeIndx)),
        obj.e().data(), GL_STATIC_DRAW);
  } else {
    auto ptr = static_cast<types::EdgeIndx *>(
        glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
    // now copy data into memory
    std::copy(obj.e().begin(), obj.e().end(), ptr);

    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  ok = glGetError() == GL_NO_ERROR;
  return ok;
}
bool ShapeUpdateVisitor::operator()(types::Mesh &obj,
                                    types::Mesh &&newObj) const {
  bool ok = pclSwitch(obj.vao(), obj.vbo(), obj.v(), std::move(newObj.v()));

  if (!ok) {
    return false;
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.ebo());
  const auto old_size = obj.f().size();
  obj.f() = newObj.f();
  if (old_size < obj.f().size()) {
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<std::int64_t>(obj.f().size() * sizeof(types::FaceIndx)),
        obj.f().data(), GL_STATIC_DRAW);
  } else {
    auto ptr = static_cast<types::FaceIndx *>(
        glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
    // now copy data into memory
    std::copy(obj.f().begin(), obj.f().end(), ptr);

    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  ok = glGetError() == GL_NO_ERROR;
  return ok;
}

}  // namespace zview
