#include "zview/drawables/shape_update_visitor.h"

#include "zview/graphics_backend/vk_buffer_utils.h"
#include "zview/graphics_backend/vulkan_context.h"

namespace zview {

bool ShapeUpdateVisitor::operator()(types::Pcl &obj,
                                    types::Pcl &&newObj) const {
  obj.v() = std::move(newObj.v());
  const auto &verts = obj.v();
  if (verts.empty()) return true;
  return uploadBuffer(verts.data(), verts.size() * sizeof(types::VertData),
                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, obj.vertexBuffer,
                      obj.vertexAlloc);
}

bool ShapeUpdateVisitor::operator()(types::Edges &obj,
                                    types::Edges &&newObj) const {
  obj.v() = std::move(newObj.v());
  obj.e() = std::move(newObj.e());
  const auto &verts = obj.v();
  if (!verts.empty() &&
      !uploadBuffer(verts.data(), verts.size() * sizeof(types::VertData),
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, obj.vertexBuffer,
                    obj.vertexAlloc))
    return false;
  const auto &edges = obj.e();
  if (!edges.empty() &&
      !uploadBuffer(edges.data(), edges.size() * sizeof(types::EdgeIndx),
                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT, obj.indexBuffer,
                    obj.indexAlloc))
    return false;
  return true;
}

bool ShapeUpdateVisitor::operator()(types::Mesh &obj,
                                    types::Mesh &&newObj) const {
  obj.v() = std::move(newObj.v());
  obj.f() = std::move(newObj.f());
  const auto &verts = obj.v();
  if (!verts.empty() &&
      !uploadBuffer(verts.data(), verts.size() * sizeof(types::VertData),
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, obj.vertexBuffer,
                    obj.vertexAlloc))
    return false;
  const auto &faces = obj.f();
  if (!faces.empty() &&
      !uploadBuffer(faces.data(), faces.size() * sizeof(types::FaceIndx),
                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT, obj.indexBuffer,
                    obj.indexAlloc))
    return false;
  return true;
}

}  // namespace zview
