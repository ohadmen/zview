import platform
print(platform.python_version())
import unittest
import numpy as np
from pyzview import Pyzview

class TestBasic(unittest.TestCase):

  def z_shifted(self,offset):
      n = 12345
      pts = (np.random.randn(n,7)).astype(np.float32)*10
      pts[:,3:] = np.random.rand(pts.shape[0],4)*255
      pts[:,2]+=offset
      return pts
  def test_points(self):
      pts = self.z_shifted(0)
      assert(Pyzview().plot_points("interface_check", pts))

  def test_mesh(self):
      pts = self.z_shifted(50)
      indices = np.random.permutation(np.arange(pts.shape[0]))[:pts.shape[0]-(pts.shape[0]%3)].reshape((-1,3))
      assert(Pyzview().plot_mesh("interface_check_mesh",pts,indices))

  def test_edges(self):
      pts = self.z_shifted(100)
      indices = np.random.permutation(np.arange(pts.shape[0]))[:pts.shape[0]-(pts.shape[0]%2)].reshape((-1,2))
      assert(Pyzview().plot_edges("interface_check_edges",pts,indices))


      
      
    


if __name__ == "__main__":
  unittest.main()