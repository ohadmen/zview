#pragma once

#include <vector>
#include <string>
#include <QtCore/QStringList>
#include "zview/common/common_types.h"

namespace io
{
std::vector <types::Shape> readFileList(const QStringList &files);
}