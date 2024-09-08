

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include "zview/io/shared_memory/shared_memory_client.h"

namespace py = pybind11;
namespace zview {

class ZviewPyInf {
  zview::SharedMemoryClient m_client;

 public:
 public:
  bool plot(const char *name, const py::array_t<float> &xyz) {
    py::buffer_info buff = xyz.request();
    const auto ptr = static_cast<float *>(buff.ptr);
    if (buff.ndim != 2) {
      throw std::runtime_error("xyz should be Nx3");
    }
    return m_client.plot(name, ptr, buff.shape[0], buff.shape[1]);
  }
  bool plot(const char *name, const py::array_t<float> &xyz,
            const py::array_t<int32_t> &indices) {
    py::buffer_info buff = xyz.request();
    const auto ptr = static_cast<float *>(buff.ptr);
    if (buff.ndim != 2) {
      throw std::runtime_error("xyz should be Nx3");
    }
    py::buffer_info buff_indices = indices.request();
    const auto ptr_indices = static_cast<uint32_t *>(buff_indices.ptr);
    if (buff_indices.ndim != 2) {
      throw std::runtime_error("indices should be Nx3");
    }
    return m_client.plot(name, ptr, buff.shape[0], buff.shape[1], ptr_indices,
                         buff_indices.shape[0], buff_indices.shape[1]);
  }
  bool removeShape(const char *name) { return m_client.removeShape(name); }
};

}  // namespace zview

// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
PYBIND11_MODULE(pyzview_inf, m) {
  py::class_<zview::ZviewPyInf>(m, "interface")
      .def(py::init())
      .def("plot", py::overload_cast<const char *, const py::array_t<float> &>(
                       &zview::ZviewPyInf::plot))
      .def("plot", py::overload_cast<const char *, const py::array_t<float> &,
                                     const py::array_t<int32_t> &>(
                       &zview::ZviewPyInf::plot))
      .def("removeShape", &zview::ZviewPyInf::removeShape);
}
