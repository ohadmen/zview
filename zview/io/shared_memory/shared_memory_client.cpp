#include "zview/io/shared_memory/shared_memory_client.h"

#include <cstring>
#include <string>
#include <vector>

#include "zview/io/shared_memory/shared_memory_types.h"
#include "zview/types/external_types.h"
#include "zview/utils/recast.h"

namespace zview {
SharedMemoryClient::SharedMemoryClient() {}
// generate random Vertex values

bool SharedMemoryClient::send() {
  // Populate req.
  const std::size_t n_points = 10000000U;
  std::string name = "point_cloud";
  name.resize(MAX_NAME_LENGTH, '\0');
  const auto req_size = sizeof(SharedMemMessageType) + MAX_NAME_LENGTH +
                        sizeof(std::size_t) + n_points * sizeof(Vertex);
  std::vector<std::uint8_t> req_data(req_size);

  std::uint8_t* ptr = req_data.data();

  *recast<SharedMemMessageType*>(ptr) = SharedMemMessageType::ADD_PCL;
  std::advance(ptr, sizeof(SharedMemMessageType));
  std::memcpy(ptr, name.data(), MAX_NAME_LENGTH);
  std::advance(ptr, MAX_NAME_LENGTH);
  *recast<std::size_t*>(ptr) = n_points;
  std::advance(ptr, sizeof(std::size_t));
  [[maybe_unused]] auto fp = recast<Vertex*>(ptr);
  return true;
}
}  // namespace zview