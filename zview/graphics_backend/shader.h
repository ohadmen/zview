#pragma once

#include <vulkan/vulkan.h>

#include <array>
#include <cstdint>

namespace zview {

// Push-constant layouts — each must be ≤128 bytes (Vulkan minimum guarantee).
// All structs share the same slot (offset 0) so only one can be active at once.

struct PCLPushConstants {
  float mvp[16];        // 64
  float ptsize;         //  4
  float nearPlaneDist;  //  4
  int32_t txt;          //  4
  float pad;            //  4
  float lightDir[4];    // 16 (vec4 for alignment)
                        // total: 96
};

struct MeshPushConstants {
  float mvp[16];      // 64
  int32_t txt;        //  4
  float pad0;         //  4
  float pad1;         //  4
  float pad2;         //  4
  float lightDir[4];  // 16
                      // total: 96
};

struct EdgesPushConstants {
  float mvp[16];  // 64
};

struct PickingPushConstants {
  float mvp[16];         // 64
  uint32_t objectIndex;  //  4
  float pad[3];          // 12
                         // total: 80
};

struct GridPushConstants {
  float mvp[16];   // 64
  float shift[2];  //  8
  float scale;     //  4
  float pad;       //  4
                   // total: 80
};

class Shader {
 public:
  enum class ShaderType : uint8_t { PCL, EDGES, MESH, PICKING, GRID };

  Shader() = default;
  ~Shader();
  Shader(const Shader&) = delete;
  Shader& operator=(const Shader&) = delete;
  Shader(Shader&&) noexcept;
  Shader& operator=(Shader&&) noexcept;

  bool init(ShaderType type, VkRenderPass renderPass, bool enableBlend = false,
            bool enableDepth = true,
            VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

  VkPipeline pipeline() const { return m_pipeline; }
  VkPipelineLayout pipelineLayout() const { return m_layout; }
  ShaderType type() const { return m_type; }

 private:
  VkShaderModule createModule(const uint32_t* code, uint32_t len) const;

  VkPipeline m_pipeline{VK_NULL_HANDLE};
  VkPipelineLayout m_layout{VK_NULL_HANDLE};
  ShaderType m_type{ShaderType::PCL};
};

}  // namespace zview
