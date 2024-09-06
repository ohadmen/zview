#include "zview/ui/tree_view.h"

#include <imgui.h>  // for Imvec2

#include <deque>
#include <iostream>
// https://github.com/google/styleguide/issues/194
// NOLINTNEXTLINE[build/c++11]
#include <regex>
#include <string>
#include <vector>

namespace zview {
TreeView::TreeView(

    std::function<bool &(const std::uint32_t &)> set_shape_visibility,
    std::function<void(const std::vector<std::uint32_t> &)> zoom_to_selection,
    std::function<void(const std::uint32_t &)> delete_key)
    : m_shape_visibility{std::move(set_shape_visibility)},
      m_zoom_to_selection{std::move(zoom_to_selection)},
      m_delete_key(std::move(delete_key)) {}
void TreeView::push(std::string name, const std::uint32_t object_key) {
  if (object_key == 0) {
    // no need to do anything when key is only update
    return;
  }

  // sanizing the string: make sure doesn't end with "/", and if it does - omit
  // it
  if (name.back() == '/') {
    name.pop_back();
  }
  std::regex rgx("/");
  std::sregex_token_iterator iter(name.begin(), name.end(), rgx, -1);
  std::sregex_token_iterator end;
  std::deque<std::string> name_parts(iter, end);
  // if name is falt with no slashes
  TreeNode *current_node = &m_root;
  while (!name_parts.empty()) {
    auto it = std::find_if(current_node->children.begin(),
                           current_node->children.end(),
                           [&name_parts](const TreeNode &node) {
                             return node.name == name_parts.front();
                           });
    if (it != current_node->children.end()) {
      current_node = &(*it);
    } else {
      // coult not find parent, create the branch for it
      while (name_parts.size() != 1) {
        current_node->children.push_back({name_parts.front(), current_node, 0});
        current_node = &current_node->children.back();
        name_parts.pop_front();
      }
      current_node->children.push_back(
          {name_parts.front(), current_node, object_key});
      return;
    }

    name_parts.pop_front();
  }
  // last part of the name was not found --> add it as a child
  if (name_parts.size() == 1) {
    current_node->children.push_back({name_parts[0], current_node, object_key});
  } else if (name_parts.empty()) {
    // last part was found, thi means there is already an object with the same
    // name, but there should not be a linked object

    if (current_node->object_key != 0) {
      std::cerr << "Object with the same name already exists ("
                << current_node->name << ")" << std::endl;
      return;
    }
    current_node->object_key = object_key;
  }
}
void TreeView::setChildrenVisibility(const TreeNode &node, bool v) const {
  for (auto &child : node.children) {
    if (child.object_key != 0) {
      m_shape_visibility(child.object_key) = v;
    }
    setChildrenVisibility(child, v);
  }
}
std::int32_t TreeView::getChildrenVisibility(const TreeNode &node) const {
  std::int32_t v{0};
  for (const auto &child : node.children) {
    if (child.object_key != 0) {
      v += m_shape_visibility(child.object_key) ? 1 : -1;
    }
    v += getChildrenVisibility(child);
  }

  return v;
}

void TreeView::getChildObjectsKeys(
    const TreeNode &node, std::vector<std::uint32_t> *selected_objects_keys_p,
    bool enabled_only) const {
  /*
   * This function is used to get all the object keys that are enabled in the
   * tree view and are children of node
   */
  if (node.object_key != 0) {
    if (!enabled_only ||
        (enabled_only && m_shape_visibility(node.object_key))) {
      selected_objects_keys_p->push_back(node.object_key);
    }
  }
  for (const auto &child : node.children) {
    getChildObjectsKeys(child, selected_objects_keys_p, enabled_only);
  }
}

void TreeView::drawTree(TreeNode &node) const {
  static constexpr ImGuiTreeNodeFlags flag =
      ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf;
  static constexpr ImVec4 col_active{1, 1, 1, 1};
  static constexpr ImVec4 col_inactive{0.5, 0.5, 0.5, 1};
  static constexpr ImVec4 col_noobj{0.8, 0.9, 1.0, 1};

  if (node.object_key != 0) {
    bool vval = m_shape_visibility(node.object_key);
    ImGui::PushStyleColor(ImGuiCol_Text, vval ? col_active : col_inactive);
  } else {
    ImGui::PushStyleColor(ImGuiCol_Text, col_noobj);
  }

  if (!node.name.empty() && ImGui::TreeNodeEx(node.name.c_str(), flag)) {
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
      // has a linked item
      if (node.object_key != 0) {
        bool &v_ref = m_shape_visibility(node.object_key);
        v_ref = !v_ref;
        setChildrenVisibility(node, v_ref);
      } else {
        bool v = getChildrenVisibility(node) > 0;
        setChildrenVisibility(node, !v);
      }
    } else if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
      std::vector<std::uint32_t> selected_objects_keys;
      getChildObjectsKeys(node, &selected_objects_keys, true);
      m_zoom_to_selection(selected_objects_keys);
    } else if (ImGui::IsItemHovered() &&
               ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete))) {
      deleteNode(node);
    }

    for (auto &child : node.children) {
      drawTree(child);
    }
    ImGui::TreePop();
  }
  ImGui::PopStyleColor();
}
void TreeView::deleteNode(TreeNode &node) const {
  if (node.object_key != 0) {
    m_delete_key(node.object_key);
    node.object_key = 0;
  } else {
    for (auto &ch : node.children) {
      deleteNode(ch);
    }
    node.children.clear();
  }
  // search for this node in the parent and remove it
  if (node.parent != nullptr) {
    auto it =
        std::find_if(node.parent->children.begin(), node.parent->children.end(),
                     [&node](const TreeNode &n) { return &n == &node; });
    if (it != node.parent->children.end()) {
      node.parent->children.erase(it);
    }
  }
}

TreeView::TreeNode *TreeView::find(std::uint32_t key, TreeNode *root) {
  if (!root) {
    return nullptr;
  }
  if (root->object_key == key) {
    return root;
  }
  for (auto &ch : root->children) {
    auto it = find(key, &ch);
    if (it) {
      return it;
    }
  }
  return nullptr;
}
void TreeView::remove(std::uint32_t obj_key) {
  auto it = find(obj_key, &m_root);
  if (!it) {
    return;
  }
  deleteNode(*it);
}
void TreeView::removeAll() {
  for (auto &child : m_root.children) {
    deleteNode(child);
  }
}
void TreeView::draw(bool *showTreeP) {
  ImGui::Begin(
      "Layers", showTreeP,
      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing);

  if (m_root.children.empty()) {
    ImGui::Text("No objects to display");
  } else {
    for (auto &child : m_root.children) {
      drawTree(child);
    }
  }
  ImGui::End();
}

}  // namespace zview
