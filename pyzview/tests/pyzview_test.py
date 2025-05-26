import os
import platform
from time import sleep

print(platform.python_version())
import subprocess
import unittest
import numpy as np
from pyzview import Pyzview
from scipy.spatial.transform import Rotation as R


class TestBasic(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # run zview exectuable as recived from the //packaging:zview bazel target in a different process
        # this is needed to test the interface
        cls.zview_process = subprocess.Popen(
            ["packaging/zview"],
            shell=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        sleep(0.1)

    @classmethod
    def tearDownClass(cls):
        # kill the zview process
        cls.zview_process.kill()
        cls.zview_process.wait()

    def z_shifted(self, offset):
        n = 12345
        pts = (np.random.randn(n, 7)).astype(np.float32) * 10
        pts[:, 3:] = np.random.rand(pts.shape[0], 4) * 255
        pts[:, 2] += offset
        return pts

    def test_points(self):
        pts = self.z_shifted(0)
        assert Pyzview().plot_points("interface_check", pts)

    def test_mesh(self):
        pts = self.z_shifted(50)
        indices = np.random.permutation(np.arange(pts.shape[0]))[
            : pts.shape[0] - (pts.shape[0] % 3)
        ].reshape((-1, 3))
        assert Pyzview().plot_mesh("interface_check_mesh", pts, indices)

    def test_edges(self):
        pts = self.z_shifted(100)
        indices = np.random.permutation(np.arange(pts.shape[0]))[
            : pts.shape[0] - (pts.shape[0] % 2)
        ].reshape((-1, 2))
        assert Pyzview().plot_edges("interface_check_edges", pts, indices)

    def test_marker(self):
        rot = R.from_euler("zyx", [10, 20, 30], degrees=True).as_matrix()
        assert Pyzview().plot_marker(
            "interface_check_marker", [0, 1, 2], 200, rot, color="r", alpha=50
        )

    def test_cuboid(self):
        rot = R.from_euler("zyx", [10, 20, 30], degrees=True).as_matrix()
        assert Pyzview().plot_cuboid(
            "interface_check_cuboid",
            [0, 0, 0],
            [0.1, 0.2, 0.3],
            rot,
            color="r",
            alpha=25,
        )

    def test_cuboid_edges(self):
        rot = R.from_euler("zyx", [30, 60, 90], degrees=True).as_matrix()
        assert Pyzview().plot_cuboid_edges(
            "interface_check_cuboid_edges",
            [0, 0, 0],
            [0.1, 0.2, 0.3],
            rot,
            color="g",
            alpha=128,
        )


if __name__ == "__main__":
    unittest.main()
