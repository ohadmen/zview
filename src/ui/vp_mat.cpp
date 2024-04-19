#include "vp_mat.h"
#include "src/params/params.h"

namespace zview {



bool qFuzzyIsNull(float d)
{
    static constexpr float EPS = std::numeric_limits<float>::epsilon()*1e3;
    return std::abs(d) < EPS;
}

VPmat::VPmat() : m_w(1), m_h(1)
{
    m_proj.setIdentity();
    m_view.setIdentity();
}


types::Vector3 VPmat::xy2screen(float x,float y) const
{
    types::Vector4 uv(x / m_w * 2 - 1, -(y / m_h * 2 - 1), 1,1.0f);
    types::Vector4 sv = m_proj.inverse() * uv;
    return types::Vector3(sv[0], sv[1], sv[2]);
}

types::Matrix4x4 VPmat::getVPmatrix() const { return m_proj.matrix() * m_view.matrix(); }
void VPmat::updatePmat()
{
    
    const float aspect = float(m_w) / m_h;
    const float angle = zview::Params::i().camera_fov_deg;
    const float nearPlane = zview::Params::i().camera_z_near;
    const float farPlane = zview::Params::i().camera_z_far;

    float h_angle_rad = (angle / 2.0f) * M_PIf / 180.0f;
    float cotan;
    if(std::abs(h_angle_rad) < std::numeric_limits<float>::epsilon()*1e3)
    {
        cotan=1e3;
    }
    else 
    {
        cotan = 1.0f/std::tan(h_angle_rad);
    }
    
    float clip = farPlane - nearPlane;

    m_proj << cotan / aspect, 0, 0, 0,
        0, cotan, 0, 0,
        0, 0, -(farPlane+nearPlane)/clip, -1,
        0, 0, -2*nearPlane*farPlane/clip, 0;


}
void VPmat::setWinSize(int w, int h)
{
    m_w = w;
    m_h = h;
    updatePmat();
}

const types::Transform &VPmat::getViewMatrix() const { return m_view; }
// const types::Matrix4x4 &VPmat::getProjMatrix() const { return m_proj; }

void VPmat::setViewMatrix(const types::Transform &m) { m_view = m; }

void VPmat::rotate(float a, const types::Vector3 &v)
{
    m_view.prerotate(Eigen::AngleAxis<float>{a,v});
    return;

    return ;
}
void VPmat::scale(float v)
{
    m_view.prescale(v);
    return;
}
void VPmat::translate(const types::Vector3 &v)
{
    m_view.pretranslate(v);
    return ;
}

const std::pair<types::Vector3, types::Vector3> VPmat::xy2ray(const types::Vector2 &xy)const
{
    const auto vm = getViewMatrix();
    const auto vmI = vm.inverse();
    const auto p = vmI * types::Vector3(0, 0, 0);
    const auto p2 = vmI * xy2screen(xy[0],xy[1]);
    const auto n = (p2 - p).normalized();
    return std::make_pair(p, n);
}

}