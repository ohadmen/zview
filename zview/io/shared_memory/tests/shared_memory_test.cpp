#include <boost/interprocess/shared_memory_object.hpp>
#include <catch2/catch_test_macros.hpp>
#include <vector>

#include "zview/io/shared_memory/shared_memory_client.h"
#include "zview/io/shared_memory/shared_memory_server.h"
#include "zview/io/shared_memory/shared_memory_types.h"

std::size_t getNumPoints(const zview::types::Pcl &shape) {
  return shape.v().size();
}

std::size_t getNumPoints(const zview::types::Edges &shape) { return 0; }

std::size_t getNumPoints(const zview::types::Mesh &shape) { return 0; }

SCENARIO("Shared Memory Test") {
  boost::interprocess::shared_memory_object::remove(zview::SHARED_MEMORY_NAME);

  WHEN("Initializing a shared memory client before a server") {
    THEN("An exception should be thrown") {
      CHECK_THROWS(zview::SharedMemoryClient());
    }
  }

  GIVEN("A shared memory server") {
    int add_shape_calls = 0;
    std::vector<std::string> added_shape_names{};
    std::vector<std::size_t> added_shape_num_points{};
    int remove_shape_calls = 0;

    const auto add_shape =
        [&add_shape_calls, &added_shape_names,
         &added_shape_num_points](zview::types::Shape &&shape) {
          ++add_shape_calls;
          added_shape_names.emplace_back(
              std::visit([](auto &&arg) { return arg.getName(); }, shape));
          added_shape_num_points.emplace_back(
              std::visit([](auto &&arg) { return getNumPoints(arg); }, shape));
          return 0;
        };
    const auto remove_shape = [&remove_shape_calls](const std::string &) {
      ++remove_shape_calls;
    };

    zview::SharedMemoryServer server{add_shape, remove_shape};

    WHEN("Creating another server") {
      THEN("An exception should be thrown") {
        CHECK_THROWS(zview::SharedMemoryServer(add_shape, remove_shape));
      }
    }

    WHEN("Calling the step function") {
      server.step();

      THEN("The add_shape_calls and remove_shape_calls should be 0") {
        CHECK(add_shape_calls == 0);
        CHECK(remove_shape_calls == 0);
      }
    }

    GIVEN("A shared memory client") {
      zview::SharedMemoryClient client;

      WHEN("Adding a PCL") {
        std::string name = "pcl";
        constexpr std::size_t n_points = 10U;
        std::array<zview::Vertex, n_points> vertices;

        const auto res = client.addPcl(name, n_points, vertices.data());

        THEN("The result should be true") {
          CHECK(res == true);

          AND_WHEN("Calling the step function") {
            server.step();

            THEN("The add_shape_calls should be 1") {
              CHECK(add_shape_calls == 1);
            }
            AND_THEN(
                "The added_shape_names should contain the name of the PCL") {
              CHECK(added_shape_names.size() == 1);
              CHECK(added_shape_names[0] == name);
            }
            AND_THEN(
                "The added_shape_num_points should contain the number of "
                "points in the PCL") {
              CHECK(added_shape_num_points.size() == 1);
              CHECK(added_shape_num_points[0] == n_points);
            }

            AND_WHEN("Calling step again") {
              server.step();

              THEN("The add_shape_calls should still be 1") {
                CHECK(add_shape_calls == 1);
              }

              AND_WHEN("Adding two more PCLs without calling step in between") {
                std::string name2 = "pcl2";
                constexpr std::size_t n_points2 = 15U;
                std::array<zview::Vertex, n_points2> vertices2;

                std::string name3 = "pcl3";
                constexpr std::size_t n_points3 = 12U;
                std::array<zview::Vertex, n_points3> vertices3;

                const auto res2 =
                    client.addPcl(name2, n_points2, vertices2.data());
                const auto res3 =
                    client.addPcl(name3, n_points3, vertices3.data());

                THEN("The results should be true") {
                  CHECK(res2 == true);
                  CHECK(res3 == true);

                  AND_WHEN("Calling the step function") {
                    server.step();

                    THEN("The add_shape_calls should be 3") {
                      CHECK(add_shape_calls == 3);
                    }
                    AND_THEN(
                        "The added_shape_names should contain the names of the "
                        "PCLs") {
                      CHECK(added_shape_names.size() == 3);
                      CHECK(added_shape_names[1] == name2);
                      CHECK(added_shape_names[2] == name3);
                    }
                    AND_THEN(
                        "The added_shape_num_points should contain the number "
                        "of points in the PCLs") {
                      CHECK(added_shape_num_points.size() == 3);
                      CHECK(added_shape_num_points[1] == n_points2);
                      CHECK(added_shape_num_points[2] == n_points3);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  GIVEN("A new shared memory server") {
    int add_shape_calls = 0;

    const auto add_shape = [&add_shape_calls](zview::types::Shape &&shape) {
      ++add_shape_calls;
      return 0;
    };
    const auto remove_shape = [](const std::string &) {};

    boost::interprocess::shared_memory_object::remove(
        zview::SHARED_MEMORY_NAME);
    zview::SharedMemoryServer server{add_shape, remove_shape};

    GIVEN("A shared memory client") {
      zview::SharedMemoryClient client;

      int num_successful_calls = 2;

      std::string name = "pcl";
      const auto total_available_size =
          zview::SHARED_MEMORY_SIZE - sizeof(zview::SharedMemoryInfo);
      const auto num_requests = num_successful_calls + 1;
      const auto required_request_size = total_available_size / num_requests;
      const auto n_points =
          (required_request_size - zview::REQUEST_HEADER_SIZE) /
              sizeof(zview::Vertex) +
          1;
      std::vector<zview::Vertex> vertices(n_points);

      WHEN("Adding a large PCL") {
        const auto res1 = client.addPcl(name, n_points, vertices.data());

        THEN("The result should be true") {
          CHECK(res1 == true);

          AND_WHEN("Calling the step function") {
            server.step();

            THEN("The add_shape_calls should be 1") {
              CHECK(add_shape_calls == 1);

              AND_WHEN("Adding another large PCL") {
                const auto res2 =
                    client.addPcl(name, n_points, vertices.data());
                THEN("The result should be true") {
                  CHECK(res2 == true);

                  AND_WHEN("Calling the step function") {
                    server.step();

                    THEN("The add_shape_calls should be 2") {
                      CHECK(add_shape_calls == 2);

                      AND_WHEN("Adding a PCL that is too large") {
                        const auto res3 =
                            client.addPcl(name, n_points + 1, vertices.data());
                        THEN("The result should be false") {
                          CHECK(res3 == false);

                          AND_WHEN("Calling the step function") {
                            server.step();

                            THEN("The add_shape_calls should still be 2") {
                              CHECK(add_shape_calls == 2);
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}