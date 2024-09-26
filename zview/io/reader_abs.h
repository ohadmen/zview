#pragma once
#include <string>
#include <variant>
#include <vector>

#include "zview/types/types.h"
namespace zview::io {

class ReaderAbstract {
 public:
  ReaderAbstract() = default;
  virtual ~ReaderAbstract() = default;
  // copy constructor
  ReaderAbstract(const ReaderAbstract &other) = default;
  // move constructor
  ReaderAbstract(ReaderAbstract &&other) = default;
  // copy assignment
  ReaderAbstract &operator=(const ReaderAbstract &other) = default;
  // move assignment
  ReaderAbstract &operator=(ReaderAbstract &&other) = default;

  virtual std::vector<types::Shape> read(const std::string &fn) = 0;
};
}  // namespace zview::io