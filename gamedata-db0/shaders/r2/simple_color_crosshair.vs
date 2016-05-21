//TB3D 109993 Adapted from Meltac 2.0 DX11 shaders
#include "common.h"
struct vf {
float4 hpos:POSITION;
float4 C:COLOR0;
};
uniform float4 tfactor;
vf main(float4 P:POSITION) {
vf o;
o.hpos=mul(m_WVP,P);

float dist = length(hpos, float4(0,0,0,0));

o.C=tfactor * 5 * dist;

return o;
}