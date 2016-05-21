//TB3D 109993 Adapted from Meltac 2.0 DX11 shaders
#include "common.h"

#ifdef DX11
struct vf {
float4 hpos:POSITION;
float4 C:COLOR0;
};
uniform float4 tfactor;
vf main(float4 P:POSITION) {
vf o;
o.hpos=mul(m_WVP,P);
o.C=tfactor;
return o;
}

#else

struct vf {
float4 hpos:POSITION;
float4 C:COLOR0;
};
uniform float4 tfactor;
vf main(float4 P:POSITION) {
vf o;
o.hpos=mul(m_WVP,P);
o.C=tfactor;
return o;
}

#endif