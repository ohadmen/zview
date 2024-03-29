#pragma once
#include <QtGui/QVector3D>
#include <QtGui/QVector4D>
#include <vector>
#include <array>
#include <variant>
#include <cmath>

namespace Types
{

class Roi3d
{
	std::array<float, 2> m_x;
	std::array<float, 2> m_y;
	std::array<float, 2> m_z;

public:
	Roi3d() : m_x{0, 0}, m_y{0, 0}, m_z{0, 0} {}
	Roi3d(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax) : m_x{xmin, xmax}, m_y{ymin, ymax}, m_z{zmin, zmax} {}
	void operator|=(const Roi3d &other)
	{
		m_x[0] = std::min(m_x[0], other.m_x[0]);
		m_x[1] = std::max(m_x[1], other.m_x[1]);
		m_y[0] = std::min(m_y[0], other.m_y[0]);
		m_y[1] = std::max(m_y[1], other.m_y[1]);
		m_z[0] = std::min(m_z[0], other.m_z[0]);
		m_z[1] = std::max(m_z[1], other.m_z[1]);
	}
	float rangex() const { return m_x[1] - m_x[0]; }
	float rangey() const { return m_y[1] - m_y[0]; }
	float rangez() const { return m_z[1] - m_z[0]; }

	float midx() const { return (m_x[0] + m_x[1]) / 2; }
	float midy() const { return (m_y[0] + m_y[1]) / 2; }
	float midz() const { return (m_z[0] + m_z[1]) / 2; }
	QVector3D mid() { return {midx(), midy(), midz()}; }

	float minx() const { return m_x[0]; }
	float maxx() const { return m_x[1]; }
	float miny() const { return m_y[0]; }
	float maxy() const { return m_y[1]; }
	float minz() const { return m_z[0]; }
	float maxz() const { return m_z[1]; }
};

struct VertData
{
	float x;
	float y;
	float z;
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

	VertData() : x(0), y(0), z(0), r(0), g(0), b(0), a(255) {}
	VertData(float x_, float y_, float z_, uint8_t r_ = 0, uint8_t g_ = 0, uint8_t b_ = 0, uint8_t a_ = 255) : x(x_), y(y_), z(z_), r(r_), g(g_), b(b_), a(a_) {}
	VertData(const QVector3D& xyz, const QVector4D& rgba):x(xyz[0]),y(xyz[1]),z(xyz[2]),r(rgba.x()),g(rgba.y()),b(rgba.z()),a(rgba.w()){}

	operator QVector3D() const { return QVector3D(x, y, z); }
	operator QVector4D() const { return QVector4D(r, g, b,a); }
	bool operator!=(const VertData &rhs) const
	{
		return x != rhs.x || y != rhs.y || z != rhs.z;
	}
	float operator[](int i){return i==0? x: i==1? y:i==2?z:y==3?r:y==4?g:y==5?b:y==6? a:std::numeric_limits<float>::quiet_NaN();}
	bool operator<(const VertData &rhs) const
	{
		if (x != rhs.x)
			return x < rhs.x;
		else if (y != rhs.y)
			return y < rhs.y;
		else if (z != rhs.z)
			return z < rhs.z;
		else
			return false;
	}
};
using FaceIndx = std::array<int32_t, 3>;
using EdgeIndx = std::array<int32_t, 2>;

class Pcl
{

	static std::array<QVector3D,2> nanminmax(const std::vector<Types::VertData>& v)
	{
		QVector3D mmin(std::numeric_limits<float>::max(),std::numeric_limits<float>::max(),std::numeric_limits<float>::max());
		QVector3D mmax(std::numeric_limits<float>::lowest(),std::numeric_limits<float>::lowest(),std::numeric_limits<float>::lowest());
		for(const auto& a:v)
		{
			float aa[] = {a.x,a.y,a.z};
			for(int i=0;i!=3;++i)
			{
				if(std::isnan(aa[i]) )
					break;
				mmin[i]=std::min(mmin[i],aa[i]);
				mmax[i]=std::max(mmax[i],aa[i]);
			}
		}
		return {mmin,mmax};
	}
protected:
	std::string m_name;
	std::vector<Types::VertData> m_v;

public:
	Pcl(const std::string& name):m_name(name){}
	const std::string& getName() const{return m_name;}
	
	std::vector<VertData> &v() { return m_v; }
	const std::vector<VertData> &v() const { return m_v; }

	Roi3d get3dbbox() const
	{
		if (m_v.size() == 0)
		{
            static const float e = 0.001f;
			return Types::Roi3d(-e, e, -e, e, -e, e);
		}
		else if (m_v.size() == 1)
		{
            static const float e =  0.001f;
			return Types::Roi3d(m_v[0].x - e, m_v[0].x + e, m_v[0].y - e, m_v[0].y + e, m_v[0].z - e, m_v[0].z + e);
		}
		else
		{	
			auto [mmin,mmax] = nanminmax(m_v);
			return Types::Roi3d(mmin[0],mmax[0],mmin[1],mmax[1],mmin[2],mmax[2]);
		}
	}
};

class Mesh : public Pcl
{
protected:
	std::vector<FaceIndx> m_f;

public:
	Mesh(const std::string& name):Pcl(name){}
	std::vector<FaceIndx> &f() { return m_f; }
	const std::vector<FaceIndx> &f() const { return m_f; }
};

class Edges : public Pcl
{
protected:
	std::vector<EdgeIndx> m_e;

public:
	Edges(const std::string& name):Pcl(name){}
	std::vector<EdgeIndx> &e() { return m_e; }
	const std::vector<EdgeIndx> &e() const { return m_e; }
};
using Shape = std::variant<Pcl, Edges, Mesh>;
} // namespace Types
