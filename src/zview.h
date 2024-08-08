#pragma once
#include <memory>
#include <string>
#include <vector>

#include "src/types/external_types.h"

/*
*** Interface class for Zview application
*/
namespace zview {
// forward declaration
class ZviewMainApp;

class Zview {
 public:
  Zview();
  // copy constructor
  Zview(const Zview &) = delete;
  // move constructor
  Zview(Zview &&) = delete;
  // copy assignment
  Zview &operator=(const Zview &) = delete;
  // move assignment
  Zview &operator=(Zview &&) = delete;
  ~Zview();
  /*
  *** Initialize the application
  *** @return true if the application was initialized successfully
  */
  bool init();

  /*
  *** Load files into the application
  *** @param files the files to load
  */
  void plot(const std::vector<std::string> &files);
  /*
  *** Plot a mesh consisting of vertices and faces. If the name already exists,
  *the mesh will be updated
  *** @param name the name of the mesh
  *** @param vertices the vertices of the mesh
  *** @param faces the faces of the mesh
  */
  void plot(const std::string &name, std::vector<Vertex> &&vertices,
            std::vector<Face> &&faces);
  /*
  *** Plot a point cloud. If the name already exists, the point cloud will be
  *updated. To reduce memory copying, this is move only;
  *** @param name the name of the point cloud
  *** @param vertices the vertices of the point cloud
  */
  void plot(const std::string &name, std::vector<Vertex> &&vertices);
  /*
  *** Plot a graph consisting of vertices and edges. If the name already exists,
  *the graph will be updated
  *** @param name the name of the graph
  *** @param vertices the vertices of the graph
  *** @param edges the edges of the graph
  */
  void plot(const std::string &name, std::vector<Vertex> &&vertices,
            std::vector<Edge> &&edges);
  /*+
  *** Run the application
  *** @param win_sz_wh the size of the window
  *** @return true if the application should continue running
  */
  bool draw();

 private:
  std::unique_ptr<ZviewMainApp> m_app;
};
}  // namespace zview