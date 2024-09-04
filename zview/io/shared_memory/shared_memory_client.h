#pragma once

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#include "zview/types/external_types.h"

namespace zview {

class SharedMemoryClient {
  boost::interprocess::shared_memory_object m_data_shm;
  boost::interprocess::shared_memory_object m_cmd_shm;
  boost::interprocess::mapped_region m_data_region;
  boost::interprocess::mapped_region m_cmd_region;

  bool sendServerResizeRequest(std::size_t size);
  bool checkResponse();

 public:
  explicit SharedMemoryClient();

  /// @brief Remove a shape from the shared memory
  /// @param name Name of the shape to remove
  /// @return True if the shape was removed successfully, false otherwise
  bool removeShape(const std::string &name);

  /// @brief Plot a shape in the shared memory
  /// @param name Name of the shape
  /// @param xyz Pointer to the array of points
  /// @param n_points Number of points
  /// @param dim_points Dimension of the points (3 for xyz, 4 for xyzi, 6 for
  /// xyzrgb, 7 for xyzrgba)
  /// @param indices Pointer to the array of indices
  /// @param n_indices Number of indices
  /// @param dim_indices Dimension of the indices (2 for edges, 3 for faces)
  /// @return True if the shape was plotted successfully, false otherwise
  bool plot(const std::string &name, const float *xyz, size_t n_points,
            std::uint16_t dim_points, const std::uint32_t *indices = nullptr,
            size_t n_indices = 0, std::uint16_t dim_indices = 0);
};

}  // namespace zview