//TB3D 109993 Adapted from Meltac 2.0 DX11 shaders
#include "common.h"

#ifdef DX11
struct a2v {
float4 P:POSITION;
float4 tc0:TEXCOORD0;
};
v_shadow_direct main(a2v I) {
v_shadow_direct O;
O.hpos=mul(m_WVP,I.P);
#ifndef USE_HWSMAP
O.depth=O.hpos.z;
#endif
return O;
}
FXVS;

#else

struct a2v {
float4 P:POSITION;
float4 tc0:TEXCOORD0;
};
v_shadow_direct main(a2v I) {
v_shadow_direct O;
O.hpos=mul(m_WVP,I.P);
#ifndef USE_HWSMAP
O.depth=O.hpos.z;
#endif
return O;
}
FXVS;

#endif