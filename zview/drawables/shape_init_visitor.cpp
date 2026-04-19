#include "zview/drawables/shape_init_visitor.h"

#include "zview/graphics_backend/vk_buffer_utils.h"
#include "zview/graphics_backend/vulkan_context.h"

namespace zview {

bool ShapeInitVisitor::operator()(types::Pcl& obj) const {
  auto& ctx = VulkanContext::get();
  if (!obj.shader().init(Shader::ShaderType::PCL, ctx.offscreenRenderPass,
                         /*blend=*/true, /*depth=*/true,
                         VK_PRIMITIVE_TOPOLOGY_POINT_LIST))
    return false;
  const auto& verts = obj.v();
  if (verts.empty()) return true;
  return uploadBuffer(verts.data(), verts.size() * sizeof(types::VertData),
                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, obj.vertexBuffer,
                      obj.vertexAlloc);
}

bool ShapeInitVisitor::operator()(types::Edges& obj) const {
  auto& ctx = VulkanContext::get();
  if (!obj.shader().init(Shader::ShaderType::EDGES, ctx.offscreenRenderPass,
                         /*blend=*/true, /*depth=*/true,
                         VK_PRIMITIVE_TOPOLOGY_LINE_LIST))
    return false;
  const auto& verts = obj.v();
  if (!verts.empty() &&
      !uploadBuffer(verts.data(), verts.size() * sizeof(types::VertData),
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, obj.vertexBuffer,
                    obj.vertexAlloc))
    return false;
  const auto& edges = obj.e();
  if (!edges.empty() &&
      !uploadBuffer(edges.data(), edges.size() * sizeof(types::EdgeIndx),
                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT, obj.indexBuffer,
                    obj.indexAlloc))
    return false;
  return true;
}

bool ShapeInitVisitor::operator()(types::Mesh& obj) const {
  auto& ctx = VulkanContext::get();
  if (!obj.shader().init(Shader::ShaderType::MESH, ctx.offscreenRenderPass,
                         /*blend=*/true, /*depth=*/true,
                         VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST))
    return false;
  const auto& verts = obj.v();
  if (!verts.empty() &&
      !uploadBuffer(verts.data(), verts.size() * sizeof(types::VertData),
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, obj.vertexBuffer,
                    obj.vertexAlloc))
    return false;
  const auto& faces = obj.f();
  if (!faces.empty() &&
      !uploadBuffer(faces.data(), faces.size() * sizeof(types::FaceIndx),
                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT, obj.indexBuffer,
                    obj.indexAlloc))
    return false;
  return true;
}

}  // namespace zview
