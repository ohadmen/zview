#pragma once
#include <functional>
#include <string>
#include <vector>
namespace zview {

class TreeView {
  struct TreeNode {
    std::string name;
    std::uint32_t object_key{0};
    std::vector<TreeNode> children{};
    TreeNode(const std::string& name_, std::uint32_t object_key_ = 0U)
        : name(name_), object_key{object_key_} {}
  };

  TreeNode m_root{"root"};
  void drawTree(const TreeNode& node) const;

  std::function<bool&(const std::uint32_t&)> m_shape_visibility;
  std::function<void(const std::vector<std::uint32_t>&)> m_zoom_to_selection;
  void setChildrenVisibility(const TreeNode& node, bool v) const;
  std::int32_t getChildrenVisibility(const TreeNode& node) const;
  void getEnabledObjectsKeys(
      const TreeNode& node,
      std::vector<std::uint32_t>* selected_objects_keysP) const;

 public:
  TreeView(
      std::function<bool&(const std::uint32_t&)> shape_visibility,
      std::function<void(const std::vector<std::uint32_t>&)> zoom_to_selection);
  void push(std::string name, const std::uint32_t object_key);
  void draw();
};
}  // namespace zview
