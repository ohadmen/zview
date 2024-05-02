#pragma once
#include <string>
namespace zview
{
class Params
{
    static float deg2rad(float deg);
    private:
    Params(){};
    public:
    static Params& i();

    
    void load(std::string fn);

    float camera_fov_rad{deg2rad(60.0f)};
    float camera_z_near{1e-1f};
    float camera_z_far{1e4f};
    int texture_type{0};
    float point_size{1.0f};
    int background_color{0};


};
}//namespce zview