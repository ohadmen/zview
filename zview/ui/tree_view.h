#pragma once
#include <deque>
#include <functional>
#include <string>
#include <vector>
namespace zview {

class TreeView {
  struct TreeNode {
    std::string name;
    std::uint32_t object_key{0};
    std::deque<TreeNode> children{};
    TreeNode* parent;
    TreeNode(const std::string& name_, TreeNode* parent_,
             std::uint32_t object_key_ = 0U)
        : name(name_), object_key{object_key_}, parent{parent_} {}
  };

  TreeNode m_root{"root", nullptr};

  void drawTree(TreeNode& node) const;

  std::function<bool&(const std::uint32_t&)> m_shape_visibility;
  std::function<void(const std::vector<std::uint32_t>&)> m_zoom_to_selection;
  std::function<void(const std::uint32_t&)> m_delete_key;
  void setChildrenVisibility(const TreeNode& node, bool v) const;
  std::int32_t getChildrenVisibility(const TreeNode& node) const;
  void getChildObjectsKeys(const TreeNode& node,
                           std::vector<std::uint32_t>* selected_objects_keysP,
                           bool enabled_only) const;
  void deleteNode(TreeNode& node) const;
  static TreeNode* find(std::uint32_t key, TreeNode* root);
  static TreeNode* find(const std::string& obj_name, TreeNode* root, const std::string& parent_name);

 public:
  TreeView(
      std::function<bool&(const std::uint32_t&)> shape_visibility,
      std::function<void(const std::vector<std::uint32_t>&)> zoom_to_selection,
      std::function<void(const std::uint32_t&)> delete_key);
  void push(std::string name, const std::uint32_t object_key);
  void draw(bool* showTreeP);
  void remove(std::uint32_t obj_key);
  void remove(const std::string& name);
  void removeAll();
};
}  // namespace zview
