#pragma once

namespace zview {
class SharedMemoryClient {
 public:
  explicit SharedMemoryClient();
  bool send();
};

}  // namespace zview