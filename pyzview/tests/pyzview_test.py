import platform
print(platform.python_version())
import unittest
import numpy as np
import pyzview_inf
import time




class TestBasic(unittest.TestCase):

  def test_add(self):
    inf = pyzview_inf.interface()
    for i in range(1,10):
      n = 1000*(i**2)
      print(n)
      
      pts = (np.random.randn(n,7)*i).astype(np.float32)
      pts[:,3:] = np.random.rand(pts.shape[0],4)*255
      
      ok=inf.plot("interface_check", pts)
      indices = np.random.randint(0,pts.shape[0],size=(n,3)).astype(np.int32)
      print(indices.shape)
      pts[:,2]+=i*5
      ok=inf.plot("interface_check_mesh",pts,indices)
      pts[:,2]+=i*5
      ok=inf.plot("interface_check_edges",pts,indices[:,0:2])

      print (f"response: {ok}")
      time.sleep(1)
    


if __name__ == "__main__":
  unittest.main()