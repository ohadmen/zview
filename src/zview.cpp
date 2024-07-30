#include "src/zview.h"

#include <memory>

#include "src/ui/zview_main_app.h"
#include "src/utils/recast.h"

namespace zview {

Zview::Zview() : m_app(std::make_unique<ZviewMainApp>()) {}
Zview::~Zview() = default;

bool Zview::init(const std::array<int, 2> &win_sz_wh) {
  return m_app->init(win_sz_wh);
}
void Zview::plot(const std::vector<std::string> &files) {
  m_app->loadFiles(files);
}
bool Zview::draw(const std::array<int, 2> &win_sz_wh) {
  return m_app->draw(win_sz_wh);
}

// template<typename T>
// std::variant<std::reference_wrapper<T>> unvisit(T &x) {
//     return std::ref(x);
// }
void Zview::plot(const std::string &name, std::vector<Vertex> &&vertices) {
  types::Pcl pcl{name};
  // Vertex and types::VertData are the same type and have the same memory
  // layout. To save copying, we can reinterpret_cast the vector of vertices to
  // a vector of types::VertData
  pcl.v() =
      std::move(*zview::recast<std::vector<types::VertData> *>(&vertices));

  m_app->plot(pcl);
}
void Zview::plot(const std::string &name, std::vector<Vertex> &&vertices,
                 std::vector<Face> &&faces) {
  types::Mesh mesh{name};
  // Vertex and types::VertData are the same type and have the same memory
  // layout. To save copying, we can reinterpret_cast the vector of vertices to
  // a vector of types::VertData
  mesh.v() = std::move(*recast<std::vector<types::VertData> *>(&vertices));
  // Face and types::FaceIndx are the same type and have the same memory layout.
  // To save copying, we can reinterpret_cast the vector of faces to a vector of
  // types::FaceIndx NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  mesh.f() = std::move(*recast<std::vector<types::FaceIndx> *>(&faces));
  m_app->plot(mesh);
}
void Zview::plot(const std::string &name, std::vector<Vertex> &&vertices,
                 std::vector<Edge> &&edges) {
  types::Edges graph{name};
  // Vertex and types::VertData are the same type and have the same memory
  // layout. To save copying, we can reinterpret_cast the vector of vertices to
  // a vector of types::VertData
  graph.v() = std::move(*recast<std::vector<types::VertData> *>(&vertices));
  // Edge and types::EdgeIndx are the same type and have the same memory layout.
  // To save copying, we can reinterpret_cast the vector of edges to a vector of
  // types::EdgeIndx NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  graph.e() = std::move(*recast<std::vector<types::EdgeIndx> *>(&edges));
  m_app->plot(graph);
}

}  // namespace zview
