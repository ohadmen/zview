#include "zview/io/read_file.h"

#include <filesystem>
#include <memory>
#include <string>

#include "zview/io/reader_pcd.h"
#include "zview/io/reader_ply.h"

namespace zview::io {

std::vector<types::Shape> read_file(const std::string &fn) {
  std::unique_ptr<ReaderAbstract> reader;

  if (std::filesystem::path{fn}.extension() == ".ply") {
    reader = std::make_unique<ReaderPly>();

  } else if (std::filesystem::path{fn}.extension() == ".pcd") {
    reader = std::make_unique<ReaderPcd>();

  } else {
    throw std::runtime_error("file is not readable");
  }
  const auto shapes = reader->read(fn);

  return shapes;
}
}  // namespace zview::io
