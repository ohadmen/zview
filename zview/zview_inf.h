#pragma once

#include <memory>
#include <string>
#include <vector>

#include "zview/types/external_types.h"

/*
*** Interface class for ZviewInf application
*/
namespace zview {

class ZviewInf {
 public:
  /*
  *** Initialize the application
  *** @return true if the application was initialized successfully
  */
  virtual bool init() = 0;

  /*
  *** Load files into the application
  *** @param files the files to load
  */
  virtual void loadFiles(const std::vector<std::string> &files) = 0;
  /*
  *** Plot a mesh consisting of vertices and faces. If the name already exists,
  *the mesh will be updated
  *** @param name the name of the mesh
  *** @param vertices the vertices of the mesh
  *** @param faces the faces of the mesh
  */
  virtual void plot(const std::string &name, std::vector<Vertex> &&vertices,
                    std::vector<Face> &&faces) = 0;
  /*
  *** Plot a point cloud. If the name already exists, the point cloud will be
  *updated. To reduce memory copying, this is move only;
  *** @param name the name of the point cloud
  *** @param vertices the vertices of the point cloud
  */
  virtual void plot(const std::string &name,
                    std::vector<Vertex> &&vertices) = 0;
  /*
  *** Plot a graph consisting of vertices and edges. If the name already exists,
  *the graph will be updated
  *** @param name the name of the graph
  *** @param vertices the vertices of the graph
  *** @param edges the edges of the graph
  */
  virtual void plot(const std::string &name, std::vector<Vertex> &&vertices,
                    std::vector<Edge> &&edges) = 0;

  virtual void remove(const std::string &name) = 0;
  /*
  *** Run the application
  *** @param win_sz_wh the size of the window
  *** @return true if the application should continue running
  */
  virtual bool draw() = 0;

  /*
  *** Create an instance of the ZviewInf application
  *** @return the instance of the ZviewInf application
  */
  static std::shared_ptr<ZviewInf> create();

  virtual ~ZviewInf() = default;
  ZviewInf() = default;
  ZviewInf(const ZviewInf &) = delete;
  ZviewInf &operator=(const ZviewInf &) = delete;
  ZviewInf(ZviewInf &&) = delete;
  ZviewInf &operator=(ZviewInf &&) = delete;
};
}  // namespace zview