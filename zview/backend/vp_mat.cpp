#include "zview/backend/vp_mat.h"
#include "zview/common/params.h"


bool qFuzzyIsNull(float d)
{
    static constexpr float EPS = std::numeric_limits<float>::epsilon()*1e3;
    return std::abs(d) < EPS;
}
types::Vector3 mult(const types::Matrix4x4 &m, const types::Vector3 &v)
{
	types::Vector4 v4(v[0], v[1], v[2], 1);
	const auto res =  m*v4;
	return types::Vector3(res[0], res[1], res[2]);

}
VPmat::VPmat() : m_w(1), m_h(1)
{
    m_proj.setIdentity();
    m_view.setIdentity();
}


types::Vector3 VPmat::xy2screen(const types::Vector2 &xy) const
{
    types::Vector4 uv(xy.x() / m_w * 2 - 1, -(xy.y() / m_h * 2 - 1), 1, 1);
    types::Vector4 sv = m_proj.inverse() * uv;
    return types::Vector3(sv[0], sv[1], sv[2]);
}

types::Matrix4x4 VPmat::getVPmatrix() const { return m_proj * m_view; }
void VPmat::updatePmat()
{
    float aspect = float(m_w) / m_h;
    float angle = Params::camFOV();
    float nearPlane = Params::camZnear();
    float farPlane = Params::camZfar();

    float radians = (angle / 2.0f) * M_PI / 180.0f;
    float sine = std::sin(radians);
    if (sine == 0.0f)
        return;
    float cotan = std::cos(radians) / sine;
    float clip = farPlane - nearPlane;
    m_proj(0,0) = cotan / aspect;
    m_proj(1,0) = 0.0f;
    m_proj(2,0) = 0.0f;
    m_proj(3,0) = 0.0f;
    m_proj(0,1) = 0.0f;
    m_proj(1,1) = cotan;
    m_proj(2,1) = 0.0f;
    m_proj(3,1) = 0.0f;
    m_proj(0,2) = 0.0f;
    m_proj(1,2) = 0.0f;
    m_proj(2,2) = -(nearPlane + farPlane) / clip;
    m_proj(3,2) = -(2.0f * nearPlane * farPlane) / clip;
    m_proj(0,3) = 0.0f;
    m_proj(1,3) = 0.0f;
    m_proj(2,3) = -1.0f;
    m_proj(3,3) = 0.0f;


}
void VPmat::setWinSize(int w, int h)
{
    m_w = w;
    m_h = h;
    updatePmat();
}

const types::Matrix4x4 &VPmat::getViewMatrix() const { return m_view; }
const types::Matrix4x4 &VPmat::getProjMatrix() const { return m_proj; }

void VPmat::setViewMatrix(const types::Matrix4x4 &m) { m_view = m; }

types::Matrix4x4 VPmat::rotate(float a, const types::Vector3 &v)
{
    types::Matrix4x4 m;
    float x = v.x();
    float y = v.y();
    float z = v.z();

       if (a == 0.0f)
       {
        return types::Matrix4x4::Identity();
       }
        float c = std::cos(a);
        float s = std::sin(a);
    
   
        float len = x * x + y * y + z * z;
        if (!qFuzzyIsNull(len - 1.0f) && !qFuzzyIsNull(len)) {
            len = std::sqrt(len);
            x /= len;
            y /= len;
            z /= len;
        }
        const float ic = 1.0f - c;
        m(0,0) = x * x * ic + c;
        m(1,0) = x * y * ic - z * s;
        m(2,0) = x * z * ic + y * s;
        m(3,0) = 0.0f;
        m(0,1) = y * x * ic + z * s;
        m(1,1) = y * y * ic + c;
        m(2,1) = y * z * ic - x * s;
        m(3,1) = 0.0f;
        m(0,2) = x * z * ic - y * s;
        m(1,2) = y * z * ic + x * s;
        m(2,2) = z * z * ic + c;
        m(3,2) = 0.0f;
        m(0,3) = 0.0f;
        m(1,3) = 0.0f;
        m(2,3) = 0.0f;
        m(3,3) = 1.0f;
    
    
    return m;
}
types::Matrix4x4 VPmat::scale(float v)
{
    types::Matrix4x4 m = types::Matrix4x4::Identity();
    m(0,0) = m(1,1)=m(2,2)=v;
    return m;
}
types::Matrix4x4 VPmat::translate(const types::Vector3 &v)
{
    types::Matrix4x4 m = types::Matrix4x4::Identity();
    m(0,3) = v.x();
    m(1,3) = v.y();
    m(2,3) = v.z();
    return m;
}

const std::pair<types::Vector3, types::Vector3> VPmat::xy2ray(const types::Vector2 &xy)const
{
    types::Matrix4x4 vm = getViewMatrix();
    types::Matrix4x4 vmI = vm.inverse();
    types::Vector3 p = mult(vmI , types::Vector3(0, 0, 0));
    types::Vector3 p2 = mult(vmI , xy2screen(xy));
    types::Vector3 n = (p2 - p).normalized();
    return std::make_pair(p, n);
}

