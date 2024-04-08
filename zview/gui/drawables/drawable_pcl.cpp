#include <cmath>
#include "drawable_pcl.h"


DrawablePcl::DrawablePcl(const std::string& name) :Pcl(name){}

DrawablePcl::~DrawablePcl()
{
	if (m_vBuff.isCreated())
		m_vBuff.destroy();
}
const std::string& DrawablePcl::getName() const{return m_name;};
void DrawablePcl::initializeGL()
{
	initializeOpenGLFunctions();
	m_vBuff.create();
	m_vBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);

	m_vBuff.bind();

	m_vBuff.allocate(m_v.data(), int(m_v.size() * sizeof(types::VertData)));
	m_vBuff.write(0, m_v.data(), int(m_v.size() * sizeof(types::VertData)));
	m_vBuff.release();

	privInitShader("point");

    

}
types::Shape DrawablePcl::getShape() const {	return *this;};

void DrawablePcl::paintGL(const QMatrix4x4& mvp)
{
    if (!m_vBuff.isCreated())
	{
        initializeGL();
	}
    if (!m_active)
	{
        return;
	}
	int txt = Params::drawablesTexture();
    

	m_vBuff.bind();
	m_meshShader.bind();
	m_meshShader.setUniformValue("mvp_matrix", mvp);

	
	m_meshShader.setUniformValue("u_ptSize", Params::pointSize());
	m_meshShader.setUniformValue("u_lightDir",Params::lightDir());
	m_meshShader.setUniformValue("u_txt", txt);

	int vp = m_meshShader.attributeLocation("a_xyz");
	m_meshShader.enableAttributeArray(vp);
	m_meshShader.setAttributeBuffer(vp, GL_FLOAT, 0 * sizeof(float), 3, sizeof(types::VertData));

	int vc = m_meshShader.attributeLocation("a_rgb");
	m_meshShader.enableAttributeArray(vc);
	m_meshShader.setAttributeBuffer(vc, GL_UNSIGNED_BYTE, 3 * sizeof(float), 4, sizeof(types::VertData));


	
	

	glDrawArrays(GL_POINTS, 0,m_vBuff.size() / sizeof(float));

	m_vBuff.release();

	// Clean up state machine
	m_meshShader.disableAttributeArray(vp);
	m_meshShader.disableAttributeArray(vc);







}


types::Roi3d DrawablePcl::get3dbbox() const {return types::Pcl::get3dbbox();}


bool isnan(const QVector3D& v)
{
    return std::isnan(v[0]) | std::isnan(v[1]) | std::isnan(v[2]);
}

types::VertData DrawablePcl::picking(const QVector3D& p, const QVector3D& n) const
{
    static const float pi = std::acos(0)*2;
    static const float angularthresholt = std::tan(2.0*pi/180.0);
    constexpr float inf = std::numeric_limits<float>::infinity();
	float closestDistance = inf;
	types::VertData closestPoint(inf,inf,inf,0,0,0);
	for(const auto& pt:m_v)
	{
		QVector3D pt_(pt);
        if(isnan(pt_))
			continue;
		float d = QVector3D::dotProduct(n,pt_-p);
		if(d<0)//behind camera
			continue;
		if(closestDistance<d)
			continue;
		float a = (p+n*d-pt_).length();
		if(a/d>angularthresholt)
			continue;
		closestDistance = d;
		closestPoint = pt;
	}
	return closestPoint;

}
bool DrawablePcl::updateVertexBuffer(const types::VertData* data,size_t n)
{
    if(n>m_v.size())
    {
        return false;
    }
	std::copy(data,data+n,m_v.begin());
	return DrawableBase::updateVertexBuffer(data,n);
}
