#include "zview/io/reader_ply.h"

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
using ElemData =
    std::variant<std::vector<types::VertData>, std::vector<types::FaceIndx>,
                 std::vector<types::EdgeIndx>>;

using ReaderFunc = std::function<ElemData(std::ifstream &ss, size_t count)>;
static std::unordered_map<std::string, ReaderFunc> getReaderMap() {
  std::unordered_map<std::string, ReaderFunc> map;
  // xyzrgba
  map["vertexpropertyfloatxpropertyfloatypropertyfloatzpropertyucharrproperty"
      "uc"
      "hargpropertyucharbpropertyuchara"] = [](std::ifstream &ss,
                                               size_t count) -> ElemData {
    std::vector<types::VertData> v(count);
    ss.read(recast<char *>(&v[0]),
            static_cast<std::int64_t>(sizeof(v[0]) * count));
    return v;
  };
  // xyz
  map["vertexpropertyfloatxpropertyfloatypropertyfloatz"] =
      [](std::ifstream &ss, size_t count) -> ElemData {
    std::vector<types::VertData> v(count);
    for (size_t i = 0; i != count; ++i) {
      ss.read(recast<char *>(&v[i]), sizeof(float) * 3);
    }
    return v;
  };
  // edge
  map["edgepropertyintvertex1propertyintvertex2"] =
      [](std::ifstream &ss, size_t count) -> ElemData {
    std::vector<types::EdgeIndx> v(count);
    for (size_t i = 0; i != count; ++i) {
      ss.read(recast<char *>(&v[i]), 2 * sizeof(int32_t));
    }
    return v;
  };

  // face
  map["facepropertylistucharintvertex_indices"] = [](std::ifstream &ss,
                                                     size_t count) -> ElemData {
    std::vector<types::FaceIndx> v(count);
    uint8_t listsz{0};

    for (size_t i = 0; i != count; ++i) {
      ss.read(recast<char *>(&listsz), 1);
      if (listsz != 3) throw std::runtime_error("support only tri meshes");

      ss.read(recast<char *>(&v[i]), 3 * sizeof(int32_t));
    }

    return v;
  };

  return map;
}
static std::string getName(const std::string &header) {
  static const std::regex rx("comment\\s+([^\\s]+)");

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
    header += line + " ";
    if (line.find("end_header") != std::string::npos) break;
  }

  return header;
}

static std::vector<ElementHeader> getElementHeaders(const std::string &header) {
  std::vector<ElementHeader> elems;

  static const std::regex rx(
      "element\\s+(\\w+)\\s+(\\d+)\\s*(.+?(?=element|end_header))");

  auto elemBegin = std::sregex_iterator(header.begin(), header.end(), rx);
  auto elemEnd = std::sregex_iterator();

  for (std::sregex_iterator i = elemBegin; i != elemEnd; ++i) {
    const std::smatch &m = *i;
    elems.push_back({m[1], m[2], m[3]});
  }

  return elems;
}

static std::vector<ElemData> readElems(
    const std::vector<ElementHeader> &elemHeaders, std::ifstream &ss) {
  std::vector<ElemData> elems;
  static const auto readerMap = getReaderMap();
  for (const auto &h : elemHeaders) {
    auto it = readerMap.find(h.signature);
    if (it == readerMap.end()) {
      throw std::runtime_error(
          "Could no parse ply file: element type " + h.type +
          " with the following propery list is not upported: " + h.signature);
    }
    const auto &reader = it->second;
    if (h.count == 0) {
      continue;
    }
    elems.push_back(reader(ss, h.count));
  }

  return elems;
}

static types::Shape elemArrayToshape(const std::vector<ElemData> &elems,
                                     const std::string &name) {
  const std::vector<types::VertData> *verticesP = nullptr;
  const std::vector<types::FaceIndx> *faceP = nullptr;
  const std::vector<types::EdgeIndx> *edgesP = nullptr;

  for (const ElemData &a : elems) {
    if (std::holds_alternative<std::vector<types::VertData>>(a)) {
      if (verticesP) {
        throw std::runtime_error("data holds more than a single vertex data");
      }
      verticesP = &std::get<std::vector<types::VertData>>(a);
    } else if (std::holds_alternative<std::vector<types::FaceIndx>>(a)) {
      if (faceP) {
        throw std::runtime_error("data holds more than a single face data");
      }
      faceP = &std::get<std::vector<types::FaceIndx>>(a);
    } else if (std::holds_alternative<std::vector<types::EdgeIndx>>(a)) {
      if (edgesP) {
        throw std::runtime_error("data holds more than a single edge data");
      }
      edgesP = &std::get<std::vector<types::EdgeIndx>>(a);
    }
  }
  if (!verticesP) throw std::runtime_error("data doesn't hold vertex data");
  if (faceP && edgesP) {
    throw std::runtime_error(
        "data holds both mesh and edge data, currently not supported");
  }

  if (faceP) {
    types::Mesh obj(name);
    obj.v() = *verticesP;
    obj.f() = *faceP;
    return obj;
  } else if (edgesP) {
    types::Edges obj(name);
    obj.v() = *verticesP;
    obj.e() = *edgesP;
    return obj;
  } else {
    types::Pcl obj(name);
    obj.v() = *verticesP;
    return obj;
  }
}
std::vector<types::Shape> ReaderPly::read(const std::string &fn) {
  std::vector<types::Shape> container;
  std::ifstream ss(fn, std::ios::in | std::ios::binary);
  if (ss.fail()) throw std::runtime_error("failed to open " + std::string(fn));

  while (!ss.eof()) {
    std::string header = readHeader(ss);
    if (header.empty()) break;
    auto elemHeaders = getElementHeaders(header);
    std::string name = getName(header);
    if (name.empty()) {
      std::regex rx("([^\\\\\\/]+?)(\\.[^.]*$|$)");
      auto elemBegin = std::sregex_iterator(fn.begin(), fn.end(), rx);
      const auto nmatches = std::distance(elemBegin, std::sregex_iterator());
      if (nmatches != 1) {
        name = "unknown";
      } else {
        name = (*elemBegin)[1];
      }
    }
    auto elems = readElems(elemHeaders, ss);
    types::Shape obj = elemArrayToshape(elems, name);
    container.emplace_back(obj);
  }
  return container;
}
}  // namespace zview::io