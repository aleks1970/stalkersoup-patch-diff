//TB3D 109993 Adapted from Meltac 2.0 DX11 shaders
#include "common.h"

#ifdef DX11
struct vv {
float4 P:POSITION;
float2 tc:TEXCOORD0;
float4 c:COLOR0;
};
struct vf {
float4 hpos:POSITION;
float2 tc:TEXCOORD0;
float4 c:COLOR0;
};
vf main(vv v) {
vf o;
o.hpos=mul(m_WVP,v.P);
o.hpos.z=abs(o.hpos.z);
o.hpos.w=abs(o.hpos.w);
o.tc=v.tc;
o.c=v.c;
return o;
}

#else

struct vv {
float4 P:POSITION;
float2 tc:TEXCOORD0;
float4 c:COLOR0;
};
struct vf {
float4 hpos:POSITION;
float2 tc:TEXCOORD0;
float4 c:COLOR0;
};
vf main(vv v) {
vf o;
o.hpos=mul(m_WVP,v.P);
o.hpos.z=abs(o.hpos.z);
o.hpos.w=abs(o.hpos.w);
o.tc=v.tc;
o.c=v.c;
return o;
}

#endif