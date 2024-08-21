#include "zview/io/write_ply.h"

#include <fstream>

#include "zview/utils/recast.h"

namespace zview {
struct Writer {
  std::ofstream fid_;
  explicit Writer(const std::string &fn)
      : fid_(fn, std::ios::out | std::ios::binary) {}
  void operator()(const types::Pcl &obj) {
    fid_ << "ply" << std::endl;
    fid_ << "format binary_little_endian 1.0" << std::endl;
    fid_ << "comment " << obj.getName() << std::endl;
    fid_ << "element vertex " << obj.v().size() << std::endl;
    fid_ << "property float x" << std::endl;
    fid_ << "property float y" << std::endl;
    fid_ << "property float z" << std::endl;
    fid_ << "property uchar r" << std::endl;
    fid_ << "property uchar g" << std::endl;
    fid_ << "property uchar b" << std::endl;
    fid_ << "property uchar a" << std::endl;
    fid_ << "end_header" << std::endl;
    fid_.write(
        recast<const char *>(&obj.v()[0]),
        static_cast<std::int64_t>(sizeof(types::VertData) * obj.v().size()));
    fid_.flush();
  }

  void operator()(const types::Edges &obj) {
    fid_ << "ply" << std::endl;
    fid_ << "format binary_little_endian 1.0" << std::endl;
    fid_ << "comment " << obj.getName() << std::endl;
    fid_ << "element vertex " << obj.v().size() << std::endl;
    fid_ << "property float x" << std::endl;
    fid_ << "property float y" << std::endl;
    fid_ << "property float z" << std::endl;
    fid_ << "property uchar r" << std::endl;
    fid_ << "property uchar g" << std::endl;
    fid_ << "property uchar b" << std::endl;
    fid_ << "property uchar a" << std::endl;
    fid_ << "element edge " << obj.e().size() << std::endl;
    fid_ << "property int vertex1" << std::endl;
    fid_ << "property int vertex2" << std::endl;
    fid_ << "end_header" << std::endl;

    fid_.write(
        recast<const char *>(&obj.v()[0]),
        static_cast<std::int64_t>(sizeof(types::VertData) * obj.v().size()));
    fid_.write(
        recast<const char *>(&obj.e()[0]),
        static_cast<std::int64_t>(sizeof(types::EdgeIndx) * obj.e().size()));
    fid_.flush();
  }

  void operator()(const types::Mesh &obj) {
    fid_ << "ply" << std::endl;
    fid_ << "format binary_little_endian 1.0" << std::endl;
    fid_ << "comment " << obj.getName() << std::endl;
    fid_ << "element vertex " << obj.v().size() << std::endl;
    fid_ << "property float x" << std::endl;
    fid_ << "property float y" << std::endl;
    fid_ << "property float z" << std::endl;
    fid_ << "property uchar r" << std::endl;
    fid_ << "property uchar g" << std::endl;
    fid_ << "property uchar b" << std::endl;
    fid_ << "property uchar a" << std::endl;
    fid_ << "element face " << obj.f().size() << std::endl;
    fid_ << "property list uchar int vertex_indices" << std::endl;
    fid_ << "end_header" << std::endl;

    fid_.write(
        recast<const char *>(&obj.v()[0]),
        static_cast<std::int64_t>(sizeof(types::VertData) * obj.v().size()));
    uint8_t nfaces = 3;
    for (const auto &f : obj.f()) {
      fid_.write(recast<const char *>(&nfaces), 1);
      fid_.write(recast<const char *>(&f), sizeof(types::FaceIndx));
    }
    fid_.flush();
  }
};

void io::writePly(std::string fn, const std::vector<types::Shape> &shapes) {
  auto pos = fn.find_last_of('.');
  std::string suffix =
      pos == std::string::npos ? fn : fn.substr(pos, std::string::npos);
  if (suffix != ".ply") fn += ".ply";

  Writer w(fn);

  for (const auto &objv : shapes) {
    std::visit(w, objv);
  }
}
};  // namespace zview
