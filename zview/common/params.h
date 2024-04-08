#pragma once
#include "version.h"
#include "common_types.h"

#define paramsStaticParameter(paramName, paramType,paramDefaultValue,paramUnknownValue) \
static paramType paramName(paramType vin = paramUnknownValue) \
    {                               \
    static paramType vs(paramDefaultValue);               \
    if (vin != paramUnknownValue)                   \
        vs = vin;                   \
    return vs;                       \
    }                               \


class Params
{
public:
    static constexpr Version VERSION{1,5,0};
    static constexpr size_t SHARED_MEM_SIZE{(1U)<<27};
    paramsStaticParameter(drawablesTexture, int, 1,-1);
	paramsStaticParameter(viewGrid, int, 1,-1);
    paramsStaticParameter(viewAxes, int, 1,-1);
    paramsStaticParameter(trackBallRadius, float, 0.5f,-1);
    paramsStaticParameter(nStatusLines, int, 3,-1);
    paramsStaticParameter(camZnear, float, 0.1f,-1);
    paramsStaticParameter(camZfar, float, 1e2f,-1);
    paramsStaticParameter(camFOV, float, 60.0f,-1);
    paramsStaticParameter(whiteBackground, int, 0,-1);
	paramsStaticParameter(pointSize, float, 3,-1);
    paramsStaticParameter(lightDir, types::Vector3,types::Vector3(0.4f, 0.48f, 0.51f),types::Vector3(-1, -1, -1) );
    
    
private:
    Params() {}                    // namespace

};
 
