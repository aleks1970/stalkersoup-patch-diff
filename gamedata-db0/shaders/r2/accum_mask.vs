//TB3D 109993 Adapted from Meltac 2.0 DX11 shaders
#include "common.h"
#ifdef DX11

float4 main(float4 P:POSITION):POSITION {
return mul(m_WVP,P);
}
FXVS;

#else

float4 main(float4 P:POSITION):POSITION {
return mul(m_WVP,P);
}
FXVS;

#endif