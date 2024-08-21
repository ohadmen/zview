#include "zview/drawables/shape_draw_visitor.h"

#include <GL/glew.h>  // Initialize with glewInit()
#include <GLFW/glfw3.h>

#include "zview/graphics_backend/shader.h"
#include "zview/params/params.h"
namespace zview {
void ShapeDrawVisitor::operator()(const types::Pcl& obj,
                                  const float* tform) const {
  if (tform) {
    std::array<int, 4> viewport{};
    glGetIntegerv(GL_VIEWPORT, viewport.data());

    float hfovx = Params::i().camera_fov_rad * 0.5f;
    float heightOfNearPlane =
        (float)abs(viewport[2] - viewport[0]) * 0.5f / std::tan(hfovx);
    obj.shader().use();
    obj.shader().setUniform("u_transformation", tform);
    obj.shader().setUniform("u_nearPlaneDist", heightOfNearPlane);
    obj.shader().setUniform("u_ptsize", zview::Params::i().point_size);
    obj.shader().setUniform("u_lightDir", zview::Params::i().light_dir);
    obj.shader().setUniform("u_txt", zview::Params::i().texture_type);
  }
  glBindVertexArray(obj.vao());
  glDrawArrays(GL_POINTS, 0, static_cast<int>(obj.v().size()));
  glBindVertexArray(0);
}

void ShapeDrawVisitor::operator()(const types::Edges& obj,
                                  const float* tform) const {
  if (tform) {
    obj.shader().use();
    obj.shader().setUniform("u_transformation", tform);
  }
  glBindVertexArray(obj.vao());
  glDrawElements(GL_LINES, static_cast<int>(obj.e().size() * 2),
                 GL_UNSIGNED_INT, NULL);
  glBindVertexArray(0);
}

void ShapeDrawVisitor::operator()(const types::Mesh& obj,
                                  const float* tform) const {
  // rendering our geometries
  if (tform) {
    obj.shader().use();
    obj.shader().setUniform("u_transformation", tform);
    obj.shader().setUniform("u_lightDir", zview::Params::i().light_dir);
    obj.shader().setUniform("u_txt", zview::Params::i().texture_type);
  }
  glBindVertexArray(obj.vao());
  glDrawElements(GL_TRIANGLES, static_cast<int>(obj.f().size() * 3),
                 GL_UNSIGNED_INT, NULL);
  glBindVertexArray(0);
}
}  // namespace zview
