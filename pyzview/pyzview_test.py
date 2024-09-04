import platform
print(platform.python_version())
import unittest
import numpy as np
import pyzview_inf


class TestBasic(unittest.TestCase):

  def test_add(self):

    pts = np.random.rand(1000, 3)*100
    inf = pyzview_inf.interface()
    ok=inf.plot("interface_check", pts)
    print (f"response: {ok}")
    


if __name__ == "__main__":
  unittest.main()