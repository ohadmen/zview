import pyzview.pyzview_inf
import numpy as np


class Singleton(type):
    _instances = {}

    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super(Singleton, cls).__call__(*args, **kwargs)
        return cls._instances[cls]


class Pyzview(metaclass=Singleton):

 
    @staticmethod
    def _str2rgb(colorstr):
        if colorstr == 'r':
            col = [255, 0, 0]
        elif colorstr == 'g':
            col = [0, 255, 0]
        elif colorstr == 'b':
            col = [0, 0, 255]
        elif colorstr == 'c':
            col = [0, 255, 255]
        elif colorstr == 'm':
            col = [255, 0, 255]
        elif colorstr == 'y':
            col = [255, 255, 0]
        elif colorstr == 'w':
            col = [255, 255, 255]
        elif colorstr == 'k':
            col = [0, 0, 0]
        elif colorstr == 'R':
            col = list(np.random.rand(3)*255)
        else:
            raise RuntimeError("unknown color name")
        return col

    @staticmethod
    def xyz2xyzrgba(xyz,color,alpha):
        xyz_dim = xyz.shape[1]
        xyzrgba = np.c_[xyz, np.ones((xyz.shape[0],7-xyz_dim))*255]
        if color is not None:
            xyzrgba[:,3:6] = Pyzview._str2rgb(color)
        if alpha is not None:
            xyzrgba[:,6] = alpha
        return xyzrgba
    
    def connect(self):
        if self.zv is not None:
            return True
        try:
            self.zv = pyzview.pyzview_inf.interface()  # get interface
            return True
        except RuntimeError:
            raise RuntimeWarning("Could not connect to zview")
            self.zv = None
            return False


    def __init__(self):
        self.zv = None
        self.connect()
        
    
    def remove_shape(self, namehandle=""):
        self.zv.removeShape(namehandle)
        
    def plot_points(self, namehandle, xyz, color=None, alpha=None):
        if not self.connect():
            return False
        xyzrgba = self.xyz2xyzrgba(xyz, color, alpha)
        ok = self.zv.plot(namehandle, xyzrgba)
        return ok
    def plot_mesh(self, namehandle, xyz, indices, color=None, alpha=None):
        if not self.connect():
            return False
        xyzrgba = self.xyz2xyzrgba(xyz, color, alpha)
        ok = self.zv.plot(namehandle, xyzrgba, indices[:,0:3])
        return ok
    def plot_edges(self, namehandle, xyz, indices, color=None, alpha=None):
        if not self.connect():
            return False
        xyzrgba = self.xyz2xyzrgba(xyz, color, alpha)
        ok = self.zv.plot(namehandle, xyzrgba, indices[:,0:2])
        return ok
         
