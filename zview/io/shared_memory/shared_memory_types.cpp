#include "zview/io/shared_memory/shared_memory_types.h"

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>

namespace zview {

void removeOldSharedMemory() {
  if (boost::interprocess::shared_memory_object::remove(
          SHARED_MEMORY_CMD_NAME)) {
    std::cout << "Removing data command memory" << std::endl;
  }
  if (boost::interprocess::shared_memory_object::remove(
          SHARED_MEMORY_DATA_NAME)) {
    std::cout << "Removing data shared memory" << std::endl;
  }
}

SharedMemoryHandler::SharedMemoryHandler(bool is_server)
    : m_is_server{is_server} {
  if (is_server) {
    removeOldSharedMemory();
  }

  if (is_server) {
    m_cmd_shm = std::make_unique<boost::interprocess::shared_memory_object>(
        boost::interprocess::create_only, SHARED_MEMORY_CMD_NAME,
        boost::interprocess::read_write);

    m_data_shm = std::make_unique<boost::interprocess::shared_memory_object>(
        boost::interprocess::create_only, SHARED_MEMORY_DATA_NAME,
        boost::interprocess::read_write);
    m_cmd_shm->truncate(sizeof(SharedMemoryInfo));
    m_data_shm->truncate(SHARED_MEMORY_INIT_SIZE);
  } else {
    m_cmd_shm = std::make_unique<boost::interprocess::shared_memory_object>(
        boost::interprocess::open_only, SHARED_MEMORY_CMD_NAME,
        boost::interprocess::read_write);

    m_data_shm = std::make_unique<boost::interprocess::shared_memory_object>(
        boost::interprocess::open_only, SHARED_MEMORY_DATA_NAME,
        boost::interprocess::read_write);
  }
  m_cmd_region = boost::interprocess::mapped_region(
      *m_cmd_shm, boost::interprocess::read_write);

  m_data_region = boost::interprocess::mapped_region(
      *m_data_shm, boost::interprocess::read_write);

  if (is_server) {
    SharedMemoryInfo &info =
        *static_cast<SharedMemoryInfo *>(m_cmd_region.get_address());
    info.type = SharedMemMessageType::UNKNOWN;
  }
}
SharedMemoryHandler::~SharedMemoryHandler() {
  if (m_is_server) {
    removeOldSharedMemory();
  }
}

SharedMemoryInfo &SharedMemoryHandler::getCmdInfo() {
  return *static_cast<SharedMemoryInfo *>(m_cmd_region.get_address());
}

void SharedMemoryHandler::resizeData(size_t size) {
  m_data_shm->truncate(ssize_t(size));
  m_data_region = boost::interprocess::mapped_region(
      *m_data_shm, boost::interprocess::read_write);
}
std::size_t SharedMemoryHandler::getAvailableSize() const {
  return m_data_region.get_size();
}

}  // namespace zview