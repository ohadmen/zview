import platform
print(platform.python_version())
import unittest
import numpy as np
import pyzview_inf


class TestBasic(unittest.TestCase):

  def test_add(self):

    pts = (np.random.randn(10000000,3)*2).astype(np.float32)
    inf = pyzview_inf.interface()
    ok=inf.plot("interface_check", pts)
    print (f"response: {ok}")
    


if __name__ == "__main__":
  unittest.main()