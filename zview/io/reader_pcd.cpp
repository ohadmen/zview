#include "zview/io/reader_pcd.h"

#include <fstream>
#include <functional>
#include <regex>
#include <string>
#include <unordered_map>

#include "zview/utils/recast.h"

namespace zview::io {
struct ElementHeader {
  std::string type;
  size_t count{0};
  std::string signature;
  ElementHeader(const std::string &type_, const std::string &count_,
                const std::string &propList_)
      : type(type_), count(std::atoi(count_.c_str())), signature{type_} {
    for (size_t i = 0; i != propList_.size(); ++i) {
      if (!isspace(propList_[i])) {
        signature.push_back(propList_[i]);
      }
    }
  }
};

static std::string getName(const std::string &header) {
  static const std::regex rx("#\\s+([^\\s]+)");

  auto itbeg = std::sregex_iterator(header.begin(), header.end(), rx);
  auto itend = std::sregex_iterator();
  std::string name;
  for (std::sregex_iterator i = itbeg; i != itend; ++i) {
    const std::smatch &m = *i;
    name = m[1];
    break;
  }

  return name;
}

static std::string readHeader(std::ifstream &ss) {
  std::string header;
  for (std::string line; std::getline(ss, line);) {
    line.erase(std::find_if(line.rbegin(), line.rend(),
                            [](int ch) { return !std::isspace(ch); })
                   .base(),
               line.end());
    if (isdigit(line[0]) || line[0] == '-' || line[0] == '+') {
      ss.seekg(-static_cast<int64_t>(line.size()), std::ios_base::cur);
      break;
    }

    header += line + "\n";
    if (line.empty()) {
      continue;
    }
    // if first charecter is a number or -/+ then it is a data line
  }

  return header;
}

static std::vector<types::VertData> readVerts(std::ifstream &ss) {
  std::vector<types::VertData> verts;
  std::string line;
  // regex to get all numbers from a line
  std::regex rx("[-+]?[0-9]*\\.?[0-9]+");

  while (true) {
    std::getline(ss, line);
    // get all numbers from a line
    auto begin = std::sregex_iterator(line.begin(), line.end(), rx);
    auto end = std::sregex_iterator();
    const auto n = std::distance(begin, end);
    if (n < 3) {
      // rollback the stream
      ss.seekg(-static_cast<int64_t>(line.size()), std::ios_base::cur);
      break;
    }

    std::vector<float> numbers(n);
    std::transform(begin, end, numbers.begin(),
                   [](const std::smatch &m) { return std::stof(m.str()); });
    switch (n) {
      case 3:
        verts.emplace_back(numbers[0], numbers[1], numbers[2]);
        break;
      case 4:
        verts.emplace_back(numbers[0], numbers[1], numbers[2], numbers[3]);
        break;
      case 6:
        verts.emplace_back(numbers[0], numbers[1], numbers[2], numbers[3],
                           numbers[4], numbers[5]);
        break;
      case 7:
        verts.emplace_back(numbers[0], numbers[1], numbers[2], numbers[3],
                           numbers[4], numbers[5], numbers[6]);
        break;
      default:
        throw std::runtime_error("unsupported number of elements in a line");
    }
    if (ss.eof()) {
      break;
    }
  }
  return verts;
}

std::vector<types::Shape> ReaderPcd::read(const std::string &fn) {
  std::vector<types::Shape> container;
  std::ifstream ss(fn, std::ios::in | std::ios::binary);
  if (ss.fail()) throw std::runtime_error("failed to open " + std::string(fn));

  while (!ss.eof()) {
    std::string header = readHeader(ss);
    if (header.empty()) break;
    std::string name = getName(header);
    if (name.empty()) {
      std::regex rx("([^\\\\\\/]+?)(\\.[^.]*$|$)");
      auto elemBegin = std::sregex_iterator(fn.begin(), fn.end(), rx);
      const auto nmatches = std::distance(elemBegin, std::sregex_iterator());
      if (nmatches > 0) {
        name = (*elemBegin)[1];
      }
    }
    auto elems = readVerts(ss);
    types::Pcl obj{name};
    obj.v() = elems;
    container.emplace_back(obj);
  }
  return container;
}
}  // namespace zview::io