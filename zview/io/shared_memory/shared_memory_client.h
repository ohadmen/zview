#pragma once

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#include "zview/types/external_types.h"

namespace zview {

class SharedMemoryClient {
  boost::interprocess::shared_memory_object m_shm;
  boost::interprocess::mapped_region m_region;

  /// @brief Send a request to the shared memory
  /// @param req_data Pointer to the request data
  /// @param req_size Size of the request data
  /// @return True if the request was sent successfully, false otherwise
  bool addRequest(const std::uint8_t* const req_data,
                  const std::size_t req_size);

 public:
  explicit SharedMemoryClient();

  /// @brief Add a point cloud to the shared memory
  /// @param name Name of the point cloud
  /// @param n_points Number of points in the point cloud
  /// @param vertices Pointer to the array of vertices
  /// @return True if the point cloud was added successfully, false otherwise
  bool addPcl(std::string& name, const std::size_t n_points,
              const zview::Vertex* const vertices);

  /// @brief Add a mesh to the shared memory
  /// @param name Name of the mesh
  /// @param n_points Number of points in the mesh
  /// @param vertices Pointer to the array of vertices
  /// @param n_faces Number of faces in the mesh
  /// @param faces Pointer to the array of faces
  /// @return True if the mesh was added successfully, false otherwise
  bool addMesh(std::string& name, const std::size_t n_points,
               const zview::Vertex* const vertices, const std::size_t n_faces,
               const zview::Face* const faces);

  /// @brief Add an edge to the shared memory
  /// @param name Name of the edge
  /// @param n_points Number of points in the edge
  /// @param vertices Pointer to the array of vertices
  /// @param n_edges Number of edges in the edge
  /// @param edges Pointer to the array of edges
  /// @return True if the edge was added successfully, false otherwise
  bool addEdge(std::string& name, const std::size_t n_points,
               const zview::Vertex* const vertices, const std::size_t n_edges,
               const zview::Edge* const edges);

  /// @brief Remove a shape from the shared memory
  /// @param name Name of the shape to remove
  /// @return True if the shape was removed successfully, false otherwise
  bool removeShape(const std::string& name);
};

}  // namespace zview