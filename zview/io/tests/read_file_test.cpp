#include "zview/io/read_file.h"

#include <catch2/catch_test_macros.hpp>
#include <vector>

SCENARIO("read PLY file") {
  WHEN("readin bad file") { CHECK_THROWS(zview::io::read_file("unknown.ply")); }

  AND_WHEN("reading a PLY file") {
    auto obj = zview::io::read_file("models/horse.ply");
    THEN("The result should be true") {
      CHECK(obj.size() == 1);
      AND_THEN("The added shape names and num_points should be correct") {
        const auto name =
            std::visit([](const auto &arg) { return arg.getName(); }, obj[0]);
        // get Pcl from variant
        const auto &mesh = std::get<zview::types::Mesh>(obj[0]);

        CHECK(mesh.getName() == "demo/object/meshes/horse");

        CHECK(mesh.v().size() == 50730);
        CHECK(mesh.f().size() == 16910);
        CHECK(mesh.f()[10] == std::array<unsigned int, 3>{30, 31, 32});
      }
    }
  }
  AND_WHEN("reading a PCD file") {
    auto obj = zview::io::read_file("models/lucy.pcd");
    THEN("The result should be true") {
      CHECK(obj.size() == 1);
      AND_THEN("The added shape names and num_points should be correct") {
        const auto name =
            std::visit([](const auto &arg) { return arg.getName(); }, obj[0]);
        // get Pcl from variant
        const auto &pcl = std::get<zview::types::Pcl>(obj[0]);

        CHECK(pcl.getName() == "lucy/pcl");

        CHECK(pcl.v().size() == 8840);
      }
    }
  }
}