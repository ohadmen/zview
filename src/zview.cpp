#include "src/zview.h"

#include <memory>

#include "src/ui/zview_main_app.h"

namespace zview {
Zview::Zview() : m_app(std::make_unique<ZviewMainApp>()) {}
Zview::~Zview() = default;
bool Zview::init(bool init_gl_backend) { return m_app->init(init_gl_backend); }
void Zview::plot(const std::vector<std::string> &files) {
  m_app->loadFiles(files);
}
bool Zview::loop() { return m_app->loop(); }

void Zview::plot(const std::string &name, std::vector<Vertex> &&vertices) {
  types::Pcl pcl{name};
  pcl.v().reserve(vertices.size());
  std::move(vertices.begin(), vertices.end(), pcl.v().begin());
  m_app->plot(pcl);
}

}  // namespace zview
