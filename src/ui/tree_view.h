#pragma once
#include <functional>
#include <string>
namespace zview {

class TreeView {

    struct TreeNode
   {
   std::string name;
   std::uint32_t object_key;
   std::vector<TreeNode> children;
   };
   TreeNode m_root{"root", 0, {}};
   void drawTree(const  TreeNode& node)const ;

  std::function<bool&(const std::uint32_t&)> m_shape_visibility;
  std::function<void(const std::vector<std::uint32_t>&)> m_zoom_to_selection;
  void setChildrenVisibility(const TreeNode& node,bool v)const;
  std::int32_t getChildrenVisibility(const TreeNode& node)const;
  void getEnabledObjectsKeys(const TreeNode& node,std::vector<std::uint32_t>& selected_objects_keys)const;
public:
  TreeView(std::function<bool&(const std::uint32_t&)> shape_visibility,
  std::function<void(const std::vector<std::uint32_t>&) > zoom_to_selection);
  void push(std::string name,const std::uint32_t object_key);
  void draw();

};
} // namespace zview