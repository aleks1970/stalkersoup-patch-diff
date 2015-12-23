//TB3D 109993 Adapted from Meltac 2.0 DX11 shaders
#include "common.h"
#ifdef DX11

struct v2p {
float4 hpos:POSITION;
float4 tc:TEXCOORD0;
#ifdef USE_SJITTER_SUN
float4 tcJ:TEXCOORD1;
#endif
};
uniform float4x4 m_texgen;
uniform float4x4 m_texgen_J;
v2p main(float4 P:POSITION) {
v2p O;
O.hpos=mul(m_WVP,P);
O.tc=mul(m_texgen,P);

#ifdef USE_SJITTER_SUN
	O.tcJ=mul(m_texgen_J,P); // not sure what this does, except that it prevents the game from crashing ;)
#else
	#ifdef USE_SJITTER_OTHER
		O.tcJ=mul(m_texgen_J,P);
	#endif
#endif

return O;
}
FXVS;

#else

struct v2p {
float4 hpos:POSITION;
float4 tc:TEXCOORD0;
#ifdef USE_SJITTER
float4 tcJ:TEXCOORD1;
#endif
};
uniform float4x4 m_texgen;
uniform float4x4 m_texgen_J;
v2p main(float4 P:POSITION) {
v2p O;
O.hpos=mul(m_WVP,P);
O.tc=mul(m_texgen,P);
#ifdef USE_SJITTER
O.tcJ=mul(m_texgen_J,P);
#endif
return O;
}
FXVS;

#endif