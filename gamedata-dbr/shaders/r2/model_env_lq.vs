//TB3D 109993 Adapted from Meltac 2.0 DX11 shaders
#include "common.h"
#include "skin.h"

#ifdef DX11
struct vf {
float4 hpos:POSITION;
float2 tc0:TEXCOORD0;
float3 tc1:TEXCOORD1;
float3 c0:COLOR0;
float fog:FOG;
};
vf _main(v_model v) {
vf o;
float4 pos=v.P;
float3 pos_w=mul(m_W, pos);
float3 norm_w=normalize(mul(m_W,v.N));
o.hpos=mul(m_WVP,pos);
o.tc0=v.tc.xy;
o.tc1=calc_reflection(pos_w,norm_w);
o.c0=calc_model_lq_lighting(norm_w);
o.fog=calc_fogging(float4(pos_w,1));
return o;
}
#ifdef SKIN_NONE
vf main(v_model v) { return _main(v);}
#endif
#ifdef SKIN_0
vf main(v_model_skinned_0 v) { return _main(skinning_0(v));}
#endif
#ifdef SKIN_1
vf main(v_model_skinned_1 v) { return _main(skinning_1(v));}
#endif
#ifdef SKIN_2
vf main(v_model_skinned_2 v) { return _main(skinning_2(v));}
#endif

#else

struct vf {
float4 hpos:POSITION;
float2 tc0:TEXCOORD0;
float3 tc1:TEXCOORD1;
float3 c0:COLOR0;
float fog:FOG;
};
vf _main(v_model v) {
vf o;
float4 pos=v.P;
float3 pos_w=mul(m_W, pos);
float3 norm_w=normalize(mul(m_W,v.N));
o.hpos=mul(m_WVP,pos);
o.tc0=v.tc.xy;
o.tc1=calc_reflection(pos_w,norm_w);
o.c0=calc_model_lq_lighting(norm_w);
o.fog=calc_fogging(float4(pos_w,1));
return o;
}
#ifdef SKIN_NONE
vf main(v_model v) { return _main(v);}
#endif
#ifdef SKIN_0
vf main(v_model_skinned_0 v) { return _main(skinning_0(v));}
#endif
#ifdef SKIN_1
vf main(v_model_skinned_1 v) { return _main(skinning_1(v));}
#endif
#ifdef SKIN_2
vf main(v_model_skinned_2 v) { return _main(skinning_2(v));}
#endif

#endif