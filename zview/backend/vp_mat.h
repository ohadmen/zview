#pragma  once
#include "zview/common/common_types.h"




class VPmat 
{
	types::Matrix4x4 m_proj;
	types::Matrix4x4 m_view;
	int m_w;
	int m_h;


	

public:
	VPmat();

    types::Matrix4x4 getVPmatrix() const;
    void setWinSize(int w, int h);
	void updatePmat();

    
	types::Vector3 xy2screen(const types::Vector2& xy) const;
	const std::pair<types::Vector3,types::Vector3> xy2ray(const types::Vector2& xy) const;
	
	const types::Matrix4x4& getViewMatrix() const ;
	const types::Matrix4x4& getProjMatrix() const ;
	
    void setViewMatrix(const types::Matrix4x4& m);

	
	// Returns a rotation matrix that rotates around the axis v by the angle a (in radians).
    static types::Matrix4x4 rotate(float a, const types::Vector3& v);
    
	// Returns a scaling matrix that scales by the factor v.
	static types::Matrix4x4 scale(float       v);

	// Returns a translation matrix that translates by the vector v.
    static types::Matrix4x4 translate(const types::Vector3& v);
};

