#pragma  once
#include "src/types/types.h"
#include <Eigen/Dense>


namespace zview{
class VPmat 
{
	types::Matrix4x4 m_proj;
	types::Transform m_view;
	int m_w;
	int m_h;


	

public:
	VPmat();

    types::Matrix4x4 getVPmatrix() const;
    void setWinSize(int w, int h);
	void updatePmat();

    
	types::Vector3 xy2screen(float x,float y) const;
	const std::pair<types::Vector3,types::Vector3> xy2ray(const types::Vector2& xy) const;
	
	const types::Transform& getViewMatrix() const ;
	// const types::Matrix4x4& getProjMatrix() const ;
	
    void setViewMatrix(const types::Transform& m);
	
    void rotate(float a, const types::Vector3& v);
	
	void scale(float       v);
	
    void translate(const types::Vector3& v);
};

}//namespce zview