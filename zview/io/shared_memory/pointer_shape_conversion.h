#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <vector>

#include "zview/types/types.h"
namespace zview {

template <size_t N>
std::vector<std::array<std::uint32_t, N>> ptr2indices(const std::uint32_t* ptr,
                                                      size_t n_indices) {
  std::vector<std::array<std::uint32_t, N>> vec(n_indices);
  for (ssize_t i{0}; i < ssize_t(n_indices); ++i) {
    std::memcpy(vec[i].data(), std::next(ptr, i * N),
                N * sizeof(std::uint32_t));
  }
  return vec;
}

std::vector<types::VertData> ptr2vertData(const float* ptr, size_t n_points,
                                          std::uint32_t dim_points) {
  std::vector<types::VertData> vec(n_points);
  switch (dim_points) {
    case 3:  // xyz
    {
      for (ssize_t i{0}; i < ssize_t(n_points); ++i) {
        vec[i].x = static_cast<float>(*std::next(ptr, i * dim_points));
        vec[i].y = static_cast<float>(*std::next(ptr, i * dim_points + 1));
        vec[i].z = static_cast<float>(*std::next(ptr, i * dim_points + 2));
      }

      break;
    }
    case 4:  // xyzi
    {
      for (ssize_t i{0}; i < ssize_t(n_points); ++i) {
        vec[i].x = static_cast<float>(*std::next(ptr, i * dim_points));
        vec[i].y = static_cast<float>(*std::next(ptr, i * dim_points + 1));
        vec[i].z = static_cast<float>(*std::next(ptr, i * dim_points + 2));
        vec[i].r = vec[i].b = vec[i].g =
            static_cast<std::uint8_t>(*std::next(ptr, i * dim_points + 3));
      }
      break;
    }
    case 6:  // xyzrgb
    {
      for (ssize_t i{0}; i < ssize_t(n_points); ++i) {
        vec[i].x = static_cast<float>(*std::next(ptr, i * dim_points));
        vec[i].y = static_cast<float>(*std::next(ptr, i * dim_points + 1));
        vec[i].z = static_cast<float>(*std::next(ptr, i * dim_points + 2));
        vec[i].r =
            static_cast<std::uint8_t>(*std::next(ptr, i * dim_points + 3));
        vec[i].g =
            static_cast<std::uint8_t>(*std::next(ptr, i * dim_points + 4));
        vec[i].b =
            static_cast<std::uint8_t>(*std::next(ptr, i * dim_points + 5));
      }

      break;
    }
    case 7:  // xyzrgba
    {
      for (ssize_t i{0}; i < ssize_t(n_points); ++i) {
        vec[i].x = static_cast<float>(*std::next(ptr, i * dim_points));
        vec[i].y = static_cast<float>(*std::next(ptr, i * dim_points + 1));
        vec[i].z = static_cast<float>(*std::next(ptr, i * dim_points + 2));
        vec[i].r =
            static_cast<std::uint8_t>(*std::next(ptr, i * dim_points + 3));
        vec[i].g =
            static_cast<std::uint8_t>(*std::next(ptr, i * dim_points + 4));
        vec[i].b =
            static_cast<std::uint8_t>(*std::next(ptr, i * dim_points + 5));
        vec[i].a =
            static_cast<std::uint8_t>(*std::next(ptr, i * dim_points + 6));
      }

      break;
    }
    default: {
      break;
    }
  }
  return vec;
}

}  // namespace zview