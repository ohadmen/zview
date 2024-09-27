#include "zview/drawables/shape_draw_visitor.h"

#include <GL/glew.h>  // Initialize with glewInit()
#include <GLFW/glfw3.h>
#include <iostream>

#include "zview/graphics_backend/shader.h"
#include "zview/params/params.h"

namespace zview {
void ShapeDrawVisitor::operator()(const types::Pcl& obj,
                                  const std::optional<MVPmat>& mvp) const {
  if (mvp) {
    obj.shader().use();
    obj.shader().setUniform("u_transformation", mvp.value().getMVPmatrix().data());
    obj.shader().setUniform("u_nearPlaneDist", mvp.value().getHeightOfNearPlane());
    obj.shader().setUniform("u_viewDistance", mvp.value().getViewDistance());

    obj.shader().setUniform("u_color_factor_shift", zview::Params::i().color_factor_shift);
    obj.shader().setUniform("u_color_factor_scale",zview::Params::i().color_factor_scale);
    obj.shader().setUniform("u_ptsize", zview::Params::i().point_size);
    obj.shader().setUniform("u_lightDir", zview::Params::i().light_dir);
    obj.shader().setUniform("u_txt", zview::Params::i().texture_type);
  }
  glBindVertexArray(obj.vao());
  glDrawArrays(GL_POINTS, 0, static_cast<int>(obj.v().size()));
  glBindVertexArray(0);
}

void ShapeDrawVisitor::operator()(const types::Edges& obj,
                                  const std::optional<MVPmat>& mvp) const {
  if (mvp) {
    obj.shader().use();
    obj.shader().setUniform("u_transformation", mvp.value().getMVPmatrix().data());
  }
  glBindVertexArray(obj.vao());
  glDrawElements(GL_LINES, static_cast<int>(obj.e().size() * 2),
                 GL_UNSIGNED_INT, NULL);
  glBindVertexArray(0);
}

void ShapeDrawVisitor::operator()(const types::Mesh& obj,
                                  const std::optional<MVPmat>& mvp) const {
  // rendering our geometries
  if (mvp) {
    obj.shader().use();
    obj.shader().setUniform("u_transformation", mvp.value().getMVPmatrix().data());
    obj.shader().setUniform("u_lightDir", zview::Params::i().light_dir);
    obj.shader().setUniform("u_txt", zview::Params::i().texture_type);
  }
  glBindVertexArray(obj.vao());
  glDrawElements(GL_TRIANGLES, static_cast<int>(obj.f().size() * 3),
                 GL_UNSIGNED_INT, NULL);
  glBindVertexArray(0);
}
}  // namespace zview
