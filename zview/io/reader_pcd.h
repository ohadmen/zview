#pragma once
#include "zview/io/reader_abs.h"

namespace zview::io {
class ReaderPcd : public ReaderAbstract {
 public:
  std::vector<types::Shape> read(const std::string &fn) override;
};
}  // namespace zview::io