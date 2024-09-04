import platform
print(platform.python_version())
import unittest
import numpy as np
import pyzview_inf


class TestBasic(unittest.TestCase):

  def test_add(self):

    pts = np.random.rand(100, 3)
    inf = pyzview_inf.interface()
    inf.plot("interface_check", pts)
    


if __name__ == "__main__":
  unittest.main()