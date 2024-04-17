#pragma once
#include <string>
namespace zview
{
class Params
{
    private:
    Params(){};
    public:
    static Params& i();
    
    void load(std::string fn);

    float camera_fov_deg{60.0f};
    float camera_z_near{0.1f};
    float camera_z_far{100.0f};
    float trackball_radius{0.5};

};
}//namespce zview