#include <stddef.h>

#include <boost/interprocess/shared_memory_object.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <thread>
#include <vector>

#include "zview/io/shared_memory/shared_memory_client.h"
#include "zview/io/shared_memory/shared_memory_server.h"

SCENARIO("Shared Memory Test") {
  std::vector<std::string> added_shape_names{};
  std::vector<std::size_t> added_shape_num_points{};
  int remove_shape_calls = 0;

  const auto add_shape = [&added_shape_names, &added_shape_num_points](
                             zview::types::Shape &&shape) {
    added_shape_names.push_back(
        std::visit([](const auto &arg) { return arg.getName(); }, shape));
    added_shape_num_points.push_back(
        std::visit([](const auto &arg) { return arg.v().size(); }, shape));
    return 0;
  };
  const auto remove_shape = [&remove_shape_calls](const std::string &) {
    ++remove_shape_calls;
  };

  WHEN("Initializing a shared memory client before a server") {
    {
      // create server and destory it to clean shared memory
      zview::SharedMemoryServer{add_shape, remove_shape};
    }
    THEN("An exception should be thrown") {
      CHECK_THROWS(zview::SharedMemoryClient{});
    }
  }

  GIVEN("A shared memory server") {
    zview::SharedMemoryServer server{add_shape, remove_shape};
    bool stop_server = false;
    std::thread server_thread([&server, &stop_server]() {
      // NOLINTNEXTLINE(bugprone-infinite-loop)
      while (!stop_server) {
        server.step();
      }
    });

    WHEN("checking the call stack for the first time") {
      THEN("The add_shape_calls and remove_shape_calls should be 0") {
        CHECK(added_shape_names.size() == 0);
        CHECK(remove_shape_calls == 0);
      }
    }

    AND_GIVEN("A shared memory client") {
      zview::SharedMemoryClient client;
      // start server on a thread

      const std::string name_pcl{"pcl"};
      const std::string name_mesh{"mesh"};
      const std::string name_edge{"edge"};

      WHEN("Plotting") {
        constexpr std::size_t n_points1 = 10U;
        constexpr std::size_t dim_points1 = 3U;
        std::array<float, n_points1 * dim_points1> vertices{};

        const auto res =
            client.plot(name_pcl, vertices.data(), n_points1, dim_points1);

        THEN("The result should be true") {
          CHECK(res == true);

          AND_THEN("The added shape names and num_points should be correct") {
            CHECK(added_shape_names.size() == 1);
            CHECK(added_shape_names[0] == name_pcl);
            CHECK(added_shape_num_points.size() == 1);
            CHECK(added_shape_num_points[0] == n_points1);
          }
        }

        constexpr std::size_t n_points_mesh = 15U;
        constexpr std::size_t n_indices_mesh = 19U;
        constexpr std::size_t dim_points_mesh = 4U;
        std::array<float, n_points_mesh * dim_points_mesh> vertices_mesh{};
        std::array<std::uint32_t, n_indices_mesh * 3> indices_mesh{};
        const auto res_mesh = client.plot(
            name_mesh, vertices_mesh.data(), n_points_mesh, dim_points_mesh,
            indices_mesh.data(), n_indices_mesh, 3);
        THEN("The results should be true") {
          CHECK(res_mesh == true);

          AND_THEN("The added shape names and num_points should be correct") {
            CHECK(added_shape_names.size() == 2);
            CHECK(added_shape_names[1] == name_mesh);
            CHECK(added_shape_num_points.size() == 2);
            CHECK(added_shape_num_points[1] == n_points_mesh);
          }
        }

        constexpr std::size_t n_points_edge = 15U;
        constexpr std::size_t n_indices_edge = 19U;
        constexpr std::size_t dim_points_edge = 4U;
        std::array<float, n_points_edge * dim_points_edge> vertices_edge{};
        std::array<std::uint32_t, n_indices_edge * 2> indices_edge{};
        const auto res_edge = client.plot(
            name_edge, vertices_edge.data(), n_points_edge, dim_points_edge,
            indices_edge.data(), n_indices_edge, 2);
        THEN("The results should be true") {
          CHECK(res_edge == true);
          AND_THEN("The added shape names and num_points should be correct") {
            CHECK(added_shape_names.size() == 3);
            CHECK(added_shape_names[2] == name_edge);
            CHECK(added_shape_num_points.size() == 3);
            CHECK(added_shape_num_points[2] == n_points_edge);
          }
        }
      }

      AND_WHEN("removing a shape") {
        client.removeShape(name_pcl);
        THEN("The remove_shape_calls should be 1") {
          CHECK(remove_shape_calls == 1);
        }
      }
    }

    stop_server = true;
    server_thread.join();
  }
}