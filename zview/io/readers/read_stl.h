#pragma once
#include <string>
#include "zview/common/common_types.h"


namespace io
{
	types::Mesh readstl(const std::string& filename);
	//void write3dd(const std::string& filename, std::vector<types::Mesh*> meshes, std::vector<types::Pcl*> pcls);
};

